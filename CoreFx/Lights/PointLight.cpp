#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



PointLight::PointLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, GLfloat constantAttenuation, GLfloat linearAttenuation, GLfloat quadraticAttenuation)
	: Light(Light::Point_Light, __property_count__, color, intensity)
{
	SetProperty(glm::vec4(constantAttenuation, linearAttenuation, quadraticAttenuation, 0.f), Attenuation_Property);
	SetPosition(position);
}


PointLight::~PointLight()
{

}

void PointLight::TransformInViewCoords(const glm::mat4 & viewMatrix) 
{
	const GLfloat * p = GetProperty(World_Position_Property);
	glm::vec4 worldPosition(p[0], p[1], p[2], 1.0f);
	glm::vec4 pos = viewMatrix * worldPosition;
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
