#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{



SceneObject::SceneObject(SceneObjectType objectType)
	: mFrame(objectType != SceneObjectTypeId::NoLocationObject ? Engine::GetInstance()->CreateObjectLocation(objectType) : nullptr)
{
}


SceneObject::~SceneObject()
{
	Engine::GetInstance()->DeleteObjectLocation(mFrame);
}

//void SceneObject::Update()
//{
//	if (mFrame != nullptr)
//		mFrame->BuildMatrix();
//}


} // namespace CoreFx