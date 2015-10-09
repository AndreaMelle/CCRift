#include "CCRiftUVSphere.h"
#include "PlatformUtils.h"

using namespace CCRift;

UVSphere::UVSphere(glm::vec3 pos, ShaderFill * fill)
    : Pos(pos)
    , numVertices(0)
	, vertexBuffer(nullptr)
	, mShowGrid(false)
	, mGridMix(0.5f)
{
	Rot.x = 0;
	Rot.y = 1;
	Rot.z = 0;
	Rot.w = 0;

	Mat = glm::mat4(1.0f);
	Mat = glm::mat4_cast(Rot);
	Mat = glm::translate(Mat, Pos);

	mGridTexture = LoadTextureFromBitmap("gridUpsideDown.BMP");

	Fill = fill;
	
	posLoc = glGetAttribLocation(Fill->program, "Position");
	uvLoc = glGetAttribLocation(Fill->program, "TexCoord");
}

UVSphere::~UVSphere()
{
	FreeBuffers();
	delete mGridTexture;
}

glm::mat4& UVSphere::GetMatrix()
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
#ifdef CCRIFT_MSW
	glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);
#endif
    
    vertexBuffer = new VertexBuffer(&Vertices[0], numVertices * sizeof(Vertices[0]));
	
	//glBindVertexArray(0);
}

void UVSphere::FreeBuffers()
{
#ifdef CCRIFT_MSW
	glDeleteVertexArrays(1, &vao);
#endif
    
	delete vertexBuffer;
	vertexBuffer = nullptr;
}

void UVSphere::AddSolidSphere(float radius, float segments)
{
	GLfloat theta1 = 0.0f;
	GLfloat theta2 = 0.0f;
	GLfloat theta3 = 0.0f;

	GLfloat px, py, pz, pu, pv, nx, ny, nz;


	for (int i = 0; i < segments / 2.0f; ++i)
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

			pu = (j / (GLfloat)segments);
			pv = 2.0f * (i + 1.0f) / (float)segments;

			v1.x = px;
			v1.y = py;
			v1.z = pz;
			v1.u = pu;
			v1.v = pv;

			AddVertex(v1);

			nx = cosf(theta1) * cosf(theta3);
			ny = sinf(theta1);
			nz = cosf(theta1) * sinf(theta3);
			px = radius * nx;
			py = radius * ny;
			pz = radius * nz;

			pu = (j / (GLfloat)segments);
			pv = 2.0f * i / (GLfloat)segments;

			v2.x = px;
			v2.y = py;
			v2.z = pz;
			v2.u = pu;
			v2.v = pv;

			AddVertex(v2);
		}
	}
}

void UVSphere::Render(glm::mat4 view, glm::mat4 proj)
{
	glm::mat4 combined = proj * view * Mat;

#ifdef CCRIFT_MSW
    glBindVertexArray(vao);
#endif
    
	glUseProgram(Fill->program);
	glUniform1i(glGetUniformLocation(Fill->program, "Texture0"), 0);
	glUniformMatrix4fv(glGetUniformLocation(Fill->program, "matWVP"), 1, GL_FALSE, &combined[0][0]);
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
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	
    glEnableVertexAttribArray(uvLoc);
    glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices);

    glDisableVertexAttribArray(posLoc);
	glDisableVertexAttribArray(uvLoc);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    
#ifdef CCRIFT_MSW
	glBindVertexArray(0);
#endif
    
	glUseProgram(0);
	
}