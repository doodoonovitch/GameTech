#pragma once
#ifndef COREFX_MATHS_DUALQUATERNION_H
#define COREFX_MATHS_DUALQUATERNION_H


namespace CoreFx
{
	// =======================================================================
	// =======================================================================




class DualQuaternion 
{
public:

	DualQuaternion()
		: m_qReal()
		, m_qDual(0.f, 0.f, 0.f, 0.f)
	{
	}

	DualQuaternion(Quaternion const & qReal, Quaternion const & qDual)
		: m_qReal(glm::normalize(qReal.m_q))
		, m_qDual(qDual)
	{
	}

	DualQuaternion(Quaternion const & q, glm::vec3 t)
	{
		FromRotTrans(q.m_q, t);
	}

	~DualQuaternion()
	{
	}

	DualQuaternion(const DualQuaternion& rhs)
		: m_qReal(rhs.m_qReal)
		, m_qDual(rhs.m_qDual)
	{
	}

	DualQuaternion& operator=(DualQuaternion const & rhs)
	{
		if (this != &rhs)
		{
			m_qReal = rhs.m_qReal;
			m_qDual = rhs.m_qDual;
		}
		return *this;
	}

	DualQuaternion& operator+=(DualQuaternion const & rhs)
	{
		m_qReal.m_q += rhs.m_qReal.m_q;
		m_qDual.m_q += rhs.m_qDual.m_q;
		return *this;
	}

	DualQuaternion& operator*=(DualQuaternion const & rhs)
	{
		Quaternion qReal(m_qReal);
		m_qReal.m_q = m_qReal.m_q * rhs.m_qReal.m_q;
		m_qDual.m_q = qReal.m_q * rhs.m_qDual.m_q + m_qDual.m_q * rhs.m_qReal.m_q;
		return *this;
	}

	DualQuaternion& operator*=(float s)
	{
		m_qReal.m_q *= s;
		m_qDual.m_q *= s;
		return *this;
	}

	DualQuaternion& operator/=(float s)
	{
		m_qReal.m_q /= s;
		m_qDual.m_q /= s;
		return *this;
	}

	DualQuaternion& Normalize()
	{
		float mag = Dot(m_qReal, m_qReal);
		assert(mag > 0.000001f);
		m_qReal.m_q *= 1.0f / mag;
		m_qDual.m_q *= 1.0f / mag;
		return *this;
	}

public:

	friend float Dot(DualQuaternion const & a, DualQuaternion const & b)
	{
		return Dot(a.m_qReal, b.m_qReal);
	}

	friend DualQuaternion Normalize(DualQuaternion const & q)
	{
		float mag = Dot(q.m_qReal, q.m_qReal);
		assert(mag > 0.000001f);
		DualQuaternion ret = q;
		ret.m_qReal *= 1.0f / mag;
		ret.m_qDual *= 1.0f / mag;
		return ret;
	}

	friend DualQuaternion operator*(DualQuaternion const & lhs, DualQuaternion const & rhs) 
	{
		return DualQuaternion(lhs.m_qReal.m_q * rhs.m_qReal.m_q, lhs.m_qReal.m_q * rhs.m_qDual.m_q + lhs.m_qDual.m_q * rhs.m_qReal.m_q);
	}

	friend DualQuaternion operator*(float scale, DualQuaternion const & q)
	{
		return DualQuaternion(scale * q.m_qReal.m_q, scale * q.m_qDual.m_q);
	}

	friend DualQuaternion operator*(DualQuaternion const & q, float scale)
	{
		return DualQuaternion(scale * q.m_qReal.m_q, scale * q.m_qDual.m_q);
	}

	friend DualQuaternion operator+(DualQuaternion const & lhs, DualQuaternion const & rhs) 
	{
		return DualQuaternion(lhs.m_qReal.m_q + rhs.m_qReal.m_q, lhs.m_qDual.m_q + rhs.m_qDual.m_q);
	}

protected:

	DualQuaternion(glm::quat const & qReal, glm::quat const &qDual)
		: m_qReal(qReal)
		, m_qDual(qDual)
	{}

	void FromRotTrans(glm::quat const & q, glm::vec3 t)
	{
		m_qReal.m_q = q;

		m_qDual.m_q.w = -0.5f * ( t.x * q.x + t.y * q.y + t.z * q.z);
		m_qDual.m_q.x =  0.5f * ( t.x * q.w + t.y * q.z - t.z * q.y);
		m_qDual.m_q.y =  0.5f * (-t.x * q.z + t.y * q.w + t.z * q.x);
		m_qDual.m_q.z =  0.5f * ( t.x * q.y - t.y * q.x + t.z * q.w);
	}

private:

	Quaternion m_qReal;
	Quaternion m_qDual;

};






	// =======================================================================
	// =======================================================================
} // namespace CoreFx
#endif // COREFX_MATHS_DUALQUATERNION_H