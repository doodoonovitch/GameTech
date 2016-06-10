#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



IcosahedronRendererBase::IcosahedronRendererBase(GLuint capacity)
	: RendererHelper<2>(0, "IcosahedronRenderer", "IcosahedronWireFrameRendererBase", Renderer::Forward_Pass)
	, mVertexArrayCapacity(capacity)
	, mIndexCount(0)
{
}

IcosahedronRendererBase::~IcosahedronRendererBase()
{
}

void IcosahedronRendererBase::LoadShaders()
{
	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/IcosahedronShader.vs.glsl");
	mShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/IcosahedronShader.tcs.glsl");
	mShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/IcosahedronShader.tes.glsl");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/IcosahedronShader.forward.fs.glsl");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_InnerTessLevel",
		"u_OuterTessLevel",
		"u_DrawColor",
	};

	mShader.CreateAndLinkProgram();

	mShader.Use();
	mShader.AddUniforms(uniformNames, __uniforms_count__);


	mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;
}

void IcosahedronRendererBase::Initialize()
{
	LoadShaders();

	const int Faces[] = 
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

	const float Verts[] = 
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * mIndexCount, &Faces[0], GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;
}



IcosahedronRenderer::IcosahedronRenderer(GLuint capacity)
	: IcosahedronRendererBase(capacity)
{
	mSphereList.reserve(mVertexArrayCapacity);

	std::cout << std::endl;
	std::cout << "Initialize BasicTessSphereRenderer...." << std::endl;

	Initialize();

	std::cout << "... BasicTessSphereRenderer initialized!" << std::endl << std::endl;
}

IcosahedronRenderer::~IcosahedronRenderer()
{
}


void IcosahedronRenderer::Render()
{
	mShader.Use();
	glBindVertexArray(mVaoID);

	glDrawArrays(GL_PATCHES, 0, (GLuint)mSphereList.size());
	glBindVertexArray(0);
	mShader.UnUse();
}

void IcosahedronRenderer::RenderWireFrame()
{
	Render();
}




	} // namespace Renderers
} // namespace CoreFx