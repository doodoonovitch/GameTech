#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



Light::Light(LightType lightType, GLuint propertyCount, glm::vec3 const & ambient, glm::vec3 const & diffuse, glm::vec3 const & specular)
	: SceneObject(true)
	, mLightDesc( ((GLuint)lightType) & LIGHT_TYPE_MASK )
	, mPropertyData(propertyCount)
{
	SetProperty(glm::vec4(ambient, 1.f), Ambient_Color_Property);
	SetProperty(glm::vec4(diffuse, 1.f), Diffuse_Color_Property);
	SetProperty(glm::vec4(specular, 1.f), Specular_Color_Property);
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
