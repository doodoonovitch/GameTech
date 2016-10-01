#pragma once

#ifndef COREFX_RENDERERS_PERLINNOISEOCEAN_WAVEPROPERTY_H
#define COREFX_RENDERERS_PERLINNOISEOCEAN_WAVEPROPERTY_H


namespace CoreFx
{
	namespace Renderers
	{
		namespace PerlinNoiseOcean
		{
		// =======================================================================
		// =======================================================================


struct WaveProperty
{
	WaveProperty()
	{
	}

	WaveProperty(const glm::vec3 & direction, GLfloat waveLength, GLfloat amplitude, GLfloat velocity)
		: mDirection(glm::normalize(direction))
		, mWaveLength(waveLength)
		, mAmplitude(amplitude)
		, mVelocity(velocity)
	{
	}

	WaveProperty(GLfloat directionAngleDegrees, GLfloat waveLength, GLfloat amplitude, GLfloat velocity)
		: mDirection(Maths::Helpers::AngleToDirection(directionAngleDegrees))
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

typedef std::vector<WaveProperty> WavePropertyList;


		} // namespace PerlinNoiseOcean
	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_PERLINNOISEOCEAN_WAVEPROPERTY_H