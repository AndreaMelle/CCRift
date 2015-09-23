#include "CCRiftScene.h"

using namespace CCRift;

Scene::Scene()
{
}

Scene::~Scene()
{
}

bool Scene::init(ovrSizei windowSize, ovrSizei frameSize)
{
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

	GLuint    vshader = createShader(GL_VERTEX_SHADER, VertexShaderSrc);
	GLuint    fshader = createShader(GL_FRAGMENT_SHADER, FragmentShaderSrc);

	ShaderFill * grid_material;

	size_t tempBufferLength = frameSize.w * frameSize.h * 4; //hardcoded depth, bad
	unsigned char* tempBuffer = new unsigned char[tempBufferLength];
	memset(tempBuffer, 0, tempBufferLength);

	FrameTexture * generated_texture = new FrameTexture(frameSize, true, false, 1, tempBuffer);

	delete[] tempBuffer;

	grid_material = new ShaderFill(vshader, fshader, generated_texture);


	glDeleteShader(vshader);
	glDeleteShader(fshader);

	sphere = new UVSphere(Vector3f(0, 0, 0), grid_material);
	sphere->AddSolidSphere(20.0f, 32.0f);
	sphere->AllocateBuffers();

	

	return true;
}

void Scene::release()
{
	delete sphere;
}

void Scene::render(Matrix4f view, Matrix4f proj)
{
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	sphere->Render(view, proj);
}

void Scene::updateFrameTexture(const unsigned char* data)
{
	Sizei texSize = sphere->Fill->texture->GetSize();
	GLuint tex = sphere->Fill->texture->GetTexturePointer();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texSize.w, texSize.h, GL_BGRA, GL_UNSIGNED_BYTE, data);
	//glFinish();
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Scene::createShader(GLenum type, const GLchar* src)
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

