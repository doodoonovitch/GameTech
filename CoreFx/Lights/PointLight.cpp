#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



PointLight::PointLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, GLfloat radius)
	: Light(Light::Point_Light, __property_count__, color, intensity, radius)
{
	SetPosition(position);
}


PointLight::~PointLight()
{

}

//void PointLight::TransformInViewCoords(const glm::mat4 & viewMatrix) 
//{
//	const GLfloat * p = GetProperty(World_Position_Property);
//	glm::vec4 worldPosition(p[0], p[1], p[2], 1.0f);
//	glm::vec4 pos = viewMatrix * worldPosition;
//	SetProperty(pos, Position_Property);
//}


	} // namespace Light
} // namespace CoreFx
