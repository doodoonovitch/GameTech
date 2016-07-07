#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



TerrainRenderer::TerrainRenderer(const Desc & desc)
	: RendererHelper<1>(0, "TerrainRenderer", "TerrainWireRenderer")
	, mMapSize(desc.mHeightMapWidth, desc.mHeightMapDepth)
	, mPatchCount(desc.mHeightMapWidth / 64, desc.mHeightMapDepth / 64)
	, mScale(desc.mScale)
	, mLowSlope(desc.mLowSlope)
	, mHighSlope(desc.mHighSlope)
	, mHeightMapTextureId(0)
	, mMapCount(0)
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
	CreateBuffers();

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

	PerMapData * modelMatrixBuffer = new PerMapData[mMapCount];
	for (int i = 0; i < mMapCount; ++i)
	{
		modelMatrixBuffer[i].mModelDQ = desc.mTerrains[i].mModelDQ;
	}
	mModelMatrixBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, (mMapCount * sizeof(PerMapData)), modelMatrixBuffer);


	//UpdateMaterialTextureIndex(desc);
	LoadShaders(desc);

	std::cout << "... TerrainRenderer initialized!" << std::endl << std::endl;
}


TerrainRenderer::~TerrainRenderer()
{
	mModelMatrixBuffer.ReleaseResource();
	glDeleteTextures(1, &mHeightMapTextureId);
	mHeightMapTextureId = 0;
}

void TerrainRenderer::LoadShaders(const Desc & desc)
{
	LoadMainShader(desc);
	LoadWireFrameShader(desc);
}

void TerrainRenderer::LoadMainShader(const Desc & desc)
{
	PRINT_MESSAGE("Initialize Terrain Renderer Shaders : \n\n");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_TexScale",
		"u_HeightMap",
		"u_PerMapDataSampler",
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
	//desc;
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
	glUniform1i(mShader.GetUniform(u_PerMapDataSampler), 1); GL_CHECK_ERRORS;


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


	PRINT_MESSAGE("... done.\n");
	PRINT_MESSAGE("-------------------------------------------------\n\n");
}

void TerrainRenderer::LoadWireFrameShader(const Desc & /*desc*/)
{
	PRINT_MESSAGE("Initialize Terrain Renderer (Wire Frame) Shaders : \n\n");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_TexScale",
		"u_HeightMap",
		"u_PerMapDataSampler",
	};


	//setup shader
	mWireFrameShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/terrain.vs.glsl");
	mWireFrameShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/terrain.tcs.glsl");
	mWireFrameShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/terrain.tes.glsl");
	mWireFrameShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/terrain.gs.glsl");
	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/terrain.wireframe.fs.glsl");

	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();

	mWireFrameShader.AddUniforms(uniformNames, __uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform2iv(mWireFrameShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2iv(mWireFrameShader.GetUniform(u_MapSize), 1, glm::value_ptr(mMapSize)); GL_CHECK_ERRORS;
	glUniform3fv(mWireFrameShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;

	glUniform1i(mWireFrameShader.GetUniform(u_HeightMap), 0); GL_CHECK_ERRORS;
	glUniform1i(mWireFrameShader.GetUniform(u_PerMapDataSampler), 1); GL_CHECK_ERRORS;

	mWireFrameShader.SetupFrameDataBlockBinding();
	mWireFrameShader.UnUse();

	GL_CHECK_ERRORS;


	PRINT_MESSAGE("... done.\n");
	PRINT_MESSAGE("-------------------------------------------------\n\n");
}

#define PRINT_MATERIAL_COLOR(diffuseVar, specularVar, specularPowerVar, heightVar, mat) \
{\
	sprintf_s(tmpBuffer, tmpBufferCount, "\t\t%s = %f;\r\n", specularPowerVar, mat.mRoughness); \
	generatedSource.append(tmpBuffer); \
	int diffuseSamplerIndex = (mat.mDiffuseTextureIndex != Renderer::NoTexture) ? texInfo[mat.mDiffuseTextureIndex].GetSamplerIndex() : -1; \
	int specularSamplerIndex = (mat.mSpecularTextureIndex != Renderer::NoTexture) ? texInfo[mat.mSpecularTextureIndex].GetSamplerIndex() : -1; \
	int heightSamplerIndex = (mat.mNormalTextureIndex != Renderer::NoTexture) ? texInfo[mat.mNormalTextureIndex].GetSamplerIndex() : -1; \
	if (diffuseSamplerIndex >= 0 || specularSamplerIndex >= 0 || heightSamplerIndex >= 0)\
	{\
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\ttexCoord = uvs * %f;\r\n", mat.mTexScale); \
		generatedSource.append(tmpBuffer); \
	}\
	if (diffuseSamplerIndex >= 0)\
	{\
		int layerIndex = texInfo[mat.mDiffuseTextureIndex].GetLayerIndex(); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\ttexColorY = texture(u_textureSampler[%i], vec3(texCoord.xz, %i)).xyz;\r\n", diffuseSamplerIndex, layerIndex); \
		generatedSource.append(tmpBuffer); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\ttexColorX = texture(u_textureSampler[%i], vec3(texCoord.zy, %i)).xyz;\r\n", diffuseSamplerIndex, layerIndex); \
		generatedSource.append(tmpBuffer); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\ttexColorZ = texture(u_textureSampler[%i], vec3(texCoord.xy, %i)).xyz;\r\n", diffuseSamplerIndex, layerIndex); \
		generatedSource.append(tmpBuffer); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\t%s = vec3(%f, %f, %f) * ((blendWeights.y * texColorY) + (blendWeights.x * texColorX) + (blendWeights.z * texColorZ));\r\n", diffuseVar, mat.mDiffuse.r, mat.mDiffuse.g, mat.mDiffuse.b); \
		generatedSource.append(tmpBuffer); \
	}\
	else\
	{\
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\t%s = vec3(%f, %f, %f);\r\n", diffuseVar, mat.mDiffuse.r, mat.mDiffuse.g, mat.mDiffuse.b); \
		generatedSource.append(tmpBuffer); \
	}\
	if (specularSamplerIndex >= 0)\
	{\
		int layerIndex = texInfo[mat.mSpecularTextureIndex].GetLayerIndex(); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\ttexColorY = texture(u_textureSampler[%i], vec3(texCoord.xz, %i)).xyz;\r\n", specularSamplerIndex, layerIndex); \
		generatedSource.append(tmpBuffer); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\ttexColorX = texture(u_textureSampler[%i], vec3(texCoord.zy, %i)).xyz;\r\n", specularSamplerIndex, layerIndex); \
		generatedSource.append(tmpBuffer); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\ttexColorZ = texture(u_textureSampler[%i], vec3(texCoord.xy, %i)).xyz;\r\n", specularSamplerIndex, layerIndex); \
		generatedSource.append(tmpBuffer); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\t%s = vec3(%f, %f, %f) * ((blendWeights.y * texColorY) + (blendWeights.x * texColorX) + (blendWeights.z * texColorZ));\r\n", specularVar, mat.mSpecular.r, mat.mSpecular.g, mat.mSpecular.b); \
		generatedSource.append(tmpBuffer); \
	}\
	else\
	{\
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\t%s = vec3(%f, %f, %f);\r\n", specularVar, mat.mSpecular.r, mat.mSpecular.g, mat.mSpecular.b); \
		generatedSource.append(tmpBuffer); \
	}\
	if (heightSamplerIndex >= 0)\
	{\
		int layerIndex = texInfo[mat.mNormalTextureIndex].GetLayerIndex(); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\theightY = texture(u_textureSampler[%i], vec3(texCoord.xz, %i)).x;\r\n", heightSamplerIndex, layerIndex); \
		generatedSource.append(tmpBuffer); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\theightX = texture(u_textureSampler[%i], vec3(texCoord.zy, %i)).x;\r\n", heightSamplerIndex, layerIndex); \
		generatedSource.append(tmpBuffer); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\theightZ = texture(u_textureSampler[%i], vec3(texCoord.xy, %i)).x;\r\n", heightSamplerIndex, layerIndex); \
		generatedSource.append(tmpBuffer); \
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\t%s = (blendWeights.y * heightY) + (blendWeights.x * heightX) + (blendWeights.z * heightZ);\r\n", heightVar); \
		generatedSource.append(tmpBuffer); \
	}\
	else\
	{\
		sprintf_s(tmpBuffer, tmpBufferCount, "\t\t%s = 0;\r\n", heightVar); \
		generatedSource.append(tmpBuffer); \
	}\
}

void TerrainRenderer::GenerateGetMaterialByHeight(std::string & generatedSource, const MaterialDescList & matDescList, const TextureInfoList & texInfo)
{
	const int tmpBufferCount = 300;
	char tmpBuffer[tmpBufferCount];

	generatedSource.append("\tvec3 diffuseColor = vec3(0, 0, 0);\r\n");
	generatedSource.append("\tvec3 diffuseColor2 = vec3(0, 0, 0);\r\n");
	generatedSource.append("\tvec3 specularColor = vec3(0, 0, 0);\r\n");
	generatedSource.append("\tvec3 specularColor2 = vec3(0, 0, 0);\r\n");
	generatedSource.append("\tfloat specularPower = 0;\r\n");
	generatedSource.append("\tfloat specularPower2 = 0;\r\n");
	generatedSource.append("\tfloat height = 0;\r\n");
	generatedSource.append("\tfloat height2 = 0;\r\n");
	generatedSource.append("\tfloat blend = 0;\r\n");
	generatedSource.append("\t\r\n");

	generatedSource.append("\tvec3 texCoord, texColorX, texColorY, texColorZ;\r\n");
	generatedSource.append("\tfloat heightX, heightY, heightZ;\r\n");
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

				PRINT_MATERIAL_COLOR("mat.DiffuseColor", "mat.SpecularColor", "mat.Roughness", "mat.Height", currMat);

				generatedSource.append("\t}\r\n");

				sprintf_s(tmpBuffer, tmpBufferCount, "\telse if (position.y >= %f && position.y < %f)\r\n", nextMat.mHeightMin, currMat.mHeightMax);
				generatedSource.append(tmpBuffer);
				generatedSource.append("\t{\r\n");

				PRINT_MATERIAL_COLOR("diffuseColor", "specularColor", "specularPower", "height", currMat);
				generatedSource.append("\t\r\n");


				sprintf_s(tmpBuffer, tmpBufferCount, "\t\tblend = (position.y - %f) / (%f - %f);\r\n", nextMat.mHeightMin, currMat.mHeightMax, nextMat.mHeightMin);
				generatedSource.append(tmpBuffer);

				PRINT_MATERIAL_COLOR("diffuseColor2", "specularColor2", "specularPower2", "height2", nextMat);

				generatedSource.append("\r\n");
				generatedSource.append("\t\tmat.DiffuseColor = mix(diffuseColor, diffuseColor2, blend);\r\n");
				generatedSource.append("\t\tmat.SpecularColor = mix(specularColor, specularColor2, blend);\r\n");
				generatedSource.append("\t\tmat.Roughness = mix(specularPower, specularPower2, blend);\r\n");
				generatedSource.append("\t\tmat.Height = mix(height, height2, blend);\r\n");

				generatedSource.append("\t}\r\n");

				heightMin = nextMat.mHeightMin;
			}
			else
			{
				generatedSource.append("\telse\r\n");
				generatedSource.append("\t{\r\n");

				PRINT_MATERIAL_COLOR("mat.DiffuseColor", "mat.SpecularColor", "mat.Roughness", "mat.Height", currMat);

				generatedSource.append("\t}\r\n");
			}
		}
	}

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

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());

	for (int i = 0; i < (int)mTextureMapping.mMapping.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + FIRST_TEXTURE_SAMPLER_INDEX + i);
		glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureMapping.mMapping[i].mTexture->GetResourceId());
	}

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y * mMapCount);

	glBindVertexArray(0);
	mShader.UnUse();
}

void TerrainRenderer::RenderWireFrame()
{
	mWireFrameShader.Use();
	glBindVertexArray(mVaoID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, mHeightMapTextureId);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y * mMapCount);

	glBindVertexArray(0);
	mWireFrameShader.UnUse();
}


void TerrainRenderer::LoadHeightMap(const MapDescList & terrainDescList)
{
	mMapCount = (GLint)terrainDescList.size();
	size_t layerBufferSize = mMapSize.x * mMapSize.y;
	size_t bufferMemorySize = layerBufferSize * mMapCount * sizeof(GLfloat);

	GLfloat * buffer = (GLfloat*)malloc(bufferMemorySize);
	if (buffer == nullptr)
	{
		PRINT_MESSAGE("Error: Cannot allocate memory to load terrain height maps ! (Needed memory : %ld)\n", bufferMemorySize);
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
				if (n != (size_t)readCount)
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