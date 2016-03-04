#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



Light::Light(LightType lightType, GLuint propertyCount, glm::vec3 const & ambient, glm::vec3 const & diffuse, glm::vec3 const & specular)
	: SceneObject(true)
	, mLightDesc( ((GLuint)lightType) & LIGHT_TYPE_MASK )
	, mData(new GLfloat[propertyCount * 4])
	, mPropertyCount(propertyCount)
	, mIsModified(true)
{
	SetProperty(glm::vec4(ambient, 1.f), Ambient_Color_Property);
	SetProperty(glm::vec4(diffuse, 1.f), Diffuse_Color_Property);
	SetProperty(glm::vec4(specular, 1.f), Specular_Color_Property);
}


Light::~Light()
{
	SAFE_DELETE_ARRAY(mData);
}

void Light::SetProperty(const glm::vec4 & value, GLuint propertyIndex)
{
	assert(propertyIndex < mPropertyCount);
	memcpy(&mData[4 * propertyIndex], glm::value_ptr(value), GetPropertySize());
	SetIsModified(true);
}

void Light::SetProperty(const glm::vec3 & value, GLuint propertyIndex)
{
	assert(propertyIndex < mPropertyCount);
	memcpy(&mData[4 * propertyIndex], glm::value_ptr(value), sizeof(glm::vec3));
	SetIsModified(true);
}



	} // namespace Light
} // namespace CoreFx
