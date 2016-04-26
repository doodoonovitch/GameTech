#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



DeepOceanRenderer::DeepOceanRenderer(const Desc & desc)
	: RendererHelper<Renderables::Grid, 1>(0, "DeepOceanRenderer")
	, mMapSize(desc.mMapWidth, desc.mMapDepth)
	, mPatchCount(desc.mMapWidth / 64, desc.mMapDepth / 64)
	, mScale(desc.mScale)
	, mMapCount(0)
	, mDrawNormalShader("OceanDrawNormals")
{
	std::cout << std::endl;
	std::cout << "Initialize DeepOceanRenderer...." << std::endl;

	const glm::vec3 vertices[] =
	{
		glm::vec3(0.0f,	0.0f, 0.0f),
		glm::vec3(1.0f,	0.0f, 0.0f),
		glm::vec3(0.0f,	0.0f, 1.0f),
		glm::vec3(1.0f,	0.0f, 1.0f)
	};
		

	//setup vao and vbo stuff
	glGenVertexArrays(1, &mVaoID);
	glGenBuffers(mVboCount, mVboIDs);

	glBindVertexArray(mVaoID);

	glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
	glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	GL_CHECK_ERRORS;

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	PerMapData * modelMatrixBuffer = new PerMapData[mMapCount];
	for (int i = 0; i < mMapCount; ++i)
	{
		modelMatrixBuffer[i].mModelDQ = desc.mMaps[i].mModelDQ;
	}
	mModelMatrixBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, (mMapCount * sizeof(PerMapData)), modelMatrixBuffer);


	LoadShaders(desc);

	std::cout << "... DeepOceanRenderer initialized!" << std::endl << std::endl;
}


DeepOceanRenderer::~DeepOceanRenderer()
{
	mModelMatrixBuffer.ReleaseResource();
}

void DeepOceanRenderer::LoadShaders(const Desc & /*desc*/)
{
	PRINT_MESSAGE("Initialize Terrain Renderer Shaders : \n\n");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_PerMapDataSampler",
	};


	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/deepOcean.vs.glsl");
	mShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/deepOcean.tcs.glsl");
	mShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/deepOcean.tes.glsl");
	mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/deepOcean.gs.glsl");

	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/deepOcean.deferred.fs.glsl");

	mShader.CreateAndLinkProgram();
	mShader.Use();

	mShader.AddUniforms(uniformNames, __uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform2iv(mShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2iv(mShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
	glUniform3fv(mShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_PerMapDataSampler), 1); GL_CHECK_ERRORS;


	mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;


	const char * uniformNames2[__uniforms2_count__] =
	{
		"u_NormalMagnitude",
		"u_VertexNormalColor",
	};

	mDrawNormalShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/deepOcean.vs.glsl");
	mDrawNormalShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/deepOceann.tcs.glsl");
	mDrawNormalShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/deepOcean.tes.glsl");
	mDrawNormalShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/terrain_vertex_normal.gs.glsl");
	mDrawNormalShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/vertex_normal.fs.glsl");
	mDrawNormalShader.CreateAndLinkProgram();
	mDrawNormalShader.Use();

	mDrawNormalShader.AddUniforms(uniformNames, __uniforms_count__);
	mDrawNormalShader.AddUniforms(uniformNames2, __uniforms2_count__);

	//pass values of constant uniforms at initialization
	glUniform2iv(mDrawNormalShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2iv(mDrawNormalShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
	glUniform3fv(mDrawNormalShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;

	glUniform1i(mDrawNormalShader.GetUniform(u_PerMapDataSampler), 1); GL_CHECK_ERRORS;


	mDrawNormalShader.SetupFrameDataBlockBinding();
	mDrawNormalShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE("... done.\n");
	PRINT_MESSAGE("-------------------------------------------------\n\n");
}
 
void DeepOceanRenderer::Render()
{
	mShader.Use();
	glBindVertexArray(mVaoID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());

	glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y * mMapCount);

	glBindVertexArray(0);
	mShader.UnUse();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void DeepOceanRenderer::DebugRender()
{
	Engine * engine = Engine::GetInstance();

	if (engine->IsDrawVertexNormalEnabled())
	{
		mDrawNormalShader.Use();

		glUniform1f(mDrawNormalShader.GetUniform(__uniforms_count__ + u_NormalMagnitude), engine->GetDrawVertexNormalMagnitude());
		glUniform4fv(mDrawNormalShader.GetUniform(__uniforms_count__ + u_VertexNormalColor), 1, glm::value_ptr(engine->GetDrawVertexNormalColor()));

		glBindVertexArray(mVaoID);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());

		glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y * mMapCount);

		glBindVertexArray(0);

		mDrawNormalShader.UnUse();
	}
}


//void DeepOceanRenderer::UpdateMaterialTextureIndex(const Desc & desc)
//{
//	for (MaterialDescList::const_iterator matIt = desc.mLowSlopeMaterials.begin(); matIt != desc.mLowSlopeMaterials.end(); ++matIt)
//	{
//
//	}
//
//	for (TextureMappingList::const_iterator it = mTextureMapping.mMapping.begin(); it != mTextureMapping.mMapping.end(); ++it)
//	{
//		if (it->mTexture->GetCategory() == TextureCategory::Diffuse)
//		{
//			mDiffuseTextures = it->mTexture;
//			break;
//		}			
//	}
//}


	} // namespace Renderers
} // namespace CoreFx