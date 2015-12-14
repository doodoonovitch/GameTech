#pragma once

#ifndef COREFX_FREECAMERA_H
#define COREFX_FREECAMERA_H

namespace CoreFx
{



class FreeCamera :	public CameraBase
{
public:
	FreeCamera();
	~FreeCamera();

	void Update();
	void Rotate(const float yaw, const float pitch, const float roll);
	void Walk(const float amount);
	void Strafe(const float amount);
	void Lift(const float amount);
	 

protected:

	float _yaw, _pitch, _roll;
	
	glm::vec3 _translation;
};




} // namespace Core
#endif // COREFX_FREECAMERA_H