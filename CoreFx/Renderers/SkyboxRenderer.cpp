#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



	SkyboxRenderer::SkyboxRenderer(const std::string & skyboxCubeMapTextureFilename)
		: RendererHelper<1>(0, "SkyboxRenderer")
		, mCubeMapTexture(Engine::GetInstance()->GetTextureManager()->LoadTextureCubeMap(skyboxCubeMapTextureFilename))
{
	std::cout << std::endl;
	std::cout << "Initialize SkyboxRenderer...." << std::endl;

	const char * uniformNames[__uniforms_count__] =
	{
		"u_SkyboxCubeMapSampler"
	};

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/skybox.vs.glsl");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/skybox.deferred.fs.glsl");
	mShader.CreateAndLinkProgram();
	mShader.Use();
		mShader.AddUniforms(uniformNames, __uniforms_count__);

		glUniform1i(mShader.GetUniform(u_SkyboxCubeMapSampler), 0); GL_CHECK_ERRORS;

		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	GLfloat skyboxVertices[] = 
	{
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	//setup vao and vbo stuff
	glGenVertexArrays(1, &mVaoID);
	glGenBuffers(mVboCount, mVboIDs);
	 
	glBindVertexArray(mVaoID);

		glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	std::cout << "... SkyboxRenderer initialized!" << std::endl << std::endl;
}

SkyboxRenderer::~SkyboxRenderer()
{
}

void SkyboxRenderer::Render()
{
	mShader.Use();
		glDepthMask(GL_FALSE);
			glBindVertexArray(mVaoID);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, mCubeMapTexture->GetResourceId());
				glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		glDepthMask(GL_TRUE);
	mShader.UnUse();
}
 
void SkyboxRenderer::DebugRender()
{
}



	} // namespace Renderers
} // namespace CoreFx