#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



CompassRenderer::CompassRenderer()
	: RendererHelper<1>("CompassRenderer", "CompassWireframeRenderer", Renderer::HUD_Pass)
	, mCompassTexture(Engine::GetInstance()->GetTextureManager()->LoadTexture2D("medias/compass.tif"))
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize CompassRenderer.....");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_Translation",
		"u_ViewDir",
		"u_CompassTextureSampler"
	};


	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/compass.vs.glsl");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/compass.forward.fs.glsl");
	mShader.CreateAndLinkProgram();
	mShader.Use();
		mShader.AddUniforms(uniformNames, __uniforms_count__);

		glUniform1i(mShader.GetUniform(u_CompassTextureSampler), 0); GL_CHECK_ERRORS;
		
		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	//setup shader
	mWireFrameShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/compass.vs.glsl");
	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/compass.wireframe.fs.glsl");
	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();
	mWireFrameShader.AddUniforms(uniformNames, __uniforms_count__ - 1);
	mWireFrameShader.SetupFrameDataBlockBinding();
	mWireFrameShader.UnUse();

	GL_CHECK_ERRORS;

	const GLfloat vertices[] = 
	{
		0.f, 140.f,		0.f, 0.f,
		0.f, 0.f,		0.f, 1.f,
		140.f, 140.f,	1.f, 0.f,
		140.f, 0.f,		1.f, 1.f
	};

	//setup vao and vbo stuff
	CreateBuffers();

	glBindVertexArray(mVaoID);

		glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	mIsInitialized = true;

	PRINT_MESSAGE(".....CompassRenderer initialized!\n");
	PRINT_END_SECTION;
}

CompassRenderer::~CompassRenderer()
{
}

void CompassRenderer::UpdateUniforms(Shader const & shader)
{
	glUniform3f(shader.GetUniform(u_Translation), 70.f, 70.f, 0.f); 
	Camera * cam = Engine::GetInstance()->GetCamera();
	glUniform3fv(shader.GetUniform(u_ViewDir), 1, glm::value_ptr(cam->GetLook())); 
}

void CompassRenderer::Render()
{
	mShader.Use();
		glBindVertexArray(mVaoID);
			UpdateUniforms(mShader);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(mCompassTexture->GetTarget(), mCompassTexture->GetResourceId());
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	mShader.UnUse();
}
 
void CompassRenderer::RenderWireFrame()
{
	mWireFrameShader.Use();
		glBindVertexArray(mVaoID);
			UpdateUniforms(mWireFrameShader);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	mWireFrameShader.UnUse();
}



	} // namespace Renderers
} // namespace CoreFx