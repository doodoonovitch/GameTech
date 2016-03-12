#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{


Renderer::Renderer(GLuint propertyCount)
	: mMaterials(propertyCount)
	, mMaterialBaseIndex(0)
{
}


Renderer::~Renderer()
{
}



} // namespace CoreFx
