#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderables
	{



Axis::Axis()
{
	//setup shader
	m_shader.LoadFromFile(GL_VERTEX_SHADER, "shaders/axis_shader.vert");
	m_shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/axis_shader.frag");
	m_shader.CreateAndLinkProgram();
	m_shader.Use();
		m_shader.AddAttribute("vVertex");
		m_shader.AddAttribute("vColor");
		m_shader.AddUniform("MVP");
		//pass values of constant uniforms at initialization
	m_shader.UnUse();

	GL_CHECK_ERRORS;

	Vertex vertices[] =
	{
		{ glm::vec3(0.0f, 0.0f,  0.0f),	glm::vec4(1.0f, 0.0f, 0.f, 1.0f) },	// X-Axis
		{ glm::vec3(1.0f, 0.0f,  0.0f),	glm::vec4(1.0f, 0.0f, 0.f, 1.0f) },	// X-Axis

		{ glm::vec3(0.0f, 0.0f,  0.0f),	glm::vec4(0.0f, 1.0f, 0.f, 1.0f) },	// Y-Axis
		{ glm::vec3(0.0f, 1.0f,  0.0f),	glm::vec4(0.0f, 1.0f, 0.f, 1.0f) },	// Y-Axis

		{ glm::vec3(0.0f, 0.0f,  0.0f),	glm::vec4(0.f, 0.0f, 1.0f, 1.0f) },	// Z-Axis
		{ glm::vec3(0.0f, 0.0f,  1.0f),	glm::vec4(0.f, 0.0f, 1.0f, 1.0f) },	// Z-Axis
	};
	 

	//setup vao and vbo stuff
	glGenVertexArrays(1, &m_vaoID);
	glGenBuffers(m_vboCount, m_vboIDs);
	 
	glBindVertexArray(m_vaoID);

		glBindBuffer (GL_ARRAY_BUFFER, m_vboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		GL_CHECK_ERRORS;
		 
		glEnableVertexAttribArray(m_shader["vVertex"]);
		glVertexAttribPointer(m_shader["vVertex"], 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(m_shader["vColor"]);
		glVertexAttribPointer(m_shader["vColor"], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(offsetof(Vertex, color)));
		GL_CHECK_ERRORS;

		 
	glBindVertexArray(0);

	GL_CHECK_ERRORS;
}


Axis::~Axis()
{
}
 
void Axis::Render(glm::mat4 const & VP)
{
	glm::mat4 M = GetFrame()->GetMatrix();
	glm::mat4 MVP = VP * M;


	m_shader.Use();
		glUniformMatrix4fv(m_shader("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
		glBindVertexArray(m_vaoID);
			glDrawArrays(GL_LINES, 0, 6);
		glBindVertexArray(0);
	m_shader.UnUse();

}



	} // namespace Renderables
} // namespace CoreFx