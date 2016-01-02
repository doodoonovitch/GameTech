#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	// =======================================================================
	// =======================================================================





Frame::Frame()
	: m_position(0.f, 0.f, 0.f)
	, m_scale(1.f, 1.f, 1.f)
{
}


Frame::~Frame()
{
}

void Frame::BuildMatrix() 
{
	glm::mat4 rotMat = glm::mat4_cast(m_rotation);
	glm::mat4 scaleMat = glm::scale(m_scale);
	glm::mat4 transMat = glm::translate(m_position);
	m_matrix = transMat * rotMat * scaleMat;

	glm::mat4 invScaleMat = glm::scale(glm::vec3(1.f / m_scale.x, 1.f / m_scale.y, 1.f / m_scale.z));
	glm::mat4 invRotMat = glm::transpose(rotMat);
	glm::mat4 invTransMat = glm::translate(-m_position);
	m_invMatrix = invScaleMat * invRotMat * invTransMat;
}




	// =======================================================================
	// =======================================================================
} // namespace CoreFx
