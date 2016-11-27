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
	template<typename T_Object, int T_vbo_count, typename T_MainShader = Shader, typename T_WireFrameShader = Shader> friend class Renderers::SceneObjectRenderer;
	friend class Engine;


public:
	enum PropertyIndex
	{
		Direction_Property = Light::__Common_Property_Count__,

		__property_count__
	};

	//virtual void TransformInViewCoords(const glm::mat4 & viewMatrix) override;

	const glm::vec3 & GetDirection() const
	{
		const GLfloat * p = GetProperty(Direction_Property);
		return glm::vec3(p[0], p[1], p[2]);
		//return mWorldDirection;
	}

	void SetDirection(const glm::vec3& value)
	{
		//mWorldDirection.x = position.x;
		//mWorldDirection.y = position.y;
		//mWorldDirection.z = position.z;
		//SetIsModified(true);
		SetProperty(glm::vec4(value, 1.0f), Direction_Property);
	}


protected:
	
	DirectionalLight(const glm::vec3 & direction, glm::vec3 const & color, GLfloat intensity);
	virtual ~DirectionalLight();

private:

	//glm::vec4 mWorldDirection;
};


	} // Namespace Lights
} // namespace CoreFx
#endif // COREFX_LIGHTS_DIRECTIONALLIGHT_H

