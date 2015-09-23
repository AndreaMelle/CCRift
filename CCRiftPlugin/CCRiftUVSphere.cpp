#include "CCRiftUVSphere.h"

using namespace CCRift;

UVSphere::UVSphere(Vector3f pos, ShaderFill * fill) :
	numVertices(0),
	Pos(pos),
	vertexBuffer(nullptr)
	, mShowGrid(false)
	, mGridMix(0.5f)
{
	Rot.x = 0;
	Rot.y = 1;
	Rot.z = 0;
	Rot.w = 0;

	Mat = Matrix4f(Rot);
	Mat = Matrix4f::Translation(Pos) * Mat;

	mGridTexture = loadBMPFromResource(IDB_BITMAP1);

	Fill = fill;
	posLoc = glGetAttribLocation(Fill->program, "Position");
	uvLoc = glGetAttribLocation(Fill->program, "TexCoord");
}

UVSphere::~UVSphere()
{
	FreeBuffers();
	delete mGridTexture;
}

Matrix4f& UVSphere::GetMatrix()
{
	return Mat;
}

void UVSphere::AddVertex(const Vertex& v)
{
	Vertices.push_back(v);
	numVertices++;
}

void UVSphere::AllocateBuffers()
{
	vertexBuffer = new VertexBuffer(&Vertices[0], numVertices * sizeof(Vertices[0]));
}

void UVSphere::FreeBuffers()
{
	delete vertexBuffer;
	vertexBuffer = nullptr;
}

void UVSphere::AddSolidSphere(float radius, float segments)
{
	GLfloat theta1 = 0.0f;
	GLfloat theta2 = 0.0f;
	GLfloat theta3 = 0.0f;

	GLfloat px, py, pz, pu, pv, nx, ny, nz;


	for (int i = 0; i < segments / 2; ++i)
	{
		theta1 = i * 2.0f * M_PI / segments - M_PI * 0.5f;
		theta2 = (i + 1) * 2.0f * M_PI / segments - M_PI * 0.5f;

		for (int j = 0; j <= segments; ++j)
		{
			Vertex v1, v2;

			theta3 = j * 2.0f * M_PI / segments;

			nx = cosf(theta2) * cosf(theta3);
			ny = sinf(theta2);
			nz = cosf(theta2) * sinf(theta3);

			px = radius * nx;
			py = radius * ny;
			pz = radius * nz;

			pu = (j / (float)segments);
			pv = 2.0f * (i + 1.0f) / (float)segments;

			v1.Pos = Vector3f(px, py, pz);
			v1.U = pu;
			v1.V = pv;

			AddVertex(v1);

			nx = cosf(theta1) * cosf(theta3);
			ny = sinf(theta1);
			nz = cosf(theta1) * sinf(theta3);
			px = radius * nx;
			py = radius * ny;
			pz = radius * nz;

			pu = (j / (float)segments);
			pv = 2.0f * i / (float)segments;

			v2.Pos = Vector3f(px, py, pz);
			v2.U = pu;
			v2.V = pv;

			AddVertex(v2);
		}
	}
}

void UVSphere::Render(Matrix4f view, Matrix4f proj)
{
	Matrix4f combined = proj * view * Mat;

	glUseProgram(Fill->program);
	glUniform1i(glGetUniformLocation(Fill->program, "Texture0"), 0);
	glUniformMatrix4fv(glGetUniformLocation(Fill->program, "matWVP"), 1, GL_TRUE, (FLOAT*)&combined);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Fill->texture->GetTexturePointer());

	if (mShowGrid && mGridTexture)
	{
		glUniform1i(glGetUniformLocation(Fill->program, "Texture1"), 1);
		glUniform1f(glGetUniformLocation(Fill->program, "mix"), mGridMix);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mGridTexture->GetTexturePointer());
	}
	else
	{
		glUniform1f(glGetUniformLocation(Fill->program, "mix"), 0.0f);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->buffer);

	glEnableVertexAttribArray(posLoc);
	glEnableVertexAttribArray(uvLoc);

	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, Pos));
	glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)OVR_OFFSETOF(Vertex, U));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices);

	glDisableVertexAttribArray(posLoc);
	glDisableVertexAttribArray(uvLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);
}