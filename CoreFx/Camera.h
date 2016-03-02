#pragma once

#ifndef COREFX_CAMERABASE_H
#define COREFX_CAMERABASE_H



namespace CoreFx
{




class Camera : public SceneObject
{

public:

	Camera();
	~Camera();

	void Update();

	// Get/Set world camera position.
	inline glm::vec3 GetPosition() const;
	inline void SetPosition(glm::vec4 const & v);
	inline void SetPosition(float x, float y, float z);
	inline void SetPosition(glm::vec3 const & p);

	// Get camera basis vectors.
	inline const glm::vec3 & GetRight() const;
	inline const glm::vec3 & GetUp() const;
	inline const glm::vec3 & GetLook() const;

	// Get frustum properties.
	inline float GetNearZ() const;
	inline float GetFarZ() const;
	inline float GetAspect() const;
	inline float GetFovY() const;
	inline float GetFovX() const;

	// Get near and far plane dimensions in view space coordinates.
	inline float GetNearWindowWidth() const;
	inline float GetNearWindowHeight() const;
	inline float GetFarWindowWidth() const;
	inline float GetFarWindowHeight() const;

	void SetupProjection(const float fovy=45.0f, const float aspectRatio=1.33333f);

	const float GetFOV() const;
	const float GetAspectRatio() const;

	void LookAt(glm::vec3 const & position, glm::vec3 const & target, glm::vec3 const & up);

	inline const glm::mat4 & GetProjectionMatrix() const;

	inline const Maths::DualQuaternion & GetViewDQ() const;

	const glm::mat4 GetViewMatrix() const;


public:

	// Rotate the camera.
	void Pitch(float angle);
	void Yaw(float angle);
	void Roll(float angle);

	// Strafe/Walk the camera a distance d.
	void Strafe(float d);
	void Walk(float d);


public:

	static glm::mat4 GetMatrixUsingYawPitchRoll(const float yaw, const float pitch, const float roll);

	
protected:	 

	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;


	static glm::vec3 UP;

	glm::mat4 mProj; //projection matrix
	Maths::DualQuaternion mViewDQ;
	glm::vec3 mRight;
	glm::vec3 mUp;
	glm::vec3 mLook;

	
	float mFov, mAspectRatio;
};


// =======================================================================
// =======================================================================


inline glm::vec3 Camera::GetPosition() const
{
	return GetFrame()->GetPosition();
}

inline void Camera::SetPosition(glm::vec4 const & v)
{
	GetFrame()->SetPosition(v);
}

inline void Camera::SetPosition(float x, float y, float z)
{
	GetFrame()->SetPosition(x, y, z);
}

inline void Camera::SetPosition(glm::vec3 const & p)
{
	GetFrame()->SetPosition(p);
}

inline const glm::vec3 & Camera::GetRight() const
{
	return mRight;
}

inline const glm::vec3 & Camera::GetUp() const
{
	return mUp;
}

inline const glm::vec3 & Camera::GetLook() const
{
	return mLook;
}

inline float Camera::GetNearZ() const
{
	return mNearZ;
}

inline float Camera::GetFarZ() const
{
	return mFarZ;
}

inline float Camera::GetAspect() const
{
	return mAspect;
}

inline float Camera::GetFovY() const
{
	return mFovY;
}

inline float Camera::GetFovX() const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / mNearZ);
}

// Get near and far plane dimensions in view space coordinates.
inline float Camera::GetNearWindowWidth()const
{
	return mAspect * mNearWindowHeight;
}

inline float Camera::GetNearWindowHeight()const
{
	return mNearWindowHeight;
}

inline float Camera::GetFarWindowWidth()const
{
	return mAspect * mFarWindowHeight;
}

inline float Camera::GetFarWindowHeight()const
{
	return mFarWindowHeight;
}

inline const Maths::DualQuaternion & Camera::GetViewDQ() const
{
	return mViewDQ;
}

inline const float Camera::GetFOV() const
{
	return mFov;
}

inline const float Camera::GetAspectRatio() const
{
	return mAspectRatio;
}

inline const glm::mat4 & Camera::GetProjectionMatrix() const
{
	return mProj;
}



} // namespace Core
#endif // COREFX_CAMERABASE_H