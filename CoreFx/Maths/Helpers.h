#pragma once
#ifndef COREFX_MATHS_HELPERS_H
#define COREFX_MATHS_HELPERS_H


namespace CoreFx
{
namespace Maths
{
namespace Helpers
{
// =======================================================================
// =======================================================================



static glm::vec3 AngleToDirection(GLfloat angleDegrees)
{
	const glm::vec3 UnitVector(1.f, 0.f, 0.f);

	float angle = glm::radians(angleDegrees);

	glm::vec3 direction(0.f);

	direction.x = (UnitVector.x * cos(angle)) - (UnitVector.z * sin(angle));
	direction.z = (UnitVector.z * cos(angle)) + (UnitVector.x * sin(angle));

	return glm::normalize(direction);

}


// =======================================================================
//	depthNDC : depth in NDC space [-1;1]
//	n : near plane
//	f : far plane
// =======================================================================
static GLfloat LinearizeDepth(GLfloat depthNDC, GLfloat n, GLfloat f)
{
	GLfloat zView = 2.f * f * n / (depthNDC * (f - n) - (f + n));

	return zView;
}

// =======================================================================
// viewRay : view ray = in_Position.xy / vec2(u_ProjMatrix[0][0], u_ProjMatrix[1][1]); 
//		in_Position.xy : [(-1-1);(1,1)]
//	depthNDC : depth in NDC space [-1;1]
//	n : near plane
//	f : far plane
// =======================================================================
static glm::vec3 PositionFromDepth(const glm::vec2 & viewRay, GLfloat depthNDC, GLfloat n, GLfloat f)
{
	GLfloat zView = LinearizeDepth(depthNDC, n, f);
	glm::vec2 xyView = viewRay * -zView;

	return glm::vec3(xyView.x, xyView.y, zView);
}




// =======================================================================
// =======================================================================
} // namespace Helpers
} // namespace Maths
} // namespace CoreFx
#endif // COREFX_MATHS_HELPERS_H