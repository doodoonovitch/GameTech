#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	// =======================================================================
	// =======================================================================





Frame::Frame(SceneObjectType sceneObjectType)
	: mSceneObjectType(sceneObjectType)
	, mDQ()
	, mScale(1.f)
	, mIsModified(true)
{
}


Frame::~Frame()
{
}





	// =======================================================================
	// =======================================================================
} // namespace CoreFx
