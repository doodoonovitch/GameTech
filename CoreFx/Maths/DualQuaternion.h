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

	glm::mat4 GetInverseMatrix() const
	{
		return ToInverseMatrix(*this);
	}

	DualQuaternion GetConjugate() const
	{
		return Conjugate(*this);
	}


public:

	friend DualQuaternion Conjugate(DualQuaternion const & a)
	{
		return DualQuaternion(glm::conjugate(a.mQr), glm::conjugate(a.mQd));
	}

	friend float Dot(DualQuaternion const & a, DualQuaternion const & b)
	{
		return glm::dot(a.mQr, b.mQr);
	}

	friend DualQuaternion Normalize(DualQuaternion const & q)
	{
		float mag = glm::dot(q.mQr, q.mQr);
		assert(mag > 0.000001f);
		DualQuaternion ret;
		ret.mQr = q.mQr / mag;
		ret.mQd = q.mQd / mag;
		return ret;
	}

	friend DualQuaternion operator*(float scale, DualQuaternion const & q)
	{
		return DualQuaternion(scale * q.mQr, scale * q.mQd);
	}

	friend DualQuaternion operator*(DualQuaternion const & q, float scale)
	{
		return DualQuaternion(scale * q.mQr, scale * q.mQd);
	}

	friend DualQuaternion operator*(DualQuaternion const & lhs, DualQuaternion const & rhs)
	{
		return DualQuaternion(lhs.mQr * rhs.mQr, lhs.mQr * rhs.mQd + lhs.mQd * rhs.mQr);
	}

	friend DualQuaternion operator+(DualQuaternion const & lhs, DualQuaternion const & rhs) 
	{
		return DualQuaternion(lhs.mQr + rhs.mQr, lhs.mQd + rhs.mQd);
	}


	friend glm::mat4 ToMatrix(DualQuaternion const & q)
	{
		glm::mat4 m = glm::mat4_cast(q.mQr);
		glm::vec3 t = q.GetTranslation();
		m[3].x = t.x;
		m[3].y = t.y;
		m[3].z = t.z;
		return m;
	}

	friend glm::mat4 ToInverseMatrix(DualQuaternion const & q)
	{
		glm::mat4 m = glm::transpose(glm::mat4_cast(q.mQr));
		glm::mat4 t = glm::translate(-q.GetTranslation());
		glm::mat4 inv = m * t;
		return inv;
	}

private:

};






	// =======================================================================
	// =======================================================================
	} // namespace Maths
} // namespace CoreFx
#endif // COREFX_MATHS_DUALQUATERNION_H