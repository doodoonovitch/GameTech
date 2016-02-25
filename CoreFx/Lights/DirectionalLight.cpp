#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



DirectionalLight::DirectionalLight(const glm::vec4 & color, const glm::vec3 & direction)
	: Light(Light::Directional_Light, 2)
	, mWorldDirection(direction, 0.f)
{
	SetProperty(color, Color_Property);
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
