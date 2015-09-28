#ifndef __CCRIFT_FRAMETEXTURE_H__
#define __CCRIFT_FRAMETEXTURE_H__

#include "CCRiftCommons.h"

namespace CCRift
{
	class FrameTexture
	{
	public:
		FrameTexture(glm::ivec2 size, bool alpha, bool generateMipMaps, int mipLevels, unsigned char * data);
		virtual ~FrameTexture();

		glm::ivec2 GetSize() const { return mTexSize; }
		GLuint GetTexturePointer() { return mTexId; }

	private:
		GLuint mTexId;
		glm::ivec2 mTexSize;
	};
}

//TODO: def this only on windows

#ifdef CCRIFT_MSW
static CCRift::FrameTexture* loadBMPFromResource(DWORD resourcename)
{
	HBITMAP hBMP;
	BITMAP  BMP;

	hBMP = (HBITMAP)LoadImage(GetMyModuleHandle(), MAKEINTRESOURCE(resourcename), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (!hBMP)
	{
		return nullptr;
	}

	GetObject(hBMP, sizeof(BMP), &BMP);

	WORD bpp = BMP.bmBitsPixel;
	unsigned int width = BMP.bmWidth;
	unsigned int height = BMP.bmHeight;

	CCRift::FrameTexture* tex = nullptr;

	if (bpp == 24)
		tex = new CCRift::FrameTexture(glm::ivec2(width, height), false, false, 1, (unsigned char*)BMP.bmBits);
	else if (bpp == 32)
		tex = new CCRift::FrameTexture(glm::ivec2(width, height), true, false, 1, (unsigned char*)BMP.bmBits);


	DeleteObject(hBMP);

	return tex;
}
#endif

#ifdef CCRIFT_MAC
static CCRift::FrameTexture* loadBMPFromResource(const char* resourcename)
{
    return nullptr;
}
#endif

#endif //__CCRIFT_FRAMETEXTURE_H__