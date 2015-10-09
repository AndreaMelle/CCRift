#ifndef __CCRIFT_BUFFERHELPERS_H__
#define __CCRIFT_BUFFERHELPERS_H__

#include "CCRiftCommons.h"
#include "CCRiftFrameTexture.h"


////---------------------------------------------------------------------------------------
//struct DepthBuffer
//{
//	GLuint        texId;
//
//	DepthBuffer(Sizei size, int sampleCount)
//	{
//		OVR_ASSERT(sampleCount <= 1); // The code doesn't currently handle MSAA textures.
//
//		glGenTextures(1, &texId);
//		glBindTexture(GL_TEXTURE_2D, texId);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//		GLenum internalFormat = GL_DEPTH_COMPONENT24;
//		GLenum type = GL_UNSIGNED_INT;
//		if (GLE_ARB_depth_buffer_float)
//		{
//			internalFormat = GL_DEPTH_COMPONENT32F;
//			type = GL_FLOAT;
//		}
//
//		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.w, size.h, 0, GL_DEPTH_COMPONENT, type, NULL);
//	}
//	~DepthBuffer()
//	{
//		if (texId)
//		{
//			glDeleteTextures(1, &texId);
//			texId = 0;
//		}
//	}
//};

//------------------------------------------------------------------------------
struct ShaderFill
{
	GLuint            program;
	CCRift::FrameTexture   * texture;

	ShaderFill(GLuint vertexShader, GLuint pixelShader, CCRift::FrameTexture* _texture)
	{
		texture = _texture;

		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);
		glLinkProgram(program);

		glDetachShader(program, vertexShader);
		glDetachShader(program, pixelShader);

		GLint r;
		glGetProgramiv(program, GL_LINK_STATUS, &r);
		if (!r)
		{
			GLchar msg[1024];
			glGetProgramInfoLog(program, sizeof(msg), 0, msg);
			printf("%s", msg);
			//OVR_DEBUG_LOG(("Linking shaders failed: %s\n", msg));
		}
	}

	~ShaderFill()
	{
		if (program)
		{
			glDeleteProgram(program);
			program = 0;
		}
		if (texture)
		{
			delete texture;
			texture = nullptr;
		}
	}
};

//----------------------------------------------------------------
struct VertexBuffer
{
	GLuint    buffer;

	VertexBuffer(void* vertices, size_t size)
	{
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}
	~VertexBuffer()
	{
		if (buffer)
		{
			glDeleteBuffers(1, &buffer);
			buffer = 0;
		}
	}
};

//----------------------------------------------------------------
struct IndexBuffer
{
	GLuint    buffer;

	IndexBuffer(void* indices, size_t size)
	{
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	}
	~IndexBuffer()
	{
		if (buffer)
		{
			glDeleteBuffers(1, &buffer);
			buffer = 0;
		}
	}
};

#endif //__CCRIFT_BUFFERHELPERS_H__