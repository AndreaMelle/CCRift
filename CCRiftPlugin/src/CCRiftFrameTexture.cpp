#include "CCRiftFrameTexture.h"

using namespace CCRift;
using namespace OVR;

FrameTexture::FrameTexture(Sizei size,
	bool alpha,
	bool generateMipMaps,
	int mipLevels,
	unsigned char * data)
	: mTexId(0)
	, mTexSize(0, 0)
{

	mTexSize = size;

	glGenTextures(1, &mTexId);
	glBindTexture(GL_TEXTURE_2D, mTexId);

	if (!generateMipMaps)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	if (alpha)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTexSize.w, mTexSize.h, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mTexSize.w, mTexSize.h, 0, GL_BGR, GL_UNSIGNED_BYTE, data);


	if (generateMipMaps && mipLevels > 1)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

}

FrameTexture::~FrameTexture()
{
	if (mTexId)
	{
		glDeleteTextures(1, &mTexId);
		mTexId = 0;
	}
}
