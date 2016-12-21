#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



TerrainRenderer::TerrainRenderer(const Desc & desc)
	: RendererHelper<1>("TerrainRenderer", "TerrainWireRenderer")
	, mMapSize(desc.mHeightMapSize)
	, mPatchCount(desc.mHeightMapSize / 64, desc.mHeightMapSize / 64)
	, mScale(desc.mScale)
	, mMapCount(0)
	, mMaterialCount(0)
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize TerrainRenderer.....");

	mMaterialCount = (GLuint)desc.mTerrains.size() * 4;
	mShaderMaterialList.resize(mMaterialCount);

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

	PerMapData * modelMatrixBuffer = new PerMapData[mMapCount];
	for (int i = 0; i < mMapCount; ++i)
	{
		modelMatrixBuffer[i].mModelDQ = desc.mTerrains[i].mModelDQ;
	}
	mLocationRawDataBuffer.CreateResource(GL_STATIC_DRAW, mMapCount * sizeof(PerMapData), modelMatrixBuffer);
	mPrecomputeDataBuffer.CreateResource(GL_DYNAMIC_COPY, mMapCount * sizeof(PerMapData), nullptr);
	mMaterialBuffer.CreateResource(GL_STATIC_DRAW, mMaterialCount * ModelRenderer::cShaderMaterialSize, nullptr/*mShaderMaterialList.data()*/);

	PRINT_GEN_SHADERSTORAGEBUFFER("[ModelRenderer]", mPrecomputeDataBuffer);
	PRINT_GEN_SHADERSTORAGEBUFFER("[ModelRenderer]", mLocationRawDataBuffer);
	
	LoadShaders(desc);

	mIsInitialized = true;

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
		"u_UvMap",
		"u_BlendMap",
	};


	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/terrain.vs.glsl");
	mShader.LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/terrain.tcs.glsl");
	mShader.LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/terrain.tes.glsl");
	mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/terrain.gs.glsl");

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
	glUniform1i(mShader.GetUniform(u_UvMap), 1); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_BlendMap), 2); GL_CHECK_ERRORS;


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

	const char * uniformNames[__uniforms_count__] =
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
	mWireFrameShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/terrain.gs.glsl");
	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/terrain.wireframe.fs.glsl");

	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();

	mWireFrameShader.AddUniforms(uniformNames, __uniforms_count__);

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
	Engine * engine = Engine::GetInstance();
	const GLuint itemSize = sizeof(PerMapData) / sizeof(glm::vec4);
	engine->ComputeViewTransform(mLocationRawDataBuffer.GetBufferId(), mPrecomputeDataBuffer.GetBufferId(), (GLuint)mMapCount, itemSize, itemSize);
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
	glBindTexture(mUvMaps->GetTarget(), mUvMaps->GetResourceId());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(mBlendMaps->GetTarget(), mBlendMaps->GetResourceId());

	mLocationRawDataBuffer.BindBufferBase(u_PerInstanceWorlMatrix);
	mPrecomputeDataBuffer.BindBufferBase(u_PerInstanceViewMatrix);
	mMaterialBuffer.BindBufferBase(u_Materials);

	for (int i = 0; i < (int)mTextureMapping.mMapping.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + FIRST_TEXTURE_SAMPLER_INDEX + i);
		glBindTexture(mTextureMapping.mMapping[i].mTexture->GetTarget(), mTextureMapping.mMapping[i].mTexture->GetResourceId());
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
	glBindTexture(mHeightMaps->GetTarget(), mHeightMaps->GetResourceId());

	mLocationRawDataBuffer.BindBufferBase(u_PerInstanceWorlMatrix);
	mPrecomputeDataBuffer.BindBufferBase(u_PerInstanceViewMatrix);

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

	const MapDescList & terrainDescList = terrainDesc.mTerrains;

	bool result = false;

	const int mipMapCount = TextureManager::GetNumberOfMipMapLevels(mMapSize, mMapSize);

	mMapCount = (GLint)terrainDescList.size();
	size_t layerBufferTexelCount = mMapSize * mMapSize;
	size_t heightBufferSize = layerBufferTexelCount * 3 * sizeof(GLfloat);
	size_t uvBufferSize = layerBufferTexelCount * 2 * sizeof(GLfloat);
	size_t blendBufferSize = layerBufferTexelCount * 4 * sizeof(GLubyte);

	GLfloat * heightBuffer = (GLfloat*)malloc(heightBufferSize);
	GLfloat * uvBuffer = (GLfloat*)malloc(uvBufferSize);
	GLubyte * blendBuffer = (GLubyte*)malloc(blendBufferSize);

	if (heightBuffer == nullptr || uvBuffer == nullptr || blendBuffer == nullptr)
	{
		PRINT_MESSAGE("Error: Cannot allocate memory to load terrain height maps ! (Needed memory : %ld)\n", heightBufferSize + uvBufferSize + blendBufferSize);
		goto ExitLoadHeightMap;
	}

	TextureManager * textureManager = Engine::GetInstance()->GetTextureManager();

	mHeightMaps = textureManager->CreateTexture2DArray(mipMapCount, GL_RGB16F, mMapSize, mMapSize, mMapCount, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
	mUvMaps = textureManager->CreateTexture2DArray(mipMapCount, GL_RG32F, mMapSize, mMapSize, mMapCount, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
	mBlendMaps = textureManager->CreateTexture2DArray(mipMapCount, GL_RGBA8, mMapSize, mMapSize, mMapCount, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);

	int mapIndex = 0;
	for (MapDescList::const_iterator it = terrainDescList.begin(); it != terrainDescList.end(); ++it, ++mapIndex)
	{
		const MapDesc & mapDesc = *it;

		assert(mapDesc.mHeightMapTextureSize >= mMapSize);

		GLsizei texWidth, texHeight;
		bool loaded = TextureManager::LoadTiffImage(texWidth, texHeight, mapDesc.mFilename, [this, heightBuffer, uvBuffer](uint32_t w, uint32_t h, const uint32_t * raster)
		{
			h = std::min<uint32_t>(mMapSize, h);
			w = std::min<uint32_t>(mMapSize, w);

			GLfloat * heightPtr = heightBuffer;
			GLfloat * uvPtr = uvBuffer;

			const uint32_t * src = raster;
			for (GLuint y = 0; y < h; ++y)
			{
				GLfloat u = 0.f;

				for (GLuint x = 0; x < w; ++x)
				{
					GLfloat heightValue = ((GLfloat)((GLubyte *)src)[0]) / 255.f;
					++src;

					glm::vec2 d(mScale.x * (GLfloat)x, mScale.y * heightValue);
					u += d.length();

					heightPtr[0] = heightValue;
					heightPtr += 3;

					uvPtr[1] = u;
					uvPtr += 2;
				}
			}
		}, (uint32_t *)&mMapSize, (uint32_t *)&mMapSize);

		if (!loaded)
		{
			PRINT_MESSAGE("Error: Cannot load heigh map file '%s'!\n", mapDesc.mFilename.c_str());
			goto ExitLoadHeightMap;
		}

		// height (and texture uv) map generation
		{
			GLfloat v = 0;
			GLfloat * heightPtr = heightBuffer;
			GLfloat * uvPtr = uvBuffer;
			for (GLint y = 0; y < mMapSize; ++y)
			{
				GLfloat heightValue = heightPtr[0];
				glm::vec2 d(mScale.z * (GLfloat)y, mScale.y * heightValue);
				v += d.length();

				for (GLint x = 0; x < mMapSize; ++x)
				{
					uvPtr[2] = v;
					uvPtr += 2;
				}

				heightPtr += mMapSize * 3;
			}


			glBindTexture(mUvMaps->GetTarget(), mUvMaps->GetResourceId());
			glTexSubImage3D(mUvMaps->GetTarget(), 0, 0, 0, mapIndex, mMapSize, mMapSize, 1, GL_RG, GL_FLOAT, uvBuffer);
			GL_CHECK_ERRORS;
		}

		// normal map generation
		{
			GLfloat * dxdzPtr = heightBuffer;
			for (GLint y = 0; y < mMapSize; ++y)
			{
				for (GLint x = 0; x < mMapSize; ++x)
				{
					glm::vec2 dxdz;

					int x_1 = x == 0 ? mMapSize - 1 : ((x - 1) % mMapSize);
					dxdz.x = heightBuffer[(y * mMapSize + x_1) * 3] - heightBuffer[(y * mMapSize + ((x + 1) % mMapSize)) * 3];

					int y_1 = y == 0 ? mMapSize - 1 : ((y - 1) % mMapSize);
					dxdz.y = heightBuffer[(y_1 * mMapSize + x) * 3] - heightBuffer[(((y + 1) % mMapSize) * mMapSize + x) * 3];

					dxdzPtr[0] = dxdz.x;
					dxdzPtr[1] = dxdz.y;
					dxdzPtr += 3;
				}
			}

			glBindTexture(mHeightMaps->GetTarget(), mHeightMaps->GetResourceId());
			glTexSubImage3D(mHeightMaps->GetTarget(), 0, 0, 0, mapIndex, mMapSize, mMapSize, 1, GL_RGB, GL_FLOAT, heightBuffer);
			GL_CHECK_ERRORS;
		}

		// blend map generation
		{
			GLubyte * blendPtr = blendBuffer;
			GLfloat * heightPtr = heightBuffer;
			for (GLint y = 0; y < mMapSize; ++y)
			{
				for (GLint x = 0; x < mMapSize; ++x)
				{
					GLfloat height = mScale.y * heightPtr[0];

					glm::vec3 normal(heightPtr[1], 2.f, heightPtr[2]);
					normal = glm::normalize(normal);

					GLfloat slope = normal.y;

					heightPtr += 3;

					glm::vec4 blendFactor(0.f);
					GLfloat totalBlend = 0.f;
					for (int i = 0; i < 4; ++i)
					{
						const MaterialDesc & mat = mapDesc.mMaterials[i];
						GLfloat heightBlend = ComputeWeight(height, mat.mMinHeight, mat.mMaxHeight);
						GLfloat slopeBlend = ComputeWeight(slope, mat.mMinSlope, mat.mMaxSlope);

						GLfloat blend = heightBlend * slopeBlend;
						blendFactor += mask[i] * blend;
						totalBlend += blend;
					}

					GLfloat blendScale = 255.f / totalBlend;
					blendFactor *= blendScale;
					for (int i = 0; i < 4; ++i)
					{
						*blendPtr = glm::clamp<GLubyte>((GLubyte)blendFactor[i], 0, 255);
						++blendPtr;
					}
				}
			}

			glBindTexture(mBlendMaps->GetTarget(), mBlendMaps->GetResourceId());
			glTexSubImage3D(mBlendMaps->GetTarget(), 0, 0, 0, mapIndex, mMapSize, mMapSize, 1, GL_RGBA, GL_UNSIGNED_BYTE, blendBuffer);
			GL_CHECK_ERRORS;
		}
	}

	glBindTexture(mUvMaps->GetTarget(), mUvMaps->GetResourceId());
	glGenerateMipmap(mUvMaps->GetTarget());
	GL_CHECK_ERRORS;

	glBindTexture(mBlendMaps->GetTarget(), mBlendMaps->GetResourceId());
	glGenerateMipmap(mBlendMaps->GetTarget());
	GL_CHECK_ERRORS;

	glBindTexture(mHeightMaps->GetTarget(), mHeightMaps->GetResourceId());
	glGenerateMipmap(mHeightMaps->GetTarget());
	GL_CHECK_ERRORS;

	glBindTexture(mHeightMaps->GetTarget(), 0);

	result = true;

	Engine::GetInstance()->DisplayTexture2DArray(mBlendMaps, 0);

ExitLoadHeightMap:
	free(heightBuffer);
	free(uvBuffer);
	free(blendBuffer);

	return result;
}

void TerrainRenderer::UpdateMaterialTextureIndex(const Desc & desc)
{
	PRINT_MESSAGE("Update material texture index :");

	const TextureInfoList & texInfo = GetTextureInfoList();

	GLuint materialIndex = 0;
	for (MapDescList::const_iterator it = desc.mTerrains.begin(); it != desc.mTerrains.end(); ++it)
	{
		const MapDesc & mapDesc = *it;

		for (int i = 0; i < 4; ++i)
		{
			const MaterialDesc & matDesc = mapDesc.mMaterials[i];

			ModelRenderer::ShaderMaterial & mat = mShaderMaterialList[materialIndex];

			mat.mBaseColorR = matDesc.mBaseColor.r;
			mat.mBaseColorG = matDesc.mBaseColor.g;
			mat.mBaseColorB = matDesc.mBaseColor.b;
			
			mat.mMetallic = matDesc.mMetallic;
			mat.mRoughness = matDesc.mRoughness;
			mat.mPorosity = matDesc.mPorosity;
			mat.mEmissive = matDesc.mEmissive;
			
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

			PRINT_MESSAGE("\t- Material %i : (Sampler, Texture) BaseColor=(%i, %i), Metallic=(%i, %i), Roughness=(%i, %i), Normal=(%i, %i), Emissive=(%i, %i)", materialIndex, mat.mBaseColorSamplerIndex, mat.mBaseColorTextureIndex, mat.mMetallicSamplerIndex, mat.mMetallicTextureIndex, mat.mRoughnessSamplerIndex, mat.mRoughnessTextureIndex, mat.mNormalSamplerIndex, mat.mNormalTextureIndex, mat.mEmissiveSamplerIndex, mat.mEmissiveTextureIndex);

			++materialIndex;
		}
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
		weight = 1.f - weight;
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