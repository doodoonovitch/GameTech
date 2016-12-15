#pragma once

#ifndef COREFX_RENDERERS_PERLINNOISEOCEAN_COMPUTESHADER_H
#define COREFX_RENDERERS_PERLINNOISEOCEAN_COMPUTESHADER_H


namespace CoreFx
{
	namespace Renderers
	{
		namespace PerlinNoiseOcean
		{
		// =======================================================================
		// =======================================================================


class HeightMapCS : public ComputeShaderHelper<1>
{
public:


public:

	HeightMapCS();
	virtual ~HeightMapCS();

	virtual void Compute() override;

	GLuint GetHeightMapTextureId() const { return mHeightMapTextureId; }
	GLfloat GetMaxAmplitude() const	{ return mMaxAmplitude;	}
	GLuint GetNormalMapTextureId() const { return mNormalMapTextureId; }

	void LoadShader(const WavePropertyList & waveProps, const glm::ivec2 & textureSize);

private:

	void GenerateHeightMap();

	void LoadNormalMapShader();
	void GenerateNormalMap();

private:

	enum EBufferIndex
	{
		WavePropsBufferIndex,
	};

	enum class EHMapCSBindings
	{
		u_ImageOut,
		u_WaveParamsBlock,
	};

	enum class EHMapCSUniforms
	{
		u_WaveCount,
		u_TextureSize,
		u_Time,
		u_NoiseSampler,
		__hmap_cs_uniforms_count__
	};

	enum class ENMapCSUniforms
	{
		u_TextureSize,
		u_HeightMapSampler,
		__nmap_cs_uniforms_count__
	};

	enum EWaveParamsOffset
	{
		WaveParams_DirectionX,
		WaveParams_DirectionY,
		WaveParams_WaveLength,
		WaveParams_Amplitude,
		WaveParams_Velocity,

		__waveparams_count__
	};

	Texture2D const * mNoiseTexture;

	GLint mWaveCount;
	GLuint mHeightMapTextureId;
	GLfloat mMaxAmplitude;

	glm::ivec2 mTextureSize;

	Shader mNormalCS;
	GLuint mNormalMapTextureId;
};



		} // namespace PerlinNoiseOcean
	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_PERLINNOISEOCEAN_COMPUTESHADER_H