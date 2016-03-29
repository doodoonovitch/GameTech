#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



Light::Light(LightType lightType, GLuint propertyCount, glm::vec3 const & color, glm::vec3 const & intensity)
	: SceneObject(true)
	, mLightDesc( ((GLuint)lightType) & LIGHT_TYPE_MASK )
	, mPropertyData(propertyCount)
{
	SetProperty(glm::vec4(color, 1.f), Color_Property);
	SetProperty(intensity, Intensity_Property);
}

Light::Light(LightType lightType, GLuint propertyCount, glm::vec3 const & color, GLfloat ambientIntensity, GLfloat diffuseIntensity, GLfloat specularIntensity)
: SceneObject(true)
, mLightDesc(((GLuint)lightType) & LIGHT_TYPE_MASK)
, mPropertyData(propertyCount)
{
	SetProperty(glm::vec4(color, 1.f), Color_Property);
	SetProperty(glm::vec3(ambientIntensity, diffuseIntensity, specularIntensity), Intensity_Property);
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
