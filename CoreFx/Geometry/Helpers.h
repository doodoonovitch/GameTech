#pragma once
#ifndef COREFX_GEOMETRY_HELPERS_H
#define COREFX_GEOMETRY_HELPERS_H


namespace CoreFx
{
namespace Geometry
{
namespace Helpers
{
// =======================================================================
// =======================================================================


// =======================================================================
//	x : 
//	y : 
//	z : 
// =======================================================================
static void GenerateRadialGrid(GLsizei & outVertexCount, GLsizei & outIndexCount, glm::vec3 *& outVertices, GLushort *& outIndices, 
	GLushort slides = 36, GLushort rings = 100, GLfloat maxDetail = 0.1f, GLfloat gridSize = 40000.f)
{
	outVertexCount = slides * rings + 1;
	outIndexCount = (slides * 3) + (slides * (rings - 1) * 6);

	GLfloat n_1 = (GLfloat)(rings - 1);
	GLfloat n_1_x2 = n_1 * n_1;
	GLfloat n_1_x4 = n_1_x2 * n_1_x2;
	GLfloat a0 = maxDetail;
	GLfloat a1 = (gridSize - maxDetail) / n_1_x4;

	outVertices = new glm::vec3[outVertexCount];

	outVertices[0] = glm::vec3(0.f);
	glm::vec3 * vertex = &outVertices[1];

	GLfloat r = 0.f;
	const GLfloat twoPI = glm::two_pi<GLfloat>();
	GLfloat M = (GLfloat)slides;
	for (GLushort i = 0; i < rings; i++)
	{
		GLfloat i_2 = (GLfloat)(i * i);
		GLfloat i_4 = i_2 * i_2;
		r = a0 + a1 * i_4;

		for (GLushort j = 0; j < slides; j++)
		{
			GLfloat angle = twoPI * (GLfloat)j / M;

			vertex->x = r * glm::cos(angle);
			vertex->y = 0.0f;
			vertex->z = -r * glm::sin(angle);

			++vertex;
		}
	}

	outIndices = new GLushort[outIndexCount];
	GLushort * index = outIndices;
	GLushort baseIndex = 1;
	for (GLushort j = 0; j < slides; ++j)
	{
		index[0] = 0;
		index[1] = baseIndex + j;
		index[2] = baseIndex + ((j + 1) % slides);

		index += 3;
	}

	for (GLushort i = 1; i < rings; i++)
	{
		for (GLushort j = 0; j < slides; ++j)
		{
			GLushort v0 = baseIndex + j;
			GLushort v1 = baseIndex + ((j + 1) % slides);
			GLushort v7 = baseIndex + slides + j;
			GLushort v8 = baseIndex + slides + ((j + 1) % slides);

			index[0] = v0;
			index[1] = v7;
			index[2] = v8;

			index[3] = v0;
			index[4] = v8;
			index[5] = v1;

			index += 6;
		}

		baseIndex += slides;
	}
}




// =======================================================================
// =======================================================================
} // namespace Helpers
} // namespace Geometry
} // namespace CoreFx
#endif // COREFX_GEOMETRY_HELPERS_H