#pragma once
#ifndef COREFX_FRAME_H
#define COREFX_FRAME_H


namespace CoreFx
{
	// =======================================================================
	// =======================================================================




class Frame 
{
public:

	Frame();
	~Frame();

public:

	inline void SetRotation(const glm::quat& q);

	inline void SetPosition(glm::vec4 const & v);
	inline void SetPosition(float x, float y, float z);
	inline void SetPosition(glm::vec3 const & p);

	inline glm::vec3 const & GetPosition() const;
	inline glm::quat const & GetRotation() const;

	inline glm::mat4 const & GetMatrix() const;
	inline glm::mat4 const & GetMatrixInverse() const;

	inline glm::vec3 GetRight() const;
	inline glm::vec3 GetUp() const;
	inline glm::vec3 GetLook() const;


	void BuildMatrix();

	inline bool HasNewValue() const;
	inline void ClearHasNewValue();

private:

	Frame(const Frame& rhs) = delete;
	Frame& operator=(const Frame& rhs) = delete;

private:

	glm::mat4 mMatrix;
	glm::mat4 mInvMatrix;
	glm::quat mRotation;
	glm::vec3 mPosition;
	glm::vec3 mScale;

	bool mHasNewValue;
	bool mIsDirty;
};








inline void Frame::SetRotation(const glm::quat& q)
{
	mRotation = q;
	mIsDirty = true;
}

inline void Frame::SetPosition(glm::vec4 const & v)
{
	mPosition.x = v.x;
	mPosition.y = v.y;
	mPosition.z = v.z;
	mIsDirty = true;
}

inline void Frame::SetPosition(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
	mIsDirty = true;
}

inline void Frame::SetPosition(glm::vec3 const & p)
{
	mPosition = p;
	mIsDirty = true;
}

inline glm::vec3 const & Frame::GetPosition() const
{
	return mPosition;
}

inline glm::quat const & Frame::GetRotation() const
{
	return mRotation;
}

inline glm::mat4 const & Frame::GetMatrix() const
{
	return mMatrix;
}

inline glm::mat4 const & Frame::GetMatrixInverse() const
{
	return mInvMatrix;
}

inline glm::vec3 Frame::GetRight() const
{
	return glm::vec3(mMatrix[0]);
	//return glm::vec3(mMatrix[0].x, mMatrix[1].x, mMatrix[2].x);
}

inline glm::vec3 Frame::GetUp() const
{
	return glm::vec3(mMatrix[1]);
	//return glm::vec3(mMatrix[0].y, mMatrix[1].y, mMatrix[2].y);
}

inline glm::vec3 Frame::GetLook() const
{
	return glm::vec3(mMatrix[2]);
	//return glm::vec3(mMatrix[0].z, mMatrix[1].z, mMatrix[2].z);
}

inline bool Frame::HasNewValue() const
{
	return mHasNewValue;
}

inline void Frame::ClearHasNewValue()
{
	mHasNewValue = false;
}




	// =======================================================================
	// =======================================================================
} // namespace CoreFx
#endif // COREFX_FRAME_H