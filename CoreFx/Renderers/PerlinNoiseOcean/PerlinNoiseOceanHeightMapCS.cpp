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


HeightMapCS::HeightMapCS()
	: ComputeShaderHelper<1>("PerlinNoiseOceanComputeShader")
	, mNoiseTexture(Engine::GetInstance()->GetTextureManager()->LoadTexture2D("Medias/Textures/Hmap/WaterH01.tif", GL_REPEAT, GL_REPEAT))
	, mWaveCount(0)
	, mHeightMapTextureId(0)
	, mMaxAmplitude(0)
	, mNormalCS("NormalMapComputeShader")
	, mNormalMapTextureId(0)
{
	CreateBuffers();
}

HeightMapCS::~HeightMapCS()
{
	glDeleteTextures(1, &mHeightMapTextureId);
	Engine::GetInstance()->GetTextureManager()->ReleaseTexture2D(mNoiseTexture);

	glDeleteTextures(1, &mNormalMapTextureId);
}


void HeightMapCS::LoadShader(const WavePropertyList & waveProps, const glm::ivec2 & textureSize)
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
			mMaxAmplitude += src.mAmplitude;
			wavePropsBuffer[WaveParams_Amplitude] = src.mAmplitude;
			wavePropsBuffer[WaveParams_DirectionX] = src.mDirection.x;
			wavePropsBuffer[WaveParams_DirectionY] = src.mDirection.z;
			wavePropsBuffer[WaveParams_Velocity] = src.mVelocity;
			wavePropsBuffer[WaveParams_WaveLength] = src.mWaveLength;
			wavePropsBuffer += __waveparams_count__;
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER); GL_CHECK_ERRORS;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenTextures(1, &mHeightMapTextureId);
	glBindTexture(GL_TEXTURE_2D, mHeightMapTextureId);
	TextureManager::CreateTexStorage2D(GL_TEXTURE_2D, mTextureSize.x, mTextureSize.y, nullptr, false, GL_LINEAR, GL_LINEAR/*GL_NEAREST, GL_NEAREST*/, GL_REPEAT, GL_REPEAT, GL_R16F, GL_RED, GL_FLOAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ---------------------------------------

	mShader.LoadFromFile(GL_COMPUTE_SHADER, "shaders/PerlinNoiseOcean.hmap.cs.glsl", Shader::EInclude::ComputeShadersCommon);

	mShader.CreateAndLinkProgram();
	mShader.Use();

	const char * uniformNames[(int)EHMapCSUniforms::__hmap_cs_uniforms_count__] =
	{
		"u_WaveCount",
		"u_TextureSize",
		"u_Time",
		"u_NoiseSampler"
	};

	mShader.AddUniforms(uniformNames, (int)EHMapCSUniforms::__hmap_cs_uniforms_count__);

	glUniform1i(mShader.GetUniform((int)EHMapCSUniforms::u_WaveCount), mWaveCount); GL_CHECK_ERRORS;
	glUniform2iv(mShader.GetUniform((int)EHMapCSUniforms::u_TextureSize), 1, glm::value_ptr(mTextureSize)); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform((int)EHMapCSUniforms::u_NoiseSampler), 0); GL_CHECK_ERRORS;


	mShader.UnUse();

	PRINT_MESSAGE("Height map texture id : %li", mHeightMapTextureId);
	PRINT_MESSAGE("Wave props buffer id : %li", mBufferIds[WavePropsBufferIndex]);

	mIsInitialized = true;

	PRINT_MESSAGE(".....done.");

	LoadNormalMapShader();
}

void HeightMapCS::LoadNormalMapShader()
{
	PRINT_MESSAGE("Initialize Perlin Noise Ocean Renderer (Normal Map Compute) Shaders : .....");

	glGenTextures(1, &mNormalMapTextureId);
	glBindTexture(GL_TEXTURE_2D, mNormalMapTextureId);
	TextureManager::CreateTexStorage2D(GL_TEXTURE_2D, mTextureSize.x, mTextureSize.y, nullptr, false, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, GL_RGBA8_SNORM, GL_RGBA, GL_UNSIGNED_BYTE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// ---------------------------------------

	mNormalCS.LoadFromFile(GL_COMPUTE_SHADER, "shaders/HeightFieldOcean.normal.cs.glsl", Shader::EInclude::ComputeShadersCommon);

	mNormalCS.CreateAndLinkProgram();
	mNormalCS.Use();

	const char * uniformNames[(int)ENMapCSUniforms::__nmap_cs_uniforms_count__] =
	{
		"u_TextureSize",
		"u_HeightMapSampler"
	};

	mNormalCS.AddUniforms(uniformNames, (int)ENMapCSUniforms::__nmap_cs_uniforms_count__);

	glUniform2iv(mNormalCS.GetUniform((int)ENMapCSUniforms::u_TextureSize), 1, glm::value_ptr(mTextureSize)); GL_CHECK_ERRORS;
	glUniform1i(mNormalCS.GetUniform((int)ENMapCSUniforms::u_HeightMapSampler), 0); GL_CHECK_ERRORS;

	mNormalCS.UnUse();

	PRINT_MESSAGE("Normal map texture id : %li", mNormalMapTextureId);


	PRINT_MESSAGE(".....done.");
}

void HeightMapCS::GenerateHeightMap()
{
	mShader.Use();

	glUniform1f(mShader.GetUniform((int)EHMapCSUniforms::u_Time), Engine::GetInstance()->GetTime()); GL_CHECK_ERRORS;

	glBindImageTexture((GLuint)EHMapCSBindings::u_ImageOut, mHeightMapTextureId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16F);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (GLuint)EHMapCSBindings::u_WaveParamsBlock, mBufferIds[WavePropsBufferIndex]); GL_CHECK_ERRORS;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mNoiseTexture->GetResourceId());

	//glDispatchCompute(mTextureSize.x / Shader::GetComputeWorkgroupSize(Shader::EComputeWorkgroupId::X), mTextureSize.y / Shader::GetComputeWorkgroupSize(Shader::EComputeWorkgroupId::Y), 1);
	//glDispatchCompute(mTextureSize.x / 32, mTextureSize.y / 32, 1);
	glDispatchCompute(mTextureSize.x, mTextureSize.y, 1);

	glBindTexture(GL_TEXTURE_2D, 0);

	mShader.UnUse();
}

void HeightMapCS::GenerateNormalMap()
{
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	mNormalCS.Use();

	glUniform1f(mNormalCS.GetUniform((int)EHMapCSUniforms::u_Time), Engine::GetInstance()->GetTime()); GL_CHECK_ERRORS;

	glBindImageTexture((GLuint)EHMapCSBindings::u_ImageOut, mNormalMapTextureId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8_SNORM);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mHeightMapTextureId);

	//glDispatchCompute(mTextureSize.x / Shader::GetComputeWorkgroupSize(Shader::EComputeWorkgroupId::X), mTextureSize.y / Shader::GetComputeWorkgroupSize(Shader::EComputeWorkgroupId::Y), 1);
	//glDispatchCompute(mTextureSize.x / 32, mTextureSize.y / 32, 1);
	glDispatchCompute(mTextureSize.x, mTextureSize.y, 1);

	glBindTexture(GL_TEXTURE_2D, 0);

	mNormalCS.UnUse();
}

void HeightMapCS::Compute()
{
	GenerateHeightMap();
	GenerateNormalMap();
}




} // namespace PerlinNoiseOcean
} // namespace Renderers
} // namespace CoreFx