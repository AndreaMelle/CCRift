#include "CCRiftScene.h"

using namespace CCRift;

void Scene::Loop(OGLPlatform& context)
{
	static float Yaw(3.141592f);
	if (context.Key[VK_LEFT])  Yaw += 0.02f;
	if (context.Key[VK_RIGHT]) Yaw -= 0.02f;


	//TODO: this totally does not account for mouse going out the window!!!!

	if (context.MouseDown && !wasDown)
	{
		onMouseDownMouseX = (float)context.MouseX;
		onMouseDownMouseY = (float)context.MouseY;
		onMouseDownLon = lon;
		onMouseDownLat = lat;
		wasDown = true;
	}
	else if (wasDown && context.MouseDown)
	{
		lon = (onMouseDownMouseX - context.MouseX) * mMouseSensitivity + onMouseDownLon;
		lat = (context.MouseY - onMouseDownMouseY) * mMouseSensitivity + onMouseDownLat;
	}
	else
	{
		wasDown = false;
	}

	// Get view and projection matrices
	Matrix4f rollPitchYaw = Matrix4f::RotationY(Yaw);
	Matrix4f finalRollPitchYaw = rollPitchYaw;
	Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
	Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
	Vector3f shiftedEyePos = Vector3f(0,0,0);
	//Matrix4f view = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);


	float phi = (90.0f - lat) * M_PI / 180.0f;
	float theta = lon * M_PI / 180.0f;

	float targetX = 500 * sin(phi) * cos(theta);
	float targetY = 500 * cos(phi);
	float targetZ = 500 * sin(phi) * sin(theta);

	Matrix4f view = Matrix4f::LookAtRH(shiftedEyePos, Vector3f(targetX, targetY, targetZ), finalUp);

	float verticalFovRadians = 60.0f * 3.14159265f / 180.0f;
	ovrFovPort fov;

	fov.DownTan = tan(verticalFovRadians / 2);
	fov.UpTan = fov.DownTan;
	fov.LeftTan = aspectRatio * fov.DownTan;
	fov.RightTan = fov.LeftTan;

	Matrix4f proj = ovrMatrix4f_Projection(fov, 0.2f, 1000.0f, ovrProjection_RightHanded);

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	DWORD dwWaitResult = WaitForSingleObject(frameBufferMutex, 50);

	switch (dwWaitResult)
	{
		// The thread got ownership of the mutex
	case WAIT_OBJECT_0:
		__try {
			if (frameDataBufferNewFlag)
			{
				Sizei texSize = sphere->Fill->texture->GetSize();
				GLuint tex = sphere->Fill->texture->texId;
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tex);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texSize.w, texSize.h, GL_BGRA, GL_UNSIGNED_BYTE, frameDataBuffer);
				//glFinish();
				glBindTexture(GL_TEXTURE_2D, 0);
				frameDataBufferNewFlag = false;
			}
		}
		__finally {
			// Release ownership of the mutex object
			if (!ReleaseMutex(frameBufferMutex))
			{
				// Handle error.
			}
		}
		break;
	case WAIT_ABANDONED:
		return;
	}

	
	
	Render(view, proj);

	SwapBuffers(context.hDC);

}

void Scene::Render(Matrix4f view, Matrix4f proj)
{
	sphere->Render(view, proj);
}

void Scene::CopyFrameData(unsigned char * data, DWORD timeout)
{
	DWORD dwWaitResult = WaitForSingleObject(frameBufferMutex, timeout);

	switch (dwWaitResult)
	{
		// The thread got ownership of the mutex
	case WAIT_OBJECT_0:
		__try {
			memcpy(frameDataBuffer, data, frameDataBufferSize);
			frameDataBufferNewFlag = true;
		}
		__finally {
			// Release ownership of the mutex object
			if (!ReleaseMutex(frameBufferMutex))
			{
				// Handle error.
			}
		}
		break;
	case WAIT_ABANDONED:
		return;
	}
}

GLuint Scene::CreateShader(GLenum type, const GLchar* src)
{
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	GLint r;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
	if (!r)
	{
		GLchar msg[1024];
		glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
		if (msg[0]) {
			OVR_DEBUG_LOG(("Compiling shader failed: %s\n", msg));
		}
		return 0;
	}

	return shader;
}

bool Scene::Init(OGLPlatform& context, ovrSizei windowSize)
{
	aspectRatio = (float)windowSize.w / (float)windowSize.h;

	static const GLchar* VertexShaderSrc =
		"#version 150\n"
		"uniform mat4 matWVP;\n"
		"in      vec4 Position;\n"
		"in      vec2 TexCoord;\n"
		"out     vec2 oTexCoord;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = (matWVP * Position);\n"
		"   oTexCoord   = TexCoord;\n"
		"}\n";

	static const char* FragmentShaderSrc =
		"#version 150\n"
		"uniform sampler2D Texture0;\n"
		"in      vec2      oTexCoord;\n"
		"out     vec4      FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = texture2D(Texture0, oTexCoord);\n"
		"}\n";

	GLuint    vshader = CreateShader(GL_VERTEX_SHADER, VertexShaderSrc);
	GLuint    fshader = CreateShader(GL_FRAGMENT_SHADER, FragmentShaderSrc);

	// Make textures
	ShaderFill * grid_material;
	//TextureBuffer * generated_texture = loadBMP_custom("D:/main/Workspace/CCRiftPlugin/Transmitter/StandalonePreview/pano.BMP");

	frameBufferMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

	if (frameBufferMutex == NULL)
	{
		MessageBoxA(NULL, "CreateMutex error", "Preview", MB_ICONERROR | MB_OK);
		return false;
	}

	frameDataBufferNewFlag = false;
	frameDataBufferSize = 1920 * 960 * 4;
	frameDataBuffer = new unsigned char[frameDataBufferSize];

	memset(frameDataBuffer, 0, frameDataBufferSize);

	BasicTexture * generated_texture = new BasicTexture(OVR::Sizei(1920, 960), true, false, 1, frameDataBuffer);

	grid_material = new ShaderFill(vshader, fshader, generated_texture);


	glDeleteShader(vshader);
	glDeleteShader(fshader);

	sphere = new UVSphere(Vector3f(0, 0, 0), grid_material);
	sphere->AddSolidSphere(20.0f, 32.0f);
	sphere->AllocateBuffers();

	// Turn off vsync to let the compositor do its magic
	//wglSwapIntervalEXT(0);

	isVisible = true;
	onMouseDownMouseX = 0;
	onMouseDownMouseY = 0;
	lon = 0;
	onMouseDownLon = 0;
	lat = 0;
	onMouseDownLat = 0;
	mMouseSensitivity = 0.1f;
	wasDown = false;

	return true;
}

Scene::Scene()
{

}

Scene::~Scene()
{
}

void Scene::Release(OGLPlatform& context)
{
	delete sphere;
	delete[] frameDataBuffer;
}

