#pragma once

#ifndef COREFX_RENDERERS_PERLINNOISEOCEANCOMPUTESHADER_H
#define COREFX_RENDERERS_PERLINNOISEOCEANCOMPUTESHADER_H


namespace CoreFx
{
	namespace Renderers
	{


class PerlinNoiseOceanComputeShader : public ComputeShaderHelper<1>
{
public:


public:

	PerlinNoiseOceanComputeShader(const PerlinNoiseOceanRenderer::Desc & desc);
	virtual ~PerlinNoiseOceanComputeShader();

	virtual void Compute() override;


private:

	void LoadShaders(const Desc & desc);

	void GenerateHeightMap();

private:

	enum VertexBufferIndex
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
		u_NoiseSampler,
		__hmap_cs_uniforms_count__
	};

	const int FIRST_TEXTURE_SAMPLER_INDEX = 2;

//#pragma pack(push, 1)
//	struct HMapCSParam
//	{
//		glm::vec2 m_Direction;
//		GLfloat __padding_1__[2];
//		GLfloat m_WaveLength;
//		GLfloat m_Amplitude;
//		GLfloat m_Velocity;
//		GLfloat __padding_2__;
//	};
//#pragma pack(pop)

	enum EWaveParamsOffset
	{
		WaveParams_DirectionX,
		WaveParams_DirectionY,
		WaveParams_WaveLength,
		WaveParams_Amplitude,
		WaveParams_Velocity,

		__waveparams_count__
	};

	CubeMapTexture const * mCubeMapTexture;
	Texture2D const * mNoiseHeightTexture;

	GLfloat * mWaveProps;
	GLint mWaveCount;
	GLuint mHeightMapTextureId;

	glm::ivec2 mTextureSize;
	glm::ivec2 mMapSize;
	glm::ivec2 mPatchCount;
	glm::vec3 mScale;
	GLint mMapCount;

	Shader mHMapCompShader;
	Shader mDrawNormalShader;
	TextureBuffer mModelMatrixBuffer;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_PERLINNOISEOCEANCOMPUTESHADER_H