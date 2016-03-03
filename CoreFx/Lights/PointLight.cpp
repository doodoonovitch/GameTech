#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



PointLight::PointLight(const glm::vec4 & color, const glm::vec3 & position, GLfloat constantAttenuation, GLfloat linearAttenuation, GLfloat quadraticAttenuation)
	: LightWithAttenuation<PointLightEnums::Attenuation_Property>(Light::Point_Light, PointLightEnums::__property_count__, constantAttenuation, linearAttenuation, quadraticAttenuation)
	, mWorldPosition(position, 1.f)
{
	SetProperty(color, PointLightEnums::Color_Property);
}


PointLight::~PointLight()
{

}

void PointLight::TransformInViewCoords(const glm::mat4 & viewMatrix) 
{
	glm::vec4 pos = viewMatrix * mWorldPosition;
	SetProperty(pos, PointLightEnums::Position_Property);
}




	} // namespace Light
} // namespace CoreFx
