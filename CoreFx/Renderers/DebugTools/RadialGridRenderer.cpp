#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



RadialGridRenderer::RadialGridRenderer(GLushort slides, GLushort rings, GLfloat maxDetail, GLfloat gridSize)
	: RendererHelper<2>("RadialGridRenderer", "RadialGridWireFrameRenderer", Forward_Pass)
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize RadialGridRenderer.....");

	const char * uniformNames[__uniforms_count__] =
	{
		"vGridSize"
	};

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/RadialGrid.vs.glsl");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/grid.forward.fs.glsl");
	mShader.CreateAndLinkProgram();
	mShader.Use();

		mShader.AddUniforms(uniformNames, __uniforms_count__);

		//pass values of constant uniforms at initialization
		glUniform2f(mShader.GetUniform(vGridSize), (float)slides, (float)rings);

		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	//setup shader
	mWireFrameShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/RadialGrid.vs.glsl");
	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/wireframe.fs.glsl");
	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();

	mWireFrameShader.AddUniforms(uniformNames, __uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform2f(mWireFrameShader.GetUniform(vGridSize), (float)slides, (float)rings);

	mWireFrameShader.SetupFrameDataBlockBinding();
	mWireFrameShader.UnUse();

	GL_CHECK_ERRORS;

	glm::vec3 * vertices = nullptr;
	GLushort * indices = nullptr;
	Geometry::Helpers::GenerateRadialGrid(mVertexCount, mIndexCount, vertices, indices, slides, rings, maxDetail, gridSize);

	//setup vao and vbo stuff
	CreateBuffers();

	glBindVertexArray(mVaoID);

		glBindBuffer (GL_ARRAY_BUFFER, mVboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, mVertexCount * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;
		 
		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 0, 0);
		GL_CHECK_ERRORS; 

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVboIDs[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * mIndexCount, indices, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	delete[] vertices;	 
	delete[] indices;

	mIsInitialized = true;

	PRINT_MESSAGE("\tVertex count = %li", mVertexCount);
	PRINT_MESSAGE("\tIndex count = %li", mIndexCount);
	PRINT_MESSAGE("\tTriangle count = %li", mIndexCount / 3);

	PRINT_MESSAGE(".....RadialGridRenderer initialized!");
	PRINT_END_SECTION;
}


RadialGridRenderer::~RadialGridRenderer()
{
}
 
void RadialGridRenderer::Render()
{
	mShader.Use();
	glBindVertexArray(mVaoID);
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);
	mShader.UnUse();
}

void RadialGridRenderer::RenderWireFrame()
{
	mWireFrameShader.Use();
	glBindVertexArray(mVaoID);
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);
	mWireFrameShader.UnUse();
}


	} // namespace Renderers
} // namespace CoreFx