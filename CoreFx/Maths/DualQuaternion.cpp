#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Maths
	{
		// =======================================================================
		// =======================================================================


DualQuaternion Conjugate(DualQuaternion const & a)
{
	return DualQuaternion(glm::conjugate(a.mQr), glm::conjugate(a.mQd));
}

float Dot(DualQuaternion const & a, DualQuaternion const & b)
{
	return glm::dot(a.mQr, b.mQr);
}

DualQuaternion Normalize(DualQuaternion const & q)
{
	float mag = glm::dot(q.mQr, q.mQr);
	assert(mag > 0.000001f);
	DualQuaternion ret;
	ret.mQr = q.mQr / mag;
	ret.mQd = q.mQd / mag;
	return ret;
}

DualQuaternion operator*(float scale, DualQuaternion const & q)
{
	return DualQuaternion(scale * q.mQr, scale * q.mQd);
}

DualQuaternion operator*(DualQuaternion const & q, float scale)
{
	return DualQuaternion(scale * q.mQr, scale * q.mQd);
}

DualQuaternion operator*(DualQuaternion const & lhs, DualQuaternion const & rhs)
{
	return DualQuaternion(lhs.mQr * rhs.mQr, lhs.mQr * rhs.mQd + lhs.mQd * rhs.mQr);
}

DualQuaternion operator+(DualQuaternion const & lhs, DualQuaternion const & rhs)
{
	return DualQuaternion(lhs.mQr + rhs.mQr, lhs.mQd + rhs.mQd);
}


glm::mat4 ToMatrix(DualQuaternion const & dualQuat)
{
	DualQuaternion q = Normalize(dualQuat);

	glm::mat4 m(1.f);
	float w = q.mQr.w;
	float x = q.mQr.x;
	float y = q.mQr.y;
	float z = q.mQr.z;

	// Extract rotational information
	m[0].x = w*w + x*x - y*y - z*z;
	m[0].y = 2 * x*y + 2 * w*z;
	m[0].z = 2 * x*z - 2 * w*y;

	m[1].x = 2 * x*y - 2 * w*z;
	m[1].y = w*w + y*y - x*x - z*z;
	m[1].z = 2 * y*z + 2 * w*x;
	m[2].x = 2 * x*z + 2 * w*y;
	m[2].y = 2 * y*z - 2 * w*x;
	m[2].z = w*w + z*z - x*x - y*y;

	// Extract translation information
	glm::quat t = (q.mQd * 2.0f) * glm::conjugate(q.mQr);

	m[3].x = t.x;
	m[3].y = t.y;
	m[3].z = t.z;

	return m;
	

	//glm::mat4 identityMat = glm::mat4(1.0f);
	//glm::mat4 rotMatrix = glm::mat4_cast(dualQuat.GetRotation());   //rotation is glm::quat
	//glm::mat4 transMatrix = glm::translate(identityMat, dualQuat.GetTranslation());
	//glm::mat4 m = transMatrix * rotMatrix;

	//return m;
}






		// =======================================================================
		// =======================================================================
	} // namespace Maths
} // namespace FxEngine
