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

		void Render(Matrix4f view, Matrix4f proj);
		GLuint CreateShader(GLenum type, const GLchar* src);

		void SetFrameData(GLsizei w, GLsizei h, unsigned char * data);
		
		void Release(OGLPlatform& context);
		bool Init(OGLPlatform& context, ovrSizei windowSize);
		void Loop(OGLPlatform& context);

	private:
		UVSphere *sphere;

		bool isVisible;
		float onMouseDownMouseX;
		float onMouseDownMouseY;
		float lon;
		float onMouseDownLon;
		float lat;
		float onMouseDownLat;
		float mMouseSensitivity;
		bool wasDown;
		float aspectRatio;
	};

}

#endif //__CCRIFT_SCENE_H__