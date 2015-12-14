#pragma once
#ifndef COREFX_MATHS_QUATERNION_H
#define COREFX_MATHS_QUATERNION_H


namespace CoreFx
{
	// =======================================================================
	// =======================================================================




class Quaternion 
{
public:

	Quaternion()
		: m_q(1.0f, 0.0f, 0.0f, 0.0f)
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

	Quaternion(const Quaternion& rhs)
		: m_q(rhs.m_q)
	{

	}

	Quaternion& operator=(const Quaternion& rhs)
	{
		if (this != &rhs)
		{
			m_q = rhs.m_q;
		}
		return *this;
	}

	bool operator==(const Quaternion& rhs) const
	{
		if (this == &rhs)
			return true;

		return glm::abs(glm::dot(m_q, rhs.m_q)) < (1.f - glm::epsilon<float>());
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

	Quaternion operator*(const Quaternion& other) const
	{
		return Quaternion(m_q * other.m_q);
	}

protected:

	Quaternion(glm::quat const & q)
		: m_q(q)
	{}

private:

	glm::quat m_q;

};






	// =======================================================================
	// =======================================================================
} // namespace CoreFx
#endif // COREFX_MATHS_QUATERNION_H