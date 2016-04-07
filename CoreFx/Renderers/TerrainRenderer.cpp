#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



TerrainRenderer::TerrainRenderer(GLint heightMapWidth, GLint heightMapDepth, glm::vec3 const & scale)
	: RendererHelper<Renderables::Grid, 1>(0, "TerrainRenderer")
	, mMapSize(heightMapWidth, heightMapDepth)
	, mPatchCount(heightMapWidth / 64, heightMapDepth / 64)
	, mScale(scale)
	, mPatchPerTexture(mPatchCount.x * mPatchCount.y)
{
	std::cout << std::endl;
	std::cout << "Initialize TerrainRenderer...." << std::endl;

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_PatchPerTexture",
	};

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/terrain.vs.glsl");
	mShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/terrain.tcs.glsl");
	mShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/terrain.tes.glsl");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/terrain.deferred.fs.glsl");
	mShader.CreateAndLinkProgram();
	mShader.Use();

		mShader.AddUniforms(uniformNames, __uniforms_count__);

		//pass values of constant uniforms at initialization
		glUniform2iv(mShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount));
		glUniform2iv(mShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize));
		glUniform3fv(mShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale));
		glUniform1i(mShader.GetUniform(u_PatchCount), mPatchPerTexture);

		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	//setup vao and vbo stuff
	glGenVertexArrays(1, &mVaoID);
	glBindVertexArray(mVaoID);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glBindVertexArray(0);

	GL_CHECK_ERRORS;
	
	std::cout << "... TerrainRenderer initialized!" << std::endl << std::endl;
}


TerrainRenderer::~TerrainRenderer()
{
}
 
void TerrainRenderer::Render()
{
	mShader.Use();
	glBindVertexArray(mVaoID);

	if (GetWireFrame())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchPerTexture);

	glBindVertexArray(0);
	mShader.UnUse();
}



	} // namespace Renderers
} // namespace CoreFx