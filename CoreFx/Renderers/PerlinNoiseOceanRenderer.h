#pragma once

#ifndef COREFX_RENDERERS_PERLINNOISEOCEANRENDERER_H
#define COREFX_RENDERERS_PERLINNOISEOCEANRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class PerlinNoiseOceanRenderer : public RendererHelper<1>
{
public:

	enum
	{
		MAX_WAVE_TO_SUM = 4
	};

	static glm::vec3 AngleToDirection(GLfloat angleDegrees)
	{
		const glm::vec3 UnitVector(1.f, 0.f, 0.f);

		float angle = glm::radians(angleDegrees);

		glm::vec3 direction(0.f);

		direction.x = (UnitVector.x * cos(angle)) - (UnitVector.z * sin(angle));
		direction.z = (UnitVector.z * cos(angle)) + (UnitVector.x * sin(angle));

		return glm::normalize(direction);

	}


	struct MapDesc
	{
		MapDesc(const glm::vec3 & origin, const glm::quat & rotation)
		{
			mModelDQ.SetRotationTranslation(rotation, origin);
		}

		Maths::DualQuaternion mModelDQ;
	};

	typedef std::vector<MapDesc> MapDescList;

	struct WaveProps
	{
		WaveProps()
		{
		}

		WaveProps(const glm::vec3 & direction, GLfloat waveLength, GLfloat amplitude, GLfloat velocity)
			: mDirection(glm::normalize(direction))
			, mWaveLength(waveLength)
			, mAmplitude(amplitude)
			, mVelocity(velocity)
		{
		}

		WaveProps(GLfloat directionAngleDegrees, GLfloat waveLength, GLfloat amplitude, GLfloat velocity)
			: mDirection(PerlinNoiseOceanRenderer::AngleToDirection(directionAngleDegrees))
			, mWaveLength(waveLength)
			, mAmplitude(amplitude)
			, mVelocity(velocity)
		{

		}

		glm::vec3 mDirection;
		GLfloat mWaveLength;
		GLfloat mAmplitude;
		GLfloat mVelocity;
	};

	struct Desc : public Renderer::Desc
	{
		Desc(GLint mapWidth, GLint mapDepth, const glm::vec2 & scale, const std::string & skyboxCubeMapTextureFilename)
			: Renderer::Desc()
			, mMapWidth(mapWidth)
			, mMapDepth(mapDepth)
			, mScale(scale.x, 1.0f, scale.y)
			, mSkyboxCubeMapTextureFilename(skyboxCubeMapTextureFilename)
		{ }

		GLint mMapWidth;
		GLint mMapDepth;
		glm::vec3 mScale;
		MapDescList mMaps;
		WaveProps mWaveProps[MAX_WAVE_TO_SUM];
		std::string mSkyboxCubeMapTextureFilename;
	};

public:
	PerlinNoiseOceanRenderer(const Desc & desc);
	virtual ~PerlinNoiseOceanRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;


private:


	struct WavePropUniformIndex
	{
		GLint mDirectionUniformIndex[MAX_WAVE_TO_SUM];
		GLint mWaveLengthUniformIndex[MAX_WAVE_TO_SUM];
		GLint mAmplitudeUniformIndex[MAX_WAVE_TO_SUM];
		GLint mVelocityUniformIndex[MAX_WAVE_TO_SUM];
		GLbyte mWavePropModified[MAX_WAVE_TO_SUM];
	};


	void LoadShaders(const Desc & desc);
	void LoadMainShader(const Desc & desc);
	void LoadWireFrameShader(const Desc & desc);
	void GetWavePropertyUniformIndex(Shader & shader, WavePropUniformIndex & waveProps);
	void SetWavePropertyUniformValues(WavePropUniformIndex & waveProps);

private:

	enum EUniformIndex
	{
		u_PatchCount,
		u_MapSize,
		u_Scale,
		u_noiseHeightSampler,
		u_PerMapDataSampler,
		u_SkyboxCubeMapSampler,
		u_textureSampler,

		__uniforms_count__
	};

	struct PerMapData
	{
		Maths::DualQuaternion mModelDQ;
	};

	enum 
	{
		E_WaveProps_Direction_modified = 1,
		E_WaveProps_WaveLength_modified = 1 << 1,
		E_WaveProps_Amplitude_modified = 1 << 2,
		E_WaveProps_Velocity_modified = 1 << 3,
	};

	const int FIRST_TEXTURE_SAMPLER_INDEX = 2;

	CubeMapTexture const * mCubeMapTexture;
	Texture2D const * mNoiseHeightTexture;

	glm::ivec2 mMapSize;
	glm::ivec2 mPatchCount;
	glm::vec3 mScale;
	GLint mMapCount;
	WaveProps mWaveProps[MAX_WAVE_TO_SUM];

	Shader mDrawNormalShader;
	TextureBuffer mModelMatrixBuffer;

	WavePropUniformIndex mShaderWaveProps;
	WavePropUniformIndex mWireFrameShaderWaveProps;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_PERLINNOISEOCEANRENDERER_H