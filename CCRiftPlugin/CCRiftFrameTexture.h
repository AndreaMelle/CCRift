#ifndef __CCRIFT_FRAMETEXTURE_H__
#define __CCRIFT_FRAMETEXTURE_H__

#include "GL/CAPI_GLE.h"
#include "Extras/OVR_Math.h"
#include "Kernel/OVR_Log.h"
#include "OVR_CAPI_GL.h"

namespace CCRift
{
	class FrameTexture
	{
	public:
		FrameTexture(OVR::Sizei size, bool alpha, bool generateMipMaps, int mipLevels, unsigned char * data);
		virtual ~FrameTexture();

		OVR::Sizei GetSize() const { return mTexSize; }
		GLuint GetTexturePointer() { return mTexId; }

	private:
		GLuint mTexId;
		OVR::Sizei mTexSize;
	};
}

#endif //__CCRIFT_FRAMETEXTURE_H__