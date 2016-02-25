#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Lights
	{



Light::Light(LightType lightType, GLuint propertyCount)
	: SceneObject(true)
	, mLightDesc( ((GLuint)lightType) & LIGHT_TYPE_MASK )
	, mData(new GLfloat[propertyCount * 4])
	, mPropertyCount(propertyCount)
	, mIsModified(true)
{
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



	} // namespace Light
} // namespace CoreFx
