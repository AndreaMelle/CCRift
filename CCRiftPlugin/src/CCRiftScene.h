#ifndef __CCRIFT_SCENE_H__
#define __CCRIFT_SCENE_H__

#include "GL/CAPI_GLE.h"
#include "Extras/OVR_Math.h"
#include "Kernel/OVR_Log.h"
#include "OVR_CAPI_GL.h"
#include "CCRiftCommons.h"
#include "CCRiftModel.h"
#include "CCRiftOGLPlatform.h"
#include "CCRiftUVSphere.h"

using namespace OVR;

namespace CCRift
{
	class Scene
	{
	public:
		Scene();
		virtual ~Scene();
		
		bool init(ovrSizei windowSize, ovrSizei frameSize);
		void release();

		void updateFrameTexture(const unsigned char* data);

		void render(Matrix4f view, Matrix4f proj);

		UVSphere* getSphere() { return sphere; }

	private:
		UVSphere *sphere;
		float aspectRatio;

		GLuint createShader(GLenum type, const GLchar* src);
	};

}

#endif //__CCRIFT_SCENE_H__