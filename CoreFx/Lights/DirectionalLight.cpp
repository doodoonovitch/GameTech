#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



DirectionalLight::DirectionalLight(const glm::vec3 & direction, glm::vec3 const & color, GLfloat intensity)
	: Light(Light::Directional_Light, __property_count__, color, intensity)
	, mWorldDirection(direction, 0.f)
{
}


DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::TransformInViewCoords(const glm::mat4 & viewMatrix)
{
	glm::vec4 dir = viewMatrix * mWorldDirection;
	SetProperty(dir, Direction_Property);
}


	} // namespace Light
} // namespace CoreFx
