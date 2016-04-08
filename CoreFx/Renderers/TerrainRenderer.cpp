#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



TerrainRenderer::TerrainRenderer(GLint heightMapWidth, GLint heightMapDepth, glm::vec3 const & scale)
	: RendererHelper<Renderables::Grid, 1>(0, "TerrainRenderer")
	, mMapSize(heightMapWidth, heightMapDepth)
	, mPatchCount(heightMapWidth / 64, heightMapDepth / 64)
	, mScale(scale)
	, mHeightMapTextureId(0)
{
	std::cout << std::endl;
	std::cout << "Initialize TerrainRenderer...." << std::endl;

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_Scale",
		"u_HeightMap",
	};

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/terrain.vs.glsl");
	mShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/terrain.tcs.glsl");
	mShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/terrain.tes.glsl");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/terrain.deferred.fs.glsl");
	mShader.CreateAndLinkProgram();
	mShader.Use();

		mShader.AddUniforms(uniformNames, __uniforms_count__);

		//pass values of constant uniforms at initialization
		glUniform2iv(mShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
		glUniform3fv(mShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;

		glUniform1i(mShader.GetUniform(u_HeightMap), 0); GL_CHECK_ERRORS;

		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	const float epsilon = 0.00001f;
	const glm::vec3 vertices[] =
	{
		glm::vec3(0.0f,				0.0f, 0.0f),
		glm::vec3(1.0f - epsilon,	0.0f, 0.0f),
		glm::vec3(0.0f,				0.0f, 1.0f - epsilon),
		glm::vec3(1.0f - epsilon,	0.0f, 1.0f - epsilon)
	};
		

	//setup vao and vbo stuff
	glGenVertexArrays(1, &mVaoID);
	glGenBuffers(mVboCount, mVboIDs);

	glBindVertexArray(mVaoID);

	glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
	glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	GL_CHECK_ERRORS;

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	LoadHeightMap("medias/alps-valley-height-2048.raw");
	
	std::cout << "... TerrainRenderer initialized!" << std::endl << std::endl;
}


TerrainRenderer::~TerrainRenderer()
{
	glDeleteTextures(1, &mHeightMapTextureId);
	mHeightMapTextureId = 0;
}
 
void TerrainRenderer::Render()
{
	mShader.Use();
	glBindVertexArray(mVaoID);

	if (GetWireFrame())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, mHeightMapTextureId);

	glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y);

	glBindVertexArray(0);
	mShader.UnUse();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TerrainRenderer::LoadHeightMap(const char * filename)
{
	size_t bufferSize = mMapSize.x * mMapSize.y * sizeof(GLfloat);
	GLfloat * buffer = (GLfloat*)malloc(bufferSize);

	FILE * filePtr = nullptr;

	if (fopen_s(&filePtr, filename, "rb") == 0)
	{
		size_t n = fread(buffer, 1, bufferSize, filePtr);
		if (n != bufferSize)
		{
			char errmsg[200];
			strerror_s(errmsg, 200, errno);

			PRINT_MESSAGE("Error: Cannot read heigh map file '%s' : '%s'!\n", filename, errmsg);
			goto ExitLoadHeightMap;
		}		
	}
	else
	{
		PRINT_MESSAGE("Error: Cannot open heigh map file '%s'!\n", filename);
		goto ExitLoadHeightMap;
	}

	glGenTextures(1, &mHeightMapTextureId);
	PRINT_MESSAGE("[TerrainRenderer] : Height map texture id : %i.\n", mHeightMapTextureId);

	glBindTexture(GL_TEXTURE_2D_ARRAY, mHeightMapTextureId);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R32F, mMapSize.x, mMapSize.y, 1, 0, GL_RED, GL_FLOAT, buffer);

	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_REPEAT);
	
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

ExitLoadHeightMap:
	if (filePtr != nullptr)
		fclose(filePtr);
	free(buffer);
}



	} // namespace Renderers
} // namespace CoreFx