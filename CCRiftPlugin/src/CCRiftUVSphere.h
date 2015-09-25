#ifndef __CCRIFT_UVSPHERE_H__
#define __CCRIFT_UVSPHERE_H__

#include "CCRiftCommons.h"
#include "CCRiftFrameTexture.h"
#include "CCRiftBufferHelpers.h"

namespace CCRift
{
	class UVSphere
	{
	public:
		UVSphere(glm::vec3 pos, ShaderFill * fill);
		virtual ~UVSphere();

		glm::mat4& GetMatrix();

		void AllocateBuffers();
		void AddSolidSphere(float radius, float segments = 32.0f);
		void Render(glm::mat4 view, glm::mat4 proj);
		
		ShaderFill    * Fill;

		void toggleGrid() { mShowGrid = !mShowGrid; }
		bool Grid() { return mShowGrid; }
		void setGridMix(float mix) { mGridMix = mix; }
		float getGridMix() { return mGridMix; }

	private:

		struct Vertex
		{
			glm::vec3  Pos;
			float     U, V;
		};

		glm::vec3        Pos;
		glm::quat       Rot;
		glm::mat4        Mat;
		int             numVertices;
		std::vector<Vertex>          Vertices;

		FrameTexture *mGridTexture;

		GLuint posLoc;
		GLuint uvLoc;
		VertexBuffer  * vertexBuffer;

		void FreeBuffers();
		void AddVertex(const Vertex& v);


		bool mShowGrid;
		float mGridMix;
	};
}


#endif //__CCRIFT_UVSPHERE_H__