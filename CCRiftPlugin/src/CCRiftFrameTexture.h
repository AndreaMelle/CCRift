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


#endif //__CCRIFT_FRAMETEXTURE_H__