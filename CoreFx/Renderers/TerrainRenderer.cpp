#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



TerrainRenderer::TerrainRenderer(const Desc & desc)
	: RendererHelper<Renderables::Grid, 1>(0, "TerrainRenderer")
	, mMapSize(desc.mHeightMapWidth, desc.mHeightMapDepth)
	, mPatchCount(desc.mHeightMapWidth / 64, desc.mHeightMapDepth / 64)
	, mScale(desc.mScale)
	, mHeightMapTextureId(0)
	, mDrawNormalShader("TerrainDrawNormals")
	, mDiffuseTextures(nullptr)
{
	std::cout << std::endl;
	std::cout << "Initialize TerrainRenderer...." << std::endl;

	for (TextureDescList::const_iterator it = desc.mTextures.begin(); it != desc.mTextures.end(); ++it)
	{
		AddTexture(*it);
	}

	const glm::vec3 vertices[] =
	{
		glm::vec3(0.0f,	0.0f, 0.0f),
		glm::vec3(1.0f,	0.0f, 0.0f),
		glm::vec3(0.0f,	0.0f, 1.0f),
		glm::vec3(1.0f,	0.0f, 1.0f)
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

	LoadHeightMap(desc.mTerrains);

	std::cout << "... TerrainRenderer initialized!" << std::endl << std::endl;
}


TerrainRenderer::~TerrainRenderer()
{
	glDeleteTextures(1, &mHeightMapTextureId);
	mHeightMapTextureId = 0;
}

void TerrainRenderer::LoadShaders()
{
	PRINT_MESSAGE("Initialize Terrain Renderer Shaders : \n\n");

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

	PRINT_MESSAGE("... done.\n");
	PRINT_MESSAGE("-------------------------------------------------\n\n");
}
 
void TerrainRenderer::Render()
{
	if (!mShader.IsLoaded())
	{
		LoadShaders();
	}

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


void TerrainRenderer::LoadHeightMap(const MapDescList & terrainDescList)
{
	size_t layerBufferSize = mMapSize.x * mMapSize.y;
	size_t bufferMemorySize = layerBufferSize * terrainDescList.size() * sizeof(GLfloat);

	GLfloat * buffer = (GLfloat*)malloc(bufferMemorySize);
	if (buffer == nullptr)
	{
		PRINT_MESSAGE("Error: Cannot allocate memory to load terrain height maps ! (Needed memory \n");
		return;
	}
	
	GLfloat * layerBufferPtr = buffer;

	for (MapDescList::const_iterator it = terrainDescList.begin(); it != terrainDescList.end(); ++it)
	{
		const MapDesc & desc = *it;

		assert(desc.mHeightMapTextureWidth >= mMapSize.x);

		FILE * filePtr = nullptr;

		if (fopen_s(&filePtr, desc.mFilename.c_str(), "rb") == 0)
		{
			GLfloat * ptr;
			GLint incY;

			if (desc.mInvertY)
			{
				ptr = layerBufferPtr + (mMapSize.x * (mMapSize.y - 1));
				incY = -mMapSize.x;
			}
			else
			{
				ptr = layerBufferPtr;
				incY = mMapSize.x;
			}

			GLint seekCount = (desc.mHeightMapTextureWidth - mMapSize.x) * sizeof(GLfloat);
			GLint readCount = mMapSize.x * sizeof(GLfloat);

			for (GLint y = 0; y < mMapSize.y; ++y)
			{
				size_t n = fread(ptr, 1, readCount, filePtr);
				if (n != readCount)
				{
					char errmsg[200];
					strerror_s(errmsg, 200, errno);

					PRINT_MESSAGE("Error: Cannot read heigh map file '%s' : '%s'!\n", desc.mFilename.c_str(), errmsg);
					fclose(filePtr);
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
			PRINT_MESSAGE("Error: Cannot open heigh map file '%s'!\n", desc.mFilename.c_str());
			goto ExitLoadHeightMap;
		}

		layerBufferPtr += layerBufferSize;
	}

	glGenTextures(1, &mHeightMapTextureId);
	PRINT_MESSAGE("[TerrainRenderer] : Height map texture id : %i.\n", mHeightMapTextureId);

	glBindTexture(GL_TEXTURE_2D_ARRAY, mHeightMapTextureId);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R32F, mMapSize.x, mMapSize.y, (GLsizei)terrainDescList.size(), 0, GL_RED, GL_FLOAT, buffer);

	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

ExitLoadHeightMap:
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