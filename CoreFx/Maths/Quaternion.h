#pragma once
#ifndef COREFX_MATHS_QUATERNION_H
#define COREFX_MATHS_QUATERNION_H


namespace CoreFx
{
	// =======================================================================
	// =======================================================================


	class DualQuaternion;


class Quaternion 
{
public:

	Quaternion()
		: m_q(1.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	Quaternion(float w, float x, float y, float z)
		: m_q(w, x, y, z)
	{
	}

	Quaternion(glm::vec3 const & normalizedAxis, float angle)
		: m_q(angle, normalizedAxis)
	{
	}

	Quaternion(glm::mat4 const & rotationMatrix)
		: m_q(rotationMatrix)
	{
	}

	~Quaternion()
	{
	}

	Quaternion(Quaternion const & rhs)
		: m_q(rhs.m_q)
	{

	}

	Quaternion& operator=(Quaternion const & rhs)
	{
		if (this != &rhs)
		{
			m_q = rhs.m_q;
		}
		return *this;
	}

	bool operator==(Quaternion const & rhs) const
	{
		if (this == &rhs)
			return true;

		return glm::abs(glm::dot(m_q, rhs.m_q)) < (1.f - glm::epsilon<float>());
	}

	Quaternion& operator+=(Quaternion const & rhs)
	{
		m_q += rhs.m_q;
		return *this;
	}

	Quaternion& operator*=(Quaternion const & rhs)
	{
		m_q *= rhs.m_q;
		return *this;
	}

	Quaternion& operator*=(float s)
	{
		m_q *= s;
		return *this;
	}

	Quaternion& operator/=(float s)
	{
		m_q /= s;
		return *this;
	}

	Quaternion& Normalize()
	{
		m_q = glm::normalize(m_q);
		return *this;
	}

public:

	Quaternion& FromAxisAngle(glm::vec3 const & axis, float angle)
	{
		glm::vec3 normalizedAxis = glm::normalize(axis);
		return FromNormalizedAxisAngle(normalizedAxis, angle);
	}

	Quaternion& FromNormalizedAxisAngle(glm::vec3 const & axis, float angle)
	{
		m_q = glm::angleAxis(angle, axis);
		return *this;
	}

	Quaternion& FromRotationMatrix(glm::mat4 const & rotationMatrix)
	{
		new (&m_q) Quaternion(rotationMatrix);
		return *this;
	}

public:

	glm::mat4 ToRotationMatrix() const
	{
		return glm::mat4_cast(m_q);
	}

	void ToAxisAngle(glm::vec3& axis, float& angle)
	{
		axis = glm::axis(m_q);
		angle = glm::angle(m_q);
	}

public:

	friend float Dot(Quaternion const & a, Quaternion const & b)
	{
		return glm::dot(a.m_q, b.m_q);
	}

	friend Quaternion Normalize(Quaternion const & q)
	{
		return Quaternion(glm::normalize(q.m_q));
	}

	friend Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
	{
		return Quaternion(lhs.m_q * rhs.m_q);
	}

	friend Quaternion operator*(float scale, const Quaternion& rhs)
	{
		return Quaternion(scale * rhs.m_q);
	}

	friend Quaternion operator*(const Quaternion& lhs, float scale)
	{
		return Quaternion(lhs.m_q * scale);
	}

	friend Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs)
	{
		return Quaternion(lhs.m_q + rhs.m_q);
	}

protected:

	Quaternion(glm::quat const & q)
		: m_q(q)
	{}

private:

	glm::quat m_q;

	friend class DualQuaternion;
	friend DualQuaternion operator*(DualQuaternion const & lhs, DualQuaternion const & rhs);
	friend DualQuaternion operator*(float scale, DualQuaternion const & q);
	friend DualQuaternion operator*(DualQuaternion const & q, float scale);
	friend DualQuaternion operator+(DualQuaternion const & lhs, DualQuaternion const & rhs);
};






	// =======================================================================
	// =======================================================================
} // namespace CoreFx
#endif // COREFX_MATHS_QUATERNION_H