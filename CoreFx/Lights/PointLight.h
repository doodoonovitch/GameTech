#pragma once
#ifndef COREFX_LIGHTS_POINTLIGHT_H
#define COREFX_LIGHTS_POINTLIGHT_H


namespace CoreFx
{

	class Engine;

	namespace Lights
	{


		namespace PointLightEnums
		{
			enum PropertyIndex
			{
				Color_Property,
				Position_Property,
				Attenuation_Property,

				__property_count__
			};
		}


class PointLight : public LightWithAttenuation<PointLightEnums::Attenuation_Property>
{
	template<typename T_Object, int T_vbo_count> friend class Renderers::SceneObjectRenderer;
	friend class Engine;


public:

	virtual void TransformInViewCoords(const glm::mat4 & viewMatrix) override;

	const glm::vec4 & GetColor() const { return *(const glm::vec4*)GetProperty(PointLightEnums::Color_Property); }

	void SetColor(const glm::vec4 & color)
	{
		SetProperty(color, PointLightEnums::Color_Property);
	}

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

protected:


	PointLight(const glm::vec4 & color, const glm::vec3 & position, GLfloat constantAttenuation, GLfloat linearAttenuation, GLfloat quadraticAttenuation);
	virtual ~PointLight();

protected:

	glm::vec4 mWorldPosition;
};


	} // Namespace Lights
} // namespace CoreFx
#endif // COREFX_LIGHTS_POINTLIGHT_H

