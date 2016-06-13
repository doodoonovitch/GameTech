#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



IcosahedronRendererBase::IcosahedronRendererBase(const char * shaderTitle, const char * wireFramShaderTitle)
	: RendererHelper<2>(0, shaderTitle, wireFramShaderTitle, Renderer::Forward_Pass)
	, mIndexCount(0)
{
}

IcosahedronRendererBase::~IcosahedronRendererBase()
{
}

void IcosahedronRendererBase::LoadShaders(Shader & shader, const glm::vec4 & uDrawColor, const char * gs, const char * fs, const char * vs, const char * tcs, const char * tes)
{
	//setup shader
	shader.LoadFromFile(GL_VERTEX_SHADER, vs);
	shader.LoadFromFile(GL_TESS_CONTROL_SHADER, tcs);
	shader.LoadFromFile(GL_TESS_EVALUATION_SHADER, tes);
	shader.LoadFromFile(GL_GEOMETRY_SHADER, gs);
	shader.LoadFromFile(GL_FRAGMENT_SHADER, fs);

	const char * uniformNames[__uniforms_count__] =
	{
		"u_InnerTessLevel",
		"u_OuterTessLevel",
		"u_perInstanceDataSampler",
		"u_DrawColor",
	};

	shader.CreateAndLinkProgram();

	shader.Use();
	shader.AddUniforms(uniformNames, __uniforms_count__);

	glUniform1i(shader.GetUniform(u_perInstanceDataSampler), 0);
	glUniform1f(shader.GetUniform(u_InnerTessLevel), 1.f);
	glUniform1f(shader.GetUniform(u_OuterTessLevel), 2.f);

	glUniform4fv(shader.GetUniform(u_DrawColor), 1, glm::value_ptr(uDrawColor));
	
	shader.SetupFrameDataBlockBinding();
	shader.UnUse();

	GL_CHECK_ERRORS;
}

void IcosahedronRendererBase::Initialize(const glm::vec4 & uDrawColor, const glm::vec4 & uWireFrameDrawColor, const char * gs, const char * fs, const char * vs, const char * tcs, const char * tes)
{
	LoadShaders(mShader, uDrawColor, gs, fs, vs, tcs, tes);
	LoadShaders(mWireFrameShader, uWireFrameDrawColor, gs, fs, vs, tcs, tes);
	InitializeVertexBuffer();
}


void IcosahedronRendererBase::InitializeVertexBuffer()
{
	const GLushort Faces[] = 
	{
		2, 1, 0,
		3, 2, 0,
		4, 3, 0,
		5, 4, 0,
		1, 5, 0,
		11, 6,  7,
		11, 7,  8,
		11, 8,  9,
		11, 9,  10,
		11, 10, 6,
		1, 2, 6,
		2, 3, 7,
		3, 4, 8,
		4, 5, 9,
		5, 1, 10,
		2,  7, 6,
		3,  8, 7,
		4,  9, 8,
		5, 10, 9,
		1, 6, 10 
	};

	const GLfloat Verts[] =
	{
		0.000f,  0.000f,  1.000f,
		0.894f,  0.000f,  0.447f,
		0.276f,  0.851f,  0.447f,
		-0.724f,  0.526f,  0.447f,
		-0.724f, -0.526f,  0.447f,
		0.276f, -0.851f,  0.447f,
		0.724f,  0.526f, -0.447f,
		-0.276f,  0.851f, -0.447f,
		-0.894f,  0.000f, -0.447f,
		-0.276f, -0.851f, -0.447f,
		0.724f, -0.526f, -0.447f,
		0.000f,  0.000f, -1.000f 
	};

	mIndexCount = sizeof(Faces) / sizeof(Faces[0]);

	//setup vao and vbo stuff
	glGenVertexArrays(1, &mVaoID);
	glGenBuffers(mVboCount, mVboIDs);

	glBindVertexArray(mVaoID);

	glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[VBO_Vertex]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
	glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	GL_CHECK_ERRORS;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVboIDs[VBO_Index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glPatchParameteri(GL_PATCH_VERTICES, 3);

	glBindVertexArray(0);

	GL_CHECK_ERRORS;
}

void IcosahedronRendererBase::InternalRender(Shader & shader, GLsizei instanceCount, GLuint instanceDataBufferId)
{
	shader.Use();
	glBindVertexArray(mVaoID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, instanceDataBufferId);

	glDrawElementsInstanced(GL_PATCHES, mIndexCount, GL_UNSIGNED_SHORT, 0, instanceCount);

	glBindVertexArray(0);
	shader.UnUse();
}

void IcosahedronRendererBase::InternalRender(GLsizei  instanceCount, GLuint instanceDataBufferId)
{
	InternalRender(mShader, instanceCount, instanceDataBufferId);
}
void IcosahedronRendererBase::InternalRenderWireFrame(GLsizei instanceCount, GLuint instanceDataBufferId)
{
	InternalRender(mWireFrameShader, instanceCount, instanceDataBufferId);
}



IcosahedronRenderer::IcosahedronRenderer(GLsizei capacity)
	: IcosahedronRendererBase()
{
	mSphereList.reserve(capacity);

	std::cout << std::endl;
	std::cout << "Initialize BasicTessSphereRenderer...." << std::endl;

	const glm::vec4 uWireFrameColor = Engine::GetInstance()->GetWireFrameColor();
	glm::vec4 uDrawColor(uWireFrameColor.g, uWireFrameColor.r, uWireFrameColor.b, uWireFrameColor.a);
	Initialize(uDrawColor, uWireFrameColor);

	std::cout << "... BasicTessSphereRenderer initialized!" << std::endl << std::endl;
}

IcosahedronRenderer::~IcosahedronRenderer()
{
}


void IcosahedronRenderer::Render()
{
}

void IcosahedronRenderer::RenderWireFrame()
{
}




	} // namespace Renderers
} // namespace CoreFx