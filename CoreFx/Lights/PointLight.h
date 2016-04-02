#pragma once
#ifndef COREFX_LIGHTS_POINTLIGHT_H
#define COREFX_LIGHTS_POINTLIGHT_H


namespace CoreFx
{

	class Engine;

	namespace Lights
	{



class PointLight : public Light
{
	template<typename T_Object, int T_vbo_count> friend class Renderers::SceneObjectRenderer;
	friend class Engine;

public:

	enum PropertyIndex
	{
		Position_Property = Light::__Common_Property_Count__,
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


protected:


	PointLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, GLfloat constantAttenuation, GLfloat linearAttenuation, GLfloat quadraticAttenuation);
	virtual ~PointLight();

protected:

	glm::vec4 mWorldPosition;
};


	} // Namespace Lights
} // namespace CoreFx
#endif // COREFX_LIGHTS_POINTLIGHT_H

