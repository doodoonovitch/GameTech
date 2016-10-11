#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{
		namespace PerlinNoiseOcean
		{
		// =======================================================================
		// =======================================================================



Renderer::Renderer(const Desc & desc)
	: RendererHelper<1>(0, "PerlinNoiseOceanRenderer", "PerlinNoiseOceanWireFrameRenderer", Renderer::ERenderPass::Deferred_Pass)
	, mHeightMapCS(nullptr)
	, mCubeMapTexture(Engine::GetInstance()->GetTextureManager()->LoadTextureCubeMap(desc.mSkyboxCubeMapTextureFilename))
	, mOceanColorTexture(Engine::GetInstance()->GetTextureManager()->LoadTexture2D("medias/textures/OceanColor256.tif", GL_REPEAT, GL_REPEAT))
	//, mOceanColorTexture(Engine::GetInstance()->GetTextureManager()->GetDefaultTexture2D())
	, mHeightMapTextureSize(desc.mHeightMapTextureSize)
	, mMapSize(desc.mMapWidth, desc.mMapDepth)
	, mPatchCount(desc.mMapWidth / 64, desc.mMapDepth / 64)
	, mMapCount(0)
	, mDrawNormalShader("TerrainDrawNormals")
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize PerlinNoiseOceanRenderer.....");

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

	PRINT_MESSAGE(".....PerlinNoiseOceanRenderer initialized!");
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

void Renderer::LoadMainShader(const Desc & desc)
{
	PRINT_MESSAGE("Initialize Perlin Noise Ocean Renderer Shaders : .....");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_HeightMapTextureSize",
		"u_MaxAmplitude",
		"u_HeightMapSampler",
		"u_PerMapDataSampler",
		"u_SkyboxCubeMapSampler",
		"u_textureSampler",
		"u_NormalMapSampler",
	};


	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/HeightFieldOcean.vs.glsl");

	mShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/HeightFieldOcean.tcs.glsl");

	if(desc.mNormalMode==ENormalMode::PerVertexNormal)
	{
		std::vector<std::string> shaderSources;
		shaderSources.push_back("#define PER_VERTEX_NORMAL\n");

		std::string csSource;
		Shader::MergeFile(csSource, "shaders/PerlinNoiseOcean.tes.glsl");
		shaderSources.push_back(csSource);
		mShader.LoadFromString(GL_TESS_EVALUATION_SHADER, shaderSources);
	}
	else
	{
		mShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/PerlinNoiseOcean.tes.glsl");
	}

	//mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/HeightFieldOcean.gs.glsl");

	if (desc.mNormalMode == ENormalMode::PerPixelNormal)
	{
		std::vector<std::string> shaderSources;
		shaderSources.push_back("#define PER_PIXEL_NORMAL\n");

		std::string csSource;
		Shader::MergeFile(csSource, "shaders/HeightFieldOcean.deferred.fs.glsl");
		shaderSources.push_back(csSource);
		mShader.LoadFromString(GL_FRAGMENT_SHADER, shaderSources);
	}
	else
	{
		mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/HeightFieldOcean.deferred.fs.glsl");
	}

	mShader.CreateAndLinkProgram();
	mShader.Use();

	mShader.AddUniforms(uniformNames, __uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform2iv(mShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2iv(mShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
	glUniform2iv(mShader.GetUniform(u_HeightMapTextureSize), 1, glm::value_ptr(mHeightMapTextureSize)); GL_CHECK_ERRORS;	
	glUniform1f(mShader.GetUniform(u_MaxAmplitude), mHeightMapCS->GetMaxAmplitude()); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_HeightMapSampler), 0); GL_CHECK_ERRORS;
	//glUniform1i(mShader.GetUniform(u_PerMapDataSampler), 1); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_SkyboxCubeMapSampler), 2); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_textureSampler), 3); GL_CHECK_ERRORS;
	glUniform1i(mWireFrameShader.GetUniform(u_NormalMapSampler), 1); GL_CHECK_ERRORS;

	mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....done.");
}

void Renderer::LoadWireFrameShader(const Desc & /*desc*/)
{
	PRINT_MESSAGE("Initialize Perlin Noise Ocean Renderer (Wire Frame) Shaders : .....");

	const char * uniformNames[] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_HeightMapTextureSize",
		"u_MaxAmplitude",
		"u_HeightMapSampler",
		"u_PerMapDataSampler",
	};


	//setup shader
	mWireFrameShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/HeightFieldOcean.vs.glsl");

	mWireFrameShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/HeightFieldOcean.tcs.glsl");

	mWireFrameShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/PerlinNoiseOcean.tes.glsl");

	//mWireFrameShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/HeightFieldOcean.gs.glsl");

	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/HeightFieldOcean.wireframe.fs.glsl");

	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();

	mWireFrameShader.AddUniforms(uniformNames, 6);

	//pass values of constant uniforms at initialization
	glUniform2iv(mWireFrameShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2iv(mWireFrameShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
	glUniform2iv(mShader.GetUniform(u_HeightMapTextureSize), 1, glm::value_ptr(mHeightMapTextureSize)); GL_CHECK_ERRORS;
	glUniform1f(mShader.GetUniform(u_MaxAmplitude), mHeightMapCS->GetMaxAmplitude()); GL_CHECK_ERRORS;
	glUniform1i(mWireFrameShader.GetUniform(u_HeightMapSampler), 0); GL_CHECK_ERRORS;
	glUniform1i(mWireFrameShader.GetUniform(u_PerMapDataSampler), 1); GL_CHECK_ERRORS;

	//GetWavePropertyUniformIndex(mWireFrameShader, mWireFrameShaderWaveProps);

	mWireFrameShader.SetupFrameDataBlockBinding();
	mWireFrameShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....done.");
}

void Renderer::LoadHeightMapComputeShader(const Desc & desc)
{
	PRINT_MESSAGE("Initialize Perlin Noise Ocean Renderer (Height Map Compute) Shaders : .....");

	mHeightMapCS = new HeightMapCS();
	glm::vec2 scale = glm::vec2((GLfloat)mHeightMapTextureSize.x / (GLfloat)mMapSize.x, (GLfloat)mHeightMapTextureSize.y / (GLfloat)mMapSize.y);
	mHeightMapCS->LoadShader(desc.mWaveProps, mHeightMapTextureSize, scale);

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

			//glActiveTexture(GL_TEXTURE1);
			//glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());
	
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(mCubeMapTexture->GetTarget(), mCubeMapTexture->GetResourceId());

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, mOceanColorTexture->GetResourceId());

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mHeightMapCS->GetNormalMapTextureId());

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

		} // namespace PerlinNoiseOcean
	} // namespace Renderers
} // namespace CoreFx