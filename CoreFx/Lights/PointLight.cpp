#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



PointLight::PointLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, GLfloat constantAttenuation, GLfloat linearAttenuation, GLfloat quadraticAttenuation)
	: Light(Light::Point_Light, __property_count__, color, intensity)
	, mWorldPosition(position, 1.f)
{
	SetProperty(glm::vec4(constantAttenuation, linearAttenuation, quadraticAttenuation, 0.f), Attenuation_Property);
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
