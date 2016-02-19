#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Geometry
	{

MaterialData::MaterialData()
	: mNormalTexture(-1)
	, mAbientTexture(-1)
	, mDiffuseTexture(-1)
	, mReflectionTexture(-1)
	, mSpecularTexture(-1)
{
}


MaterialData::~MaterialData()
{
}

	} // namespace Geometry
} // namespace Core
