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
	, mPatchPerTexture(mPatchCount.x * mPatchCount.y)
	, mHeightMapTextureId(0)
{
	std::cout << std::endl;
	std::cout << "Initialize TerrainRenderer...." << std::endl;

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_PatchPerTexture",
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
		glUniform2iv(mShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
		glUniform3fv(mShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;
		glUniform1i(mShader.GetUniform(u_PatchPerTexture), mPatchPerTexture); GL_CHECK_ERRORS;

		glUniform1i(mShader.GetUniform(u_HeightMap), 0); GL_CHECK_ERRORS;

		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	//setup vao and vbo stuff
	glGenVertexArrays(1, &mVaoID);
	glBindVertexArray(mVaoID);

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

	glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchPerTexture);

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

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

ExitLoadHeightMap:
	if (filePtr != nullptr)
		fclose(filePtr);
	free(buffer);
}



	} // namespace Renderers
} // namespace CoreFx