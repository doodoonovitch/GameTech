#pragma once
#ifndef COREFX_MATHS_DUALQUATERNION_H
#define COREFX_MATHS_DUALQUATERNION_H


namespace CoreFx
{
	namespace Maths
	{
	// =======================================================================
	// =======================================================================



struct DualQuat
{
	DualQuat()
		: mQr(0.f, 0.f, 0.f, 1.f)
		, mQd(0.f, 0.f, 0.f, 0.f)
	{}

	DualQuat(glm::quat const & qReal, glm::quat const & qDual)
		: mQr(glm::normalize(qReal))
		, mQd(qDual)
	{}

	DualQuat(const DualQuat & rhs)
		: mQr(rhs.mQr)
		, mQd(rhs.mQd)
	{}

	glm::quat mQr;
	glm::quat mQd;
};



class DualQuaternion : public DualQuat
{

public:

	DualQuaternion()
		: DualQuat()
	{
	}

	DualQuaternion(glm::quat const & qReal, glm::quat const & qDual)
		: DualQuat(qReal, qDual)
	{
	}

	DualQuaternion(glm::quat const & q, glm::vec3 t)
	{
		SetRotationTranslation(q, t);
	}

	~DualQuaternion()
	{
	}

	DualQuaternion(const DualQuaternion& rhs)
		: DualQuat(rhs)
	{
	}

	glm::quat const & GetRealPart() const
	{
		return mQr;
	}

	glm::quat const & GetDualPart() const
	{
		return mQd;
	}

	void Set(glm::quat const & realPart, glm::quat const & dualPart)
	{
		mQr = realPart;
		mQd = dualPart;
	}

	void SetRotationTranslation(glm::quat const & rotation, glm::vec3 const & translation)
	{
		mQr = rotation;
		mQd = (0.5f * glm::quat(0, translation)) * rotation;
	}

	void SetRotation(glm::quat const & q)
	{
		glm::vec3 t = GetTranslation();
		SetRotationTranslation(q, t);
	}

	void SetTranslation(glm::vec3 const & translation)
	{
		mQd = (0.5f * glm::quat(0, translation)) * mQr;
	}

	glm::quat GetRotation() const
	{
		return mQr;
	}

	glm::vec3 GetTranslation() const
	{
		glm::quat t = (mQd * 2.0f) * glm::conjugate(mQr);
		return glm::vec3(t.x, t.y, t.z);
	}

	DualQuaternion& operator=(DualQuaternion const & rhs)
	{
		if (this != &rhs)
		{
			mQr = rhs.mQr;
			mQd = rhs.mQd;
		}
		return *this;
	}

	DualQuaternion& operator+=(DualQuaternion const & rhs)
	{
		mQr += rhs.mQr;
		mQd += rhs.mQd;
		return *this;
	}

	DualQuaternion& operator*=(DualQuaternion const & rhs)
	{
		glm::quat qr(mQr);
		mQr = mQr * rhs.mQr;
		mQd = qr * rhs.mQd + mQd * rhs.mQr;
		return *this;
	}

	DualQuaternion& operator*=(float s)
	{
		mQr *= s;
		mQd *= s;
		return *this;
	}

	DualQuaternion& operator/=(float s)
	{
		mQr /= s;
		mQd /= s;
		return *this;
	}

	DualQuaternion& Normalize()
	{
		float mag = glm::dot(mQr, mQr);
		assert(mag > 0.000001f);
		mQr /= mag;
		mQd /= mag;
		return *this;
	}

	glm::mat4 GetMatrix() const
	{
		return ToMatrix(*this);
	}

	DualQuaternion GetConjugate() const
	{
		return Conjugate(*this);
	}


public:
	friend DualQuaternion Conjugate(DualQuaternion const & a);
	friend float Dot(DualQuaternion const & a, DualQuaternion const & b);
	friend DualQuaternion Normalize(DualQuaternion const & q);

	friend DualQuaternion operator*(float scale, DualQuaternion const & q);
	friend DualQuaternion operator*(DualQuaternion const & q, float scale);
	friend DualQuaternion operator*(DualQuaternion const & lhs, DualQuaternion const & rhs);
	friend DualQuaternion operator+(DualQuaternion const & lhs, DualQuaternion const & rhs);

	friend glm::mat4 ToMatrix(DualQuaternion const & dualQuat);

};







	// =======================================================================
	// =======================================================================
	} // namespace Maths
} // namespace CoreFx
#endif // COREFX_MATHS_DUALQUATERNION_H