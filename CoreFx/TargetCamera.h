#pragma once

#ifndef COREFX_TARGETCAMERA_H
#define COREFX_TARGETCAMERA_H

namespace CoreFx
{




class TargetCamera :
	public CameraBase
{
public:
	TargetCamera();
	~TargetCamera();

	void Update();
	void Rotate(const float yaw, const float pitch, const float roll);
	 
	void SetTarget(const glm::vec3 tgt);
	const glm::vec3 GetTarget() const;

	void Pan(const float dx, const float dy);
	void Zoom(const float amount );
	void Move(const float dx, const float dz);

protected:
	glm::vec3 _target;  
	float _yaw, _pitch, _roll;
	 
	float _minRy, _maxRy;
	float _distance;
	float _minDistance, _maxDistance;

};




} // namespace CoreFx

#endif // COREFX_TARGETCAMERA_H