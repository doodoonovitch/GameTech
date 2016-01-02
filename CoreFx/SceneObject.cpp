#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{



SceneObject::SceneObject(bool hasFrame /*= false*/)
	: m_pFrame(hasFrame ? new Frame() : nullptr)
{
}


SceneObject::~SceneObject()
{
	delete m_pFrame;
	m_pFrame = nullptr;
}

void SceneObject::Update()
{
	if (m_pFrame != nullptr)
		m_pFrame->BuildMatrix();
}


} // namespace CoreFx