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

	enum PropertyIndex
	{
		Color_Property,
		Position_Property
	};

public:

	virtual void TransformInViewCoords(const glm::mat4 & viewMatrix) override;

	const glm::vec4 & GetColor() const { return *(const glm::vec4*)GetProperty(Color_Property); }

	void SetColor(const glm::vec4 & color)
	{
		SetProperty(color, Color_Property);
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


	PointLight(const glm::vec4 & color, const glm::vec3 & position);
	virtual ~PointLight();

protected:

	glm::vec4 mWorldPosition;
};


	} // Namespace Lights
} // namespace CoreFx
#endif // COREFX_LIGHTS_POINTLIGHT_H

