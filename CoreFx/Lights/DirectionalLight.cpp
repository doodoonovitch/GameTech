#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



DirectionalLight::DirectionalLight(const glm::vec3 & direction, glm::vec3 const & ambient, glm::vec3 const & diffuse, glm::vec3 const & specular)
	: Light(Light::Directional_Light, __property_count__, ambient, diffuse, specular)
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
