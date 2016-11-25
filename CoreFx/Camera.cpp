#include "stdafx.h"
#include "CoreFx.h" 


namespace CoreFx
{



glm::vec3 Camera::UP = glm::vec3(0,1,0);

Camera::Camera(void)
	: SceneObject(true)
	, mNearZ(1.f)
	, mFarZ(1000.0f)
{  
}


Camera::~Camera(void)
{
}

void Camera::Update()
{
	if (GetFrame()->IsModified())
	{
		mViewDQ =  GetFrame()->GetDualQuaternion().GetConjugate();
		mView = mViewDQ.GetMatrix();
		mViewProj = mProj * mView;

		glm::mat4 m = GetFrame()->GetDualQuaternion().GetMatrix();
		mRight = glm::normalize(glm::vec3(m[0]));
		mUp = glm::normalize(glm::vec3(m[1]));
		mLook = glm::normalize(glm::vec3(m[2]));
		GetFrame()->SetIsModified(false);
	}
}

void Camera::SetupProjection(const float fovy, const float aspRatio, float zNear, float zFar)
{
	mNearZ = zNear;
	mFarZ = zFar;
	mFovY = fovy;
	mAspect = aspRatio;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f*mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f*mFovY);

	mProj = glm::perspective(mFovY, mAspect, mNearZ, mFarZ);
	mInvProj = glm::inverse(mProj);
} 

glm::mat4 Camera::GetMatrixUsingYawPitchRoll(const float yaw, const float pitch, const float roll)
{	 
	return glm::yawPitchRoll(yaw, pitch, roll);  
}

void Camera::LookAt(glm::vec3 const & position, glm::vec3 const & target, glm::vec3 const & up)
{
	GetFrame()->SetPosition(position);

	glm::vec3 zAxis = glm::normalize(GetFrame()->GetPosition() - target);
	glm::vec3 xAxis = glm::normalize(glm::cross(up, zAxis));
	glm::vec3 yAxis = glm::cross(zAxis, xAxis);

	glm::mat3 m(xAxis, yAxis, zAxis);	

	GetFrame()->SetRotation(glm::quat(m));
}
 
void Camera::Pitch(float angle)
{
	glm::quat q = glm::angleAxis(angle, GetRight()) * GetFrame()->GetRotation();
	GetFrame()->SetRotation(q);
}

void Camera::Yaw(float angle)
{
	glm::quat q = glm::angleAxis(angle, GetUp()) * GetFrame()->GetRotation();
	GetFrame()->SetRotation(q);
}

void Camera::Roll(float angle)
{
	glm::quat q = glm::angleAxis(angle, GetLook()) * GetFrame()->GetRotation();
	GetFrame()->SetRotation(q);
}


void Camera::Strafe(float d)
{
	glm::vec3 v = GetRight() * d;
	glm::vec3 p = v + GetPosition();
	GetFrame()->SetPosition(p);
}

void Camera::Walk(float d)
{
	glm::vec3 v = GetLook() * -d;
	glm::vec3 p = v + GetPosition();
	GetFrame()->SetPosition(p);
}

void Camera::SlideUp(float d)
{
	glm::vec3 v = GetUp() * d;
	glm::vec3 p = v + GetPosition();
	GetFrame()->SetPosition(p);
}


} // namespace Core

