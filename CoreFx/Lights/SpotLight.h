#pragma once
#ifndef COREFX_LIGHTS_SPOTLIGHT_H
#define COREFX_LIGHTS_SPOTLIGHT_H


namespace CoreFx
{

	class Engine;

	namespace Lights
	{



class SpotLight : public Light
{
	template<typename T_Object, int T_vbo_count> friend class Renderers::SceneObjectRenderer;
	friend class Engine;

public:

	enum PropertyIndex
	{
		Position_Property = Light::__Common_Property_Count__,
		Direction_Property,
		Attenuation_Property,

		__property_count__
	};


	virtual void TransformInViewCoords(const glm::mat4 & viewMatrix) override;

	const glm::vec4 & GetPosition() const
	{
		return mWorldPosition;
	}

	void SetPosition(const glm::vec3& position)
	{
		mWorldPosition.x = position.x;
		mWorldPosition.y = position.y;
		mWorldPosition.z = position.z;
		SetIsModified(true);
	}


	const glm::vec4 & GetDirection() const
	{
		return mWorldDirection;
	}

	void SetDirection(const glm::vec3& direction)
	{
		mWorldDirection.x = direction.x;
		mWorldDirection.y = direction.y;
		mWorldDirection.z = direction.z;
		SetIsModified(true);
	}
	
	GLfloat GetConstantAttenuation() const
	{
		return GetProperty(Attenuation_Property)[0];
	}

	GLfloat GetLinearAttenuation() const
	{
		return GetProperty(Attenuation_Property)[1];
	}

	GLfloat GetQuadraticAttenuation() const
	{
		return GetProperty(Attenuation_Property)[2];
	}


	void SetConstantAttenuation(GLfloat value)
	{
		GetProperty(Attenuation_Property)[0] = value;
		SetIsModified(true);
	}

	void SetLinearAttenuation(GLfloat value)
	{
		GetProperty(Attenuation_Property)[1] = value;
		SetIsModified(true);
	}

	void SetQuadraticAttenuation(GLfloat value) 
	{
		GetProperty(Attenuation_Property)[2] = value;
		SetIsModified(true);
	}

	float GetInnerConeAngle() const
	{
		return mInnerConeAngle;
	}

	float GetOuterConeAngle() const
	{
		return mOuterConeAngle;
	}

	void SetInnerConeAngle(float value);
	void SetOuterConeAngle(float value);

protected:


	SpotLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, const glm::vec3 & direction, float innerConeAngle, float outerConeAngle, GLfloat constantAttenuation, GLfloat linearAttenuation, GLfloat quadraticAttenuation);
	virtual ~SpotLight();

protected:

	glm::vec4 mWorldPosition;
	glm::vec4 mWorldDirection;
	float mInnerConeAngle;
	float mOuterConeAngle;
	//float mInnerConeCos;
	//float mOuterConeCos;
};


	} // Namespace Lights
} // namespace CoreFx
#endif // COREFX_LIGHTS_SPOTLIGHT_H

