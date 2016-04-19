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
	, mLowSlopeMax(desc.mLowSlopeMax)
	, mHiSlopeMin(desc.mHiSlopeMin)
	, mHeightMapTextureId(0)
	, mDrawNormalShader("TerrainDrawNormals")
	//, mDiffuseTextures(nullptr)
{
	std::cout << std::endl;
	std::cout << "Initialize TerrainRenderer...." << std::endl;

	AddTextures(desc.mTextures);
	BuildTextureMapping(nullptr);
	LoadTextures();

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

	//UpdateMaterialTextureIndex(desc);
	LoadShaders(desc);

	std::cout << "... TerrainRenderer initialized!" << std::endl << std::endl;
}


TerrainRenderer::~TerrainRenderer()
{
	glDeleteTextures(1, &mHeightMapTextureId);
	mHeightMapTextureId = 0;
}

void TerrainRenderer::LoadShaders(const Desc & desc)
{
	PRINT_MESSAGE("Initialize Terrain Renderer Shaders : \n\n");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_TexScale",
		"u_HeightMap",
		//"u_DiffuseMap"
	};


	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/terrain.vs.glsl");
	mShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/terrain.tcs.glsl");
	mShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/terrain.tes.glsl");
	mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/terrain.gs.glsl");

//	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/terrain.deferred.fs.glsl");
	PRINT_MESSAGE("Loading shader file : shaders/terrain.deferred.fs.glsl\n");
	// fragment shader
	std::vector<std::string> lightFsGlsl(3);
	Shader::MergeFile(lightFsGlsl[0], "shaders/terrain.deferred.fs.glsl");
	std::string & textureFuncSource = lightFsGlsl[1];
	Shader::GenerateTexGetFunction(textureFuncSource, (int)mTextureMapping.mMapping.size());
	std::string & getMaterialsFuncSource = lightFsGlsl[2];
	BuildMaterailShader(getMaterialsFuncSource, desc);
	mShader.LoadFromString(GL_FRAGMENT_SHADER, lightFsGlsl);

	mShader.CreateAndLinkProgram();
	mShader.Use();

	mShader.AddUniforms(uniformNames, __uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform2iv(mShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2iv(mShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
	glUniform3fv(mShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;

	glUniform1i(mShader.GetUniform(u_HeightMap), 0); GL_CHECK_ERRORS;
	//glUniform1i(mShader.GetUniform(u_DiffuseMap), 1); GL_CHECK_ERRORS;


	for (int i = 0; i < (int)mTextureMapping.mMapping.size(); ++i)
	{
		char uniformName[50];
		sprintf_s(uniformName, 50, "u_textureSampler[%i]", i);
		int uniformIndex = glGetUniformLocation(mShader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		if (uniformIndex > 0)
		{
			glUniform1i(uniformIndex, i + FIRST_TEXTURE_SAMPLER_INDEX);	GL_CHECK_ERRORS;
			std::cout << "\t" << uniformName << " : " << uniformIndex << std::endl;
		}
	}

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

#define PRINT_CURR_MAT \
sprintf_s(tmpBuffer, tmpBufferCount, "\t\tmat.DiffuseColor = vec3(%f, %f, %f);\r\n", currMat.mDiffuse.r, currMat.mDiffuse.g, currMat.mDiffuse.b); \
generatedSource.append(tmpBuffer); \
sprintf_s(tmpBuffer, tmpBufferCount, "\t\tmat.DiffuseSamplerIndex = %i;\r\n", texInfo[currMat.mDiffuseTextureIndex].GetSamplerIndex()); \
generatedSource.append(tmpBuffer); \
sprintf_s(tmpBuffer, tmpBufferCount, "\t\tmat.DiffuseLayerIndex = %i;\r\n", texInfo[currMat.mDiffuseTextureIndex].GetLayerIndex()); \
generatedSource.append(tmpBuffer);


void TerrainRenderer::BuildMaterailShader(std::string & generatedSource, const Desc & desc)
{
	const TextureInfoList & texInfo = GetTextureInfoList();

	const int tmpBufferCount = 200;
	char tmpBuffer[tmpBufferCount];

	// find low slope textures indexes
	generatedSource.append("void FindLowSlopeTextures(out Material mat, vec3 position)\r\n");
	generatedSource.append("{\r\n");

	generatedSource.append("\tmat.DiffuseColor2 = vec3(0, 0, 0);\r\n");
	generatedSource.append("\tmat.DiffuseSamplerIndex2 = -1;\r\n");
	generatedSource.append("\tmat.DiffuseLayerIndex2 = -1;\r\n");
	generatedSource.append("\t\r\n");
	generatedSource.append("\tmat.DiffuseBlend = 0;\r\n");

	if (!desc.mLowSlopeMaterials.empty())
	{		
		GLfloat heightMin = desc.mLowSlopeMaterials.front().mHeightMin;

		{
			const MaterialDesc & currMat = desc.mLowSlopeMaterials[0];
			sprintf_s(tmpBuffer, tmpBufferCount, "\tif (position.y < %f)\r\n", heightMin);
			generatedSource.append(tmpBuffer);
			generatedSource.append("\t{\r\n");

			PRINT_CURR_MAT;

			generatedSource.append("\t}\r\n");
		}

		int count = (int)desc.mLowSlopeMaterials.size();
		for (int curr = 0; curr < count; ++curr)
		{	
			const MaterialDesc & currMat = desc.mLowSlopeMaterials[curr];
			int next = curr + 1;

			if (next < count)
			{
				const MaterialDesc & nextMat = desc.mLowSlopeMaterials[next];

				sprintf_s(tmpBuffer, tmpBufferCount, "\telse if (position.y >= %f && position.y < %f)\r\n", heightMin, nextMat.mHeightMin);
				generatedSource.append(tmpBuffer);
				generatedSource.append("\t{\r\n");
				
				PRINT_CURR_MAT;

				generatedSource.append("\t}\r\n");

				sprintf_s(tmpBuffer, tmpBufferCount, "\telse if (position.y >= %f && position.y < %f)\r\n", nextMat.mHeightMin, currMat.mHeightMax);
				generatedSource.append(tmpBuffer);
				generatedSource.append("\t{\r\n");

				PRINT_CURR_MAT;
				generatedSource.append("\t\r\n");

				sprintf_s(tmpBuffer, tmpBufferCount, "\t\tmat.DiffuseColor2 = vec3(%f, %f, %f);\r\n", nextMat.mDiffuse.r, nextMat.mDiffuse.g, nextMat.mDiffuse.b);
				generatedSource.append(tmpBuffer);
				sprintf_s(tmpBuffer, tmpBufferCount, "\t\tmat.DiffuseSamplerIndex2 = %i;\r\n", texInfo[nextMat.mDiffuseTextureIndex].GetSamplerIndex());
				generatedSource.append(tmpBuffer);
				sprintf_s(tmpBuffer, tmpBufferCount, "\t\tmat.DiffuseLayerIndex2 = %i;\r\n", texInfo[nextMat.mDiffuseTextureIndex].GetLayerIndex());
				generatedSource.append(tmpBuffer);

				sprintf_s(tmpBuffer, tmpBufferCount, "\t\tmat.DiffuseBlend = (position.y - %f) / (%f - %f);\r\n", nextMat.mHeightMin, currMat.mHeightMax, nextMat.mHeightMin);
				generatedSource.append(tmpBuffer);

				generatedSource.append("\t}\r\n");

				heightMin = nextMat.mHeightMin;
			}
			else
			{
				generatedSource.append("\telse\r\n");
				generatedSource.append("\t{\r\n");

				PRINT_CURR_MAT;

				generatedSource.append("\t}\r\n");
			}
		}
	}
	else
	{
		generatedSource.append("\tmat.DiffuseColor = vec3(0, 0, 0);\r\n");
		generatedSource.append("\tmat.DiffuseSamplerIndex = -1;\r\n");
		generatedSource.append("\tmat.DiffuseLayerIndex = -1;\r\n");
	}
	generatedSource.append("}\r\n\r\n");

	generatedSource.append("void GetMaterial(out BlendedMaterial blendedMat, vec3 uvs, vec3 blendWeights, vec3 normal, vec3 position)\r\n");
	generatedSource.append("{\r\n");

	//sprintf_s(tmpBuffer, tmpBufferCount, "\tif (normal.y < %f)\r\n", desc.mHiSlopeMin);
	//generatedSource.append(tmpBuffer);
	//generatedSource.append("\t{\r\n");

	generatedSource.append("\t\tMaterial lowSlopeMat;\r\n");
	generatedSource.append("\t\tFindLowSlopeTextures(lowSlopeMat, position);\r\n\r\n");

	generatedSource.append("\t\tBlendMaterials(blendedMat, lowSlopeMat, uvs, blendWeights);\r\n\r\n");

	//generatedSource.append("\t}\r\n");
	//sprintf_s(tmpBuffer, tmpBufferCount, "\telse if (normal.y < %f)\r\n", desc.mLowSlopeMax);
	//generatedSource.append(tmpBuffer);
	//generatedSource.append("\t{\r\n");

	//generatedSource.append("\t}\r\n");
	//generatedSource.append("\telse\r\n");
	//generatedSource.append("\t{\r\n");

	//generatedSource.append("\t}\r\n");


	generatedSource.append("}\r\n");
}
 
void TerrainRenderer::Render()
{
	//if (!mShader.IsLoaded())
	//{
	//	LoadShaders();
	//}

	mShader.Use();
	glBindVertexArray(mVaoID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, mHeightMapTextureId);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D_ARRAY, mDiffuseTextures->GetResourceId());

	for (int i = 0; i < (int)mTextureMapping.mMapping.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + FIRST_TEXTURE_SAMPLER_INDEX + i);
		glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureMapping.mMapping[i].mTexture->GetResourceId());
	}

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

//void TerrainRenderer::UpdateMaterialTextureIndex(const Desc & desc)
//{
//	for (MaterialDescList::const_iterator matIt = desc.mLowSlopeMaterials.begin(); matIt != desc.mLowSlopeMaterials.end(); ++matIt)
//	{
//
//	}
//
//	for (TextureMappingList::const_iterator it = mTextureMapping.mMapping.begin(); it != mTextureMapping.mMapping.end(); ++it)
//	{
//		if (it->mTexture->GetCategory() == TextureCategory::Diffuse)
//		{
//			mDiffuseTextures = it->mTexture;
//			break;
//		}			
//	}
//}


	} // namespace Renderers
} // namespace CoreFx