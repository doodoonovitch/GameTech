#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



GridRenderer::GridRenderer(int width, int depth)
	: RendererHelper<Renderables::Grid, 1>()
{
	std::cout << std::endl;
	std::cout << "Initialize GridRenderer...." << std::endl;

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/grid_shader.vert");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/grid_shader.frag");
	mShader.CreateAndLinkProgram();
	mShader.Use();
		mShader.AddAttribute("vVertex");

		mShader.AddUniform("vGridSize");

		//pass values of constant uniforms at initialization
		glUniform2f(mShader.GetUniform("vGridSize"), (float)width, (float)depth);

		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;



	std::cout << std::endl;
	std::cout << "Vertex attribute index : " << std::endl;
	std::cout << "\t vVertex : " << mShader.GetAttribute("vVertex") << std::endl;
	std::cout << "Uniform attribute index : " << std::endl;
	std::cout << "\t vGridSize : " << mShader.GetUniform("vGridSize") << std::endl;


	m_vertexCount = ((width + 1) + (depth + 1)) * 2;
	glm::vec3* vertices = new glm::vec3[m_vertexCount];
	 
	int count = 0;
	int width_2 = width/2;
	int depth_2 = depth/2;
	int i=0 ;

	for (i = -width_2; i <= width_2; i++)
	{
		vertices[count++] = glm::vec3(i, -0.1f, -depth_2);
		vertices[count++] = glm::vec3(i, -0.1f, depth_2);
	}

	for (i = -depth_2; i <= depth_2; i++)
	{
		vertices[count++] = glm::vec3(-width_2, -0.1f, i);
		vertices[count++] = glm::vec3(width_2, -0.1f, i);
	}

	//setup vao and vbo stuff
	glGenVertexArrays(1, &mVaoID);
	glGenBuffers(mVboCount, mVboIDs);
	 
	glBindVertexArray(mVaoID);

		glBindBuffer (GL_ARRAY_BUFFER, mVboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, ((width + 1) + (depth + 1)) * 2 * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		GL_CHECK_ERRORS;
		 
		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 0, 0);
		GL_CHECK_ERRORS;
		 
	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	std::cout << "\t mVaoID : " << mVaoID << std::endl;
	std::cout << "\t mVboID[0] : " << mVboIDs[0] << std::endl;
	std::cout << "... GridRenderer initialized!" << std::endl << std::endl;

	delete [] vertices;	 
}


GridRenderer::~GridRenderer()
{
}
 
void GridRenderer::Render()
{
	mShader.Use();
		glBindVertexArray(mVaoID);
			glDrawArrays(GL_LINES, 0, m_vertexCount);
		glBindVertexArray(0);
	mShader.UnUse();
}



	} // namespace Renderers
} // namespace CoreFx