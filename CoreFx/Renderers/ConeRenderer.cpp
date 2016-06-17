#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



ConeRendererBase::ConeRendererBase(const char * shaderTitle, const char * wireFramShaderTitle)
	: RendererHelper<1>(0, shaderTitle, wireFramShaderTitle, Renderer::Forward_Pass)
{
}

ConeRendererBase::~ConeRendererBase()
{
}

void ConeRendererBase::LoadShaders(Shader & shader, const char * gs, const char * fs, const char * vs)
{
	//setup shader
	shader.LoadFromFile(GL_VERTEX_SHADER, vs);
	shader.LoadFromFile(GL_GEOMETRY_SHADER, gs);
	shader.LoadFromFile(GL_FRAGMENT_SHADER, fs);

	shader.CreateAndLinkProgram();
}

void ConeRendererBase::InitializeVertexBuffer(GLuint numStrips)
{
	mIndexCount = numStrips + 1;
	std::vector<GLfloat> verts;
	verts.reserve(mIndexCount * 3);

	verts.push_back(0.f);
	verts.push_back(1.f);
	verts.push_back(0.f);
	for (GLuint i = 0; i < numStrips; i++)
	{
		float angle = (float)i * glm::two_pi<float>() / (float)numStrips;
		verts.push_back(0.5f * glm::cos(angle));
		verts.push_back(0.f);
		verts.push_back(0.5f * glm::sin(angle));
	}
	//setup vao and vbo stuff
	CreateBuffers();

	glBindVertexArray(mVaoID);

	glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
	glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;
}




	} // namespace Renderers
} // namespace CoreFx