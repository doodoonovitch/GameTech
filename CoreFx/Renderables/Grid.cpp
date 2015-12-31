#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderables
	{



Grid::Grid(int width, int depth)
{
	//setup shader
	m_shader.LoadFromFile(GL_VERTEX_SHADER, "shaders/grid_shader.vert");
	m_shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/grid_shader.frag");
	m_shader.CreateAndLinkProgram();
	m_shader.Use();
		m_shader.AddAttribute("vVertex");
		m_shader.AddUniform("MVP");
		m_shader.AddUniform("vGridSize");
		//pass values of constant uniforms at initialization
		glUniform2f(m_shader("vGridSize"), (float)width, (float)depth);
	m_shader.UnUse();

	GL_CHECK_ERRORS;

	m_vertexCount = ((width + 1) + (depth + 1)) * 2;
	glm::vec3* vertices = new glm::vec3[m_vertexCount];
	 
	int count = 0;
	int width_2 = width/2;
	int depth_2 = depth/2;
	int i=0 ;

	for (i = -width_2; i <= width_2; i++)
	{
		vertices[count++] = glm::vec3(i, 0, -depth_2);
		vertices[count++] = glm::vec3(i, 0, depth_2);
	}

	for (i = -depth_2; i <= depth_2; i++)
	{
		vertices[count++] = glm::vec3(-width_2, 0, i);
		vertices[count++] = glm::vec3(width_2, 0, i);
	}

	//setup vao and vbo stuff
	glGenVertexArrays(1, &m_vaoID);
	glGenBuffers(m_vboCount, m_vboIDs);
	 
	glBindVertexArray(m_vaoID);

		glBindBuffer (GL_ARRAY_BUFFER, m_vboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, ((width + 1) + (depth + 1)) * 2 * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		GL_CHECK_ERRORS;
		 
		glEnableVertexAttribArray(m_shader["vVertex"]);
		glVertexAttribPointer(m_shader["vVertex"], 3, GL_FLOAT, GL_FALSE, 0, 0);
		GL_CHECK_ERRORS;
		 
	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	delete [] vertices;	 
}


Grid::~Grid()
{
}
 
void Grid::Render(glm::mat4 const & VP)
{
	glm::mat4 M = GetFrame()->GetMatrix();
	glm::mat4 MVP = VP * M;

	m_shader.Use();
		glUniformMatrix4fv(m_shader("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
		glBindVertexArray(m_vaoID);
			glDrawArrays(GL_LINES, 0, m_vertexCount);
		glBindVertexArray(0);
	m_shader.UnUse();
}



	} // namespace Renderables
} // namespace CoreFx