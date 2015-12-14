#include "stdafx.h"
#include "CoreFx.h" 


namespace CoreFx
{



glm::vec3 CameraBase::UP = glm::vec3(0,1,0);

CameraBase::CameraBase(void)
{  
}


CameraBase::~CameraBase(void)
{
}

void CameraBase::SetupProjection(const float fovy, const float aspRatio) 
{
	_P = glm::perspective(fovy, aspRatio, 0.1f, 1000.0f); 
	_fov = fovy;
	_aspectRatio = aspRatio;
} 

const glm::mat4 CameraBase::GetViewMatrix() const 
{
	return _V;
}

const glm::mat4 CameraBase::GetProjectionMatrix() const 
{
	return _P;
}

const glm::vec3 CameraBase::GetPosition() const 
{
	return _position;
}

void CameraBase::SetPosition(const glm::vec3 p) 
{
	_position = p;
}
 
glm::mat4 CameraBase::GetMatrixUsingYawPitchRoll(const float yaw, const float pitch, const float roll) 
{
	 
	return glm::yawPitchRoll(yaw, pitch, roll);  
}

const float CameraBase::GetFOV() const 
{
	return _fov;
} 

const float CameraBase::GetAspectRatio() const 
{
	return _aspectRatio;
}

 


} // namespace Core

