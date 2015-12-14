#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{

FreeCamera::FreeCamera()
{
	_translation =glm::vec3(0);
}


FreeCamera::~FreeCamera(void)
{
}
 
void FreeCamera::Update() 
{
	glm::mat4 R = GetMatrixUsingYawPitchRoll(_yaw, _pitch, _roll); 
	_position += _translation;
	_translation = glm::vec3(0);

	_look = glm::vec3(R * glm::vec4(0,0,1,0));
	glm::vec3 tgt  = _position + _look;
	_up   = glm::vec3(R * glm::vec4(0,1,0,0));
	_right = glm::cross(_look, _up);
	_V = glm::lookAt(_position, tgt, _up);

	//normalize
	//look = glm::normalize(look);
	//up = glm::normalize(up);
	//right = glm::normalize(right);
}

void FreeCamera::Rotate(const float y, const float p, const float r) 
{
	_yaw = y;
	_pitch = p;
	_roll = r;
}


void FreeCamera::Walk(const float amount)
{
	_translation += (_look * amount);
}

void FreeCamera::Strafe(const float amount)
{
	_translation += (_right  * amount);
}

void FreeCamera::Lift(const float amount) 
{
	_translation += (_up * amount);
}



} // namespace CoreFx