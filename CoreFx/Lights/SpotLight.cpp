#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



SpotLight::SpotLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, GLfloat radius, const glm::vec3 & direction, GLfloat innerConeAngle, GLfloat outerConeAngle)
	: Light(Light::Spot_Light, __property_count__, color, intensity, radius)
	, mInnerConeAngle(innerConeAngle)
	, mOuterConeAngle(outerConeAngle)
	//, mInnerConeCos(glm::cos(innerConeAngle))
	//, mOuterConeCos(glm::cos(outerConeAngle))
{
	SetPosition(position);
	SetDirection(direction);
	GLfloat * dirProp = GetProperty(Direction_Property);
	dirProp[3] = glm::cos(mInnerConeAngle);
	GLfloat * posProp = GetProperty(Position_Property);
	posProp[3] = glm::cos(mOuterConeAngle);
}


SpotLight::~SpotLight()
{

}

//void SpotLight::TransformInViewCoords(const glm::mat4 & viewMatrix)
//{
//	const GLfloat * p = GetProperty(World_Position_Property);
//	glm::vec4 worldPosition(p[0], p[1], p[2], 1.0f);
//	glm::vec4 pos = viewMatrix * worldPosition;
//	GLfloat * posProp = GetProperty(Position_Property);
//	memcpy(posProp, glm::value_ptr(pos), 3 * sizeof(GLfloat));
//
//	p = GetProperty(World_Direction_Property);
//	glm::vec4 worldDirection(p[0], p[1], p[2], 0.0f);
//	glm::vec4 dir = viewMatrix * worldDirection;
//	GLfloat * dirProp = GetProperty(Direction_Property);
//	memcpy(dirProp, glm::value_ptr(dir), 3 * sizeof(GLfloat));
//	SetIsModified(true);
//}

void SpotLight::SetInnerConeAngle(float value)
{
	mInnerConeAngle = value;
	GLfloat * posProp = GetProperty(Direction_Property);
	posProp[3] = glm::cos(mInnerConeAngle);
	SetIsModified(true);
}

void SpotLight::SetOuterConeAngle(float value)
{
	mOuterConeAngle = value;
	GLfloat * prop = GetProperty(Position_Property);
	prop[3] = glm::cos(mOuterConeAngle);
	SetIsModified(true);
}



	} // namespace Light
} // namespace CoreFx
