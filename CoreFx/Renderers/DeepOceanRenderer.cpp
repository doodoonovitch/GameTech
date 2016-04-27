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
{
	std::cout << std::endl;
	std::cout << "Initialize DeepOceanRenderer...." << std::endl;

	memset(mWavePropModified, -1, sizeof(mWavePropModified));
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


	for (int i = 0; i < MAX_WAVE_TO_SUM; ++i)
	{
		char uniformName[50];
		sprintf_s(uniformName, 50, "u_Direction[%i]", i);
		mDirectionUniformIndex[i] = glGetUniformLocation(mShader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, mDirectionUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_WaveLength[%i]", i);
		mWaveLengthUniformIndex[i] = glGetUniformLocation(mShader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, mWaveLengthUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Frequency[%i]", i);
		mFrequencyUniformIndex[i] = glGetUniformLocation(mShader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, mFrequencyUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Amplitude[%i]", i);
		mAmplitudeUniformIndex[i] = glGetUniformLocation(mShader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, mAmplitudeUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Velocity[%i]", i);
		mVelocityUniformIndex[i] = glGetUniformLocation(mShader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, mVelocityUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Steepness[%i]", i);
		mSteepnessUniformIndex[i] = glGetUniformLocation(mShader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, mSteepnessUniformIndex[i]);

		sprintf_s(uniformName, 50, "u_Phase[%i]", i);
		mPhaseUniformIndex[i] = glGetUniformLocation(mShader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		PRINT_MESSAGE("\t%s : %i.\n", uniformName, mPhaseUniformIndex[i]);
	}


	mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE("... done.\n");
	PRINT_MESSAGE("-------------------------------------------------\n\n");
}
 
void DeepOceanRenderer::Render()
{
	mShader.Use();
	glBindVertexArray(mVaoID);

	for (int i = 0; i < MAX_WAVE_TO_SUM; ++i)
	{
		bool phaseModified = false;
		if ((mWavePropModified[i] & E_WaveProps_Direction_modified) == E_WaveProps_Direction_modified)
		{
			glUniform3fv(mDirectionUniformIndex[i], 1, glm::value_ptr(mWaveProps[i].mDirection)); 
		}
		if ((mWavePropModified[i] & E_WaveProps_WaveLength_modified) == E_WaveProps_WaveLength_modified)
		{
			//glUniform1f(mWaveLengthUniformIndex[i], mWaveProps[i].mWaveLength);
			glUniform1f(mFrequencyUniformIndex[i], mWaveProps[i].mFrequency);
			phaseModified = true;
		}
		if ((mWavePropModified[i] & E_WaveProps_Amplitude_modified) == E_WaveProps_Amplitude_modified)
		{
			glUniform1f(mAmplitudeUniformIndex[i], mWaveProps[i].mAmplitude);
		}
		if ((mWavePropModified[i] & E_WaveProps_Velocity_modified) == E_WaveProps_Velocity_modified)
		{
			//glUniform1f(mVelocityUniformIndex[i], mWaveProps[i].mVelocity);
			phaseModified = true;
		}
		if ((mWavePropModified[i] & E_WaveProps_Steepness_modified) == E_WaveProps_Steepness_modified)
		{
			glUniform1f(mSteepnessUniformIndex[i], mWaveProps[i].mSteepness);
		}
		if (phaseModified)
		{
			GLfloat phase = mWaveProps[i].mVelocity * mWaveProps[i].mFrequency;
			glUniform1f(mPhaseUniformIndex[i], phase);
		}

		if (mWavePropModified[i] != 0)
			mWavePropModified[i] = 0;
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());

	glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y * mMapCount);

	glBindVertexArray(0);
	mShader.UnUse();

}

void DeepOceanRenderer::DebugRender()
{
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