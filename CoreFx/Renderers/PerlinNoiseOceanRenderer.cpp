#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



PerlinNoiseOceanRenderer::PerlinNoiseOceanRenderer(const Desc & desc)
	: RendererHelper<3>(0, "PerlinNoiseOceanRenderer", "PerlinNoiseOceanWireFrameRenderer")
	, mCubeMapTexture(Engine::GetInstance()->GetTextureManager()->LoadTextureCubeMap(desc.mSkyboxCubeMapTextureFilename))
	, mNoiseHeightTexture(Engine::GetInstance()->GetTextureManager()->LoadTexture2D("Medias/Textures/noise.tif", GL_REPEAT, GL_REPEAT))
	, mWaveProps(nullptr)
	, mWaveCount(0)
	, mTextureSize(512, 512)
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

	mWaveCount = MAX_WAVE_TO_SUM;
	assert(mWaveCount > 0);
	mWaveProps = new HMapCSParam[mWaveCount];
	for (int i = 0; i < mWaveCount; ++i)
	{
		const WaveProps & src = desc.mWaveProps[i];
		HMapCSParam & trg = mWaveProps[i];
		trg.m_Amplitude = src.mAmplitude;
		trg.m_Direction.x = src.mDirection.x;
		trg.m_Direction.y = src.mDirection.z;
		trg.m_Velocity = src.mVelocity;
		trg.m_WaveLength = src.mWaveLength;
	}

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

	glBindBuffer(GL_UNIFORM_BUFFER, mVboIDs[WavePropsBufferIndex]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(HMapCSParam) * mWaveCount, mWaveProps, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	mMapCount = (GLint)desc.mMaps.size();

	PerMapData * modelMatrixBuffer = new PerMapData[mMapCount];
	for (int i = 0; i < mMapCount; ++i)
	{
		modelMatrixBuffer[i].mModelDQ = desc.mMaps[i].mModelDQ;
	}
	mModelMatrixBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, (mMapCount * sizeof(PerMapData)), modelMatrixBuffer);

	glBindTexture(GL_TEXTURE_2D, mVboIDs[HeightMapBufferIndex]);
	TextureManager::CreateTexStorage2D(GL_TEXTURE_2D, 256, 256, nullptr, false, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_R32F, GL_RED, GL_FLOAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	LoadShaders(desc);

	mIsInitialized = true;

	PRINT_MESSAGE(".....PerlinNoiseOceanRenderer initialized!");
	PRINT_END_SECTION;
}


PerlinNoiseOceanRenderer::~PerlinNoiseOceanRenderer()
{
	mModelMatrixBuffer.ReleaseResource();
}

void PerlinNoiseOceanRenderer::LoadShaders(const Desc & desc)
{
	LoadMainShader(desc);
	LoadWireFrameShader(desc);
}

void PerlinNoiseOceanRenderer::LoadHMapComputeShader(const Desc & /*desc*/)
{
	PRINT_MESSAGE("Initialize Perlin Noise Ocean Renderer (HMap compute) Shaders : .....");
	
	mHMapCompShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/PerlinNoiseOcean.hmap.cs.glsl");

	mShader.CreateAndLinkProgram();
	mHMapCompShader.Use();

	glBindImageTexture((GLuint)EHMapCSUniforms::u_ImageOut, mVboIDs[HeightMapBufferIndex], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
	glBindImageTexture((GLuint)EHMapCSUniforms::u_ImageIn, mNoiseHeightTexture->GetResourceId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);

	glBindBufferBase(GL_UNIFORM_BUFFER, (GLuint)EHMapCSUniforms::u_WaveParamsBlock, mVboIDs[WavePropsBufferIndex]);
	glUniform1i((GLuint)EHMapCSUniforms::u_WaveCount, mWaveCount); GL_CHECK_ERRORS;
	glUniform2iv((GLuint)EHMapCSUniforms::u_TextureSize, 1, glm::value_ptr(mTextureSize)); GL_CHECK_ERRORS;

	mHMapCompShader.UnUse();

	PRINT_MESSAGE(".....done.");
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

	//GetWavePropertyUniformIndex(mShader, mShaderWaveProps);

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

//void PerlinNoiseOceanRenderer::GetWavePropertyUniformIndex(Shader & shader, WavePropUniformIndex & waveProps)
//{
//	for (int i = 0; i < MAX_WAVE_TO_SUM; ++i)
//	{
//		char uniformName[50];
//		sprintf_s(uniformName, 50, "u_Direction[%i]", i);
//		waveProps.mDirectionUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
//		PRINT_MESSAGE("\t%s : %i.", uniformName, waveProps.mDirectionUniformIndex[i]);
//
//		sprintf_s(uniformName, 50, "u_WaveLength[%i]", i);
//		waveProps.mWaveLengthUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
//		PRINT_MESSAGE("\t%s : %i.", uniformName, waveProps.mWaveLengthUniformIndex[i]);
//
//		sprintf_s(uniformName, 50, "u_Amplitude[%i]", i);
//		waveProps.mAmplitudeUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
//		PRINT_MESSAGE("\t%s : %i.", uniformName, waveProps.mAmplitudeUniformIndex[i]);
//
//		sprintf_s(uniformName, 50, "u_Velocity[%i]", i);
//		waveProps.mVelocityUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
//		PRINT_MESSAGE("\t%s : %i.", uniformName, waveProps.mVelocityUniformIndex[i]);
//	}
//}

//void PerlinNoiseOceanRenderer::SetWavePropertyUniformValues(WavePropUniformIndex & waveProps)
//{
//	for (int i = 0; i < MAX_WAVE_TO_SUM; ++i)
//	{
//		if ((waveProps.mWavePropModified[i] & E_WaveProps_Direction_modified) == E_WaveProps_Direction_modified)
//		{
//			glUniform3fv(waveProps.mDirectionUniformIndex[i], 1, glm::value_ptr(mWaveProps[i].mDirection));
//		}
//
//		if ((waveProps.mWavePropModified[i] & E_WaveProps_WaveLength_modified) == E_WaveProps_WaveLength_modified)
//		{
//			glUniform1f(waveProps.mWaveLengthUniformIndex[i], mWaveProps[i].mWaveLength);
//		}
//
//		if ((waveProps.mWavePropModified[i] & E_WaveProps_Amplitude_modified) == E_WaveProps_Amplitude_modified)
//		{
//			glUniform1f(waveProps.mAmplitudeUniformIndex[i], mWaveProps[i].mAmplitude);
//		}
//
//		if ((waveProps.mWavePropModified[i] & E_WaveProps_Velocity_modified) == E_WaveProps_Velocity_modified)
//		{
//			glUniform1f(waveProps.mVelocityUniformIndex[i], mWaveProps[i].mVelocity);
//		}
//
//		waveProps.mWavePropModified[i] = 0;
//	}
//}

void PerlinNoiseOceanRenderer::GenerateHeightMap()
{
	mHMapCompShader.Use();

	//glBindImageTexture((GLuint)EHMapCSUniforms::u_ImageOut, mVboIDs[HeightMapBufferIndex], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
	//glBindImageTexture((GLuint)EHMapCSUniforms::u_ImageIn, mNoiseHeightTexture->GetResourceId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);
	glUniform1f((GLuint)EHMapCSUniforms::u_Time, Engine::GetInstance()->GetTime()); GL_CHECK_ERRORS;

	glDispatchCompute(mTextureSize.x / 64, mTextureSize.y / 64, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	mHMapCompShader.UnUse();
}
 
void PerlinNoiseOceanRenderer::Render()
{
	mShader.Use();
		glBindVertexArray(mVaoID);

			//SetWavePropertyUniformValues(mShaderWaveProps);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mVboIDs[HeightMapBufferIndex]);

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
			glBindTexture(GL_TEXTURE_2D, mVboIDs[HeightMapBufferIndex]);

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