#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



DirectionalLight::DirectionalLight(const glm::vec4 & color, const glm::vec3 & direction)
	: Light(Light::Directional_Light, DirectionalLightEnums::__property_count__)
	, mWorldDirection(direction, 0.f)
{
	SetProperty(color, DirectionalLightEnums::Color_Property);
}


DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::TransformInViewCoords(const glm::mat4 & viewMatrix)
{
	glm::vec4 dir = viewMatrix * mWorldDirection;
	SetProperty(dir, DirectionalLightEnums::Direction_Property);
}


	} // namespace Light
} // namespace CoreFx
