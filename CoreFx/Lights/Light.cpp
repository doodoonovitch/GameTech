#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



Light::Light(LightType lightType, GLuint propertyCount, glm::vec3 const & color, GLfloat intensity, GLfloat radius)
	: SceneObject(true)
	, mLightDesc( ((GLuint)lightType) & LIGHT_TYPE_MASK )
	, mPropertyData(propertyCount)
	, mColor(color)
	, mIntensity(intensity)
{
	glm::vec3 colorIntensity(color * intensity);
	SetProperty(glm::vec4(colorIntensity, radius), ColorIntensity_Property);
}


Light::~Light()
{
}

void Light::SetProperty(const glm::vec4 & value, GLuint propertyIndex)
{
	mPropertyData.SetProperty(value, propertyIndex);
}

void Light::SetProperty(const glm::vec3 & value, GLuint propertyIndex)
{
	mPropertyData.SetProperty(value, propertyIndex);
}



	} // namespace Light
} // namespace CoreFx
