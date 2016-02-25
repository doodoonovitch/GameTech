#pragma once
#ifndef COREFX_LIGHTS_DIRECTIONALLIGHT_H
#define COREFX_LIGHTS_DIRECTIONALLIGHT_H


namespace CoreFx
{

	class Engine;

	namespace Lights
	{


class DirectionalLight : public Light
{
	template<typename T_Object, int T_vbo_count> friend class Renderers::SceneObjectRenderer;
	friend class Engine;

	enum PropertyIndex
	{
		Color_Property,
		Direction_Property
	};

public:

	virtual void TransformInViewCoords(const glm::mat4 & viewMatrix) override;

	const glm::vec4 & GetColor() const { return *(const glm::vec4*)GetProperty(Color_Property); }

	void SetColor(const glm::vec4 & color)
	{
		SetProperty(color, Color_Property);
	}

	const glm::vec4 & GetDirection() const
	{
		return mWorldDirection;
	}

	void SetDirection(const glm::vec3& position)
	{
		mWorldDirection.x = position.x;
		mWorldDirection.y = position.y;
		mWorldDirection.z = position.z;
		SetIsModified(true);
	}


protected:
	
	DirectionalLight(const glm::vec4 & color, const glm::vec3 & direction);
	virtual ~DirectionalLight();

private:

	glm::vec4 mWorldDirection;
};


	} // Namespace Lights
} // namespace CoreFx
#endif // COREFX_LIGHTS_DIRECTIONALLIGHT_H

