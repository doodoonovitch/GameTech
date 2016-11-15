#include "stdafx.h"
#include "CoreGame.h"
#include <iostream>

using namespace std;
using namespace CoreFx;
using namespace Renderables;


namespace CoreGame
{




void SimpleCamera::OnRender()
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Engine* engine = Engine::GetInstance();

	if (!mFreezeTimer)
	{
		engine->SetDeltaTime((float)mDeltaTime);
		engine->SetTime((float)mTime);
	}

	engine->UpdateObjects();
	engine->RenderObjects();
}

void SimpleCamera::OnShutdown()
{
	Engine::Release();

	PRINT_MESSAGE("Shutdown successfull");
}

void SimpleCamera::OnWindowResize(int w, int h)
{
	mWindowWidth = (GLsizei)w;
	mWindowHeight = (GLsizei)h;
	SetupViewportAndProjection();
}

void SimpleCamera::SetupViewportAndProjection()
{
	if (mCamera != nullptr)
	{
		assert(mCamera != nullptr);
		mCamera->SetupProjection(45, (GLfloat)mWindowWidth / mWindowHeight, mCamera->GetNearZ(), mCamera->GetFarZ());
	}

	Engine::GetInstance()->SetViewport(0, 0, mWindowWidth, mWindowHeight, mWindowWidth, mWindowHeight);
}

//#define TERRAIN_SAMPLE
//#define DEEP_OCEAN_SAMPLE
//#define GERSTNER_WAVE_OCEAN_SAMPLE
//#define PERLIN_NOISE_OCEAN_SAMPLE
#define SKYDOME_SAMPLE
#define COMPASS_SAMPLE
#define MODEL_SAMPLE

void SimpleCamera::OnInit()
{
	const glm::vec3 XAxis(1.f, 0.f, 0.f);
	const glm::vec3 YAxis(0.f, 1.f, 0.f);
	const glm::vec3 ZAxis(0.f, 0.f, 1.f);

	GL_CHECK_ERRORS

	{
		GLsizei w = (GLsizei)mGameProgram.GetWindowWidth();
		GLsizei h = (GLsizei)mGameProgram.GetWindowHeight();
		Engine::Initialize(0, 0, w, h, w, h);
	}

		Engine* engine = Engine::GetInstance();

		mCamera = new Camera();
		mCamera->LookAt(glm::vec3(10, 10.f, 20.f), glm::vec3(10.f, 10.f, 5.f), glm::vec3(0, 1, 0));
		engine->SetCamera(mCamera);

		SetupViewportAndProjection();

		mCamera->Update();

		Renderers::GridRenderer * gridRenderer = new Renderers::GridRenderer(50, 50);
		engine->AttachRenderer(gridRenderer);

		Renderers::AxisRenderer * axisRenderer = new Renderers::AxisRenderer();
		engine->AttachRenderer(axisRenderer);
		

#ifdef TERRAIN_SAMPLE
		{
			float yScale = 100.f;

			Renderers::TerrainRenderer::Desc desc(512, 512, glm::vec3(1.f, yScale, 1.f), 0.6f, 0.7f);
			
			desc.mTerrains.push_back(Renderers::TerrainRenderer::MapDesc("medias/Terrain/Canyon_513x513.r32", 513, true, glm::vec3(0.f), glm::angleAxis(glm::radians(180.f), YAxis)));
			//desc.mTerrains.push_back(Renderers::TerrainRenderer::MapDesc("medias/Terrain/SweetCanyon_513x513.r32", 513, true, glm::vec3(-512.f, 0.f, 0.f), glm::angleAxis(glm::radians(0.f), YAxis)));
			//desc.mTerrains.push_back(Renderers::TerrainRenderer::MapDesc("medias/Terrain/Canyon_513x513.r32", 513, true, glm::vec3(0.f), glm::angleAxis(glm::radians(180.f), YAxis)));
			//desc.mTerrains.push_back(Renderers::TerrainRenderer::MapDesc("medias/alps-valley-height-2048.raw", 2048, true, glm::vec3(0.f), glm::angleAxis(glm::radians(180.f), YAxis)));
			//desc.mTerrains.push_back(Renderers::TerrainRenderer::MapDesc("medias/alps-valley-height-2048.raw", 2048, true, glm::vec3(-512.f, 0.f, -512.f), glm::angleAxis(glm::radians(0.f), YAxis)));

			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/diffus_219.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat)); // 0
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/diffus_108.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat)); // 1
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/diffus_104.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat)); // 2

			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/specular_219.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat)); // 3
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/specular_108.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat)); // 4
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/specular_104.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat)); // 5

			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/height_219.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat)); // 6
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/height_108.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat)); // 7
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/height_104.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat)); // 8

			desc.mLowSlopeMaterials.push_back(Renderers::TerrainRenderer::MaterialDesc(0.f * yScale, 0.95f * yScale, glm::vec3(1.f), 2, glm::vec3(.1f), 1, 5, glm::vec3(0), Renderer::NoTexture, 8, 0.1f));
			desc.mLowSlopeMaterials.push_back(Renderers::TerrainRenderer::MaterialDesc(0.91f * yScale, 1.f * yScale, glm::vec3(1.f), 1, glm::vec3(.1f), 1, 4, glm::vec3(0), Renderer::NoTexture, 7, 0.1f));
			desc.mHighSlopeMaterials.push_back(Renderers::TerrainRenderer::MaterialDesc(0.f * yScale, 0.95f * yScale, glm::vec3(1.f), 0, glm::vec3(.1f), 1, 3, glm::vec3(0), Renderer::NoTexture, 6, 0.1f));
			desc.mHighSlopeMaterials.push_back(Renderers::TerrainRenderer::MaterialDesc(0.91f * yScale, 1.f * yScale, glm::vec3(1.f), 1, glm::vec3(.1f), 1, 4, glm::vec3(0), Renderer::NoTexture, 7, 0.1f));

			Renderers::TerrainRenderer * terrain = new Renderers::TerrainRenderer(desc);
			engine->AttachRenderer(terrain);
		}
#endif // TERRAIN_SAMPLE

#ifdef DEEP_OCEAN_SAMPLE
		{
			Renderers::DeepOcean::Renderer::Desc desc(true, 512, 512, "medias/CubeMaps/uvCubeMap");
			//desc.mHeightMapTextureSize = glm::ivec2(128);

			const float AmpCoef = 10.0f;
			const float WaveLengthCoef = 2.f;
			const float VelocityCoef = 5.0f;
			desc.mWaveProps.push_back(Renderers::DeepOcean::WaveProperty(005.0f, WaveLengthCoef * 1.000f, AmpCoef * 0.04f, VelocityCoef * 0.05f, 02.0f));
			desc.mWaveProps.push_back(Renderers::DeepOcean::WaveProperty(035.0f, WaveLengthCoef * 0.620f, AmpCoef * 0.07f, VelocityCoef * 0.06f, 03.0f));
			desc.mWaveProps.push_back(Renderers::DeepOcean::WaveProperty(355.0f, WaveLengthCoef * 0.500f, AmpCoef * 0.50f, VelocityCoef * 0.04f, 01.0f));
			desc.mWaveProps.push_back(Renderers::DeepOcean::WaveProperty(335.0f, WaveLengthCoef * 0.280f, AmpCoef * 0.08f, VelocityCoef * 0.05f, 04.0f));
			//desc.mWaveProps.push_back(Renderers::DeepOcean::WaveProperty(345.0f, WaveLengthCoef * 0.080f, AmpCoef * 0.03f, VelocityCoef * 0.03f, 10.0f));
			//desc.mWaveProps.push_back(Renderers::DeepOcean::WaveProperty(190.0f, WaveLengthCoef * 0.100f, AmpCoef * 0.04f, VelocityCoef * 0.05f, 02.0f));

			desc.mMaps.push_back(Renderers::DeepOcean::Renderer::MapDesc(glm::vec3(0.f, 0.f, 0.f), glm::angleAxis(glm::radians(0.f), YAxis)));
			Renderers::DeepOcean::Renderer * ocean = new Renderers::DeepOcean::Renderer(desc);
			engine->AttachRenderer(ocean);
		}
#endif // DEEP_OCEAN_SAMPLE
#ifdef PERLIN_NOISE_OCEAN_SAMPLE
		{
			Renderers::PerlinNoiseOcean::Renderer::Desc desc(512, 512, "medias/CubeMaps/uvCubeMap", Renderers::PerlinNoiseOcean::Renderer::ENormalMode::PerVertexNormal);
			//desc.mHeightMapTextureSize = glm::ivec2(128);

			const float AmpCoef = 2.0f;
			const float WaveLengthCoef = 1.0f;
			const float VelocityCoef = 0.01f;
			desc.mWaveProps.push_back(Renderers::PerlinNoiseOcean::WaveProperty(090.0f, WaveLengthCoef / 4.00f, AmpCoef * 8.0f, VelocityCoef * 1.0f));
			desc.mWaveProps.push_back(Renderers::PerlinNoiseOcean::WaveProperty(005.0f, WaveLengthCoef / 0.70f, AmpCoef * 0.4f, VelocityCoef * 2.0f));
			desc.mWaveProps.push_back(Renderers::PerlinNoiseOcean::WaveProperty(200.0f, WaveLengthCoef / 0.03f, AmpCoef * 0.2f, VelocityCoef * 3.0f));
			desc.mWaveProps.push_back(Renderers::PerlinNoiseOcean::WaveProperty(305.0f, WaveLengthCoef / 0.50f, AmpCoef * 0.1f, VelocityCoef * 4.0f));
			//desc.mWaveProps.push_back(Renderers::PerlinNoiseOcean::WaveProperty(210.0f, WaveLengthCoef * 0.340f, AmpCoef * 0.05f, VelocityCoef * 0.03f));

			desc.mMaps.push_back(Renderers::PerlinNoiseOcean::Renderer::MapDesc(glm::vec3(0.f, 0.f, 0.f), glm::angleAxis(glm::radians(0.f), YAxis)));
			Renderers::PerlinNoiseOcean::Renderer * ocean = new Renderers::PerlinNoiseOcean::Renderer(desc);
			engine->AttachRenderer(ocean);
		}
#endif // PERLIN_NOISE_OCEAN_SAMPLE

#ifdef GERSTNER_WAVE_OCEAN_SAMPLE
		{
			Renderers::GerstnerWaves::Renderer::Desc desc(true, 512, 512, "medias/CubeMaps/uvCubeMap");
			//desc.mHeightMapTextureSize = glm::ivec2(128);

			const float AmpCoef = 10.0f;
			const float WaveLengthCoef = 2.f;
			const float VelocityCoef = 10.0f;
			desc.mWaveProps.push_back(Renderers::GerstnerWaves::WaveProperty(355.0f, WaveLengthCoef * 1.000f, AmpCoef * 0.60f, VelocityCoef * 0.05f, 0.8f));
			desc.mWaveProps.push_back(Renderers::GerstnerWaves::WaveProperty(155.0f, WaveLengthCoef * 0.620f, AmpCoef * 0.27f, VelocityCoef * 0.06f, 0.1f));
			desc.mWaveProps.push_back(Renderers::GerstnerWaves::WaveProperty(200.0f, WaveLengthCoef * 0.280f, AmpCoef * 0.48f, VelocityCoef * 0.04f, 0.4f));
			desc.mWaveProps.push_back(Renderers::GerstnerWaves::WaveProperty(005.0f, WaveLengthCoef * 0.500f, AmpCoef * 0.18f, VelocityCoef * 0.05f, 0.7f));

			desc.mMaps.push_back(Renderers::GerstnerWaves::Renderer::MapDesc(glm::vec3(0.f, 0.f, 0.f), glm::angleAxis(glm::radians(0.f), YAxis)));
			Renderers::GerstnerWaves::Renderer * ocean = new Renderers::GerstnerWaves::Renderer(desc);
			engine->AttachRenderer(ocean);
		}
#endif // GERSTNER_WAVE_OCEAN_SAMPLE

#ifdef SKYDOME_SAMPLE
		{
			//Renderers::SkyboxRenderer * skybox = new Renderers::SkyboxRenderer("medias/CubeMaps/Skybox1");
			//engine->AttachSkyboxRenderer(skybox);
			mSkydome = new Renderers::SkydomeRenderer();
			engine->AttachSkydomeRenderer(mSkydome);
		}
#endif // SKYDOME_SAMPLE

#ifdef COMPASS_SAMPLE
		{
			engine->GetTextureManager()->LoadTexture2D("medias/compass.tif");

			Renderers::CompassRenderer * compass = new Renderers::CompassRenderer();
			engine->AttachRenderer(compass);
		}
#endif // COMPASS_SAMPLE





//#define ARTORIAS_SWORD_MODEL
//#define BOX_MODEL
//#define NANOSUIT_MODEL
//#define LARACROFT_MODEL
//#define CERBERUS_MODEL
//#define SHIELD_MODEL
//#define BBUNIT_MODEL
//#define BARREL_MODEL

//#define CUBESPHERE_MODEL
//#define LOW_CUBESPHERE_MODEL
//#define SPHERE_MODEL
//#define SPHERE_300_POLY_MODEL
//#define SPHERE_1250_POLY_MODEL
//#define SPHERE_5000_POLY_MODEL
#define SPHERE_20000_POLY_MODEL
//#define SPHERE_79600_POLY_MODEL
//#define SPHERE_318400_POLY_MODEL
#if defined(CUBESPHERE_MODEL) \
			|| defined(LOW_CUBESPHERE_MODEL) \
			|| defined(SPHERE_MODEL) \
			|| defined(SPHERE_300_POLY_MODEL) \
			|| defined(SPHERE_1250_POLY_MODEL) \
			|| defined(SPHERE_5000_POLY_MODEL) \
			|| defined(SPHERE_20000_POLY_MODEL) \
			|| defined(SPHERE_79600_POLY_MODEL) \
			|| defined(SPHERE_318400_POLY_MODEL) 
#define NO_PREDEFINED_MATERIALS_MODELS
#endif // defined(...)



#ifdef MODEL_SAMPLE
		{
			Geometry::ModelData::LoadOptions opt;
			opt.SetLogInfo(true);
			//opt.SetLogBoneInfo(true);

			std::vector<glm::uvec3> instancePerModel;

			Geometry::ModelData modelData;
			Geometry::ModelData::DataContextBase dataCtxBase;



#ifdef NO_PREDEFINED_MATERIALS_MODELS
			{
#ifdef LOW_CUBESPHERE_MODEL
				{
					glm::mat4 m = glm::scale(glm::vec3(1.f));
					opt.SetPreTransformVertices(m);
					modelData.LoadModel("Medias/Objects/Sphere/cube_sphere.stl", "Medias/Objects/Sphere", opt, &dataCtxBase);
				}
#endif // LOW_CUBESPHERE_MODEL

#ifdef CUBESPHERE_MODEL
				{
					glm::mat4 m = glm::scale(glm::vec3(1.f));
					opt.SetPreTransformVertices(m);
					modelData.LoadModel("Medias/Objects/Sphere/cube-sphere.dae", "Medias/Objects/Sphere", opt, &dataCtxBase);
				}
#endif //CUBESPHERE_MODEL

#ifdef SPHERE_MODEL
				{
					glm::mat4 m = glm::scale(glm::vec3(1.f));
					opt.SetPreTransformVertices(m);
					modelData.LoadModel("Medias/Objects/Sphere/sphere.obj", "Medias/Objects/Sphere", opt, &dataCtxBase);
				}
#endif // SPHERE_MODEL

#if defined(SPHERE_300_POLY_MODEL) \
				|| defined(SPHERE_1250_POLY_MODEL) \
				|| defined(SPHERE_5000_POLY_MODEL) \
				|| defined(SPHERE_20000_POLY_MODEL) \
				|| defined(SPHERE_79600_POLY_MODEL) \
				|| defined(SPHERE_318400_POLY_MODEL)
				{
					glm::mat4 m = glm::scale(glm::vec3(.05f));
					opt.SetPreTransformVertices(m);
#ifdef SPHERE_300_POLY_MODEL
					modelData.LoadModel("Medias/Objects/Sphere/300_polygon_sphere_100mm.STL", "Medias/Objects/Sphere", opt, &dataCtxBase);
#endif // SPHERE_300_POLY_MODEL
#ifdef SPHERE_1250_POLY_MODEL
					modelData.LoadModel("Medias/Objects/Sphere/1250_polygon_sphere_100mm.STL", "Medias/Objects/Sphere", opt, &dataCtxBase);
#endif // SPHERE_1250_POLY_MODEL
#ifdef SPHERE_5000_POLY_MODEL
					modelData.LoadModel("Medias/Objects/Sphere/5000_polygon_sphere_100mm.STL", "Medias/Objects/Sphere", opt, &dataCtxBase);
#endif // SPHERE_5000_POLY_MODEL
#ifdef SPHERE_20000_POLY_MODEL
					modelData.LoadModel("Medias/Objects/Sphere/20000_polygon_sphere_100mm.STL", "Medias/Objects/Sphere", opt, &dataCtxBase);
#endif // SPHERE_20000_POLY_MODEL
#ifdef SPHERE_79600_POLY_MODEL
					modelData.LoadModel("Medias/Objects/Sphere/79600_polygon_sphere_100mm.STL", "Medias/Objects/Sphere", opt, &dataCtxBase);
#endif // SPHERE_79600_POLY_MODEL
#ifdef SPHERE_318400_POLY_MODEL
					modelData.LoadModel("Medias/Objects/Sphere/318400_polygon_sphere_100mm.STL", "Medias/Objects/Sphere", opt, &dataCtxBase);
#endif // SPHERE_318400_POLY_MODEL
				}
#endif // ...
				modelData.SetMeshMaterial(dataCtxBase.mMeshInstanceIndexBase, dataCtxBase.mMaterialIndexBase);

				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				//texList.resize(dataCtxBase.mTextureIndexBase + 4);
				//texList[dataCtxBase.mTextureIndexBase + 0] = Renderer::TextureDesc("Medias/Objects/Barrel/Barrel_01_Color.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				//texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/Barrel/Barrel_01_Metallic.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat, false);
				//texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/Barrel/Barrel_01_Roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				//texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/Barrel/Barrel_01_Normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				const GLuint modelCount = 4;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(glm::uvec3(4, 3, dataCtxBase.mModelMappingIndexBase + i));

				matList.resize(dataCtxBase.mMaterialIndexBase + modelCount);
				
				// Gold
				matList[dataCtxBase.mMaterialIndexBase + 0] = Renderer::MaterialDesc(glm::vec3(.0f), Renderer::NoTexture, glm::vec3(1.00f, 0.71f, 0.29f), Renderer::NoTexture, 0.2f, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture);

				// Copper
				matList[dataCtxBase.mMaterialIndexBase + 1] = Renderer::MaterialDesc(glm::vec3(.0f), Renderer::NoTexture, glm::vec3(0.95f, 0.64f, 0.54f), Renderer::NoTexture, 0.2f, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture);

				// Silver
				matList[dataCtxBase.mMaterialIndexBase + 2] = Renderer::MaterialDesc(glm::vec3(.0f), Renderer::NoTexture, glm::vec3(0.95f, 0.93f, 0.88f), Renderer::NoTexture, 0.2f, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture);

				// Iron
				matList[dataCtxBase.mMaterialIndexBase + 3] = Renderer::MaterialDesc(glm::vec3(.0f), Renderer::NoTexture, glm::vec3(0.56f, 0.57f, 0.58f), Renderer::NoTexture, 0.2f, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture);

				for(GLuint i = 1; i < modelCount; ++i)
					modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, dataCtxBase.mModelMappingIndexBase + i);
			}
#endif
			// Iron : 0.56f, 0.57f, 0.58f
			// Copper : 0.95f, 0.64f, 0.54f
			// Gold : 1.00f, 0.71f, 0.29f
			// Aluminum : 0.91f, 0.92f, 0.92f
			// Silver : 0.95f, 0.93f, 0.88f


#ifdef ARTORIAS_SWORD_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(0.4f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/ArtoriasSword/Artorias_Sword.obj", "Medias/Objects/ArtoriasSword", opt, &dataCtxBase);

				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.resize(dataCtxBase.mTextureIndexBase + 4);
				texList[dataCtxBase.mTextureIndexBase + 0] = Renderer::TextureDesc("Medias/Objects/ArtoriasSword/Sword_albedo.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/ArtoriasSword/Sword_metallic.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/ArtoriasSword/Sword_roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/ArtoriasSword/Sword_normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				const GLuint modelCount = 4;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(glm::ivec3(5, 5, dataCtxBase.mModelMappingIndexBase + i));

				matList.resize(dataCtxBase.mMaterialIndexBase + modelCount);
				matList[dataCtxBase.mMaterialIndexBase + 0] = Renderer::MaterialDesc(glm::vec3(1.f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.56f), dataCtxBase.mTextureIndexBase + 1, .2f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				matList[dataCtxBase.mMaterialIndexBase + 1] = Renderer::MaterialDesc(glm::vec3(0.f), Renderer::NoTexture, glm::vec3(1.00f, 0.71f, 0.29f), dataCtxBase.mTextureIndexBase + 1, .2f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				matList[dataCtxBase.mMaterialIndexBase + 2] = Renderer::MaterialDesc(glm::vec3(0.f), Renderer::NoTexture, glm::vec3(0.95f, 0.64f, 0.54f), dataCtxBase.mTextureIndexBase + 1, .2f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				matList[dataCtxBase.mMaterialIndexBase + 3] = Renderer::MaterialDesc(glm::vec3(0.f), Renderer::NoTexture, glm::vec3(0.95f, 0.93f, 0.88f), dataCtxBase.mTextureIndexBase + 1, .2f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);

				modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, dataCtxBase.mModelMappingIndexBase + 1);
				modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, dataCtxBase.mModelMappingIndexBase + 2);
				modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, dataCtxBase.mModelMappingIndexBase + 3);

			}
#endif
#ifdef BOX_MODEL
			{
				glm::mat4 m = glm::translate(glm::vec3(3.f, 3.f, -3.f)) * glm::scale(glm::vec3(0.25f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/Box/Box.3DS", "Medias/Objects/Box/Textures", opt, &dataCtxBase);
				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.resize(dataCtxBase.mTextureIndexBase + 4);
				texList[dataCtxBase.mTextureIndexBase + 0] = Renderer::TextureDesc("Medias/Objects/Box/Textures/Dif.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/Box/Textures/Met.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/Box/Textures/rough.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/Box/Textures/Nor.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				const GLuint modelCount = 5;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(glm::uvec3(6, 3, dataCtxBase.mModelMappingIndexBase + i));

				matList.resize(dataCtxBase.mMaterialIndexBase + modelCount);
				matList[dataCtxBase.mMaterialIndexBase + 0] = Renderer::MaterialDesc(glm::vec3(0.784314f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.952941f), dataCtxBase.mTextureIndexBase + 1, 1.f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				matList[dataCtxBase.mMaterialIndexBase + 1] = Renderer::MaterialDesc(glm::vec3(0.9f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.95f, 0.64f, 0.54f), dataCtxBase.mTextureIndexBase + 1, 1.f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				matList[dataCtxBase.mMaterialIndexBase + 2] = Renderer::MaterialDesc(glm::vec3(0.784314f, 0.f, 0.f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.952941f), dataCtxBase.mTextureIndexBase + 1, 1.f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				matList[dataCtxBase.mMaterialIndexBase + 3] = Renderer::MaterialDesc(glm::vec3(0.f, 0.784314f, 0.f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.952941f), dataCtxBase.mTextureIndexBase + 1, 1.f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				matList[dataCtxBase.mMaterialIndexBase + 4] = Renderer::MaterialDesc(glm::vec3(0.4f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(1.00f, 0.71f, 0.29f), dataCtxBase.mTextureIndexBase + 1, 1.f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);

				modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, 1);
				modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, 2);
				modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, 3);
				modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, 4);
			}
#endif
			//modelData.LoadModel("Medias/Objects/planet/planet.obj", "Medias/Objects/planet", opt.SetFlipWindingOrder(false).SetPreTransformVertices(false).SetFlipNormal(false));
			//modelData.LoadModel("Medias/Objects/rock/rock.obj", "Medias/Objects/rock", opt.SetFlipWindingOrder(false).SetPreTransformVertices(true));
			//modelData.LoadModel("Medias/Objects/apple/apple.obj", "Medias/Objects/apple", opt.SetFlipWindingOrder(false).SetPreTransformVertices(true));
			//modelData.LoadModel("Medias/Objects/hatorrihanzo/HattoriHanzo2.0.obj", "Medias/Objects/hatorrihanzo", opt.SetPreTransformVertices(true));
			//modelData.LoadModel("Medias/Objects/Bullet/bullet.obj", "Medias/Objects/Bullet", opt.SetFlipWindingOrder(false).SetPreTransformVertices(true).SetFlipNormal(true));
			//modelData.LoadModel("Medias/Objects/nanosuit/nanosuit.obj", "Medias/Objects/nanosuit", opt.SetFlipWindingOrder(false).SetPreTransformVertices(false).SetFlipNormal(false));
			//modelData.LoadModel("Medias/Objects/Lara_Croft_v1/Lara_Croft_v1.obj", "Medias/Objects/Lara_Croft_v1", opt.SetFlipWindingOrder(false).SetPreTransformVertices(true).SetFlipNormal(true));
			//modelData.LoadModel("Medias/Objects/Guard/boblampclean.md5mesh", "Medias/Objects/Guard", opt.SetFlipWindingOrder(false).SetPreTransformVertices(false));

#ifdef NANOSUIT_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(0.5f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/nanosuit/nanosuit.obj", "Medias/Objects/nanosuit", opt, &dataCtxBase);

				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(glm::uvec3(1, 1, dataCtxBase.mModelMappingIndexBase + i));
			}
#endif
#ifdef LARACROFT_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(2.f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/Lara_Croft_v1/Lara_Croft_v1.obj", "Medias/Objects/Lara_Croft_v1", opt, &dataCtxBase);

				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(glm::uvec3(1, 1, dataCtxBase.mModelMappingIndexBase + i));
			}
#endif
#ifdef SHIELD_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(0.1f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/Shield/Shield.fbx", "Medias/Objects/Shield", opt, &dataCtxBase);

				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.resize(dataCtxBase.mTextureIndexBase + 4);
				texList[dataCtxBase.mTextureIndexBase + 0] = Renderer::TextureDesc("Medias/Objects/Shield/shield_diffuse_map.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/Shield/shield_metallic_map.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/Shield/shield_roughness_map.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/Shield/shield_normal_map.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				const GLuint modelCount = 5;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(glm::uvec3(5, 5, dataCtxBase.mModelMappingIndexBase + i));

				matList.resize(dataCtxBase.mMaterialIndexBase + modelCount);
				matList[dataCtxBase.mMaterialIndexBase + 0] = Renderer::MaterialDesc(glm::vec3(1.0f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.56f, 0.57f, 0.58f), dataCtxBase.mTextureIndexBase + 1, .5f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				matList[dataCtxBase.mMaterialIndexBase + 1] = Renderer::MaterialDesc(glm::vec3(1.0f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.95f, 0.64f, 0.54f), dataCtxBase.mTextureIndexBase + 1, .5f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				matList[dataCtxBase.mMaterialIndexBase + 2] = Renderer::MaterialDesc(glm::vec3(1.0f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(1.00f, 0.71f, 0.29f), dataCtxBase.mTextureIndexBase + 1, .5f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				matList[dataCtxBase.mMaterialIndexBase + 3] = Renderer::MaterialDesc(glm::vec3(1.0f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.91f, 0.92f, 0.92f), dataCtxBase.mTextureIndexBase + 1, .5f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				matList[dataCtxBase.mMaterialIndexBase + 4] = Renderer::MaterialDesc(glm::vec3(1.0f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.95f, 0.93f, 0.88f), dataCtxBase.mTextureIndexBase + 1, .5f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);

				modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, 1);
				modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, 2);
				modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, 3);
				modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, 4);


			}
#endif
#ifdef BBUNIT_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(.05f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/bb-unit/bb-unit.obj", "Medias/Objects/bb-unit", opt, &dataCtxBase);

				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.resize(dataCtxBase.mTextureIndexBase + 8);
				texList[dataCtxBase.mTextureIndexBase + 0] = Renderer::TextureDesc("Medias/Objects/bb-unit/Body_Diffuse.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/bb-unit/Body_Specular.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/bb-unit/Body_Roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/bb-unit/Body_Normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				texList[dataCtxBase.mTextureIndexBase + 4] = Renderer::TextureDesc("Medias/Objects/bb-unit/Head_Diffuse.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 5] = Renderer::TextureDesc("Medias/Objects/bb-unit/Head_Specular.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 6] = Renderer::TextureDesc("Medias/Objects/bb-unit/Head_Roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 7] = Renderer::TextureDesc("Medias/Objects/bb-unit/Head_Normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				const GLuint modelCount = 1;

				matList.resize(dataCtxBase.mMaterialIndexBase + modelCount * 4);

				matList[dataCtxBase.mMaterialIndexBase + 0] = Renderer::MaterialDesc(glm::vec3(1.f, 0.f, 0.f), Renderer::NoTexture, glm::vec3(1.f, 0.f, 0.f), Renderer::NoTexture, 1.f, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture);
				matList[dataCtxBase.mMaterialIndexBase + 1] = Renderer::MaterialDesc(glm::vec3(1.0f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.56f, 0.57f, 0.58f), dataCtxBase.mTextureIndexBase + 1, 1.f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				matList[dataCtxBase.mMaterialIndexBase + 2] = Renderer::MaterialDesc(glm::vec3(1.0f), dataCtxBase.mTextureIndexBase + 4, glm::vec3(0.56f, 0.57f, 0.58f), dataCtxBase.mTextureIndexBase + 5, 1.f, dataCtxBase.mTextureIndexBase + 6, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 7);
				matList[dataCtxBase.mMaterialIndexBase + 3] = Renderer::MaterialDesc(glm::vec3(0.5f), Renderer::NoTexture, glm::vec3(0.56f, 0.57f, 0.58f), Renderer::NoTexture, 1.f, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture);

				if (modelCount >= 2)
				{
					matList[dataCtxBase.mMaterialIndexBase + 4] = Renderer::MaterialDesc(glm::vec3(1.f, 0.f, 0.f), Renderer::NoTexture, glm::vec3(1.f, 0.f, 0.f), Renderer::NoTexture, 1.f, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture);
					matList[dataCtxBase.mMaterialIndexBase + 5] = Renderer::MaterialDesc(glm::vec3(1.f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.95f, 0.64f, 0.54f), dataCtxBase.mTextureIndexBase + 1, 1.f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
					matList[dataCtxBase.mMaterialIndexBase + 6] = Renderer::MaterialDesc(glm::vec3(1.f), dataCtxBase.mTextureIndexBase + 4, glm::vec3(0.95f, 0.64f, 0.54f), dataCtxBase.mTextureIndexBase + 5, 1.f, dataCtxBase.mTextureIndexBase + 6, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 7);
					matList[dataCtxBase.mMaterialIndexBase + 7] = Renderer::MaterialDesc(glm::vec3(0.5f), Renderer::NoTexture, glm::vec3(0.95f, 0.64f, 0.54f), Renderer::NoTexture, 1.f, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture);
				}

				if (modelCount >= 3)
				{
					matList[dataCtxBase.mMaterialIndexBase + 8] = Renderer::MaterialDesc(glm::vec3(0.f), Renderer::NoTexture, glm::vec3(1.f, 0.f, 0.f), Renderer::NoTexture, 1.f, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture);
					matList[dataCtxBase.mMaterialIndexBase + 9] = Renderer::MaterialDesc(glm::vec3(1.f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(1.00f, 0.71f, 0.29f), dataCtxBase.mTextureIndexBase + 1, 1.f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
					matList[dataCtxBase.mMaterialIndexBase + 10] = Renderer::MaterialDesc(glm::vec3(1.f), dataCtxBase.mTextureIndexBase + 4, glm::vec3(1.00f, 0.71f, 0.29f), dataCtxBase.mTextureIndexBase + 5, 1.f, dataCtxBase.mTextureIndexBase + 6, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 7);
					matList[dataCtxBase.mMaterialIndexBase + 11] = Renderer::MaterialDesc(glm::vec3(0.5f), Renderer::NoTexture, glm::vec3(1.00f, 0.71f, 0.29f), Renderer::NoTexture, 1.f, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture);
				}
				
				for (GLuint i = 0; i < modelCount; ++i)
				{
					instancePerModel.push_back(glm::uvec3(3, 4, dataCtxBase.mModelMappingIndexBase + i));
					if (i >= 1)
						modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, (i + 1) * 4);
				}

			}
#endif

#ifdef BARREL_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(1.f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/Barrel/Barrel_01.obj", "Medias/Objects/Barrel", opt, &dataCtxBase);
				modelData.SetMeshMaterial(dataCtxBase.mMeshInstanceIndexBase, dataCtxBase.mMaterialIndexBase);

				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.resize(dataCtxBase.mTextureIndexBase + 4);
				texList[dataCtxBase.mTextureIndexBase + 0] = Renderer::TextureDesc("Medias/Objects/Barrel/Barrel_01_Color.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/Barrel/Barrel_01_Metallic.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/Barrel/Barrel_01_Roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/Barrel/Barrel_01_Normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(glm::uvec3(6, 3, dataCtxBase.mModelMappingIndexBase + i));

				matList.resize(dataCtxBase.mMaterialIndexBase + modelCount);

				matList[dataCtxBase.mMaterialIndexBase + 0] = Renderer::MaterialDesc(glm::vec3(1.0f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.5f), dataCtxBase.mTextureIndexBase + 1, 1.f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);

			}
#endif

			// Iron : 0.56f, 0.57f, 0.58f
			// Copper : 0.95f, 0.64f, 0.54f
			// Gold : 1.00f, 0.71f, 0.29f
			// Aluminum : 0.91f, 0.92f, 0.92f
			// Silver : 0.95f, 0.93f, 0.88f

#ifdef CERBERUS_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(10.f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/Cerberus/Cerberus_LP.dae", "Medias/Objects/Cerberus/Textures", opt, &dataCtxBase);

				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.resize(dataCtxBase.mTextureIndexBase + 4);
				texList[dataCtxBase.mTextureIndexBase + 0] = Renderer::TextureDesc("Medias/Objects/Cerberus/Textures/Cerberus_A.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/Cerberus/Textures/Cerberus_M.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/Cerberus/Textures/Cerberus_R.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/Cerberus/Textures/Cerberus_N.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				matList[dataCtxBase.mMaterialIndexBase + 0] = Renderer::MaterialDesc(glm::vec3(0.588235f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.56f, 0.57f, 0.58f), dataCtxBase.mTextureIndexBase + 1, 1.f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);

				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(glm::uvec3(1, 1, dataCtxBase.mModelMappingIndexBase + i));
			}
#endif

			GLuint capacity = 0;
			GLuint modelCount = (GLuint)instancePerModel.size();
			for (std::vector<glm::uvec3>::const_iterator it = instancePerModel.begin(); it != instancePerModel.end(); ++it)
			{
				capacity += it->x * it->y;
			}

			Renderers::ModelRenderer * modelRenderer = Renderers::ModelRenderer::CreateFromModel(engine, modelData, capacity > 0 ? capacity : 1);
			if (modelRenderer != nullptr)
			{
				GLuint modelCount = (GLuint)modelData.GetModelMappingList().size();

				glm::vec3 position = glm::vec3(0.f, 10.f, 0.f);
				glm::vec3 transl = glm::vec3(10.f, 10.f, -10.f);

				glm::vec3 rotAngle(0.f);
				glm::vec3 p(0.f);

				float rotX = glm::two_pi<float>() / (float)modelCount;

				for (GLuint i = 0; i < modelCount; ++i)
				{
					const glm::uvec3 & modelInfo = instancePerModel[i];

					glm::quat qX = glm::angleAxis(rotAngle.x, XAxis);
					rotAngle.x += rotX;

					float rotZ = glm::two_pi<float>() / (float)modelInfo.x;
					float rotY = glm::two_pi<float>() / (float)modelInfo.y;

					GLuint modelId = modelInfo.z;

					p.y = 0.f;
					for (GLuint j = 0; j < modelInfo.x; ++j)
					{
						glm::quat qY = glm::angleAxis(rotAngle.y, YAxis);
						rotAngle.y += rotY;

						p.z = 0.f;
						for (GLuint k = 0; k < modelInfo.y; ++k)
						{
							glm::quat qZ = glm::angleAxis(rotAngle.z, ZAxis);
							rotAngle.z += rotZ;

							glm::quat qRot = qX * qY * qZ;

							Renderables::Model * model = modelRenderer->CreateModelInstance(modelId);
							if (model != nullptr)
							{
								model->GetFrame()->SetPosition(position + p);
								model->GetFrame()->SetRotation(qRot);
								//model->GetFrame()->SetScale(scale);
							}

							p.z += transl.z;
						}

						p.y += transl.y;
					}

					p.x += transl.x;
				}

				PRINT_MESSAGE("Model instance count = %li.", modelRenderer->GetCount());
			}
			else
			{
				PRINT_ERROR("Cannot create ModelRenderer!");
			}

		}
#endif // MODEL_SAMPLE

		// TextRenderer
		{
			Renderers::TextRenderer * textRenderer = new Renderers::TextRenderer();
			Renderers::TextRenderer::Desc desc;
			desc.mPoliceList.resize(2);
			desc.mTextureSize = glm::ivec2(512);

			Renderers::TextRenderer::PoliceDesc & p1 = desc.mPoliceList[0];
			p1.mFont = "Medias\\Fonts\\OCRAEXT.TTF";
			p1.mCharacterHeight = 24;

			Renderers::TextRenderer::PoliceDesc & p2 = desc.mPoliceList[1];
			p2.mFont = "Medias\\Fonts\\arial.ttf";
			p2.mCharacterHeight = 48;
			for (GLuint i = L' '; i <= L'µ'; ++i)
				p2.mCharacterSet.push_back(i);

			textRenderer->Initialize(desc);
		}

	// Setup Lights
		Lights::SpotLight * spotLight1 = engine->CreateSpotLight(glm::vec3(65.f, 8.f, 15.f), glm::vec3(1.f, 1.f, 1.f), 50000.f, 10000.f, glm::normalize(glm::vec3(.3f, -0.7f, 1.f)), glm::radians(15.f), glm::radians(25.f));
		Lights::SpotLight * spotLight2 = engine->CreateSpotLight(glm::vec3(15.f, 5.f, 10.f), glm::vec3(1.f, 1.f, 1.f), 10000.f, 100.f, glm::normalize(glm::vec3(.5f, 0.5f, -1.f)), glm::radians(15.f), glm::radians(25.f));


		Lights::PointLight * ptLight2 = engine->CreatePointLight(glm::vec3(-10.f, 10.f, -20.f), glm::vec3(1.f, 1.f, 1.f), 10000.f, 1000.f);
		Lights::PointLight * ptLight3 = engine->CreatePointLight(glm::vec3(300.f, 50.f, 40.f), glm::vec3(1.f, 1.f, 1.f), 100000.f, 50000.f);
		Lights::PointLight * ptLight1 = engine->CreatePointLight(glm::vec3(30.f, 2.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 200.f, 30.f);
		Lights::PointLight * ptLight4 = engine->CreatePointLight(glm::vec3(-200.f, 200.f, -200.f), glm::vec3(1.f, 1.f, 1.f), 250000.f, 250.f);
		Lights::PointLight * ptLight5 = engine->CreatePointLight(glm::vec3(20.f, 30.f, 5.f), glm::vec3(1.f, 1.f, 1.f), 20000.f, 100.f);


		mSunLight = engine->CreateDirectionalLight(glm::normalize(glm::vec3(1.f, -1.f, 0.f)), glm::vec3(1.f, 1.f, 1.f), 100.f);
		//Lights::DirectionalLight * dirLight1 = engine->CreateDirectionalLight(glm::normalize(glm::vec3(0.f, 0.f, 1.f)), glm::vec3(1.f, 1.f, 1.f), 1.8f);

	//setup camera

	UpdateSunPosition();

	engine->CreateDynamicResources();

	PRINT_MESSAGE("Initialization successfull");


	//{
	//	mCamera->Update();
	//	glm::mat4 mvp = mCamera->GetProjectionMatrix() * mCamera->GetViewMatrix();
	//	for (auto i = 0; i < xCount; ++i)
	//	{
	//		for (auto j = 0; j < yCount; ++j)
	//		{
	//			for (auto k = 0; k < zCount; ++k)
	//			{
	//				glm::vec4 v(XStart + XMul * i, YStart + YMul * j, ZStart + ZMul * k, 1.0f);

	//				glm::vec4 v2 = mvp * v;
	//				v2 = v2 / v2.w;
	//				PRINT_MESSAGE("v(%f, %f, %f) : v2(%f, %f, %f)\n", v.x, v.y, v.z, v2.x, v2.y, v2.z);
	//			}
	//		}
	//	}
	//}

}

SimpleCamera::SimpleCamera(GameProgram & gameProgram)
	: GameEngine(gameProgram)
	, mCamera(nullptr)
	, mSkydome(nullptr)
	, mSunLight(nullptr)
{
}

SimpleCamera::~SimpleCamera()
{
	mCamera = nullptr;
}

//mosue click handler
void SimpleCamera::OnMouseButton(MouseState mouseState, int x, int y)
{
	if (mouseState.HasAButtonPressed())
	{
		mOldX = x;
		mOldY = y;
	}

	if (mouseState.IsLeftPressed())
		mState = 1;
	else if (mouseState.IsRightPressed())
		mState = 2;
	else if (mouseState.IsMiddlePressed())
		mState = 3;
	else
		mState = 0;
}

//mosue move handler
void SimpleCamera::OnMouseMove(int x, int y)
{
	if (mState == 0)
		return;

	bool hasMoved = false;
	if (mState == 3)
	{
		mDist = (y - mOldY) / 60.0f;
		//mCamera->Zoom(mDist);
		hasMoved = true;
	}
	else if (mState == 2)
	{
		float dy = float(y - mOldY) / 100.0f;
		float dx = float(mOldX - x) / 100.0f;
		if (mUseFiltering)
			filterMouseMoves(dx, dy);
		else
		{
			mMouseX = dx;
			mMouseY = dy;
		}

		mCamera->Strafe(mMouseX);
		mCamera->SlideUp(mMouseY);
		hasMoved = true;
	}
	else 
	{
		mRotY = float(y - mOldY);
		mRotX = float(mOldX - x);
		if (mUseFiltering)
			filterMouseMoves(mRotX, mRotY);
		else
		{
			mMouseX = mRotX;
			mMouseY = mRotY;
		}

		if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) == 0)
		{
			mCamera->Yaw(glm::radians(mMouseX));
		}
		if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000) == 0)
		{
			mCamera->Pitch(glm::radians(mMouseY));
		}

		hasMoved = true;
	}

	if (hasMoved)
	{
		mOldX = x;
		mOldY = y;
		//glutPostRedisplay();
	}
}

void SimpleCamera::OnKeyDown(WPARAM key, bool wasPressed, int /*repeatCount*/, bool /*altPressed*/)
{
	Engine * engine = Engine::GetInstance();

	float exposureInc = 0.01f;
	float gammaInc = 0.01f;
	switch (key)
	{
	case VK_SPACE:
		mUseFiltering = !mUseFiltering;
		break;

	case L'N':
	case L'n':
		engine->EnableDrawVertexNormal(!engine->IsDrawVertexNormalEnabled());
		//glutPostRedisplay();
		break;

	case L'B':
	case L'b':
		engine->EnableDrawGBufferNormal(!engine->IsDrawGBufferNormalEnabled());
		//glutPostRedisplay();
		break;

	case 'e':
		exposureInc = -exposureInc;
	case 'E':
		{
			float exposure = engine->GetExposure();
			exposure = glm::clamp(exposure + exposureInc, 0.01f, 10.0f);
			engine->SetExposure(exposure);
		}
		break;

	case 'g':
		gammaInc = -gammaInc;
	case 'G':
		{
			float gamma = engine->GetGamma();
			gamma = glm::clamp(gamma + gammaInc, 0.01f, 10.0f);
			engine->SetGamma(gamma);
		}
		break;

	case 'x':
	case 'X':
		{
		engine->SetWireFrame(!engine->GetWireFrame());
		}
		break;

	case 'l':
	case 'L':
	{
		engine->SetIsDrawLightPositionEnabled(!engine->IsDrawLightPositionEnabled());
	}
	break;

	case '+':
		mMoveSpeed += 1.f;
		break;

	case '-':
		mMoveSpeed -= 1.f; 
		if (mMoveSpeed < 0.f)
			mMoveSpeed = 0.f;
		break;

	case '*':
		mMoveSpeed = DEFAULT_MOVE_SPEED;
		break;

	case 'v':
	{
		glm::ivec2 grid = engine->GetDrawGBufferNormalGrid();
		grid += glm::ivec2(1);
		grid = glm::clamp(grid, glm::ivec2(5, 5), glm::ivec2(50, 50));
		engine->SetDrawGBufferNormalGrid(grid.x, grid.y);

	}
	break;

	case 'V':
	{
		glm::ivec2 grid = Engine::GetInstance()->GetDrawGBufferNormalGrid();
		grid -= glm::ivec2(1);
		grid = glm::clamp(grid, glm::ivec2(5, 5), glm::ivec2(50, 50));
		engine->SetDrawGBufferNormalGrid(grid.x, grid.y);
	}
	break;

	case ',':
	{
		GLfloat x = engine->GetDrawVertexNormalMagnitude() * 2.f;
		engine->SetDrawVertexNormalMagnitude(x);
	}
	break;

	case '?':
	{
		GLfloat x = engine->GetDrawVertexNormalMagnitude() * 0.5f;
		engine->SetDrawVertexNormalMagnitude(x);
	}
	break;

	case 'O':
	case 'o':
		{
			float inc = key == 'o' ? +0.1f : -0.1f;
			engine->SetSSAORadius(max(0.1f, engine->GetSSAORadius() + inc));
		}
		break;

	case 'K':
	case 'k':
		{
			GLuint inc = key == 'k' ? +1 : -1;
			engine->SetSSAOKernelSize(max(16, engine->GetSSAOKernelSize() + inc));
		}
		break;

	case 'p':
	case 'P':
		mFreezeTimer = !mFreezeTimer;
		break;

	case '<':
		if (mSunPositionDegree < SunPositionMax)
		{
			mSunPositionDegree += mSunPositionInc;
			if (mSunPositionDegree > SunPositionMax)
				mSunPositionDegree = SunPositionMax;

			UpdateSunPosition();
		}
		break;

	case '>':
		if (mSunPositionDegree > SunPositionMin)
		{
			mSunPositionDegree -= mSunPositionInc;
			if (mSunPositionDegree < SunPositionMin)
				mSunPositionDegree = SunPositionMin;

			UpdateSunPosition();
		}
		break;
	}
	//glutPostRedisplay();
}

void SimpleCamera::OnKeyUp(WPARAM /*key*/, bool /*wasPressed*/, int /*repeatCount*/, bool /*altPressed*/)
{
}

void SimpleCamera::filterMouseMoves(float dx, float dy)
{
	for (int i = MOUSE_HISTORY_BUFFER_SIZE - 1; i > 0; --i)
	{
		m_mouseHistory[i] = m_mouseHistory[i - 1];
	}

	// Store current mouse entry at front of array.
	m_mouseHistory[0] = glm::vec2(dx, dy);

	float averageX = 0.0f;
	float averageY = 0.0f;
	float averageTotal = 0.0f;
	float currentWeight = 1.0f;

	// Filter the mouse.
	for (int i = 0; i < MOUSE_HISTORY_BUFFER_SIZE; ++i)
	{
		glm::vec2 tmp = m_mouseHistory[i];
		averageX += tmp.x * currentWeight;
		averageY += tmp.y * currentWeight;
		averageTotal += 1.0f * currentWeight;
		currentWeight *= MOUSE_FILTER_WEIGHT;
	}

	mMouseX = averageX / averageTotal;
	mMouseY = averageY / averageTotal;

}

void SimpleCamera::OnUpdate()
{
	bool bWalk = false, bStrafe = false, bSunPos = false;
	float dx = 0, dy = 0, speed = mMoveSpeed;

	if (GetAsyncKeyState(VK_SHIFT))
	{
		speed *= 3.f;
	}

	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		PostExitGame(0);
		return;
	}

	if ((GetAsyncKeyState(VK_Z) & 0x8000) || (GetAsyncKeyState(VK_W) & 0x8000))
	{
		dy += (speed);
		bWalk = true;
	}

	if (GetAsyncKeyState(VK_S) & 0x8000)
	{
		dy -= (speed);
		bWalk = true;
	}

	if ((GetAsyncKeyState(VK_Q) & 0x8000) || (GetAsyncKeyState(VK_A) & 0x8000))
	{
		dx -= (speed);
		bStrafe = true;
	}

	if (GetAsyncKeyState(VK_D) & 0x8000)
	{
		dx += (speed);
		bStrafe = true;
	}

	static bool wasF2Pressed = false;
	if (GetAsyncKeyState(VK_F2) & 0x8000)
	{
		if (!wasF2Pressed)
		{
			mShowDeferredBufferState = (mShowDeferredBufferState + 1) % (GLint)Engine::EDeferredDebug::__count__;
			if (mShowDeferredBufferState == 0)
				Engine::GetInstance()->DisableDeferredDebug();
			else
				Engine::GetInstance()->EnableDeferredDebug((Engine::EDeferredDebug)mShowDeferredBufferState);
		}
		wasF2Pressed = true;
	}
	else
		wasF2Pressed = false;
	
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
	}

	if (bWalk)
	{
		mCamera->Walk((float)(dy * mDeltaTime));
	}
	
	if (bStrafe)
	{
		mCamera->Strafe((float)(dx * mDeltaTime));
	}

	if (bSunPos)
	{
		UpdateSunPosition();
	}

	//if (bWalk || bStrafe)
	//	glutPostRedisplay();
}

void SimpleCamera::UpdateSunPosition()
{
	float sunPosRadian = glm::radians((float)(mSunPositionDegree));
	glm::vec3 sunDir = glm::normalize(glm::vec3(cos(sunPosRadian), sin(sunPosRadian), 0.f));
	if (mSunLight != nullptr)
		mSunLight->SetDirection(sunDir);
	if (mSkydome != nullptr)
		mSkydome->SetSunDirection(sunDir);
}


} // namespace CoreGame