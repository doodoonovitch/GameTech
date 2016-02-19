#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{



SceneObject::SceneObject(bool hasFrame /*= false*/)
	: mFrame(hasFrame ? new Frame() : nullptr)
{
}


SceneObject::~SceneObject()
{
	delete mFrame;
	mFrame = nullptr;
}

//void SceneObject::Update()
//{
//	if (mFrame != nullptr)
//		mFrame->BuildMatrix();
//}


} // namespace CoreFx