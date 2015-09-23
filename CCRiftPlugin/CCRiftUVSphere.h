#ifndef __CCRIFT_UVSPHERE_H__
#define __CCRIFT_UVSPHERE_H__

#include "GL/CAPI_GLE.h"
#include "Extras/OVR_Math.h"
#include "Kernel/OVR_Log.h"
#include "OVR_CAPI_GL.h"
#include "CCRiftCommons.h"
#include "CCRiftBufferHelpers.h"

using namespace OVR;

namespace CCRift
{
	class UVSphere
	{
	public:
		struct Vertex
		{
			Vector3f  Pos;
			float     U, V;
		};

		Vector3f        Pos;
		Quatf           Rot;
		Matrix4f        Mat;
		int             numVertices;
		//, numIndices;
		Vertex          Vertices[2000]; // Note fixed maximum
		//GLushort        Indices[2000];
		ShaderFill    * Fill;
		VertexBuffer  * vertexBuffer;
		//IndexBuffer   * indexBuffer;

		UVSphere(Vector3f pos, ShaderFill * fill);
		virtual ~UVSphere();

		Matrix4f& GetMatrix();

		void AddVertex(const Vertex& v);
		//void AddIndex(GLushort a);
		void AllocateBuffers();
		void FreeBuffers();
		void AddSolidSphere(float radius, float segments = 32.0f);
		void Render(Matrix4f view, Matrix4f proj);
	};
}


#endif //__CCRIFT_UVSPHERE_H__