#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



AxisRenderer::AxisRenderer()
	: RendererHelper<1>(0, "AxisRenderer", "AxisWireFrameRenderer", Renderer::Forward_Pass)
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize AxisRenderer.....");

	const char * attributeNames[__attributes_count__] =
	{
		"vVertex",
		"vColor"
	};

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/axis.vs.glsl");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/axis.forward.fs.glsl");
	mShader.CreateAndLinkProgram();
	mShader.Use();
		mShader.AddAttributes(attributeNames, __attributes_count__);
		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

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
	CreateBuffers();

	glBindVertexArray(mVaoID);

		glBindBuffer (GL_ARRAY_BUFFER, mVboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		GL_CHECK_ERRORS;
		 
		glEnableVertexAttribArray(mShader.GetAttribute(vVertex));
		glVertexAttribPointer(mShader.GetAttribute(vVertex), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(mShader.GetAttribute(vColor));
		glVertexAttribPointer(mShader.GetAttribute(vColor), 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(offsetof(Vertex, color)));
		GL_CHECK_ERRORS;

		 
	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	mIsInitialized = true;

	PRINT_MESSAGE(".....AxisRenderer initialized!");
	PRINT_END_SECTION;
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

void AxisRenderer::RenderWireFrame()
{
	Render();
}


	} // namespace Renderers
} // namespace CoreFx