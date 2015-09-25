#ifndef __CCRIFT_UVSPHERE_H__
#define __CCRIFT_UVSPHERE_H__

#include "GL/CAPI_GLE.h"
#include "Extras/OVR_Math.h"
#include "Kernel/OVR_Log.h"
#include "OVR_CAPI_GL.h"
#include "CCRiftCommons.h"
#include "CCRiftBufferHelpers.h"
#include "CCRiftFrameTexture.h"
#include <vector>

using namespace OVR;

namespace CCRift
{
	class UVSphere
	{
	public:
		UVSphere(Vector3f pos, ShaderFill * fill);
		virtual ~UVSphere();

		Matrix4f& GetMatrix();

		void AllocateBuffers();
		void AddSolidSphere(float radius, float segments = 32.0f);
		void Render(Matrix4f view, Matrix4f proj);
		
		ShaderFill    * Fill;

		void toggleGrid() { mShowGrid = !mShowGrid; }
		bool Grid() { return mShowGrid; }
		void setGridMix(float mix) { mGridMix = mix; }
		float getGridMix() { return mGridMix; }

	private:

		struct Vertex
		{
			Vector3f  Pos;
			float     U, V;
		};

		Vector3f        Pos;
		Quatf           Rot;
		Matrix4f        Mat;
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