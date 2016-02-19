#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	// =======================================================================
	// =======================================================================





Frame::Frame()
	: mPosition(0.f, 0.f, 0.f)
	, mScale(1.f, 1.f, 1.f)
	, mHasNewValue(false)
	, mIsDirty(true)
{
}


Frame::~Frame()
{
}

void Frame::BuildMatrix() 
{
	if (mIsDirty)
	{
		glm::mat4 rotMat = glm::mat4_cast(mRotation);
		glm::mat4 scaleMat = glm::scale(mScale);
		glm::mat4 transMat = glm::translate(mPosition);
		mMatrix = transMat * rotMat * scaleMat;

		glm::mat4 invScaleMat = glm::scale(glm::vec3(1.f / mScale.x, 1.f / mScale.y, 1.f / mScale.z));
		glm::mat4 invRotMat = glm::transpose(rotMat);
		glm::mat4 invTransMat = glm::translate(-mPosition);
		mInvMatrix = invScaleMat * invRotMat * invTransMat;

		mIsDirty = false;
		mHasNewValue = true;
	}
}




	// =======================================================================
	// =======================================================================
} // namespace CoreFx
