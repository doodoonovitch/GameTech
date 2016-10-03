#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Renderers
	{
		namespace DeepOcean
		{
		// =======================================================================
		// =======================================================================



Renderer::Renderer(const Desc & desc)
	: RendererHelper<1>(0, "DeepOceanRenderer", "DeepOceanWireFrameRenderer", Renderer::ERenderPass::Deferred_Pass)
	, mHeightMapCS(nullptr)
	, mCubeMapTexture(Engine::GetInstance()->GetTextureManager()->LoadTextureCubeMap(desc.mSkyboxCubeMapTextureFilename))
	, mTextureSize(512)
	, mMapSize(desc.mMapWidth, desc.mMapDepth)
	, mPatchCount(desc.mMapWidth / 64, desc.mMapDepth / 64)
	, mScale(desc.mScale)
	, mMapCount(0)
	, mDrawNormalShader("WavesDrawNormals")
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize DeepOceanRenderer.....");

	const glm::vec3 vertices[] =
	{
		glm::vec3(0.0f,	0.0f, 0.0f),
		glm::vec3(1.0f,	0.0f, 0.0f),
		glm::vec3(0.0f,	0.0f, 1.0f),
		glm::vec3(1.0f,	0.0f, 1.0f)
	};
		

	//setup vao and vbo stuff
	CreateBuffers();

	glBindVertexArray(mVaoID);

	glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[VertexArrayBufferIndex]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
	glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	GL_CHECK_ERRORS;

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	mMapCount = (GLint)desc.mMaps.size();

	PerMapData * modelMatrixBuffer = new PerMapData[mMapCount];
	for (int i = 0; i < mMapCount; ++i)
	{
		modelMatrixBuffer[i].mModelDQ = desc.mMaps[i].mModelDQ;
	}
	mModelMatrixBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, (mMapCount * sizeof(PerMapData)), modelMatrixBuffer);

	LoadShaders(desc);

	mIsInitialized = true;

	PRINT_MESSAGE(".....DeepOceanRenderer initialized!");
	PRINT_END_SECTION;
}


Renderer::~Renderer()
{
	Engine::GetInstance()->DetachComputeShader(mHeightMapCS);
	SafeDelete(mHeightMapCS);
	mModelMatrixBuffer.ReleaseResource();
}

void Renderer::LoadShaders(const Desc & desc)
{
	LoadHeightMapComputeShader(desc);
	LoadMainShader(desc);
	LoadWireFrameShader(desc);
}

void Renderer::LoadTessellationShader(Shader & shader, GLenum whichShader, const std::string & filename, const Desc & desc)
{
	std::vector<std::string> shaderSources;
	if (desc.mPrecomputeNormals)
	{
		shaderSources.push_back("#define PRECOMPUTE_NORMAL\n");
	}

	std::string csSource;
	Shader::MergeFile(csSource, filename);
	shaderSources.push_back(csSource);
	shader.LoadFromString(whichShader, shaderSources);
}

void Renderer::LoadMainShader(const Desc & desc)
{
	PRINT_MESSAGE("Initialize Deep Ocean Renderer Shaders : .....");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_HeightMapSampler",
		"u_PerMapDataSampler",
		"u_SkyboxCubeMapSampler",
		"u_textureSampler",
	};


	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/HeightFieldOcean.vs.glsl");

	mShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/HeightFieldOcean.tcs.glsl");

	LoadTessellationShader(mShader, GL_TESS_EVALUATION_SHADER, "shaders/DeepOcean.tes.glsl", desc);

	//mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/HeightFieldOcean.gs.glsl");

	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/HeightFieldOcean.deferred.fs.glsl");

	mShader.CreateAndLinkProgram();
	mShader.Use();

	mShader.AddUniforms(uniformNames, __uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform2iv(mShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2iv(mShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
	glUniform3fv(mShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_HeightMapSampler), 0); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_PerMapDataSampler), 1); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_SkyboxCubeMapSampler), 2); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_textureSampler), 3); GL_CHECK_ERRORS;

	mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....done.");
}

void Renderer::LoadWireFrameShader(const Desc & desc)
{
	PRINT_MESSAGE("Initialize Deep Ocean Renderer (Wire Frame) Shaders : .....");

	const char * uniformNames[] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_HeightMapSampler",
		"u_PerMapDataSampler",
	};


	//setup shader
	mWireFrameShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/HeightFieldOcean.vs.glsl");

	mWireFrameShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/HeightFieldOcean.tcs.glsl");

	LoadTessellationShader(mWireFrameShader, GL_TESS_EVALUATION_SHADER, "shaders/DeepOcean.tes.glsl", desc);

	//mWireFrameShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/HeightFieldOcean.gs.glsl");

	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/HeightFieldOcean.wireframe.fs.glsl");

	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();

	mWireFrameShader.AddUniforms(uniformNames, 5);

	//pass values of constant uniforms at initialization
	glUniform2iv(mWireFrameShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2iv(mWireFrameShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
	glUniform3fv(mWireFrameShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;
	glUniform1i(mWireFrameShader.GetUniform(u_HeightMapSampler), 0); GL_CHECK_ERRORS;
	glUniform1i(mWireFrameShader.GetUniform(u_PerMapDataSampler), 1); GL_CHECK_ERRORS;

	mWireFrameShader.SetupFrameDataBlockBinding();
	mWireFrameShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....done.");
}

void Renderer::LoadHeightMapComputeShader(const Desc & desc)
{
	PRINT_MESSAGE("Initialize Deep Ocean Renderer (Height Map Compute) Shaders : .....");

	mHeightMapCS = new HeightMapCS(desc.mPrecomputeNormals);
	mHeightMapCS->LoadShader(desc.mWaveProps, mTextureSize);

	Engine::GetInstance()->AttachComputeShader(mHeightMapCS);

	PRINT_MESSAGE(".....done.");
}

void Renderer::Render()
{
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	mShader.Use();
		glBindVertexArray(mVaoID);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mHeightMapCS->GetHeightMapTextureId());

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());
	
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(mCubeMapTexture->GetTarget(), mCubeMapTexture->GetResourceId());

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, Engine::GetInstance()->GetTextureManager()->GetDefaultTexture2D()->GetResourceId());

			glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y * mMapCount);

		glBindVertexArray(0);
	mShader.UnUse();
}

void Renderer::RenderWireFrame()
{
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	mWireFrameShader.Use();
		glBindVertexArray(mVaoID);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mHeightMapCS->GetHeightMapTextureId());

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());

			glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y * mMapCount);

		glBindVertexArray(0);
	mWireFrameShader.UnUse();
}


//void Renderer::UpdateMaterialTextureIndex(const Desc & desc)
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

		} // namespace DeepOcean
	} // namespace Renderers
} // namespace CoreFx