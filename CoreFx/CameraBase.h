#pragma once

#ifndef COREFX_CAMERABASE_H
#define COREFX_CAMERABASE_H



namespace CoreFx
{




class CameraBase
{
public:
	CameraBase();
	~CameraBase();
	 
	void SetupProjection(const float fovy=45.0f, const float aspectRatio=1.33333f);
	
	virtual void Update() = 0;
	virtual void Rotate(const float yaw, const float pitch, const float roll) = 0;
	//virtual void Translate(const float dx, const float dy, const float dz) = 0;

	const glm::mat4 GetViewMatrix() const;
	const glm::mat4 GetProjectionMatrix() const;

	void SetPosition(const glm::vec3 v);
	const glm::vec3 GetPosition() const;

	static glm::mat4 GetMatrixUsingYawPitchRoll(const float yaw, const float pitch, const float roll);

	const float GetFOV() const;
	const float GetAspectRatio() const; 

	
protected:	 

	static glm::vec3 UP;

	glm::mat4 _V; //view matrix
	glm::mat4 _P; //projection matrix

	glm::vec3 _look;
	glm::vec3 _up;
	glm::vec3 _right; 
	glm::vec3 _position;
	
	float _fov, _aspectRatio;
};



} // namespace Core
#endif // COREFX_CAMERABASE_H