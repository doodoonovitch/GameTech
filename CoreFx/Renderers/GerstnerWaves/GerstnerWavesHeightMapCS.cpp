#include "stdafx.h"
#include "CoreFx.h"

#define PRECOMPUTE_NORMAL

namespace CoreFx
{
namespace Renderers
{
namespace GerstnerWaves
{
// =======================================================================
// =======================================================================


HeightMapCS::HeightMapCS(bool precomputeNormals)
	: ComputeShaderHelper<1>("PerlinNoiseOceanComputeShader")
	, mWaveCount(0)
	, mHeightMapTextureId(0)
	, mPrecomputeNormals(precomputeNormals)
{
	CreateBuffers();
}

HeightMapCS::~HeightMapCS()
{
	glDeleteTextures(1, &mHeightMapTextureId);
}


void HeightMapCS::LoadShader(const WavePropertyList & waveProps, const glm::vec2 & textureSize)
{
	PRINT_MESSAGE("Initialize Perlin Noise Ocean Renderer (Height Map Compute) Shaders : .....");

	mTextureSize = textureSize;

	mWaveCount = (GLint)waveProps.size();
	assert(mWaveCount > 0);
	GLsizei wavePropsBufferItemCount = mWaveCount * __waveparams_count__;
	GLsizei wavePropsBufferSize = wavePropsBufferItemCount * sizeof(GLfloat);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBufferIds[WavePropsBufferIndex]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, wavePropsBufferSize, nullptr, GL_STATIC_DRAW); GL_CHECK_ERRORS;
	{
		GLfloat * wavePropsBuffer = (GLfloat *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);	GL_CHECK_ERRORS;
		for (int i = 0; i < mWaveCount; ++i)
		{
			const WaveProperty & src = waveProps[i];
			wavePropsBuffer[WaveParams_Amplitude] = src.mAmplitude;
			wavePropsBuffer[WaveParams_DirectionX] = src.mDirection.x;
			wavePropsBuffer[WaveParams_DirectionY] = src.mDirection.z;
			wavePropsBuffer[WaveParams_W] = src.GetFrequency();
			wavePropsBuffer[WaveParams_Phase] = src.GetPhase();
			wavePropsBuffer[WaveParams_Steepness] = src.mSteepness;
			wavePropsBuffer += __waveparams_count__;
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER); GL_CHECK_ERRORS;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenTextures(1, &mHeightMapTextureId);
	glBindTexture(GL_TEXTURE_2D, mHeightMapTextureId);
	TextureManager::CreateTexStorage2D(GL_TEXTURE_2D, mTextureSize.x, mTextureSize.y, nullptr, false, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT, mPrecomputeNormals ? GL_RGBA32F : GL_R32F, GL_RED, GL_FLOAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ---------------------------------------


	std::vector<std::string> shaderSources;
	if (mPrecomputeNormals)
	{
		shaderSources.push_back("#define PRECOMPUTE_NORMAL\n");
	}

	std::string csSource;
	Shader::MergeFile(csSource, "shaders/GerstnerWaves.hmap.cs.glsl");
	shaderSources.push_back(csSource);
	mShader.LoadFromString(GL_COMPUTE_SHADER, shaderSources, Shader::EInclude::ComputeShadersCommon);

	mShader.CreateAndLinkProgram();
	mShader.Use();

	const char * uniformNames[__hmap_cs_uniforms_count__] =
	{
		"u_WaveCount",
		"u_TextureSize",
		"u_Time",
	};

	mShader.AddUniforms(uniformNames, __hmap_cs_uniforms_count__);

	glUniform1i(mShader.GetUniform(u_WaveCount), mWaveCount); GL_CHECK_ERRORS;
	glUniform2iv(mShader.GetUniform(u_TextureSize), 1, glm::value_ptr(mTextureSize)); GL_CHECK_ERRORS;

	mShader.UnUse();

	PRINT_MESSAGE("Height map texture id : %li", mHeightMapTextureId);
	PRINT_MESSAGE("Wave props buffer id : %li", mBufferIds[WavePropsBufferIndex]);

	mIsInitialized = true;

	PRINT_MESSAGE(".....done.");
}

void HeightMapCS::GenerateHeightMap()
{
	mShader.Use();

	glUniform1f(mShader.GetUniform(u_Time), Engine::GetInstance()->GetTime()); GL_CHECK_ERRORS;

	glBindImageTexture((GLuint)EHMapCSBindings::u_ImageOut, mHeightMapTextureId, 0, GL_FALSE, 0, GL_WRITE_ONLY, mPrecomputeNormals ? GL_RGBA32F : GL_R32F);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (GLuint)EHMapCSBindings::u_WaveParamsBlock, mBufferIds[WavePropsBufferIndex]); GL_CHECK_ERRORS;

	//glDispatchCompute(mTextureSize.x / 32, mTextureSize.y / 32, 1);
	glDispatchCompute(mTextureSize.x, mTextureSize.y, 1);

	glBindTexture(GL_TEXTURE_2D, 0);

	mShader.UnUse();
}

void HeightMapCS::Compute()
{
	GenerateHeightMap();
}




} // namespace GerstnerWaves
} // namespace Renderers
} // namespace CoreFx