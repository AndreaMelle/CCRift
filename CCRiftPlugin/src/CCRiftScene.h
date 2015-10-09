#ifndef __CCRIFT_SCENE_H__
#define __CCRIFT_SCENE_H__

#include "CCRiftCommons.h"
#include "CCRiftOGLPlatform.h"
#include "CCRiftUVSphere.h"

namespace CCRift
{
	class Scene
	{
	public:
		Scene();
		virtual ~Scene();
		
        bool init(glm::ivec2 windowSize, glm::ivec2 frameSize);
		void release();

		void updateFrameTexture(const unsigned char* data);

        void render(glm::mat4 view, glm::mat4 proj);

		UVSphere* getSphere() { return sphere; }

	private:
		UVSphere *sphere;
		//float aspectRatio;
        
		GLuint createShader(GLenum type, const GLchar* src);
	};

}

#endif //__CCRIFT_SCENE_H__