#pragma once

#ifndef COREFX_RENDERERS_DEEPOCEAN_HEIGHTMAPCS_H
#define COREFX_RENDERERS_DEEPOCEAN_HEIGHTMAPCS_H


namespace CoreFx
{
	namespace Renderers
	{
		namespace DeepOcean
		{
		// =======================================================================
		// =======================================================================


class HeightMapCS : public ComputeShaderHelper<1>
{
public:


public:

	HeightMapCS(bool precomputeNormals);
	virtual ~HeightMapCS();

	virtual void Compute() override;

	GLuint GetHeightMapTextureId() const { return mHeightMapTextureId; }

	void LoadShader(const WavePropertyList & waveProps, const glm::vec2 & textureSize);

private:

	void GenerateHeightMap();

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

	enum EHMapCSUniforms
	{
		u_WaveCount,
		u_TextureSize,
		u_Time,
		__hmap_cs_uniforms_count__
	};

	enum EWaveParamsOffset
	{
		WaveParams_DirectionX,
		WaveParams_DirectionY,
		WaveParams_W,
		WaveParams_Amplitude,
		WaveParams_Phase,
		WaveParams_Steepness,

		__waveparams_count__
	};

	GLint mWaveCount;
	GLuint mHeightMapTextureId;

	glm::ivec2 mTextureSize;

	bool mPrecomputeNormals;
};



		} // namespace DeepOcean
	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_DEEPOCEAN_HEIGHTMAPCS_H