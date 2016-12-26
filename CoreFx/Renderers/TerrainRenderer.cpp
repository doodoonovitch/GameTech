#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{

bool TerrainRenderer::Desc::CheckData() const
{
	if (mMaterials.empty() && !mTerrains.empty())
	{
		PRINT_ERROR("No terrains material !");
		return false;
	}

	for (MapDescList::const_iterator it = mTerrains.begin(); it != mTerrains.end(); ++it)
	{
		for (int i = 0; i < 4; ++i)
		{
			auto matIndex = it->mMatIndex[i];
			if (!(matIndex >= 0 && matIndex < mMaterials.size()))
			{
				PRINT_ERROR("Undefined terrains material index %li ! (materials count = %li)", matIndex, mMaterials.size());
				return false;
			}
		}
	}

	return true;
}

TerrainRenderer::TerrainRenderer(const Desc & desc)
	: RendererHelper<1>("TerrainRenderer", "TerrainWireRenderer")
	, mMapSize(desc.mHeightMapSize)
	, mPatchCount(desc.mHeightMapSize / 64, desc.mHeightMapSize / 64)
	, mScale(desc.mScale)
	, mMapCount(0)
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize TerrainRenderer.....");

	if (!desc.CheckData())
	{
		goto ExitTerrainRendererInit;
	}

	mShaderMaterialList.resize(desc.mMaterials.size());

	AddTextures(desc.mTextures);
	BuildTextureMapping();
	LoadTextures();
	UpdateMaterialTextureIndex(desc);

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

	LoadHeightMap(desc);

	size_t perInstanceDataSize = mMapCount * sizeof(TerrainPerInstanceData);
	TerrainPerInstanceData * perInstanceData = (TerrainPerInstanceData *)malloc(perInstanceDataSize);
	TerrainPerInstanceData * ptr = perInstanceData;
	for (MapDescList::const_iterator it = desc.mTerrains.begin(); it < desc.mTerrains.end(); ++it)
	{
		ptr->mOrigin = it->mOrigin;
		ptr->mMatIndex = it->mMatIndex;
		ptr->__padding__[0] = 0.f;
		++ptr;
	}
	mPerInstanceDataBuffer.CreateResource(GL_STATIC_DRAW, perInstanceDataSize, perInstanceData);

	free(perInstanceData);

	size_t materialBufferSize = desc.mMaterials.size() * ModelRenderer::cShaderMaterialSize;
	mMaterialBuffer.CreateResource(GL_STATIC_DRAW, materialBufferSize, mShaderMaterialList.data());

	PRINT_GEN_SHADERSTORAGEBUFFER("[ModelRenderer]", mMaterialBuffer);
	PRINT_GEN_SHADERSTORAGEBUFFER("[ModelRenderer]", mPerInstanceDataBuffer);
	
	LoadShaders(desc);

	mIsInitialized = true;


ExitTerrainRendererInit:

	PRINT_MESSAGE(".....TerrainRenderer initialized!");
	PRINT_END_SECTION;
	
}


TerrainRenderer::~TerrainRenderer()
{

}

void TerrainRenderer::LoadShaders(const Desc & desc)
{
	LoadMainShader(desc);
	LoadWireFrameShader(desc);
}

void TerrainRenderer::LoadMainShader(const Desc & /*desc*/)
{
	PRINT_MESSAGE("Initialize Terrain Renderer Shaders.....");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_HeightMap",
		"u_BlendMap",
		"u_NormalMap",
	};


	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/terrain.vs.glsl");
	mShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/terrain.tcs.glsl");
	mShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/terrain.tes.glsl");

	// fragment shader
	std::vector<std::string> fsGlsl(3);
	PRINT_MESSAGE("Loading shader file : shaders/DeferredShadingCommon.incl.glsl");
	Shader::MergeFile(fsGlsl[0], "shaders/DeferredShadingCommon.incl.glsl");

	PRINT_MESSAGE("Loading shader file : shaders/terrain.deferred.fs.glsl\n");
	Shader::MergeFile(fsGlsl[1], "shaders/terrain.deferred.fs.glsl");

	std::string & textureFuncSource = fsGlsl[2];
	Shader::GenerateTexGetFunction(textureFuncSource, (int)mTextureMapping.mMapping.size());
	
	mShader.LoadFromString(GL_FRAGMENT_SHADER, fsGlsl);

	mShader.CreateAndLinkProgram();
	mShader.Use();

	mShader.AddUniforms(uniformNames, __uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform2iv(mShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2i(mShader.GetUniform(u_MapSize), mMapSize, mMapSize); GL_CHECK_ERRORS;
	glUniform3fv(mShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;

	glUniform1i(mShader.GetUniform(u_HeightMap), 0); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_BlendMap), 1); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_NormalMap), 2); GL_CHECK_ERRORS;


	for (int i = 0; i < (int)mTextureMapping.mMapping.size(); ++i)
	{
		char uniformName[50];
		sprintf_s(uniformName, 50, "u_textureSampler[%i]", i);
		int uniformIndex = glGetUniformLocation(mShader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		if (uniformIndex > 0)
		{
			glUniform1i(uniformIndex, i + FIRST_TEXTURE_SAMPLER_INDEX);	GL_CHECK_ERRORS;
			PRINT_MESSAGE("\t%s : %i", uniformName, uniformIndex);
		}
	}

	mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;


	PRINT_MESSAGE(".....done.");
}

void TerrainRenderer::LoadWireFrameShader(const Desc & /*desc*/)
{
	PRINT_MESSAGE("Initialize Terrain Renderer (Wire Frame) Shaders...");

	const char * uniformNames[] =
	{
		"u_PatchCount",
		"u_MapSize",
		"u_Scale",
		"u_HeightMap",
	};


	//setup shader
	mWireFrameShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/terrain.vs.glsl");
	mWireFrameShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/terrain.tcs.glsl");
	mWireFrameShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/terrain.tes.glsl");
	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/terrain.wireframe.fs.glsl");

	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();

	mWireFrameShader.AddUniforms(uniformNames, ARRAY_SIZE_IN_ELEMENTS(uniformNames));

	//pass values of constant uniforms at initialization
	glUniform2iv(mWireFrameShader.GetUniform(u_PatchCount), 1, glm::value_ptr(mPatchCount)); GL_CHECK_ERRORS;
	glUniform2i(mWireFrameShader.GetUniform(u_MapSize), mMapSize, mMapSize); GL_CHECK_ERRORS;
	glUniform3fv(mWireFrameShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;

	glUniform1i(mWireFrameShader.GetUniform(u_HeightMap), 0); GL_CHECK_ERRORS;

	mWireFrameShader.SetupFrameDataBlockBinding();
	mWireFrameShader.UnUse();

	GL_CHECK_ERRORS;


	PRINT_MESSAGE(".....done.\n");
}

void TerrainRenderer::Update()
{
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
	glBindTexture(mHeightMaps->GetTarget(), mHeightMaps->GetResourceId());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(mBlendMaps->GetTarget(), mBlendMaps->GetResourceId());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(mNormalMaps->GetTarget(), mNormalMaps->GetResourceId());

	for (int i = 0; i < (int)mTextureMapping.mMapping.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + FIRST_TEXTURE_SAMPLER_INDEX + i);
		glBindTexture(mTextureMapping.mMapping[i].mTexture->GetTarget(), mTextureMapping.mMapping[i].mTexture->GetResourceId());
	}

	mPerInstanceDataBuffer.BindBufferBase(u_PerInstanceData);
	mMaterialBuffer.BindBufferBase(u_Materials);

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
	glBindTexture(mHeightMaps->GetTarget(), mHeightMaps->GetResourceId());

	mPerInstanceDataBuffer.BindBufferBase(u_PerInstanceData);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArraysInstanced(GL_PATCHES, 0, 4, mPatchCount.x * mPatchCount.y * mMapCount);

	glBindVertexArray(0);
	mWireFrameShader.UnUse();
}


bool TerrainRenderer::LoadHeightMap(const Desc & terrainDesc)
{
	const glm::vec4 mask[4] = 
	{
		{ 1.f, 0.f, 0.f, 0.f },
		{ 0.f, 1.f, 0.f, 0.f },
		{ 0.f, 0.f, 1.f, 0.f },
		{ 0.f, 0.f, 0.f, 1.f }
	};

	const glm::vec4 nullVector(0.f);
	const glm::ivec4 internalFormat(GL_R8, GL_RG8, GL_RGB8, GL_RGBA8);
	const glm::ivec4 dataFormat(GL_RED, GL_RG, GL_RGB, GL_RGBA);

	const MapDescList & terrainDescList = terrainDesc.mTerrains;

	bool result = false;

	const int mipMapCount = TextureManager::GetNumberOfMipMapLevels(mMapSize, mMapSize);

	mMapCount = (GLint)terrainDescList.size();
	size_t layerBufferTexelCount = mMapSize * mMapSize;
	size_t heightBufferSize = layerBufferTexelCount * sizeof(GLfloat);
	size_t normalBufferSize = layerBufferTexelCount * 3 * sizeof(GLfloat);
	size_t blendBufferSize = layerBufferTexelCount * BlendMaterialCount * sizeof(GLubyte);

	GLfloat * heightBuffer = (GLfloat*)malloc(heightBufferSize);
	GLfloat * normalBuffer = (GLfloat*)malloc(normalBufferSize);
	GLubyte * blendBuffer = (GLubyte*)malloc(blendBufferSize);

	if (heightBuffer == nullptr || blendBuffer == nullptr || normalBuffer == nullptr)
	{
		PRINT_MESSAGE("Error: Cannot allocate memory to load terrain height maps ! (Needed memory : %ld)\n", heightBufferSize + blendBufferSize + normalBufferSize);
		goto ExitLoadHeightMap;
	}

	TextureManager * textureManager = Engine::GetInstance()->GetTextureManager();

	mHeightMaps = textureManager->CreateTexture2DArray(mipMapCount, GL_R16F, mMapSize, mMapSize, mMapCount, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
	mNormalMaps = textureManager->CreateTexture2DArray(mipMapCount, GL_RGB16F, mMapSize, mMapSize, mMapCount, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
	mBlendMaps = textureManager->CreateTexture2DArray(mipMapCount, internalFormat[BlendMaterialCount - 1], mMapSize, mMapSize, mMapCount, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);

	PRINT_MESSAGE("Height maps texture : %li.", mHeightMaps->GetResourceId());
	PRINT_MESSAGE("Normal maps texture : %li.", mNormalMaps->GetResourceId());
	PRINT_MESSAGE("Blend maps texture : %li.", mBlendMaps->GetResourceId());

	int mapIndex = 0;
	for (MapDescList::const_iterator it = terrainDescList.begin(); it != terrainDescList.end(); ++it, ++mapIndex)
	{
		const MapDesc & mapDesc = *it;

		assert(mapDesc.mHeightMapTextureSize >= mMapSize);

		GLsizei texWidth, texHeight;
		bool loaded = TextureManager::LoadTiffImage(texWidth, texHeight, mapDesc.mFilename, [this, heightBuffer](uint32_t w, uint32_t h, const uint32_t * raster)
		{
			h = std::min<uint32_t>(mMapSize, h);
			w = std::min<uint32_t>(mMapSize, w);

			GLfloat * heightPtr = heightBuffer;

			const uint32_t * src = raster;
			for (GLuint y = 0; y < h; ++y)
			{
				GLfloat u = 0.f;

				for (GLuint x = 0; x < w; ++x)
				{
					GLfloat heightValue = pow(((GLfloat)((GLubyte *)src)[0]) / 255.f, 1.7f);
					++src;

					assert(heightValue >= 0.f);

					glm::vec2 d(mScale.x * (GLfloat)x, mScale.y * heightValue);
					u += d.length();

					heightPtr[0] = heightValue;
					++heightPtr;
				}
			}
		}, (uint32_t *)&mMapSize, (uint32_t *)&mMapSize);

		if (!loaded)
		{
			PRINT_MESSAGE("Error: Cannot load heigh map file '%s'!\n", mapDesc.mFilename.c_str());
			goto ExitLoadHeightMap;
		}

		glBindTexture(mHeightMaps->GetTarget(), mHeightMaps->GetResourceId());
		glTexSubImage3D(mHeightMaps->GetTarget(), 0, 0, 0, mapIndex, mMapSize, mMapSize, 1, GL_RED, GL_FLOAT, heightBuffer);
		GL_CHECK_ERRORS;

		// normal map generation
		{
			GLfloat * nPtr = normalBuffer;
			for (GLint y = 0; y < mMapSize; ++y)
			{
				for (GLint x = 0; x < mMapSize; ++x)
				{
					glm::vec3 n;

					int x_1 = x == 0 ? mMapSize - 1 : ((x - 1) % mMapSize);
					n.x = heightBuffer[(y * mMapSize + x_1)] - heightBuffer[(y * mMapSize + ((x + 1) % mMapSize))];
					n.x *= mScale.y;

					int y_1 = y == 0 ? mMapSize - 1 : ((y - 1) % mMapSize);
					n.z = heightBuffer[(y_1 * mMapSize + x)] - heightBuffer[(((y + 1) % mMapSize) * mMapSize + x)];
					n.z *= mScale.y;

					n.y = mScale.x + mScale.z;
					n = glm::normalize(n);

					nPtr[0] = n.x;
					nPtr[1] = n.y;
					nPtr[2] = n.z;

					nPtr += 3;
				}
			}

			glBindTexture(mNormalMaps->GetTarget(), mNormalMaps->GetResourceId());
			glTexSubImage3D(mNormalMaps->GetTarget(), 0, 0, 0, mapIndex, mMapSize, mMapSize, 1, GL_RGB, GL_FLOAT, normalBuffer);
			GL_CHECK_ERRORS;
		}

		// blend map generation
		{
			GLubyte * blendPtr = blendBuffer;
			GLfloat * heightPtr = heightBuffer;
			GLfloat * normalPtr = normalBuffer;
			for (GLint y = 0; y < mMapSize; ++y)
			{
				for (GLint x = 0; x < mMapSize; ++x)
				{
					GLfloat height = mScale.y * heightPtr[0];

					GLfloat slope = normalPtr[1];

					normalPtr += 3;
					++heightPtr;

					glm::vec4 blendFactor(0.f);
					GLfloat totalBlend = 0.f;
					for (int i = 0; i < BlendMaterialCount; ++i)
					{
						const MaterialDesc & mat = terrainDesc.mMaterials[mapDesc.mMatIndex[i]];
						GLfloat heightBlend = ComputeWeight(height, mat.mMinHeight, mat.mMaxHeight);
						GLfloat slopeBlend = ComputeWeight(slope, mat.mMinSlope, mat.mMaxSlope);

						GLfloat blend = heightBlend * slopeBlend * mat.mStrength;
						blendFactor += mask[i] * blend;
						totalBlend += blend;
					}

					GLfloat blendScale = 255.f / totalBlend;
					blendFactor *= blendScale;
					//if (glm::dot(blendFactor, blendFactor) < 0.001f)
					//{
					//	blendFactor = glm::vec4(255.f, 0.f, 0.f, 0.f);
					//}

					for (int i = 0; i < BlendMaterialCount; ++i)
					{
						*blendPtr = (GLubyte)glm::clamp<GLfloat>(blendFactor[i], 0.f, 255.f);
						++blendPtr;
					}
				}
			}

			glBindTexture(mBlendMaps->GetTarget(), mBlendMaps->GetResourceId());
			glTexSubImage3D(mBlendMaps->GetTarget(), 0, 0, 0, mapIndex, mMapSize, mMapSize, 1, dataFormat[BlendMaterialCount - 1], GL_UNSIGNED_BYTE, blendBuffer);
			GL_CHECK_ERRORS;
		}
	}

	glBindTexture(mBlendMaps->GetTarget(), mBlendMaps->GetResourceId());
	glGenerateMipmap(mBlendMaps->GetTarget());
	GL_CHECK_ERRORS;

	glBindTexture(mNormalMaps->GetTarget(), mNormalMaps->GetResourceId());
	glGenerateMipmap(mNormalMaps->GetTarget());
	GL_CHECK_ERRORS;

	glBindTexture(mHeightMaps->GetTarget(), mHeightMaps->GetResourceId());
	glGenerateMipmap(mHeightMaps->GetTarget());
	GL_CHECK_ERRORS;

	glBindTexture(mHeightMaps->GetTarget(), 0);

	result = true;

	//Engine::GetInstance()->DisplayTexture2DArray(mBlendMaps, 0);

ExitLoadHeightMap:
	free(heightBuffer);
	free(normalBuffer);
	free(blendBuffer);

	return result;
}

void TerrainRenderer::UpdateMaterialTextureIndex(const Desc & desc)
{
	PRINT_MESSAGE("Update material texture index :");

	const TextureInfoList & texInfo = GetTextureInfoList();

	GLuint materialIndex = 0;
	for (MaterialDescList::const_iterator it = desc.mMaterials.begin(); it != desc.mMaterials.end(); ++it)
	{
		const MaterialDesc & matDesc = *it;
		
		ModelRenderer::ShaderMaterial & mat = mShaderMaterialList[materialIndex];

		mat.mBaseColorR = matDesc.mBaseColor.r;
		mat.mBaseColorG = matDesc.mBaseColor.g;
		mat.mBaseColorB = matDesc.mBaseColor.b;

		mat.mMetallic = matDesc.mMetallic;
		mat.mRoughness = matDesc.mRoughness;
		mat.mPorosity = matDesc.mPorosity;
		mat.mEmissive = matDesc.mEmissive;
		mat.mHeightOffset = matDesc.mHeightOffset;
		mat.mHeightScale = matDesc.mHeightScale;

		mat.mBaseColorTextureIndex = matDesc.mBaseColorTextureIndex != NoTexture ? (GLint)texInfo[matDesc.mBaseColorTextureIndex].GetLayerIndex() : -1;
		mat.mBaseColorSamplerIndex = matDesc.mBaseColorTextureIndex != NoTexture ? (GLint)texInfo[matDesc.mBaseColorTextureIndex].GetSamplerIndex() : -1;

		mat.mMetallicTextureIndex = matDesc.mMetallicTextureIndex != NoTexture ? (GLint)texInfo[matDesc.mMetallicTextureIndex].GetLayerIndex() : -1;
		mat.mMetallicSamplerIndex = matDesc.mMetallicTextureIndex != NoTexture ? (GLint)texInfo[matDesc.mMetallicTextureIndex].GetSamplerIndex() : -1;

		mat.mEmissiveTextureIndex = matDesc.mEmissiveTextureIndex != NoTexture ? (GLint)texInfo[matDesc.mEmissiveTextureIndex].GetLayerIndex() : -1;
		mat.mEmissiveSamplerIndex = matDesc.mEmissiveTextureIndex != NoTexture ? (GLint)texInfo[matDesc.mEmissiveTextureIndex].GetSamplerIndex() : -1;

		mat.mNormalTextureIndex = matDesc.mNormalTextureIndex != NoTexture ? (GLint)texInfo[matDesc.mNormalTextureIndex].GetLayerIndex() : -1;
		mat.mNormalSamplerIndex = matDesc.mNormalTextureIndex != NoTexture ? (GLint)texInfo[matDesc.mNormalTextureIndex].GetSamplerIndex() : -1;

		mat.mRoughnessTextureIndex = matDesc.mRoughnessTextureIndex != NoTexture ? (GLint)texInfo[matDesc.mRoughnessTextureIndex].GetLayerIndex() : -1;
		mat.mRoughnessSamplerIndex = matDesc.mRoughnessTextureIndex != NoTexture ? (GLint)texInfo[matDesc.mRoughnessTextureIndex].GetSamplerIndex() : -1;

		mat.mHeightTextureIndex = matDesc.mHeightTextureIndex != NoTexture ? (GLint)texInfo[matDesc.mHeightTextureIndex].GetLayerIndex() : -1;
		mat.mHeightSamplerIndex = matDesc.mHeightTextureIndex != NoTexture ? (GLint)texInfo[matDesc.mHeightTextureIndex].GetSamplerIndex() : -1;

		PRINT_MESSAGE("\t- Material %i : (Sampler, Texture) BaseColor=(%i, %i), Metallic=(%i, %i), Roughness=(%i, %i), Normal=(%i, %i), Emissive=(%i, %i), Height=(%i, %i)", materialIndex, mat.mBaseColorSamplerIndex, mat.mBaseColorTextureIndex, mat.mMetallicSamplerIndex, mat.mMetallicTextureIndex, mat.mRoughnessSamplerIndex, mat.mRoughnessTextureIndex, mat.mNormalSamplerIndex, mat.mNormalTextureIndex, mat.mEmissiveSamplerIndex, mat.mEmissiveTextureIndex, mat.mHeightSamplerIndex, mat.mHeightTextureIndex);

		++materialIndex;
	}
}

GLfloat TerrainRenderer::ComputeWeight(GLfloat value, GLfloat minExtend, GLfloat maxExtend)
{
	GLfloat weight = 0.f;

	if (value >= minExtend && value <= maxExtend)
	{
		GLfloat span = maxExtend - minExtend;
		weight = value - minExtend;
		weight *= 1.f / span;

		weight -= 0.5f;
		weight *= 2.0f;

		weight *= weight;
		weight = 1.f - std::fabs(weight);
		weight = glm::clamp(weight, 0.001f, 1.f);
	}

	return weight;
}

/*
void TerrainRenderer::GenerateBlendMap(const MapDesc & desc)
{
	const glm::vec4 mask[4] = 
	{
		{ 1.f, 0.f, 0.f, 0.f },
		{ 0.f, 1.f, 0.f, 0.f },
		{ 0.f, 0.f, 1.f, 0.f },
		{ 0.f, 0.f, 0.f, 1.f }
	};

	const int BlendMapSize = 256;
	const GLfloat TexScale = 1.f / (GLfloat)BlendMapSize;
	const int mipMapCount = TextureManager::GetNumberOfMipMapLevels(BlendMapSize, BlendMapSize);
	const GLsizei TextureCount = 1;

	GLubyte * buf = new GLubyte[BlendMapSize * BlendMapSize * 4];

	GLubyte * p = buf;
	for (int y = 0; y < BlendMapSize; ++y)
	{
		GLfloat v = (GLfloat)y * TexScale;

		GLfloat height = v * mScale.y;

		for (int x = 0; x < BlendMapSize; ++x)
		{
			glm::vec4 blendFactor(0.f);
			GLfloat totalBlend = 0.f;

			GLfloat u = x * TexScale;
			for (int i = 0; i < 4; ++i)
			{
				const MaterialDesc & mat = desc.mMaterials[i];
				GLfloat heightBlend = ComputeWeight(height, mat.mMinHeight, mat.mMaxHeight);
				GLfloat slopeBlend = ComputeWeight(u, mat.mMinSlope, mat.mMaxSlope);

				GLfloat blend = heightBlend * slopeBlend;
				blendFactor += mask[i] * blend;
				totalBlend += blend;
			}

			GLfloat blendScale = 255.f / totalBlend;
			blendFactor *= blendScale;
			for (int i = 0; i < 4; ++i)
			{
				*p = glm::clamp<GLubyte>((GLubyte)blendFactor[i], 0, 255);
				++p;
			}
		}
	}

	TextureManager * textureManager = Engine::GetInstance()->GetTextureManager();
	mBlendMaps = textureManager->CreateTexture2DArray(mipMapCount, GL_RGBA8, BlendMapSize, BlendMapSize, TextureCount, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
	glBindTexture(mBlendMaps->GetTarget(), mBlendMaps->GetResourceId());
	glTexSubImage3D(mBlendMaps->GetTarget(), 0, 0, 0, 0, BlendMapSize, BlendMapSize, 1, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	GL_CHECK_ERRORS;
	glGenerateMipmap(mBlendMaps->GetTarget());
	GL_CHECK_ERRORS;

	Engine::GetInstance()->DisplayTexture2DArray(mBlendMaps, 0);

	delete[] buf;
}
*/

	} // namespace Renderers
} // namespace CoreFx