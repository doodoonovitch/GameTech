#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



DeepOceanRenderer::DeepOceanRenderer(const Desc & desc)
	: RendererHelper<1>(0, "DeepOceanRenderer", "DeepOceanWireFrameRenderer")
	, mCubeMapTexture(Engine::GetInstance()->GetTextureManager()->LoadTextureCubeMap(desc.mSkyboxCubeMapTextureFilename))
	, mMapSize(desc.mMapWidth, desc.mMapDepth)
	, mPatchCount(desc.mMapWidth / 64, desc.mMapDepth / 64)
	, mScale(desc.mScale)
	, mMapCount(0)
	, mDrawNormalShader("TerrainDrawNormals")
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize DeepOceanRenderer.....");

	memset(mShaderWaveProps.mWavePropModified, -1, sizeof(mShaderWaveProps.mWavePropModified));
	memset(mWireFrameShaderWaveProps.mWavePropModified, -1, sizeof(mWireFrameShaderWaveProps.mWavePropModified));
	memcpy(mWaveProps, desc.mWaveProps, sizeof(mWaveProps));

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

	glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[0]);
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


DeepOceanRenderer::~DeepOceanRenderer()
{
	mModelMatrixBuffer.ReleaseResource();
}

void DeepOceanRenderer::LoadShaders(const Desc & desc)
{
	LoadMainShader(desc);
	LoadWireFrameShader(desc);
}

void DeepOceanRenderer::LoadMainShader(const Desc & /*desc*/)
{
	PRINT_MESSAGE("Initialize Deep Ocean Renderer Shaders : .....");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_PerMapDataSampler",
		"u_SkyboxCubeMapSampler",
		"u_textureSampler",
	};


	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/deepOcean.vs.glsl");

	mShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/deepOcean.tcs.glsl");

	mShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/deepOcean.tes.glsl");

	//mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/deepOcean.gs.glsl");

	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/deepOcean.deferred.fs.glsl");

	mShader.CreateAndLinkProgram();
	mShader.Use();

	mShader.AddUniforms(uniformNames, __uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform2iv(mShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2iv(mShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
	glUniform3fv(mShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_PerMapDataSampler), 0); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_SkyboxCubeMapSampler), 1); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_textureSampler), 2); GL_CHECK_ERRORS;

	GetWavePropertyUniformIndex(mShader, mShaderWaveProps);

	mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....done.");
}

void DeepOceanRenderer::LoadWireFrameShader(const Desc & /*desc*/)
{
	PRINT_MESSAGE("Initialize Deep Ocean Renderer (Wire Frame) Shaders : .....");

	const char * uniformNames[] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_PerMapDataSampler",
	};


	//setup shader
	mWireFrameShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/deepOcean.vs.glsl");

	mWireFrameShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/deepOcean.tcs.glsl");

	mWireFrameShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/deepOcean.tes.glsl");

	//mWireFrameShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/deepOcean.gs.glsl");

	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/deepOcean.wireframe.fs.glsl");

	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();

	mWireFrameShader.AddUniforms(uniformNames, 4);

	//pass values of constant uniforms at initialization
	glUniform2iv(mWireFrameShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2iv(mWireFrameShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
	glUniform3fv(mWireFrameShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;
	glUniform1i(mWireFrameShader.GetUniform(u_PerMapDataSampler), 0); GL_CHECK_ERRORS;

	GetWavePropertyUniformIndex(mWireFrameShader, mWireFrameShaderWaveProps);

	mWireFrameShader.SetupFrameDataBlockBinding();
	mWireFrameShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....done.");
}

void DeepOceanRenderer::GetWavePropertyUniformIndex(Shader & shader, WavePropUniformIndex & waveProps)
{
	for (int i = 0; i < MAX_WAVE_TO_SUM; ++i)
	{
		char uniformName[50];
		sprintf_s(uniformName, 50, "u_Direction[%i]", i);
		waveProps.mDirectionUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.", uniformName, waveProps.mDirectionUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_WaveLength[%i]", i);
		waveProps.mWaveLengthUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.", uniformName, waveProps.mWaveLengthUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Frequency[%i]", i);
		waveProps.mFrequencyUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.", uniformName, waveProps.mFrequencyUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Amplitude[%i]", i);
		waveProps.mAmplitudeUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.", uniformName, waveProps.mAmplitudeUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Velocity[%i]", i);
		waveProps.mVelocityUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.", uniformName, waveProps.mVelocityUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Steepness[%i]", i);
		waveProps.mSteepnessUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.", uniformName, waveProps.mSteepnessUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Phase[%i]", i);
		waveProps.mPhaseUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.", uniformName, waveProps.mPhaseUniformIndex[i]);
	}
}

void DeepOceanRenderer::SetWavePropertyUniformValues(WavePropUniformIndex & waveProps)
{
	for (int i = 0; i < MAX_WAVE_TO_SUM; ++i)
	{
		bool phaseModified = false;
		if ((waveProps.mWavePropModified[i] & E_WaveProps_Direction_modified) == E_WaveProps_Direction_modified)
		{
			glUniform3fv(waveProps.mDirectionUniformIndex[i], 1, glm::value_ptr(mWaveProps[i].mDirection));
		}
		if ((waveProps.mWavePropModified[i] & E_WaveProps_WaveLength_modified) == E_WaveProps_WaveLength_modified)
		{
			//glUniform1f(mWaveLengthUniformIndex[i], mWaveProps[i].mWaveLength);
			glUniform1f(waveProps.mFrequencyUniformIndex[i], mWaveProps[i].mFrequency);
			phaseModified = true;
		}
		if ((waveProps.mWavePropModified[i] & E_WaveProps_Amplitude_modified) == E_WaveProps_Amplitude_modified)
		{
			glUniform1f(waveProps.mAmplitudeUniformIndex[i], mWaveProps[i].mAmplitude);
		}
		if ((waveProps.mWavePropModified[i] & E_WaveProps_Velocity_modified) == E_WaveProps_Velocity_modified)
		{
			//glUniform1f(mVelocityUniformIndex[i], mWaveProps[i].mVelocity);
			phaseModified = true;
		}
		if ((waveProps.mWavePropModified[i] & E_WaveProps_Steepness_modified) == E_WaveProps_Steepness_modified)
		{
			glUniform1f(waveProps.mSteepnessUniformIndex[i], mWaveProps[i].mSteepness);
		}
		if (phaseModified)
		{
			GLfloat phase = mWaveProps[i].mVelocity * mWaveProps[i].mFrequency;
			glUniform1f(waveProps.mPhaseUniformIndex[i], phase);
		}

		if (waveProps.mWavePropModified[i] != 0)
			waveProps.mWavePropModified[i] = 0;
	}
}
 
void DeepOceanRenderer::Render()
{
	mShader.Use();
		glBindVertexArray(mVaoID);

			SetWavePropertyUniformValues(mShaderWaveProps);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());
	
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(mCubeMapTexture->GetTarget(), mCubeMapTexture->GetResourceId());

			//glActiveTexture(GL_TEXTURE2);
			//glBindTexture(GL_TEXTURE_2D, Engine::GetInstance()->GetTextureManager()->GetDefaultTexture2D()->GetResourceId());

			glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y * mMapCount);

		glBindVertexArray(0);
	mShader.UnUse();
}

void DeepOceanRenderer::RenderWireFrame()
{
	mWireFrameShader.Use();
		glBindVertexArray(mVaoID);

			SetWavePropertyUniformValues(mWireFrameShaderWaveProps);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());

			glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y * mMapCount);

		glBindVertexArray(0);
	mWireFrameShader.UnUse();
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