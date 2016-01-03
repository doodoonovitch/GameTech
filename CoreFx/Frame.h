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


private:

	Frame(const Frame& rhs) = delete;
	Frame& operator=(const Frame& rhs) = delete;

private:

	glm::mat4 m_matrix;
	glm::mat4 m_invMatrix;
	glm::quat m_rotation;
	glm::vec3 m_position;
	glm::vec3 m_scale;
};








inline void Frame::SetRotation(const glm::quat& q)
{
	m_rotation = q;
}

inline void Frame::SetPosition(glm::vec4 const & v)
{
	m_position.x = v.x;
	m_position.y = v.y;
	m_position.z = v.z;
}

inline void Frame::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

inline void Frame::SetPosition(glm::vec3 const & p)
{
	m_position = p;
}

inline glm::vec3 const & Frame::GetPosition() const
{
	return m_position;
}

inline glm::quat const & Frame::GetRotation() const
{
	return m_rotation;
}

inline glm::mat4 const & Frame::GetMatrix() const
{
	return m_matrix;
}

inline glm::mat4 const & Frame::GetMatrixInverse() const
{
	return m_invMatrix;
}

inline glm::vec3 Frame::GetRight() const
{
	return glm::vec3(m_matrix[0]);
	//return glm::vec3(m_matrix[0].x, m_matrix[1].x, m_matrix[2].x);
}

inline glm::vec3 Frame::GetUp() const
{
	return glm::vec3(m_matrix[1]);
	//return glm::vec3(m_matrix[0].y, m_matrix[1].y, m_matrix[2].y);
}

inline glm::vec3 Frame::GetLook() const
{
	return glm::vec3(m_matrix[2]);
	//return glm::vec3(m_matrix[0].z, m_matrix[1].z, m_matrix[2].z);
}




	// =======================================================================
	// =======================================================================
} // namespace CoreFx
#endif // COREFX_FRAME_H