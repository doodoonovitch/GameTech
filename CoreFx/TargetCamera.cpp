#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{




TargetCamera::TargetCamera(void)
{  
	_right = glm::vec3(1,0,0);
	_up = glm::vec3(0,1,0);
	_look = glm::vec3(0,0,-1);
	_minRy = -60;
	_maxRy = 60;
	_minDistance = 1;
	_maxDistance = 10;
}


TargetCamera::~TargetCamera(void)
{
}
 
void TargetCamera::Update() 
{ 
	glm::mat4 R = glm::yawPitchRoll(glm::radians(_yaw), glm::radians(_pitch), 0.0f);
	glm::vec3 T = glm::vec3(0, 0, _distance);
	T = glm::vec3(R * glm::vec4(T, 0.0f));
	_position = _target + T;
	_look = glm::normalize(_target - _position);
	_up = glm::vec3(R * glm::vec4(UP,0.0f));
	_right = glm::cross(_look, _up);
	_V = glm::lookAt(_position, _target, _up); 
}

void TargetCamera::SetTarget(const glm::vec3 tgt) 
{
	_target = tgt; 
	_distance = glm::distance(_position, _target);
	_distance = std::max(_minDistance, std::min(_distance, _maxDistance));
	_V = glm::lookAt(_position, _target, _up);

	_yaw = 0;
	_pitch = 0;

	if(_V[0][0] < 0)
		_yaw = glm::degrees((float)(M_PI - asinf(-_V[2][0])) );
	else
		_yaw = glm::degrees(asinf(-_V[2][0]));

	_pitch = glm::degrees(asinf(-_V[1][2]));  
}

const glm::vec3 TargetCamera::GetTarget() const 
{
	return _target;
} 

void TargetCamera::Rotate(const float yaw, const float pitch, const float /*roll*/) 
{
 	_yaw += yaw;
	_pitch += pitch;
 	_pitch = std::min(std::max(_pitch, _minRy), _maxRy);
	Update();
}
 
void TargetCamera::Pan(const float dx, const float dy) 
{
	glm::vec3 X = _right * dx;
	glm::vec3 Y = _up * dy;
	_position += X + Y;
	_target += X + Y;
	Update();
}

 
void TargetCamera::Zoom(const float amount) 
{
	_position += _look * amount;
	_distance = glm::distance(_position, _target); 
	_distance = std::max(_minDistance, std::min(_distance, _maxDistance));
	Update();
}
 
void TargetCamera::Move(const float dx, const float dy) 
{
	glm::vec3 X = _right * dx;
	glm::vec3 Y = _look * dy;
	_position += X + Y;
	_target += X + Y;
	Update();
}




} // namespace CoreFx
