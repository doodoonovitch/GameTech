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
// =======================================================================
} // namespace Helpers
} // namespace Maths
} // namespace CoreFx
#endif // COREFX_MATHS_HELPERS_H