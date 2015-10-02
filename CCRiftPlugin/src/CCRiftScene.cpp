#include "CCRiftScene.h"

using namespace CCRift;

Scene::Scene()
{
}

Scene::~Scene()
{
}

bool Scene::init(glm::ivec2 windowSize, glm::ivec2 frameSize)
{
//	static const GLchar* VertexShaderSrc =
//		"#version 150 core\n"
//		"uniform mat4 matWVP;\n"
//		"in vec3 Position;\n"
//		"in vec2 TexCoord;\n"
//		"out vec2 oTexCoord;\n"
//		"void main()\n"
//		"{\n"
//		"   gl_Position = matWVP * vec4(Position, 1);\n"
//		"   oTexCoord   = TexCoord;\n"
//		"}\n";
//
//	static const char* FragmentShaderSrc =
//		"#version 150 core\n"
//		"uniform sampler2D Texture0;\n"
//		"uniform sampler2D Texture1;\n"
//		"uniform float mix;\n"
//		"in vec2 oTexCoord;\n"
//		"out vec4 color;\n"
//		"void main()\n"
//		"{\n"
//		"   vec3 FrameColor = texture2D(Texture0, oTexCoord).rgb;\n"
//		"   vec3 GridColor = texture2D(Texture1, oTexCoord).rgb;\n"
//		"   vec3 MixColor = (1.0 - mix) * FrameColor + mix * GridColor;\n"
//		"   color = vec4(MixColor, 1.0);\n"
//		"}\n";

    static const GLchar* VertexShaderSrc =
    //"#version 150\n"
    "uniform mat4 matWVP;\n"
    "attribute vec3 Position;\n"
    "attribute vec2 TexCoord;\n"
    "varying vec2 oTexCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = matWVP * vec4(Position, 1);\n"
    "   oTexCoord   = TexCoord;\n"
    "}\n";
    
    static const char* FragmentShaderSrc =
    //"#version 150\n"
    "uniform sampler2D Texture0;\n"
    "uniform sampler2D Texture1;\n"
    "uniform float mix;\n"
    "varying vec2 oTexCoord;\n"
    //"varying vec4 color;\n"
    "void main()\n"
    "{\n"
    "   vec3 FrameColor = texture2D(Texture0, oTexCoord).rgb;\n"
    "   vec3 GridColor = texture2D(Texture1, oTexCoord).rgb;\n"
    "   vec3 MixColor = (1.0 - mix) * FrameColor + mix * GridColor;\n"
    "   gl_FragColor = vec4(MixColor, 1.0);\n"
    "}\n";
	

	GLuint    vshader = createShader(GL_VERTEX_SHADER, VertexShaderSrc);
	GLuint    fshader = createShader(GL_FRAGMENT_SHADER, FragmentShaderSrc);
	
	ShaderFill * grid_material;

	size_t tempBufferLength = frameSize.x * frameSize.y * 4; //hardcoded depth, bad
	unsigned char* tempBuffer = new unsigned char[tempBufferLength];
	memset(tempBuffer, 255, tempBufferLength);

	FrameTexture * generated_texture = new FrameTexture(frameSize, true, false, 1, tempBuffer);

	delete[] tempBuffer;

	grid_material = new ShaderFill(vshader, fshader, generated_texture);
	

	
	glDeleteShader(vshader);
	glDeleteShader(fshader);

    sphere = new UVSphere(glm::vec3(0, 0, 0), grid_material);
	sphere->AddSolidSphere(10.0f, 64.0f);
	sphere->AllocateBuffers();

	return true;
}

void Scene::release()
{
	delete sphere;
}

void Scene::render(glm::mat4 view, glm::mat4 proj)
{    
	sphere->Render(view, proj);
}

void Scene::updateFrameTexture(const unsigned char* data)
{
    glm::ivec2 texSize = sphere->Fill->texture->GetSize();
	GLuint tex = sphere->Fill->texture->GetTexturePointer();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texSize.x, texSize.y, GL_BGRA, GL_UNSIGNED_BYTE, data);
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
			//OVR_DEBUG_LOG(("Compiling shader failed: %s\n", msg));
		}
		return 0;
	}

	return shader;
}

