#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



PerlinNoiseOceanRenderer::PerlinNoiseOceanRenderer(const Desc & desc)
	: RendererHelper<2>(0, "PerlinNoiseOceanRenderer", "PerlinNoiseOceanWireFrameRenderer")
	, mCubeMapTexture(Engine::GetInstance()->GetTextureManager()->LoadTextureCubeMap(desc.mSkyboxCubeMapTextureFilename))
	, mNoiseHeightTexture(Engine::GetInstance()->GetTextureManager()->LoadTexture2D("Medias/Textures/noise.tif", GL_REPEAT, GL_REPEAT))
	, mWaveProps(nullptr)
	, mWaveCount(0)
	, mHeightMapTextureId(0)
	, mTextureSize(512)
	, mMapSize(desc.mMapWidth, desc.mMapDepth)
	, mPatchCount(desc.mMapWidth / 64, desc.mMapDepth / 64)
	, mScale(desc.mScale)
	, mMapCount(0)
	, mDrawNormalShader("TerrainDrawNormals")
	, mHMapCompShader("ComputeHMap")
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize PerlinNoiseOceanRenderer.....");

	//memset(mShaderWaveProps.mWavePropModified, 0xFFFF, sizeof(mShaderWaveProps.mWavePropModified));
	//memset(mWireFrameShaderWaveProps.mWavePropModified, 0xFFFF, sizeof(mWireFrameShaderWaveProps.mWavePropModified));
	//memcpy(mWaveProps, desc.mWaveProps, sizeof(mWaveProps));

	const glm::vec3 vertices[] =
	{
		glm::vec3(0.0f,	0.0f, 0.0f),
		glm::vec3(1.0f,	0.0f, 0.0f),
		glm::vec3(0.0f,	0.0f, 1.0f),
		glm::vec3(1.0f,	0.0f, 1.0f)
	};
		

	//setup vao and vbo stuff
	CreateBuffers();
	//glGenVertexArrays(1, &mVaoID);
	//glGenBuffers(mVboCount, mVboIDs);

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


PerlinNoiseOceanRenderer::~PerlinNoiseOceanRenderer()
{
	SafeDeleteArray(mWaveProps);
	glDeleteTextures(1, &mHeightMapTextureId);
	mModelMatrixBuffer.ReleaseResource();
}

void PerlinNoiseOceanRenderer::LoadShaders(const Desc & desc)
{
	LoadHMapComputeShader(desc);
	LoadMainShader(desc);
	LoadWireFrameShader(desc);
}

void PerlinNoiseOceanRenderer::LoadMainShader(const Desc & /*desc*/)
{
	PRINT_MESSAGE("Initialize Perlin Noise Ocean Renderer Shaders : .....");

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
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/PerlinNoiseOcean.vs.glsl");

	mShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/PerlinNoiseOcean.tcs.glsl");

	mShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/PerlinNoiseOcean.tes.glsl");

	//mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/PerlinNoiseOcean.gs.glsl");

	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/PerlinNoiseOcean.deferred.fs.glsl");

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

void PerlinNoiseOceanRenderer::LoadWireFrameShader(const Desc & /*desc*/)
{
	PRINT_MESSAGE("Initialize Perlin Noise Ocean Renderer (Wire Frame) Shaders : .....");

	const char * uniformNames[] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_HeightMapSampler",
		"u_PerMapDataSampler",
	};


	//setup shader
	mWireFrameShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/PerlinNoiseOcean.vs.glsl");

	mWireFrameShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/PerlinNoiseOcean.tcs.glsl");

	mWireFrameShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/PerlinNoiseOcean.tes.glsl");

	//mWireFrameShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/PerlinNoiseOcean.gs.glsl");

	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/PerlinNoiseOcean.wireframe.fs.glsl");

	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();

	mWireFrameShader.AddUniforms(uniformNames, 5);

	//pass values of constant uniforms at initialization
	glUniform2iv(mWireFrameShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2iv(mWireFrameShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
	glUniform3fv(mWireFrameShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;
	glUniform1i(mWireFrameShader.GetUniform(u_HeightMapSampler), 0); GL_CHECK_ERRORS;
	glUniform1i(mWireFrameShader.GetUniform(u_PerMapDataSampler), 1); GL_CHECK_ERRORS;

	//GetWavePropertyUniformIndex(mWireFrameShader, mWireFrameShaderWaveProps);

	mWireFrameShader.SetupFrameDataBlockBinding();
	mWireFrameShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....done.");
}

void PerlinNoiseOceanRenderer::LoadHMapComputeShader(const Desc & desc)
{
	PRINT_MESSAGE("Initialize Perlin Noise Ocean Renderer (Height Map Compute) Shaders : .....");

	mWaveCount = MAX_WAVE_TO_SUM;
	assert(mWaveCount > 0);
	GLsizei wavePropsBufferItemCount = mWaveCount * __waveparams_count__;
	GLsizei wavePropsBufferSize = wavePropsBufferItemCount * sizeof(GLfloat);

	mWaveProps = new GLfloat[wavePropsBufferItemCount];
	memset(mWaveProps, 0, wavePropsBufferSize);
	for (int i = 0; i < mWaveCount; ++i)
	{
		const WaveProps & src = desc.mWaveProps[i];
		GLfloat * trg = &mWaveProps[i * __waveparams_count__];
		trg[WaveParams_Amplitude] = src.mAmplitude;
		trg[WaveParams_DirectionX] = src.mDirection.x;
		trg[WaveParams_DirectionY] = src.mDirection.z;
		trg[WaveParams_Velocity] = src.mVelocity;
		trg[WaveParams_WaveLength] = src.mWaveLength;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mVboIDs[WavePropsBufferIndex]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, wavePropsBufferSize, mWaveProps, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenTextures(1, &mHeightMapTextureId);
	glBindTexture(GL_TEXTURE_2D, mHeightMapTextureId);
	TextureManager::CreateTexStorage2D(GL_TEXTURE_2D, mTextureSize.x, mTextureSize.y, nullptr, false, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_R32F, GL_RED, GL_FLOAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ---------------------------------------

	mHMapCompShader.LoadFromFile(GL_COMPUTE_SHADER, "shaders/PerlinNoiseOcean.hmap.cs.glsl", Shader::EInclude::ComputeShadersCommon);

	mHMapCompShader.CreateAndLinkProgram();
	mHMapCompShader.Use();

	const char * uniformNames[__hmap_cs_uniforms_count__] =
	{
		"u_WaveCount",
		"u_TextureSize",
		"u_Time",
		"u_NoiseSampler"
	};

	mHMapCompShader.AddUniforms(uniformNames, __hmap_cs_uniforms_count__);

	glUniform1i(mHMapCompShader.GetUniform(u_WaveCount), mWaveCount); GL_CHECK_ERRORS;
	glUniform2iv(mHMapCompShader.GetUniform(u_TextureSize), 1, glm::value_ptr(mTextureSize)); GL_CHECK_ERRORS;
	glUniform1i(mHMapCompShader.GetUniform(u_NoiseSampler), 0); GL_CHECK_ERRORS;

	mHMapCompShader.UnUse();

	PRINT_MESSAGE("Height map texture id : %li", mHeightMapTextureId);
	PRINT_MESSAGE("Wave props buffer id : %li", mVboIDs[WavePropsBufferIndex]);

	PRINT_MESSAGE(".....done.");
}

void PerlinNoiseOceanRenderer::GenerateHeightMap()
{
	mHMapCompShader.Use();

		glUniform1f(mHMapCompShader.GetUniform(u_Time), Engine::GetInstance()->GetTime()); GL_CHECK_ERRORS;

		glBindImageTexture((GLuint)EHMapCSBindings::u_ImageOut, mHeightMapTextureId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (GLuint)EHMapCSBindings::u_WaveParamsBlock, mVboIDs[WavePropsBufferIndex]); GL_CHECK_ERRORS;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mNoiseHeightTexture->GetResourceId());

		glDispatchCompute(mTextureSize.x /*/ 32*/, mTextureSize.y /*/ 32*/, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	mHMapCompShader.UnUse();
}
 
void PerlinNoiseOceanRenderer::Render()
{
	GenerateHeightMap();

	mShader.Use();
		glBindVertexArray(mVaoID);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mHeightMapTextureId);

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

void PerlinNoiseOceanRenderer::RenderWireFrame()
{
	mWireFrameShader.Use();
		glBindVertexArray(mVaoID);

			//SetWavePropertyUniformValues(mWireFrameShaderWaveProps);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mHeightMapTextureId);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());

			glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y * mMapCount);

		glBindVertexArray(0);
	mWireFrameShader.UnUse();
}


//void PerlinNoiseOceanRenderer::UpdateMaterialTextureIndex(const Desc & desc)
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