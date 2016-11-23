#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{


SkyboxRenderer::SkyboxRenderer(CubeMapTexture const * skyboxCubeMapTexture, bool isHDR)
	: RendererHelper<1>("SkyboxRenderer", "SkyboxWireFrameRenderer", Renderer::Forward_Pass)
	, mCubeMapTexture(skyboxCubeMapTexture)
	, mIsHDR(isHDR)
{
	Initialize();
}

SkyboxRenderer::SkyboxRenderer(const std::string & skyboxCubeMapTextureFilename, bool isHDR)
	: RendererHelper<1>("SkyboxRenderer", "SkyboxWireFrameRenderer", Renderer::Forward_Pass)
	, mCubeMapTexture(Engine::GetInstance()->GetTextureManager()->LoadTextureCubeMap(skyboxCubeMapTextureFilename))
	, mIsHDR(isHDR)
{
	Initialize();
}

void SkyboxRenderer::Initialize()
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize SkyboxRenderer.....");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_SkyboxCubeMapSampler"
	};

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/skybox.vs.glsl");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/skybox.forward.fs.glsl");

	//std::vector<std::string> shaderFilenames(2);
	//shaderFilenames[0] = "shaders/DeferredShadingCommon.incl.glsl";
	//shaderFilenames[1] = "shaders/skybox.deferred.fs.glsl";
	//mShader.LoadFromFile(GL_FRAGMENT_SHADER, shaderFilenames);


	mShader.CreateAndLinkProgram();
	mShader.Use();
		mShader.AddUniforms(uniformNames, __uniforms_count__);

		glUniform1i(mShader.GetUniform(u_SkyboxCubeMapSampler), 0); GL_CHECK_ERRORS;

		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	//setup shader
	mWireFrameShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/skybox.vs.glsl");
	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/skybox.wireframe.fs.glsl");
	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();
	mWireFrameShader.SetupFrameDataBlockBinding();
	mWireFrameShader.UnUse();

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
	CreateBuffers();

	glBindVertexArray(mVaoID);

		glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	mIsInitialized = true;

	PRINT_MESSAGE(".....SkyboxRenderer initialized!");
	PRINT_END_SECTION;
}

SkyboxRenderer::~SkyboxRenderer()
{
}

void SkyboxRenderer::Render()
{
	mShader.Use();
		glBindVertexArray(mVaoID);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(mCubeMapTexture->GetTarget(), mCubeMapTexture->GetResourceId());
			glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	mShader.UnUse();
}
 
void SkyboxRenderer::RenderWireFrame()
{
	mWireFrameShader.Use();
		glBindVertexArray(mVaoID);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	mWireFrameShader.UnUse();
}



	} // namespace Renderers
} // namespace CoreFx