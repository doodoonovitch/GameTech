#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



ShallowWaterRenderer::ShallowWaterRenderer(const Desc & desc)
	: RendererHelper<1>(0, "ShallowWaterRenderer")
	, mCubeMapTexture(Engine::GetInstance()->GetTextureManager()->LoadTextureCubeMap(desc.mSkyboxCubeMapTextureFilename))
	, mMapSize(desc.mMapWidth, desc.mMapDepth)
	, mPatchCount(desc.mMapWidth / 64, desc.mMapDepth / 64)
	, mScale(desc.mScale)
	, mMapCount(0)
	, mDrawNormalShader("TerrainDrawNormals")
{
	std::cout << std::endl;
	std::cout << "Initialize ShallowWaterRenderer...." << std::endl;

	memset(mShaderWaveProps.mWavePropModified, -1, sizeof(mShaderWaveProps.mWavePropModified));
	memset(mDebugShaderWaveProps.mWavePropModified, -1, sizeof(mDebugShaderWaveProps.mWavePropModified));
	memcpy(mWaveProps, desc.mWaveProps, sizeof(mWaveProps));

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

	mMapCount = (GLint)desc.mMaps.size();

	PerMapData * modelMatrixBuffer = new PerMapData[mMapCount];
	for (int i = 0; i < mMapCount; ++i)
	{
		modelMatrixBuffer[i].mModelDQ = desc.mMaps[i].mModelDQ;
	}
	mModelMatrixBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, (mMapCount * sizeof(PerMapData)), modelMatrixBuffer);

	LoadShaders(desc);

	std::cout << "... ShallowWaterRenderer initialized!" << std::endl << std::endl;
}


ShallowWaterRenderer::~ShallowWaterRenderer()
{
	mModelMatrixBuffer.ReleaseResource();
}

void ShallowWaterRenderer::LoadShaders(const Desc & /*desc*/)
{
	PRINT_MESSAGE("Initialize Terrain Renderer Shaders : \n\n");

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



	//const char * uniformNames2[__uniforms2_count__] =
	//{
	//	"u_NormalMagnitude",
	//	"u_VertexNormalColor",
	//};

	//mDrawNormalShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/deepOcean.vs.glsl");
	//mDrawNormalShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/deepOcean.tcs.glsl");
	//mDrawNormalShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/deepOcean.tes.glsl");
	//mDrawNormalShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/deepOcean_vertex_normal.gs.glsl");
	//mDrawNormalShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/vertex_normal.fs.glsl");
	//mDrawNormalShader.CreateAndLinkProgram();
	//mDrawNormalShader.Use();

	//mDrawNormalShader.AddUniforms(uniformNames, __uniforms_count__);
	//mDrawNormalShader.AddUniforms(uniformNames2, __uniforms2_count__);

	////pass values of constant uniforms at initialization
	//glUniform2iv(mDrawNormalShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	//glUniform2iv(mDrawNormalShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
	//glUniform3fv(mDrawNormalShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;

	//glUniform1i(mDrawNormalShader.GetUniform(u_PerMapDataSampler), 0); GL_CHECK_ERRORS;

	//GetWavePropertyUniformIndex(mDrawNormalShader, mDebugShaderWaveProps);

	//mDrawNormalShader.SetupFrameDataBlockBinding();
	//mDrawNormalShader.UnUse();

	//GL_CHECK_ERRORS;

	PRINT_MESSAGE("... done.\n");
	PRINT_MESSAGE("-------------------------------------------------\n\n");
}

void ShallowWaterRenderer::GetWavePropertyUniformIndex(Shader & shader, WavePropUniformIndex & waveProps)
{

	for (int i = 0; i < MAX_WAVE_TO_SUM; ++i)
	{
		char uniformName[50];
		sprintf_s(uniformName, 50, "u_Direction[%i]", i);
		waveProps.mDirectionUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, waveProps.mDirectionUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_WaveLength[%i]", i);
		waveProps.mWaveLengthUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, waveProps.mWaveLengthUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Frequency[%i]", i);
		waveProps.mFrequencyUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, waveProps.mFrequencyUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Amplitude[%i]", i);
		waveProps.mAmplitudeUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, waveProps.mAmplitudeUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Velocity[%i]", i);
		waveProps.mVelocityUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, waveProps.mVelocityUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Steepness[%i]", i);
		waveProps.mSteepnessUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, waveProps.mSteepnessUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Phase[%i]", i);
		waveProps.mPhaseUniformIndex[i] = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, waveProps.mPhaseUniformIndex[i]);
	}
}

void ShallowWaterRenderer::SetWavePropertyUniformValues(WavePropUniformIndex & waveProps)
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
 
void ShallowWaterRenderer::Render()
{
	mShader.Use();
	glBindVertexArray(mVaoID);

	SetWavePropertyUniformValues(mShaderWaveProps);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mCubeMapTexture->GetResourceId());

	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, Engine::GetInstance()->GetTextureManager()->GetDefaultTexture2D()->GetResourceId());

	glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y * mMapCount);

	glBindVertexArray(0);
	mShader.UnUse();

}

void ShallowWaterRenderer::DebugRender()
{
	//Engine * engine = Engine::GetInstance();

	//if (engine->IsDrawVertexNormalEnabled())
	//{
	//	mDrawNormalShader.Use();

	//	SetWavePropertyUniformValues(mDebugShaderWaveProps);

	//	glUniform1f(mDrawNormalShader.GetUniform(__uniforms_count__ + u_NormalMagnitude), engine->GetDrawVertexNormalMagnitude());
	//	glUniform4fv(mDrawNormalShader.GetUniform(__uniforms_count__ + u_VertexNormalColor), 1, glm::value_ptr(engine->GetDrawVertexNormalColor()));

	//	glBindVertexArray(mVaoID);

	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());

	//	glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y * mMapCount);

	//	glBindVertexArray(0);

	//	mDrawNormalShader.UnUse();
	//}
}


//void ShallowWaterRenderer::UpdateMaterialTextureIndex(const Desc & desc)
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