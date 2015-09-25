#ifndef __CCRIFT_MODEL_H__
#define __CCRIFT_MODEL_H__

#ifdef OCULUS_RIFT_PLATFORM
#include "GL/CAPI_GLE.h"
#include "Extras/OVR_Math.h"
#include "Kernel/OVR_Log.h"
#include "OVR_CAPI_GL.h"
#endif
#include "CCRiftCommons.h"
#include "CCRiftBufferHelpers.h"

using namespace OVR;

namespace CCRift
{
	class Model
	{
	public:
		struct Vertex
		{
			Vector3f  Pos;
			DWORD     C;
			float     U, V;
		};

		Vector3f        Pos;
		Quatf           Rot;
		Matrix4f        Mat;
		int             numVertices, numIndices;
		Vertex          Vertices[2000]; // Note fixed maximum
		GLushort        Indices[2000];
		ShaderFill    * Fill;
		VertexBuffer  * vertexBuffer;
		IndexBuffer   * indexBuffer;

		Model(Vector3f pos, ShaderFill * fill);
		virtual ~Model();

		Matrix4f& GetMatrix();

		void AddVertex(const Vertex& v);
		void AddIndex(GLushort a);
		void AllocateBuffers();
		void FreeBuffers();
		void AddSolidColorBox(float x1, float y1, float z1, float x2, float y2, float z2, DWORD c);
		void Render(Matrix4f view, Matrix4f proj);
	};
}


#endif //__CCRIFT_MODEL_H__