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
	template<typename T_Object, int T_vbo_count, typename T_MainShader = Shader, typename T_WireFrameShader = Shader> friend class Renderers::SceneObjectRenderer;
	friend class Engine;

public:

	enum PropertyIndex
	{
		Position_Property = Light::__Common_Property_Count__,
		Direction_Property,
		//World_Position_Property,
		//World_Direction_Property,

		__property_count__
	};


	//virtual void TransformInViewCoords(const glm::mat4 & viewMatrix) override;

	glm::vec3 GetPosition() const
	{
		const GLfloat * p = GetProperty(Position_Property);
		return glm::vec3(p[0], p[1], p[2]);
	}

	void SetPosition(const glm::vec3& position)
	{
		SetProperty(position, Position_Property);
	}

	glm::vec3 GetDirection() const
	{
		const GLfloat * p = GetProperty(Direction_Property);
		return glm::vec3(p[0], p[1], p[2]);
	}

	void SetDirection(const glm::vec3& direction)
	{
		SetProperty(direction, Direction_Property);
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


	SpotLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, GLfloat radius, const glm::vec3 & direction, GLfloat innerConeAngle, GLfloat outerConeAngle);
	virtual ~SpotLight();

protected:

	float mInnerConeAngle;
	float mOuterConeAngle;
	//float mInnerConeCos;
	//float mOuterConeCos;
};


	} // Namespace Lights
} // namespace CoreFx
#endif // COREFX_LIGHTS_SPOTLIGHT_H

