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

GLfloat PointLight::GetAttenuationDistance() const
{
	GLfloat constant = GetConstantAttenuation();
	GLfloat linear = GetLinearAttenuation();
	GLfloat quadratic = GetQuadraticAttenuation();
	GLfloat lightMax = GetIntensity();
	GLfloat radius = (-linear + std::sqrtf(linear * linear - 4.f * quadratic * (constant - (256.0f / 5.0f) * lightMax))) / (2.f * quadratic);
	return radius;
}


	} // namespace Light
} // namespace CoreFx
