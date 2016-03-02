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

	inline glm::vec3 GetPosition() const;
	inline glm::quat GetRotation() const;

	inline bool IsModified() const;
	inline void SetIsModified(bool value);

	const Maths::DualQuaternion & GetDualQuaternion() const;

private:

	Frame(const Frame& rhs) = delete;
	Frame& operator=(const Frame& rhs) = delete;

private:

	Maths::DualQuaternion mDQ;

	bool mIsModified;
};








inline bool Frame::IsModified() const
{
	return mIsModified;
}

inline void Frame::SetIsModified(bool value)
{
	mIsModified = value;
}

inline const Maths::DualQuaternion & Frame::GetDualQuaternion() const
{
	return mDQ;
}

inline void Frame::SetRotation(const glm::quat& q)
{
	//mRotation = q;
	mDQ.SetRotation(q);
	mIsModified = true;
}

inline void Frame::SetPosition(glm::vec4 const & v)
{
	//mPosition.x = v.x;
	//mPosition.y = v.y;
	//mPosition.z = v.z;
	mDQ.SetTranslation(glm::vec3(v));
	mIsModified = true;
}

inline void Frame::SetPosition(float x, float y, float z)
{
	//mPosition.x = x;
	//mPosition.y = y;
	//mPosition.z = z;
	mDQ.SetTranslation(glm::vec3(x, y, z));
	mIsModified = true;
}

inline void Frame::SetPosition(glm::vec3 const & p)
{
	//mPosition = p;
	mDQ.SetTranslation(p);
	mIsModified = true;
}

inline glm::vec3 Frame::GetPosition() const
{
	//return mPosition;
	return mDQ.GetTranslation();
}

inline glm::quat Frame::GetRotation() const
{
	//return mRotation;
	return mDQ.GetRotation();
}





	// =======================================================================
	// =======================================================================
} // namespace CoreFx
#endif // COREFX_FRAME_H