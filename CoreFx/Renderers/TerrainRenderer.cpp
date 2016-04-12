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
	, mDrawNormalShader("TerrainDrawNormals")
	, mDiffuseTextures(nullptr)
{
	std::cout << std::endl;
	std::cout << "Initialize TerrainRenderer...." << std::endl;

	AddTexture("medias/fallforestrocks01.ktx", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, true);
	AddTexture("medias/snowstone01.ktx", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, true);
	AddTexture("medias/pineforest03.ktx", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, true);

	AddTexture("medias/fallforestrocks01_n.ktx", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, true);
	AddTexture("medias/snowstone01_n.ktx", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, true);
	AddTexture("medias/pineforest03_n.ktx", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, true);

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_TexScale",
		"u_HeightMap",
		"u_DiffuseMap"
	};

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/terrain.vs.glsl");
	mShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/terrain.tcs.glsl");
	mShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/terrain.tes.glsl");
	mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/terrain.gs.glsl");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/terrain.deferred.fs.glsl");
	mShader.CreateAndLinkProgram();
	mShader.Use();

		mShader.AddUniforms(uniformNames, __uniforms_count__);

		//pass values of constant uniforms at initialization
		glUniform2iv(mShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
		glUniform2iv(mShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;		
		glUniform3fv(mShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;

		glUniform1i(mShader.GetUniform(u_HeightMap), 0); GL_CHECK_ERRORS;
		glUniform1i(mShader.GetUniform(u_DiffuseMap), 1); GL_CHECK_ERRORS;
		
		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;


	const char * uniformNames2[__uniforms2_count__] =
	{
		"u_NormalMagnitude",
		"u_VertexNormalColor",
	};

	mDrawNormalShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/terrain.vs.glsl");
	mDrawNormalShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/terrain.tcs.glsl");
	mDrawNormalShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/terrain.tes.glsl");
	mDrawNormalShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/terrain_vertex_normal.gs.glsl");
	mDrawNormalShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/vertex_normal.fs.glsl");
	mDrawNormalShader.CreateAndLinkProgram();
	mDrawNormalShader.Use();

	mDrawNormalShader.AddUniforms(uniformNames, __uniforms_count__);
	mDrawNormalShader.AddUniforms(uniformNames2, __uniforms2_count__);

	//pass values of constant uniforms at initialization
	glUniform2iv(mDrawNormalShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2iv(mDrawNormalShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
	glUniform3fv(mDrawNormalShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;

	glUniform1i(mDrawNormalShader.GetUniform(u_HeightMap), 0); GL_CHECK_ERRORS;

	mDrawNormalShader.SetupFrameDataBlockBinding();
	mDrawNormalShader.UnUse();

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

	//LoadHeightMap("medias/alps-valley-height-2048.raw", 2048, true);
	LoadHeightMap("medias/Terrain/Canyon_513x513.r32", 513, true);


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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, mHeightMapTextureId);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_ARRAY, mDiffuseTextures->GetResourceId());

	glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y);

	glBindVertexArray(0);
	mShader.UnUse();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TerrainRenderer::DebugRender()
{
	Engine * engine = Engine::GetInstance();

	if (engine->IsDrawVertexNormalEnabled())
	{
		mDrawNormalShader.Use();

		glUniform1f(mDrawNormalShader.GetUniform(__uniforms_count__ + u_NormalMagnitude), engine->GetDrawVertexNormalMagnitude());
		glUniform4fv(mDrawNormalShader.GetUniform(__uniforms_count__ + u_VertexNormalColor), 1, glm::value_ptr(engine->GetDrawVertexNormalColor()));

		glBindVertexArray(mVaoID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, mHeightMapTextureId);

		glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y);

		glBindVertexArray(0);

		mDrawNormalShader.UnUse();
	}
}


void TerrainRenderer::LoadHeightMap(const char * filename, GLint heightMapTextureWidth, bool invertY)
{
	assert(heightMapTextureWidth >= mMapSize.x);

	size_t bufferSize = mMapSize.x * mMapSize.y * sizeof(GLfloat);
	GLfloat * buffer = (GLfloat*)malloc(bufferSize);

	FILE * filePtr = nullptr;

	if (fopen_s(&filePtr, filename, "rb") == 0)
	{
		GLfloat * ptr;
		GLint incY;

		if (invertY)
		{
			ptr = buffer + (mMapSize.x * (mMapSize.y - 1));
			incY = -mMapSize.x;
		}
		else
		{
			ptr = buffer;
			incY = mMapSize.x;
		}

		GLint seekCount = (heightMapTextureWidth - mMapSize.x) * sizeof(GLfloat);
		GLint readCount = mMapSize.x * sizeof(GLfloat);

		for (GLint y = 0; y < mMapSize.y; ++y)
		{
			size_t n = fread(ptr, 1, readCount, filePtr);
			if (n != readCount)
			{
				char errmsg[200];
				strerror_s(errmsg, 200, errno);

				PRINT_MESSAGE("Error: Cannot read heigh map file '%s' : '%s'!\n", filename, errmsg);
				goto ExitLoadHeightMap;
			}
			ptr += incY;
			if (seekCount > 0)
			{
				fseek(filePtr, seekCount, SEEK_CUR);
			}
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

void TerrainRenderer::UpdateMaterialTextureIndex()
{
	for (TextureMappingList::const_iterator it = mTextureMapping.mMapping.begin(); it != mTextureMapping.mMapping.end(); ++it)
	{
		if (it->mTexture->GetCategory() == TextureCategory::Diffuse)
		{
			mDiffuseTextures = it->mTexture;
			break;
		}			
	}
}


	} // namespace Renderers
} // namespace CoreFx