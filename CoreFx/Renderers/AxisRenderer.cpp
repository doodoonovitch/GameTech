#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



AxisRenderer::AxisRenderer()
	: RendererHelper<Renderables::Axis, 1>()
{
	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/axis_shader.vert");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/axis_shader.frag");
	mShader.CreateAndLinkProgram();
	mShader.Use();
		mShader.AddAttribute("vVertex");
		mShader.AddAttribute("vColor");
		//pass values of constant uniforms at initialization
	mShader.UnUse();

	GL_CHECK_ERRORS;

	mShader.SetupFrameDataBlockBinding();

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
	glGenVertexArrays(1, &mVaoID);
	glGenBuffers(mVboCount, mVboIDs);
	 
	glBindVertexArray(mVaoID);

		glBindBuffer (GL_ARRAY_BUFFER, mVboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		GL_CHECK_ERRORS;
		 
		glEnableVertexAttribArray(mShader["vVertex"]);
		glVertexAttribPointer(mShader["vVertex"], 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(mShader["vColor"]);
		glVertexAttribPointer(mShader["vColor"], 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(offsetof(Vertex, color)));
		GL_CHECK_ERRORS;

		 
	glBindVertexArray(0);

	GL_CHECK_ERRORS;
}


AxisRenderer::~AxisRenderer()
{
}
 
void AxisRenderer::Render()
{
	mShader.Use();
		glBindVertexArray(mVaoID);
			glDrawArrays(GL_LINES, 0, 6);
		glBindVertexArray(0);
	mShader.UnUse();

}



	} // namespace Renderers
} // namespace CoreFx