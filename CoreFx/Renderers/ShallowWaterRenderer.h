#pragma once

#ifndef COREFX_RENDERERS_SHALLOWWATERRENDERER_H
#define COREFX_RENDERERS_SHALLOWWATERRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class ShallowWaterRenderer : public RendererHelper<1>
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

		return direction;

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

		WaveProps(const glm::vec3 & direction, GLfloat waveLength, GLfloat amplitude, GLfloat velocity, GLfloat steepness)
			: mDirection(direction)
			, mWaveLength(waveLength)
			, mFrequency(glm::two_pi<GLfloat>() / waveLength)
			, mAmplitude(amplitude)
			, mVelocity(velocity)
			, mSteepness(steepness)
		{
		}

		WaveProps(GLfloat directionAngleDegrees, GLfloat waveLength, GLfloat amplitude, GLfloat velocity, GLfloat steepness)
			: mDirection(DeepOceanRenderer::AngleToDirection(directionAngleDegrees))
			, mWaveLength(waveLength)
			, mFrequency(glm::two_pi<GLfloat>() / waveLength)
			, mAmplitude(amplitude)
			, mVelocity(velocity)
			, mSteepness(steepness)
		{

		}

		glm::vec3 mDirection;
		GLfloat mWaveLength;
		GLfloat mFrequency;
		GLfloat mAmplitude;
		GLfloat mVelocity;
		GLfloat mSteepness;
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
	ShallowWaterRenderer(const Desc & desc);
	virtual ~ShallowWaterRenderer();

	virtual void Render() override;
	virtual void DebugRender() override;


private:


	struct WavePropUniformIndex
	{
		GLint mDirectionUniformIndex[MAX_WAVE_TO_SUM];
		GLint mWaveLengthUniformIndex[MAX_WAVE_TO_SUM];
		GLint mFrequencyUniformIndex[MAX_WAVE_TO_SUM];
		GLint mAmplitudeUniformIndex[MAX_WAVE_TO_SUM];
		GLint mVelocityUniformIndex[MAX_WAVE_TO_SUM];
		GLint mSteepnessUniformIndex[MAX_WAVE_TO_SUM];
		GLint mPhaseUniformIndex[MAX_WAVE_TO_SUM];
		GLbyte mWavePropModified[MAX_WAVE_TO_SUM];
	};


	void LoadShaders(const Desc & desc);
	void GetWavePropertyUniformIndex(Shader & shader, WavePropUniformIndex & waveProps);
	void SetWavePropertyUniformValues(WavePropUniformIndex & waveProps);

private:

	enum EUniformIndex
	{
		u_PatchCount,
		u_MapSize,
		u_Scale,
		u_PerMapDataSampler,

		u_SkyboxCubeMapSampler,
		u_textureSampler,

		__uniforms_count__
	};

	enum EUniformIndex2
	{
		u_NormalMagnitude,
		u_VertexNormalColor,

		__uniforms2_count__
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
		E_WaveProps_Steepness_modified = 1 << 4,
	};

	const int FIRST_TEXTURE_SAMPLER_INDEX = 2;

	CubeMapTexture const * mCubeMapTexture;

	glm::ivec2 mMapSize;
	glm::ivec2 mPatchCount;
	glm::vec3 mScale;
	GLint mMapCount;
	WaveProps mWaveProps[MAX_WAVE_TO_SUM];

	Shader mDrawNormalShader;
	TextureBuffer mModelMatrixBuffer;

	WavePropUniformIndex mShaderWaveProps;
	WavePropUniformIndex mDebugShaderWaveProps;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_SHALLOWWATERRENDERER_H