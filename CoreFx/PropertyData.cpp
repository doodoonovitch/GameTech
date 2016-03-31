#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{



PropertyData::PropertyData(GLuint propertyCount)
	: mData(propertyCount > 0 ? new GLfloat[propertyCount * Element_Per_Property] : nullptr)
	, mPropertyCount(propertyCount)
	, mIsModified(true)
{
}


PropertyData::~PropertyData()
{
	SAFE_DELETE_ARRAY(mData);
}

void PropertyData::SetProperty(const GLbitfield values[4], GLuint propertyIndex)
{
	assert(propertyIndex < mPropertyCount);
	memcpy(&mData[Element_Per_Property * propertyIndex], values, GetPropertySize());
	SetIsModified(true);
}

void PropertyData::SetProperty(const GLfloat values[4], GLuint propertyIndex)
{
	assert(propertyIndex < mPropertyCount);
	memcpy(&mData[Element_Per_Property * propertyIndex], values, GetPropertySize());
	SetIsModified(true);
}

void PropertyData::SetProperty(const glm::vec4 & value, GLuint propertyIndex)
{
	assert(propertyIndex < mPropertyCount);
	memcpy(&mData[Element_Per_Property * propertyIndex], glm::value_ptr(value), GetPropertySize());
	SetIsModified(true);
}

void PropertyData::SetProperty(const glm::vec3 & value, GLuint propertyIndex)
{
	assert(propertyIndex < mPropertyCount);
	memcpy(&mData[Element_Per_Property * propertyIndex], glm::value_ptr(value), 3 * sizeof(GLfloat));
	SetIsModified(true);
}



} // namespace CoreFx
