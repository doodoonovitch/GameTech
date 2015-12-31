#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderables
	{



Cube::Cube()
{
	//setup shader
	m_shader.LoadFromFile(GL_VERTEX_SHADER, "shaders/cube_shader.vert");
	m_shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/cube_shader.frag");
	m_shader.CreateAndLinkProgram();
	m_shader.Use();
		m_shader.AddAttribute("vVertex");
		m_shader.AddAttribute("vUV");
		m_shader.AddUniform("MVP");
		m_shader.AddUniform("texSampler1");
		//pass values of constant uniforms at initialization
		glUniform1i(m_shader("texSampler1"), 0);
	m_shader.UnUse();

	GL_CHECK_ERRORS;

	Vertex vertices[24] = 
	{
		// Vertex data for face 0
		{ glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec2(0.0f, 0.0f) },	// v0
		{ glm::vec3(1.0f, -1.0f,  1.0f),	glm::vec2(0.33f, 0.0f) },	// v1
		{ glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec2(0.0f, 0.5f) },	// v2
		{ glm::vec3(1.0f,  1.0f,  1.0f),	glm::vec2(0.33f, 0.5f) },	// v3

		// Vertex data for face 1
		{ glm::vec3(1.0f, -1.0f,  1.0f),	glm::vec2(0.0f, 0.5f) },	// v4
		{ glm::vec3(1.0f, -1.0f, -1.0f),	glm::vec2(0.33f, 0.5f) },	// v5
		{ glm::vec3(1.0f,  1.0f,  1.0f),	glm::vec2(0.0f, 1.0f) },	// v6
		{ glm::vec3(1.0f,  1.0f, -1.0f),	glm::vec2(0.33f, 1.0f) },	// v7

		// Vertex data for face 2
		{ glm::vec3(1.0f, -1.0f, -1.0f),	glm::vec2(0.66f, 0.5f) },	// v8
		{ glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec2(1.0f, 0.5f) },	// v9
		{ glm::vec3(1.0f,  1.0f, -1.0f),	glm::vec2(0.66f, 1.0f) },	// v10
		{ glm::vec3(-1.0f,  1.0f, -1.0f),	glm::vec2(1.0f, 1.0f) },	// v11

		// Vertex data for face 3
		{ glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec2(0.66f, 0.0f) },	// v12
		{ glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec2(1.0f, 0.0f) },	// v13
		{ glm::vec3(-1.0f,  1.0f, -1.0f),	glm::vec2(0.66f, 0.5f) },	// v14
		{ glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec2(1.0f, 0.5f) },	// v15

		// Vertex data for face 4
		{ glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec2(0.33f, 0.0f) },	// v16
		{ glm::vec3(1.0f, -1.0f, -1.0f),	glm::vec2(0.66f, 0.0f) },	// v17
		{ glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec2(0.33f, 0.5f) },	// v18
		{ glm::vec3(1.0f, -1.0f,  1.0f),	glm::vec2(0.66f, 0.5f) },	// v19

		// Vertex data for face 5
		{ glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec2(0.33f, 0.5f) },	// v20
		{ glm::vec3(1.0f,  1.0f,  1.0f),	glm::vec2(0.66f, 0.5f) },	// v21
		{ glm::vec3(-1.0f,  1.0f, -1.0f),	glm::vec2(0.33f, 1.0f) },	// v22
		{ glm::vec3(1.0f,  1.0f, -1.0f),	glm::vec2(0.66f, 1.0f) }	// v23
	};	

	GLushort indices[34] = 
	{
		0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
		4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
		8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
		12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
		16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
		20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
	};

	//setup vao and vbo stuff
	glGenVertexArrays(1, &m_vaoID);
	glGenBuffers(m_vboCount, m_vboIDs);
	 
	glBindVertexArray(m_vaoID);

		glBindBuffer (GL_ARRAY_BUFFER, m_vboIDs[VBO_Vertex]);
		glBufferData (GL_ARRAY_BUFFER, 24 * sizeof(Vertex), &vertices[0].pos.x, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;
		 
		glEnableVertexAttribArray(m_shader["vVertex"]);
		glVertexAttribPointer(m_shader["vVertex"], 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(m_shader["vUV"]);
		glVertexAttribPointer(m_shader["vUV"], 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(offsetof(Vertex, uv)));
		GL_CHECK_ERRORS;
		  
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIDs[VBO_Index]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 34, &indices[0], GL_STATIC_DRAW);
		GL_CHECK_ERRORS;
		 
	glBindVertexArray(0);


	//load the image using SOIL
	int texture_width = 0, texture_height = 0, channels = 0;
	GLubyte* pData = SOIL_load_image(m_filename.c_str(), &texture_width, &texture_height, &channels, SOIL_LOAD_AUTO);
	if (pData == NULL)
	{
		std::cerr << "Cannot load image: " << m_filename.c_str() << std::endl;
		exit(EXIT_FAILURE);
	}

	//setup OpenGL texture and bind to texture unit 0
	glGenTextures(1, &m_textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	//set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	//allocate texture 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);

	//free SOIL image data
	SOIL_free_image_data(pData);

	GL_CHECK_ERRORS;
}


Cube::~Cube()
{
	//Delete textures
	glDeleteTextures(1, &m_textureID);

}
 
void Cube::Render(glm::mat4 const & VP)
{
	glm::mat4 M = GetFrame()->GetMatrix();
	glm::mat4 MVP = VP * M;

	m_shader.Use();
		glUniformMatrix4fv(m_shader("MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
		glBindVertexArray(m_vaoID);
			glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
	m_shader.UnUse();
}



	} // namespace Renderables
} // namespace CoreFx