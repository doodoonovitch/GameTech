#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



SpotLight::SpotLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, const glm::vec3 & direction, float innerConeAngle, float outerConeAngle, GLfloat constantAttenuation, GLfloat linearAttenuation, GLfloat quadraticAttenuation)
	: Light(Light::Spot_Light, __property_count__, color, intensity)
	, mWorldPosition(position, 1.f)
	, mWorldDirection(direction, 0.f)
	, mInnerConeAngle(innerConeAngle)
	, mOuterConeAngle(outerConeAngle)
	//, mInnerConeCos(glm::cos(innerConeAngle))
	//, mOuterConeCos(glm::cos(outerConeAngle))
{
	SetProperty(glm::vec4(direction.x, direction.y, direction.z, glm::cos(mInnerConeAngle)), Direction_Property);
	SetProperty(glm::vec4(constantAttenuation, linearAttenuation, quadraticAttenuation, glm::cos(mOuterConeAngle)), Attenuation_Property);
}


SpotLight::~SpotLight()
{

}

void SpotLight::TransformInViewCoords(const glm::mat4 & viewMatrix)
{
	glm::vec4 pos = viewMatrix * mWorldPosition;
	SetProperty(pos, Position_Property);

	glm::vec4 dir = viewMatrix * mWorldDirection;
	GLfloat * dirProp = GetProperty(Direction_Property);
	memcpy(dirProp, glm::value_ptr(dir), 3 * sizeof(GLfloat));
}

void SpotLight::SetInnerConeAngle(float value)
{
	mInnerConeAngle = value;
	GLfloat * posProp = GetProperty(Direction_Property);
	posProp[3] = glm::cos(mInnerConeAngle);
}

void SpotLight::SetOuterConeAngle(float value)
{
	mOuterConeAngle = value;
	GLfloat * prop = GetProperty(Attenuation_Property);
	prop[3] = glm::cos(mOuterConeAngle);
}



	} // namespace Light
} // namespace CoreFx
