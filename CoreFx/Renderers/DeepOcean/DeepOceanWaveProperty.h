#pragma once

#ifndef COREFX_RENDERERS_DEEPOCEAN_WAVEPROPERTY_H
#define COREFX_RENDERERS_DEEPOCEAN_WAVEPROPERTY_H


namespace CoreFx
{
	namespace Renderers
	{
		namespace DeepOcean
		{
		// =======================================================================
		// =======================================================================


struct WaveProperty
{
	WaveProperty()
	{
	}

	WaveProperty(const glm::vec3 & direction, GLfloat waveLength, GLfloat amplitude, GLfloat velocity, GLfloat steepness)
		: mDirection(glm::normalize(direction))
		, mWaveLength(waveLength)
		, mAmplitude(amplitude)
		, mVelocity(velocity)
		, mSteepness(steepness)
	{
	}

	WaveProperty(GLfloat directionAngleDegrees, GLfloat waveLength, GLfloat amplitude, GLfloat velocity, GLfloat steepness)
		: mDirection(Maths::Helpers::AngleToDirection(directionAngleDegrees))
		, mWaveLength(waveLength)
		, mAmplitude(amplitude)
		, mVelocity(velocity)
		, mSteepness(steepness)
	{

	}

	const GLfloat G = 9.8f;

	GLfloat GetFrequency() const
	{
		return sqrtf(G * glm::two_pi<GLfloat>() / mWaveLength);
	}

	GLfloat GetPhase() const
	{
		return mVelocity * glm::two_pi<GLfloat>() / mWaveLength;
	}

	glm::vec3 mDirection;
	GLfloat mWaveLength;
	GLfloat mAmplitude;
	GLfloat mVelocity;
	GLfloat mSteepness;
};

typedef std::vector<WaveProperty> WavePropertyList;


		} // namespace DeepOcean
	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_DEEPOCEAN_WAVEPROPERTY_H