#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{
		namespace PerlinNoiseOcean
		{
		// =======================================================================
		// =======================================================================



RadialGridOceanRenderer::RadialGridOceanRenderer(const Desc & desc)
	: RendererHelper<2>("RadialGridOceanRenderer", "RadialGridOceanWireFrameRenderer", Renderer::ERenderPass::Deferred_Pass)
	, mOceanColorTexture(Engine::GetInstance()->GetTextureManager()->LoadTexture2D("medias/textures/OceanColor256.tif", GL_REPEAT, GL_REPEAT))
	//, mOceanColorTexture(Engine::GetInstance()->GetTextureManager()->GetDefaultTexture2D())
	, mHeightMapTextureSize(256)
	, mTexScale(desc.mTexScale)
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize Perlin Noise Ocean (using Radial Grid) Renderer.....");

	
	mScale.x = mTexScale / mHeightMapTextureSize;
	mScale.y = desc.mWaveAmplitude;

	glm::vec3 * vertices = nullptr;
	GLushort * indices = nullptr;
	Geometry::Helpers::GenerateRadialGrid(mVertexCount, mIndexCount, vertices, indices, desc.mGridSlides, desc.mGridRings, desc.mMaxDetail, desc.mGridSize);

	//setup vao and vbo stuff
	CreateBuffers();

	glBindVertexArray(mVaoID);

	glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[VertexArrayBufferIndex]);
	glBufferData(GL_ARRAY_BUFFER, mVertexCount * sizeof(glm::vec3), vertices, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
	glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	GL_CHECK_ERRORS;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVboIDs[IndexArrayBufferIndex]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * mIndexCount, indices, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	delete[] vertices;
	delete[] indices;

	LoadShaders(desc);

	mIsInitialized = InitializeHeightMaps();

	PRINT_MESSAGE(".....Perlin Noise Ocean (using Radial Grid) Renderer initialized!");
	PRINT_END_SECTION;
}


RadialGridOceanRenderer::~RadialGridOceanRenderer()
{
}

void RadialGridOceanRenderer::LoadShaders(const Desc & desc)
{
	LoadMainShader(desc);
	LoadWireFrameShader(desc);
}

void RadialGridOceanRenderer::LoadMainShader(const Desc & desc)
{
	PRINT_MESSAGE("Initialize Perlin Noise Ocean (using a Radial Grid) Renderer Shaders : .....");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_Frame0",
		//"u_Frame1",
		"u_Scale",

		"u_HeightMapsSampler",
		"u_NormalMapsSampler",
		"u_SkyboxCubeMapSampler",
		"u_TextureSampler",
	};


	//setup shader
	std::vector<std::string> vsGlsl(1);
	std::vector<std::string> fsGlsl(2);

	std::string & vsSource = vsGlsl[0];
	std::string & fsSource = fsGlsl[1];
	
	if (desc.mNormalMode == ENormalMode::PerPixelNormal)
	{
		vsSource.append("#define PER_PIXEL_NORMAL\n");
		fsSource.append("#define PER_PIXEL_NORMAL\n");
	}

	PRINT_MESSAGE("Loading shader file : shaders/RadialGridOcean.vs.glsl");
	Shader::MergeFile(vsSource, "shaders/RadialGridOcean.vs.glsl");
	mShader.LoadFromString(GL_VERTEX_SHADER, vsGlsl);

	PRINT_MESSAGE("Loading shader file : shaders/DeferredShadingCommon.incl.glsl");
	Shader::MergeFile(fsGlsl[0], "shaders/DeferredShadingCommon.incl.glsl");
	
	PRINT_MESSAGE("Loading shader file : shaders/RadialGridOcean.deferred.fs.glsl");
	Shader::MergeFile(fsSource, "shaders/RadialGridOcean.deferred.fs.glsl");
	mShader.LoadFromString(GL_FRAGMENT_SHADER, fsGlsl);

	mShader.CreateAndLinkProgram();
	mShader.Use();

	mShader.AddUniforms(uniformNames, __uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform1i(mShader.GetUniform(u_Frame0), 0); GL_CHECK_ERRORS;
	//glUniform1i(mShader.GetUniform(u_Frame1), 0); GL_CHECK_ERRORS;
	glUniform2fv(mShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_HeightMapsSampler), 0); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_NormalMapsSampler), 1); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_SkyboxCubeMapSampler), 2); GL_CHECK_ERRORS;
	glUniform1i(mShader.GetUniform(u_TextureSampler), 3); GL_CHECK_ERRORS;

	mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....done.");
}

void RadialGridOceanRenderer::LoadWireFrameShader(const Desc & desc)
{
	PRINT_MESSAGE("Initialize Perlin Noise Ocean (using a Radial Grid) Renderer (Wire Frame) Shaders : .....");

	const char * uniformNames[] =
	{
		"u_Frame0",
		//"u_Frame1",
		"u_Scale",

		"u_HeightMapsSampler",
	};

	//setup shader
	std::vector<std::string> vsGlsl(1);
	std::vector<std::string> fsGlsl(1);

	std::string & vsSource = vsGlsl[0];
	std::string & fsSource = fsGlsl[0];

	if (desc.mNormalMode == ENormalMode::PerPixelNormal)
	{
		vsSource.append("#define PER_PIXEL_NORMAL\n");
		fsSource.append("#define PER_PIXEL_NORMAL\n");
	}

	PRINT_MESSAGE("Loading shader file : shaders/RadialGridOcean.vs.glsl");
	Shader::MergeFile(vsSource, "shaders/RadialGridOcean.vs.glsl");
	mWireFrameShader.LoadFromString(GL_VERTEX_SHADER, vsGlsl);

	PRINT_MESSAGE("Loading shader file : shaders/RadialGridOcean.wireframe.fs.glsl");
	Shader::MergeFile(fsSource, "shaders/RadialGridOcean.wireframe.fs.glsl");
	mWireFrameShader.LoadFromString(GL_FRAGMENT_SHADER, fsGlsl);

	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();

	mWireFrameShader.AddUniforms(uniformNames, ARRAY_SIZE_IN_ELEMENTS(uniformNames));

	//pass values of constant uniforms at initialization
	//glUniform2iv(mWireFrameShader.GetUniform(u_HeightMapTextureSize), 1, glm::value_ptr(mHeightMapTextureSize)); GL_CHECK_ERRORS;
	glUniform1i(mWireFrameShader.GetUniform(u_Frame0), 0); GL_CHECK_ERRORS;
	//glUniform1i(mWireFrameShader.GetUniform(u_Frame1), 0); GL_CHECK_ERRORS;
	glUniform2fv(mWireFrameShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale)); GL_CHECK_ERRORS;
	glUniform1i(mWireFrameShader.GetUniform(u_HeightMapsSampler), 0); GL_CHECK_ERRORS;

	mWireFrameShader.SetupFrameDataBlockBinding();
	mWireFrameShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....done.");
}

void RadialGridOceanRenderer::Render()
{
	UpdateFrame();

	CubeMapTexture const * cubeMapTexture = mCubeMapTexture != nullptr ? mCubeMapTexture : Engine::GetInstance()->GetSkyCubeMap();
	if (cubeMapTexture == nullptr)
	{
		cubeMapTexture = Engine::GetInstance()->GetTextureManager()->GetDefaultCubeMapTexture();
	}

	mShader.Use();

		glUniform1i(mShader.GetUniform(u_Frame0), mFrame0);
		//glUniform1i(mShader.GetUniform(u_Frame1), mFrame1);
		glUniform2fv(mShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale));

		glBindVertexArray(mVaoID);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(mHeightMaps->GetTarget(), mHeightMaps->GetResourceId());

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(mNormalMaps->GetTarget(), mNormalMaps->GetResourceId());

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(cubeMapTexture->GetTarget(), cubeMapTexture->GetResourceId());
			
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, mOceanColorTexture->GetResourceId());

			glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, nullptr);

		glBindVertexArray(0);
	mShader.UnUse();
}

void RadialGridOceanRenderer::RenderWireFrame()
{
	UpdateFrame();

	mWireFrameShader.Use();

		glUniform1i(mWireFrameShader.GetUniform(u_Frame0), mFrame0);
		//glUniform1i(mWireFrameShader.GetUniform(u_Frame1), mFrame1); 
		glUniform2fv(mWireFrameShader.GetUniform(u_Scale), 1, glm::value_ptr(mScale));

		glBindVertexArray(mVaoID);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(mHeightMaps->GetTarget(), mHeightMaps->GetResourceId());

			glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, nullptr);

		glBindVertexArray(0);
	mWireFrameShader.UnUse();
}


bool RadialGridOceanRenderer::LoadRaw(const char * filename, void * data, size_t sizeBytes)
{
	FILE * fd;
	errno_t err = fopen_s(&fd, filename, "rb");
	if (err != 0)
	{
		PRINT_ERROR("Opening file '%s' has failed!", filename);
		return false;
	}

	size_t nRead = fread(data, sizeBytes, 1, fd);
	if (nRead != 1)
	{
		PRINT_ERROR("Loading file '%s' has failed!", filename);
		return false;
	}
	fclose(fd);

	return true;
}

/*
bool RadialGridOceanRenderer::InitializeHeightMapsOld()
{
	const int WaterImages = 16;
	const int WaterTexSize = mHeightMapTextureSize;
	const int TextureCount = WaterImages * 8;
	const int TexelCount = WaterTexSize * WaterTexSize;

	const int mipMapCount = TextureManager::GetNumberOfMipMapLevels(WaterTexSize, WaterTexSize);

	bool result = false;

	TextureManager * textureManager = Engine::GetInstance()->GetTextureManager();
	mHeightMaps = textureManager->CreateTexture2DArray(mipMapCount, GL_RGBA32F, WaterTexSize, WaterTexSize, TextureCount, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
	mNormalMaps = textureManager->CreateTexture2DArray(mipMapCount, GL_RGB16F, WaterTexSize, WaterTexSize, TextureCount, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);

	float *BMap = new float[TexelCount];
	BYTE *BMap1 = new BYTE[TexelCount];
	BYTE *BMap2 = new BYTE[TexelCount];
	float *pBuf = new float[TexelCount * 4];
	float *pBuf2 = new float[TexelCount * 4];
	float *pBuf4 = new float[TexelCount * 4];

	{ // 8 step interpolation between animation frames
		const int filenameBufferSize = 256;
		char filename[filenameBufferSize];

		for (int l = 0; l < TextureCount; l++)
		{
			// Load Water maps, scale H to 0..1
			// To do quadric /\  waves distorsion
			if ((l % 8) == 0)
			{
				if (l == 0)
				{
					sprintf_s(filename, filenameBufferSize, "medias/textures/Hmap/WaterH" "%02i" ".raw", l / 8);
					if (!LoadRaw(filename, BMap1, TexelCount))
					{
						goto InitializeHeightMapsOld_Exit;
					}
				}
				else
				{ // Copy previous
					memcpy(BMap1, BMap2, TexelCount);
				}
			}

			{// Interpolate intermediate map
				int l1 = (l / 8 + 1)&(WaterImages - 1);
				if ((l % 8) == 0)
				{
					sprintf_s(filename, filenameBufferSize, "medias/textures/Hmap/WaterH" "%02i" ".raw", l1);
					if(!LoadRaw(filename, BMap2, TexelCount))
					{
						goto InitializeHeightMapsOld_Exit;
					}
				}

				float R = float((l & 7)) / 8.0f;
				for (int y = 0; y < WaterTexSize; y++)
				{
					for (int x = 0; x < WaterTexSize; x++)
					{
						int PixAddr = y*WaterTexSize + x;
						float t = float(BMap1[PixAddr])*(1 - R) + float(BMap2[PixAddr])*R;
						float H = pow((t / 255.0f), 1.7f);
						if (H < 0) H = 0; if (H > 1.0f) H = 1.0f;
						BMap[PixAddr] = H;
					}
				}
			}

			float *pB = pBuf;
			// float MinV = 1.0f, MaxV = 0;
			int Mask = WaterTexSize - 1;
			for (int y = 0; y < WaterTexSize; y++)
			{
				for (int x = 0; x < WaterTexSize; x++)
				{
					float V = BMap[y*WaterTexSize + x];
					//   SetMinMax(V, MinV, MaxV);
					pB[0] = V;
					pB[1] = BMap[y*WaterTexSize + ((x + 1) & Mask)] - BMap[y*WaterTexSize + ((x - 1) & Mask)];
					pB[2] = BMap[x + ((y + 1) & Mask)*WaterTexSize] - BMap[x + ((y - 1) & Mask)*WaterTexSize];

					pB[1] = pB[1] * 0.5f * 0.5f + 0.5f;
					pB[2] = pB[2] * 0.5f * 0.5f + 0.5f;

					pB += 3;
				}
			}
			//pB = pBuf;

			int SZ = WaterTexSize;

			for (int y = 0; y < SZ; y++)
			{
				for (int x = 0; x < SZ; x++)
				{
					pBuf2[(y*SZ + x) * 4 + 0] = pBuf[(y*SZ + x) * 3 + 0];
				}
			}

			float *pBuf3 = new float[(SZ / 2)*(SZ / 2) * 4], *pB3 = pBuf3;
			{
				for (int k = 0; k < mipMapCount; k++)
				{
					if (k > 0)
					{
						int SZ2 = SZ * 2;

						for (int y = 0; y < SZ; y++)
						{
							for (int x = 0; x < SZ; x++)
							{
								float F = pBuf2[((y * 2)*SZ2 + x * 2) * 4 + 0];
								F += pBuf2[((y * 2)*SZ2 + x * 2 + 1) * 4 + 0];
								F += pBuf2[((y * 2 + 1)*SZ2 + x * 2) * 4 + 0];
								F += pBuf2[((y * 2 + 1)*SZ2 + x * 2 + 1) * 4 + 0];
								F /= 4;
								pBuf3[(y*SZ + x) * 4 + 0] = F;
							}
						}

						std::swap(pBuf2, pBuf3);
					}

					for (int y = 0; y < SZ; y++)
					{
						for (int x = 0; x < SZ; x++)
						{
							//pBuf2[(y*SZ + x)*4+0] = pBuf[(y*SZ + x)*4+0];
							pBuf2[(y*SZ + x) * 4 + 1] = pBuf2[(y*SZ + (x + 1) % SZ) * 4 + 0];
							pBuf2[(y*SZ + x) * 4 + 2] = pBuf2[(((y + 1) % SZ)*SZ + x) * 4 + 0];
							pBuf2[(y*SZ + x) * 4 + 3] = pBuf2[(((y + 1) % SZ)*SZ + (x + 1) % SZ) * 4 + 0];
						}
					}

					glBindTexture(mHeightMaps->GetTarget(), mHeightMaps->GetResourceId());
					glTexSubImage3D(mHeightMaps->GetTarget(), k, 0, 0, l, SZ, SZ, 1, GL_RGBA, GL_FLOAT, pBuf2);
					GL_CHECK_ERRORS;

					SZ /= 2;
				}
			}
			pBuf3 = pB3; delete[] pB3;

			{
				for (int y = 0; y < WaterTexSize; y++)
				{
					for (int x = 0; x < WaterTexSize; x++)
					{
						int ii = (y*WaterTexSize + x);
						pBuf4[ii * 4 + 0] = pBuf[ii * 3 + 0];
						pBuf4[ii * 4 + 1] = pBuf[ii * 3 + 1];
						pBuf4[ii * 4 + 2] = pBuf[ii * 3 + 2];
						pBuf4[ii * 4 + 3] = 1.0f;
					}
				}

				// Load texture - with rejecting A-channel 
				glBindTexture(mNormalMaps->GetTarget(), mNormalMaps->GetResourceId());
				glTexSubImage3D(mNormalMaps->GetTarget(), 0, 0, 0, l, WaterTexSize, WaterTexSize, 1, GL_RGBA, GL_FLOAT, pBuf4);
				GL_CHECK_ERRORS;
			}
		}

		glBindTexture(mNormalMaps->GetTarget(), mNormalMaps->GetResourceId());
		glGenerateMipmap(mNormalMaps->GetTarget());
		GL_CHECK_ERRORS;
	}

	result = true;

InitializeHeightMapsOld_Exit:

	delete[] pBuf;
	delete[] pBuf2;
	delete[] pBuf4;
	delete[] BMap;
	delete[] BMap1;
	delete[] BMap2;

	return result;
}
*/
bool RadialGridOceanRenderer::InitializeHeightMaps()
{
	const int WaterImages = 16;
	const int WaterTexSize = mHeightMapTextureSize;
	const int TextureCount = WaterImages * 8;
	const int TexelCount = WaterTexSize * WaterTexSize;

	const int mipMapCount = TextureManager::GetNumberOfMipMapLevels(WaterTexSize, WaterTexSize);

	bool result = false;

	TextureManager * textureManager = Engine::GetInstance()->GetTextureManager();
	mHeightMaps = textureManager->CreateTexture2DArray(mipMapCount, GL_R16F, WaterTexSize, WaterTexSize, TextureCount, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
	mNormalMaps = textureManager->CreateTexture2DArray(mipMapCount, GL_RG16F, WaterTexSize, WaterTexSize, TextureCount, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);

	float *BMap = new float[TexelCount];
	BYTE *BMap1 = new BYTE[TexelCount];
	BYTE *BMap2 = new BYTE[TexelCount];
	float *pBuf = new float[TexelCount * 4];

	{ // 8 step interpolation between animation frames
		const int filenameBufferSize = 256;
		char filename[filenameBufferSize];

		for (int l = 0; l < TextureCount; l++)
		{
			// Load Water maps, scale H to 0..1
			// To do quadric /\  waves distorsion
			if ((l % 8) == 0)
			{
				if (l == 0)
				{
					sprintf_s(filename, filenameBufferSize, "medias/textures/Hmap/WaterH" "%02i" ".raw", l / 8);
					if (!LoadRaw(filename, BMap1, TexelCount))
					{
						goto InitializeHeightMaps_Exit;
					}
				}
				else
				{ // Copy previous
					memcpy(BMap1, BMap2, TexelCount);
				}
			}

			{// Interpolate intermediate map
				int l1 = (l / 8 + 1)&(WaterImages - 1);
				if ((l % 8) == 0)
				{
					sprintf_s(filename, filenameBufferSize, "medias/textures/Hmap/WaterH" "%02i" ".raw", l1);
					if (!LoadRaw(filename, BMap2, TexelCount))
					{
						goto InitializeHeightMaps_Exit;
					}
				}

				float R = float((l & 7)) / 8.0f;
				for (int y = 0; y < WaterTexSize; y++)
				{
					for (int x = 0; x < WaterTexSize; x++)
					{
						int PixAddr = y*WaterTexSize + x;
						float t = float(BMap1[PixAddr])*(1 - R) + float(BMap2[PixAddr])*R;
						float H = pow((t / 255.0f), 1.7f);
						H = glm::clamp(H, 0.f, 1.f);
						BMap[PixAddr] = H;
					}
				}
			}

			float *pB = pBuf;
			for (int y = 0; y < WaterTexSize; y++)
			{
				for (int x = 0; x < WaterTexSize; x++)
				{
					float V = BMap[y*WaterTexSize + x];
					pB[0] = V;
					pB[1] = 0.0f;
					pB[2] = 0.0f;
					pB[3] = 0.0f;
					pB += 4;
				}
			}

			glBindTexture(mHeightMaps->GetTarget(), mHeightMaps->GetResourceId());
			glTexSubImage3D(mHeightMaps->GetTarget(), 0, 0, 0, l, WaterTexSize, WaterTexSize, 1, GL_RGBA, GL_FLOAT, pBuf);
			GL_CHECK_ERRORS;

			pB = pBuf;
			for (int y = 0; y < WaterTexSize; y++)
			{
				for (int x = 0; x < WaterTexSize; x++)
				{
					int x_1 = x == 0 ? WaterTexSize - 1 : ((x - 1) % WaterTexSize);
					float nX = BMap[y * WaterTexSize + x_1] - BMap[y * WaterTexSize + ((x + 1) % WaterTexSize)];
					int y_1 = y == 0 ? WaterTexSize - 1 : ((y - 1) % WaterTexSize);
					float nZ = BMap[y_1 * WaterTexSize + x] - BMap[((y + 1) % WaterTexSize) * WaterTexSize + x];
					pB[0] = nX;
					pB[1] = nZ;
					pB += 4;
				}
			}

			glBindTexture(mNormalMaps->GetTarget(), mNormalMaps->GetResourceId());
			glTexSubImage3D(mNormalMaps->GetTarget(), 0, 0, 0, l, WaterTexSize, WaterTexSize, 1, GL_RGBA, GL_FLOAT, pBuf);
			GL_CHECK_ERRORS;
		}

		glBindTexture(mNormalMaps->GetTarget(), mNormalMaps->GetResourceId());
		glGenerateMipmap(mNormalMaps->GetTarget());
		GL_CHECK_ERRORS;

		glBindTexture(mHeightMaps->GetTarget(), mHeightMaps->GetResourceId());
		glGenerateMipmap(mHeightMaps->GetTarget());
		GL_CHECK_ERRORS;

		glBindTexture(mHeightMaps->GetTarget(), 0);
	}

	result = true;

InitializeHeightMaps_Exit:

	delete[] pBuf;
	delete[] BMap;
	delete[] BMap1;
	delete[] BMap2;

	return result;
}


void RadialGridOceanRenderer::UpdateFrame()
{
	float time = Engine::GetInstance()->GetTime();

	mFrame0 = int(0.13f * time * 128) & (128 - 1);
	//mFrame1 = int(0.19f * time * 128 + 127) & (128 - 1);
}

		} // namespace PerlinNoiseOcean
	} // namespace Renderers
} // namespace CoreFx