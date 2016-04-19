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
	, mLowSlope(desc.mLowSlope)
	, mHighSlope(desc.mHighSlope)
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
	BuildMaterialShader(getMaterialsFuncSource, desc);
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

#define PRINT_CURRMAT_DIFFUSE_COLOR(varname, mat) \
{\
	sprintf_s(tmpBuffer, tmpBufferCount, "\t\t" #varname " = vec3(%f, %f, %f);\r\n", mat.mDiffuse.r, mat.mDiffuse.g, mat.mDiffuse.b); \
	generatedSource.append(tmpBuffer); \
	int samplerIndex = texInfo[mat.mDiffuseTextureIndex].GetSamplerIndex(); \
	int layerIndex = texInfo[mat.mDiffuseTextureIndex].GetLayerIndex(); \
	if (samplerIndex >= 0)\
	{\
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\tdiffuseY = texture(u_textureSampler[%i], vec3(uvs.xz, %i)).xyz;\r\n", samplerIndex, layerIndex); \
		generatedSource.append(tmpBuffer); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\tdiffuseX = texture(u_textureSampler[%i], vec3(uvs.zy, %i)).xyz;\r\n", samplerIndex, layerIndex); \
		generatedSource.append(tmpBuffer); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\tdiffuseZ = texture(u_textureSampler[%i], vec3(uvs.xy, %i)).xyz;\r\n", samplerIndex, layerIndex); \
		generatedSource.append(tmpBuffer); \
		generatedSource.append("\t\t" #varname " = " #varname " * ((blendWeights.y * diffuseY) + (blendWeights.x * diffuseX) + (blendWeights.z * diffuseZ));\r\n"); \
	}\
}

void TerrainRenderer::GenerateGetMaterialByHeight(std::string & generatedSource, const MaterialDescList & matDescList, const TextureInfoList & texInfo)
{
	const int tmpBufferCount = 300;
	char tmpBuffer[tmpBufferCount];

	generatedSource.append("\tvec3 diffuseColor = vec3(0, 0, 0);\r\n");
	generatedSource.append("\tvec3 diffuseColor2 = vec3(0, 0, 0);\r\n");
	generatedSource.append("\tfloat blend = 0;\r\n");
	generatedSource.append("\t\r\n");

	generatedSource.append("\tvec3 diffuseX, diffuseY, diffuseZ;\r\n");
	generatedSource.append("\t\r\n");

	if (!matDescList.empty())
	{
		GLfloat heightMin = matDescList.front().mHeightMin;

		int count = (int)matDescList.size();
		for (int curr = 0; curr < count; ++curr)
		{
			const MaterialDesc & currMat = matDescList[curr];
			int next = curr + 1;

			if (next < count)
			{
				const MaterialDesc & nextMat = matDescList[next];

				if (curr == 0)
				{
					sprintf_s(tmpBuffer, tmpBufferCount, "\tif (position.y < %f)\r\n", nextMat.mHeightMin);
				}
				else
				{
					sprintf_s(tmpBuffer, tmpBufferCount, "\telse if (position.y >= %f && position.y < %f)\r\n", heightMin, nextMat.mHeightMin);
				}
				generatedSource.append(tmpBuffer);
				generatedSource.append("\t{\r\n");

				PRINT_CURRMAT_DIFFUSE_COLOR(diffuseColor, currMat);

				generatedSource.append("\t}\r\n");

				sprintf_s(tmpBuffer, tmpBufferCount, "\telse if (position.y >= %f && position.y < %f)\r\n", nextMat.mHeightMin, currMat.mHeightMax);
				generatedSource.append(tmpBuffer);
				generatedSource.append("\t{\r\n");

				PRINT_CURRMAT_DIFFUSE_COLOR(diffuseColor, currMat);
				generatedSource.append("\t\r\n");


				sprintf_s(tmpBuffer, tmpBufferCount, "\t\tblend = (position.y - %f) / (%f - %f);\r\n", nextMat.mHeightMin, currMat.mHeightMax, nextMat.mHeightMin);
				generatedSource.append(tmpBuffer);

				PRINT_CURRMAT_DIFFUSE_COLOR(diffuseColor2, nextMat);

				generatedSource.append("\t}\r\n");

				heightMin = nextMat.mHeightMin;
			}
			else
			{
				generatedSource.append("\telse\r\n");
				generatedSource.append("\t{\r\n");

				PRINT_CURRMAT_DIFFUSE_COLOR(diffuseColor, currMat);

				generatedSource.append("\t}\r\n");
			}
		}
	}

	generatedSource.append("\tmat.DiffuseColor = mix(diffuseColor, diffuseColor2, blend);\r\n");
}

void TerrainRenderer::BuildMaterialShader(std::string & generatedSource, const Desc & desc)
{
	const int tmpBufferCount = 300;
	char tmpBuffer[tmpBufferCount];

	const TextureInfoList & texInfo = GetTextureInfoList();

	// find low slope textures indexes
	generatedSource.append("void GetLowSlopeMaterial(out Material mat, vec3 uvs, vec3 blendWeights, vec3 position)\r\n");
	generatedSource.append("{\r\n");

	GenerateGetMaterialByHeight(generatedSource, desc.mLowSlopeMaterials, texInfo);

	generatedSource.append("}\r\n\r\n");

	// find low slope textures indexes
	generatedSource.append("void GetHighSlopeMaterial(out Material mat, vec3 uvs, vec3 blendWeights, vec3 position)\r\n");
	generatedSource.append("{\r\n");

	GenerateGetMaterialByHeight(generatedSource, desc.mHighSlopeMaterials, texInfo);

	generatedSource.append("}\r\n\r\n");


	generatedSource.append("void GetMaterial(out Material mat, vec3 uvs, vec3 blendWeights, vec3 normal, vec3 position)\r\n");
	generatedSource.append("{\r\n");

	sprintf_s(tmpBuffer, tmpBufferCount, "\tif (normal.y > %f)\r\n", desc.mHighSlope);
	generatedSource.append(tmpBuffer);
	generatedSource.append("\t{\r\n");
	generatedSource.append("\t\tGetLowSlopeMaterial(mat, uvs, blendWeights, position);\r\n");
	generatedSource.append("\t}\r\n");

	sprintf_s(tmpBuffer, tmpBufferCount, "\telse if (normal.y < %f)\r\n", desc.mLowSlope);
	generatedSource.append(tmpBuffer);
	generatedSource.append("\t{\r\n");
	generatedSource.append("\t\tGetHighSlopeMaterial(mat, uvs, blendWeights, position);\r\n");
	generatedSource.append("\t}\r\n");

	generatedSource.append("\telse\r\n");
	generatedSource.append("\t{\r\n");
	generatedSource.append("\t\tMaterial lowSlopeMat, highSlopeMat;\r\n");
	generatedSource.append("\t\tGetLowSlopeMaterial(lowSlopeMat, uvs, blendWeights, position);\r\n");
	generatedSource.append("\t\tGetHighSlopeMaterial(highSlopeMat, uvs, blendWeights, position);\r\n");
	generatedSource.append("\r\n");
	sprintf_s(tmpBuffer, tmpBufferCount, "\t\tfloat blend = (normal.y - %f) / (%f - %f);\r\n", desc.mLowSlope, desc.mHighSlope, desc.mLowSlope);
	generatedSource.append(tmpBuffer);
	generatedSource.append("\t\tBlendMaterial(mat, highSlopeMat, lowSlopeMat, blend);\r\n");
	generatedSource.append("\t}\r\n");


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