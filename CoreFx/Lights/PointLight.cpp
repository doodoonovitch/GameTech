#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



PointLight::PointLight(const glm::vec4 & color, const glm::vec3 & position)
	: Light(Light::Point_Light, 2)
	, mWorldPosition(position, 1.f)
{
	SetProperty(color, Color_Property);
}


PointLight::~PointLight()
{

}

void PointLight::TransformInViewCoords(const glm::mat4 & viewMatrix) 
{
	glm::vec4 pos = viewMatrix * mWorldPosition;
	SetProperty(pos, Position_Property);
}




	} // namespace Light
} // namespace CoreFx
