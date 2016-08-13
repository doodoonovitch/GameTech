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

//#define CUBE_SAMPLES
//#define TERRAIN_SAMPLE
//#define DEEP_OCEAN_SAMPLE
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

		Renderers::GridRenderer * gridRenderer = new Renderers::GridRenderer(50, 50);
		engine->AttachRenderer(gridRenderer);

		Renderers::AxisRenderer * axisRenderer = new Renderers::AxisRenderer();
		engine->AttachRenderer(axisRenderer);
		
#ifdef CUBE_SAMPLES
		{
			//const float XStart = 1.f;
			//const float YStart = 0.f;
			//const float ZStart = 1.f;
			//const float XMul = 5.f;
			//const float YMul = 5.f;
			//const float ZMul = -200.f;

			const float XStart = 1.52f;
			const float YStart = 1.02f;
			const float ZStart = -1.52f;
			const float XMul = 5.f;
			const float YMul = 5.f;
			const float ZMul = -5.f;

			int xCount = 30;
			int yCount = 30;
			int zCount = 30;

			Renderers::CubeRenderer::Desc desc;
			desc.mTextures.push_back(Renderer::TextureDesc("medias/cube.tif", TextureCategory::Diffuse, TextureWrap::Clamp, TextureWrap::Clamp)); // 0
			desc.mTextures.push_back(Renderer::TextureDesc("medias/cube2.tif", TextureCategory::Diffuse, TextureWrap::Clamp, TextureWrap::Clamp)); // 1
			desc.mTextures.push_back(Renderer::TextureDesc("medias/cube_N.tif", TextureCategory::NormalMap, TextureWrap::Clamp, TextureWrap::Clamp)); // 2
			desc.mTextures.push_back(Renderer::TextureDesc("medias/fallforestrocks01.tif", TextureCategory::Diffuse, TextureWrap::Clamp, TextureWrap::Clamp)); // 3
			desc.mTextures.push_back(Renderer::TextureDesc("medias/fallforestrocks01_n.tif", TextureCategory::NormalMap, TextureWrap::Clamp, TextureWrap::Clamp)); // 4
			desc.mTextures.push_back(Renderer::TextureDesc("medias/bricks.tif", TextureCategory::Diffuse, TextureWrap::Clamp, TextureWrap::Clamp)); // 5
			desc.mTextures.push_back(Renderer::TextureDesc("medias/bricks_n.tif", TextureCategory::NormalMap, TextureWrap::Clamp, TextureWrap::Clamp)); // 6
			desc.mTextures.push_back(Renderer::TextureDesc("medias/bricks1024.tif", TextureCategory::Diffuse, TextureWrap::Clamp, TextureWrap::Clamp)); // 7
			desc.mTextures.push_back(Renderer::TextureDesc("medias/bricks1024_s.tif", TextureCategory::Specular, TextureWrap::Clamp, TextureWrap::Clamp)); // 8
			desc.mTextures.push_back(Renderer::TextureDesc("medias/bricks1024_n.tif", TextureCategory::NormalMap, TextureWrap::Clamp, TextureWrap::Clamp)); // 9

			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(.7f, .7f, .7f), 5, glm::vec3(0.664f, 0.824f, 0.850f), Renderers::CubeRenderer::NoTexture, 0.1f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 6)); // zinc

			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(0.8f, 0.8f, 0.8f), 0, glm::vec3(0.664f, 0.824f, 0.850f), Renderers::CubeRenderer::NoTexture, 0.1f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 2)); // zinc


			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(0.7f, .7f, .7f), 3, glm::vec3(0.045f, 0.045f, 0.045f), Renderers::CubeRenderer::NoTexture, 0.8f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 4));

			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(.5f, .3f, .1f), 5, glm::vec3(0.955f, 0.638f, 0.538f), Renderers::CubeRenderer::NoTexture, 0.1f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 6));


			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(.5f, .5f, .5f), 7, glm::vec3(.1f, .1f, .1f), Renderers::CubeRenderer::NoTexture, 1.f, Renderers::CubeRenderer::NoTexture, glm::vec3(0.f, 8600.f, 5000.f), 8, 9));
			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(.5f, .5f, .5f), 7, glm::vec3(.2f, .2f, .2f), 8, .9f, 
				Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 9));


			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(0.8f, 0.8f, 0.8f), 1, glm::vec3(.1f, .1f, .1f), Renderers::CubeRenderer::NoTexture, 0.f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 2));

			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(1.f, 0.f, 0.f), 0, glm::vec3(.02f, .02f, .02f), Renderers::CubeRenderer::NoTexture, 0.f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 2));

			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(0.f, 0.f, 1.f), 1, glm::vec3(.05f, .05f, .05f), Renderers::CubeRenderer::NoTexture, .3f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 2));
			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(1.f, 1.f, 0.f), 0, glm::vec3(.08f, .08f, .08f), Renderers::CubeRenderer::NoTexture, .2f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 2));

			const int materialCount = (int)desc.mMaterials.size();

			Renderers::CubeRenderer * cubeRenderer = new Renderers::CubeRenderer(desc, xCount * yCount * zCount, 1);
			engine->AttachRenderer(cubeRenderer);

			for (auto i = 0; i < xCount; ++i)
			{
				float xAngle = (glm::two_pi<float>() * (float)i) / (float)xCount;
				glm::quat qX = glm::angleAxis(xAngle, XAxis);

				for (auto j = 0; j < yCount; ++j)
				{
					float yAngle = (glm::two_pi<float>() * (float)j) / (float)yCount;
					glm::quat qY = glm::angleAxis(yAngle, YAxis);

					for (auto k = 0; k < zCount; ++k)
					{
						float zAngle = (glm::two_pi<float>() * (float)k) / (float)zCount;
						glm::quat qZ = glm::angleAxis(zAngle, ZAxis);

						Cube * cube = cubeRenderer->CreateCube(i % materialCount);
						cube->GetFrame()->SetPosition(glm::vec3(XStart + XMul * i, YStart + YMul * j, ZStart + ZMul * k));

						glm::quat qRot = qX * qY * qZ;
						cube->GetFrame()->SetRotation(qRot);
					}
				}
			}
		}
#endif // CUBE_SAMPLE

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
			Renderers::DeepOceanRenderer::Desc desc(512, 512, glm::vec2(1.f, 1.f), "medias/CubeMaps/uvCubeMap");

			const float AmpCoef = 0.1f;
			const float WaveLengthCoef = 1.f;
			const float VelocityCoef = 0.01f;
			desc.mWaveProps[0] = Renderers::DeepOceanRenderer::WaveProps(355.0f, WaveLengthCoef * 1.000f, AmpCoef * 28.0f, VelocityCoef * 5.0f, 10.0f);
			desc.mWaveProps[1] = Renderers::DeepOceanRenderer::WaveProps(155.0f, WaveLengthCoef * 0.620f, AmpCoef * 7.0f, VelocityCoef * 6.0f, 3.0f);
			desc.mWaveProps[2] = Renderers::DeepOceanRenderer::WaveProps(005.0f, WaveLengthCoef * 0.500f, AmpCoef * 18.0f, VelocityCoef * 5.0f, 4.0f);
			desc.mWaveProps[3] = Renderers::DeepOceanRenderer::WaveProps(200.0f, WaveLengthCoef * 0.280f, AmpCoef * 6.0f, VelocityCoef * 4.0f, 2.0f);

			desc.mMaps.push_back(Renderers::DeepOceanRenderer::MapDesc(glm::vec3(0.f, 0.f, 0.f), glm::angleAxis(glm::radians(0.f), YAxis)));
			Renderers::DeepOceanRenderer * ocean = new Renderers::DeepOceanRenderer(desc);
			engine->AttachRenderer(ocean);
		}
#endif // DEEP_OCEAN_SAMPLE
		/*
		{
			Renderers::ShallowWaterRenderer::Desc desc(512, 512, glm::vec2(1.f, 1.f), "medias/CubeMaps/uvCubeMap");

			const float AmpCoef = 0.2f;
			const float WaveLengthCoef = 1.f;
			const float VelocityCoef = 0.01f;
			desc.mWaveProps[0] = Renderers::ShallowWaterRenderer::WaveProps(355.0f, WaveLengthCoef * 1.000f, AmpCoef * 8.0f, VelocityCoef * 5.0f, 10.0f);
			desc.mWaveProps[1] = Renderers::ShallowWaterRenderer::WaveProps(155.0f, WaveLengthCoef * 0.620f, AmpCoef * 7.0f, VelocityCoef * 6.0f, 2.0f);
			desc.mWaveProps[2] = Renderers::ShallowWaterRenderer::WaveProps(005.0f, WaveLengthCoef * 0.500f, AmpCoef * 8.0f, VelocityCoef * 5.0f, 3.0f);
			desc.mWaveProps[3] = Renderers::ShallowWaterRenderer::WaveProps(200.0f, WaveLengthCoef * 0.280f, AmpCoef * 6.0f, VelocityCoef * 4.0f, 1.0f);

			desc.mMaps.push_back(Renderers::ShallowWaterRenderer::MapDesc(glm::vec3(0.f, 0.f, 0.f), glm::angleAxis(glm::radians(0.f), YAxis)));
			Renderers::ShallowWaterRenderer * ocean = new Renderers::ShallowWaterRenderer(desc);
			engine->AttachRenderer(ocean);
		}
		*/

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

#ifdef MODEL_SAMPLE
		{
			Geometry::ModelData::LoadOptions opt;
			opt.SetLogInfo(true);
			//opt.SetLogBoneInfo(true);

			Geometry::ModelData modelData;
			glm::vec3 scale(1.f);
			glm::vec3 position(0.f);
			glm::quat qOrientation;
			glm::vec3 transl(1.f);
			glm::vec3 rot(0.f);
			glm::uvec3 count(1, 1, 1);

#define ARTORIAS_SWORD_MODEL 0
#define BOX_MODEL 1
#define HOUSE_MODEL 0
#define BALL_MODEL 0

#if ARTORIAS_SWORD_MODEL == 1
			{
				modelData.LoadModel("Medias/Objects/ArtoriasSword/Artorias_Sword.obj", "Medias/Objects/ArtoriasSword", opt.SetFlipWindingOrder(false).SetPreTransformVertices(true));
				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();
				
				texList.clear();
				texList.push_back(Renderer::TextureDesc("Medias/Objects/ArtoriasSword/Sword_albedo.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false));
				texList.push_back(Renderer::TextureDesc("Medias/Objects/ArtoriasSword/Sword_metallic.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat, false));
				texList.push_back(Renderer::TextureDesc("Medias/Objects/ArtoriasSword/Sword_roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false));
				texList.push_back(Renderer::TextureDesc("Medias/Objects/ArtoriasSword/Sword_normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false));

				matList.clear();
				matList.push_back(Renderer::MaterialDesc(glm::vec3(1.f), 0, glm::vec3(0.56f), 1, .2f, 2, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 3));
				//matList.push_back(Renderer::MaterialDesc(glm::vec3(0.f), Renderers::CubeRenderer::NoTexture, glm::vec3(1.00f, 0.71f, 0.29f), Renderers::CubeRenderer::NoTexture, .1f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, Renderers::CubeRenderer::NoTexture));

				//position = glm::vec3(5.f, 25.f, 10.f);

				position = glm::vec3(0.f, 0.f, 0.f);
				scale = glm::vec3(0.1f);
				transl = glm::vec3(1.f, 3.f, -1.f);
				count = glm::uvec3(20);
				rot = glm::vec3(0.f, glm::two_pi<float>() / (float)count.y, 0.f);

			}
#elif BOX_MODEL == 1
			{
				glm::mat4 m = glm::scale(glm::vec3(0.25f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/Box/Box.3DS", "Medias/Objects/Box/Textures", opt);
				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.clear();
				texList.push_back(Renderer::TextureDesc("Medias/Objects/Box/Textures/Dif.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false));
				texList.push_back(Renderer::TextureDesc("Medias/Objects/Box/Textures/Met.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat, false));
				texList.push_back(Renderer::TextureDesc("Medias/Objects/Box/Textures/rough.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false));
				texList.push_back(Renderer::TextureDesc("Medias/Objects/Box/Textures/Nor.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false));

				matList.clear();
				matList.push_back(Renderer::MaterialDesc(glm::vec3(0.784314f), 0, glm::vec3(0.952941f), 1, 1.f, 2, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 3));

				position = glm::vec3(3.f, 3.f, -3.f);
				//scale = glm::vec3(0.25f);
				transl = glm::vec3(10.f, 10.f, -10.f);
				count = glm::uvec3(20);
				rot = glm::vec3(glm::two_pi<float>() / (float)count.x, glm::two_pi<float>() / (float)count.y, glm::two_pi<float>() / (float)count.z);

			}
#elif HOUSE_MODEL == 1
			{
				//
				glm::mat4 m = glm::rotate(glm::pi<float>(), YAxis) * glm::translate(glm::vec3(13.4f, 0.f, 0.f)) * glm::scale(glm::vec3(0.05f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/OldHouse/house_01.3DS", "Medias/Objects/OldHouse", opt);
				//Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				//Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				//texList.clear();
				//texList.push_back(Renderer::TextureDesc("Medias/Objects/OldHouse/DSC_5871_.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false));
				//texList.push_back(Renderer::TextureDesc("Medias/Objects/OldHouse/DSC_5871_SPEC.tif", TextureCategory::Specular, TextureWrap::Repeat, TextureWrap::Repeat, false));
				//texList.push_back(Renderer::TextureDesc("Medias/Objects/OldHouse/DSC_5871_rough.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false));
				//texList.push_back(Renderer::TextureDesc("Medias/Objects/OldHouse/DSC_5871_NRM.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false));

				//matList.clear();
				//matList.push_back(Renderer::MaterialDesc(glm::vec3(1.f), 0, glm::vec3(1.f), 1, 1.f, 2, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 3));

				position = glm::vec3(0.f, 0.f, 0.f);
				//scale = glm::vec3(0.01f);
			}
#elif BALL_MODEL == 1
			{
				modelData.LoadModel("Medias/Objects/ball/3d.STL", "Medias/Objects/ball", opt.SetFlipWindingOrder(false).SetPreTransformVertices(false));
				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.clear();

				matList.clear();
				// Iron : 0.56,0.57,0.58
				matList.push_back(Renderer::MaterialDesc(glm::vec3(0.f), Renderers::CubeRenderer::NoTexture, glm::vec3(0.56f, 0.57f, 0.58f), Renderers::CubeRenderer::NoTexture, .1f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, Renderers::CubeRenderer::NoTexture));
				// Copper : 0.95f, 0.64f, 0.54f
				matList.push_back(Renderer::MaterialDesc(glm::vec3(0.f), Renderers::CubeRenderer::NoTexture, glm::vec3(0.95f, 0.64f, 0.54f), Renderers::CubeRenderer::NoTexture, .1f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, Renderers::CubeRenderer::NoTexture));
				// Gold : 1.00f, 0.71f, 0.29f
				matList.push_back(Renderer::MaterialDesc(glm::vec3(0.f), Renderers::CubeRenderer::NoTexture, glm::vec3(1.00f, 0.71f, 0.29f), Renderers::CubeRenderer::NoTexture, .1f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, Renderers::CubeRenderer::NoTexture));
				// Aluminum : 0.91,0.92,0.92
				matList.push_back(Renderer::MaterialDesc(glm::vec3(0.f), Renderers::CubeRenderer::NoTexture, glm::vec3(0.91f, 0.92f, 0.92f), Renderers::CubeRenderer::NoTexture, .1f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, Renderers::CubeRenderer::NoTexture));
				// Silver : 0.95,0.93,0.88
				matList.push_back(Renderer::MaterialDesc(glm::vec3(0.f), Renderers::CubeRenderer::NoTexture, glm::vec3(0.95f, 0.93f, 0.88f), Renderers::CubeRenderer::NoTexture, .1f, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, Renderers::CubeRenderer::NoTexture));

				modelData.CopyAndAddModel(0, 1);
				modelData.CopyAndAddModel(0, 2);
				modelData.CopyAndAddModel(0, 3);
				modelData.CopyAndAddModel(0, 4);

				position = glm::vec3(0.f, 0.f, 0.f);
				scale = glm::vec3(0.1f);
				transl = glm::vec3(12.f, 12.f, -12.f);
				count = glm::uvec3(6);
		}
#else
			//modelData.LoadModel("Medias/Objects/planet/planet.obj", "Medias/Objects/planet", opt.SetFlipWindingOrder(false).SetPreTransformVertices(false).SetFlipNormal(false));
			//modelData.LoadModel("Medias/Objects/rock/rock.obj", "Medias/Objects/rock", opt.SetFlipWindingOrder(false).SetPreTransformVertices(true));
			//modelData.LoadModel("Medias/Objects/apple/apple.obj", "Medias/Objects/apple", opt.SetFlipWindingOrder(false).SetPreTransformVertices(true));
			//modelData.LoadModel("Medias/Objects/hatorrihanzo/HattoriHanzo2.0.obj", "Medias/Objects/hatorrihanzo", opt.SetPreTransformVertices(true));
			//modelData.LoadModel("Medias/Objects/Bullet/bullet.obj", "Medias/Objects/Bullet", opt.SetFlipWindingOrder(false).SetPreTransformVertices(true).SetFlipNormal(true));
			//modelData.LoadModel("Medias/Objects/nanosuit/nanosuit.obj", "Medias/Objects/nanosuit", opt.SetFlipWindingOrder(false).SetPreTransformVertices(false).SetFlipNormal(false));
			//modelData.LoadModel("Medias/Objects/Lara_Croft_v1/Lara_Croft_v1.obj", "Medias/Objects/Lara_Croft_v1", opt.SetFlipWindingOrder(false).SetPreTransformVertices(true).SetFlipNormal(true));
			//modelData.LoadModel("Medias/Objects/Guard/boblampclean.md5mesh", "Medias/Objects/Guard", opt.SetFlipWindingOrder(false).SetPreTransformVertices(false));
#endif

			GLuint capacity = count.x * count.y * count.z;
			Renderers::ModelRenderer * modelRenderer = Renderers::ModelRenderer::CreateFromModel(engine, modelData, capacity > 0 ? capacity : 1);
			if (modelRenderer != nullptr)
			{
				glm::vec3 rotAngle(0.f);
				glm::vec3 p(0.f);

				GLuint modelCount = (GLuint)modelData.GetModelMappingList().size();
				for (GLuint i = 0; i < count.x; ++i)
				{
					glm::quat qX = glm::angleAxis(rotAngle.x, XAxis);
					rotAngle.x += rot.x;

					p.y = 0.f;
					for (GLuint j = 0; j < count.y; ++j)
					{
						glm::quat qY = glm::angleAxis(rotAngle.y, YAxis);
						rotAngle.y += rot.y;

						p.z = 0.f;
						for (GLuint k = 0; k < count.z; ++k)
						{
							glm::quat qZ = glm::angleAxis(rotAngle.z, ZAxis);
							rotAngle.z += rot.z;

							glm::quat qRot = qX * qY * qZ * qOrientation;

							Renderables::Model * model = modelRenderer->CreateModelInstance(i % modelCount);
							if (model != nullptr)
							{
								model->GetFrame()->SetPosition(position + p);
								model->GetFrame()->SetRotation(qRot);
								model->GetFrame()->SetScale(scale);
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

	// Setup Lights
		//Lights::SpotLight * spotLight1 = engine->CreateSpotLight(glm::vec3(12.f, 5.f, 5.f), glm::vec3(1.f, 1.f, 1.f), 200.f, glm::normalize(glm::vec3(.2f, .2f, -.5f)), glm::radians(15.f), glm::radians(25.f), 0.9f, 0.1f, .1f);

		Lights::SpotLight * spotLight1 = engine->CreateSpotLight(glm::vec3(65.f, 15.f, -15.f), glm::vec3(1.f, 1.f, 1.f), 20000.f, 100.f, glm::normalize(glm::vec3(.1f, -0.1f, 1.f)), glm::radians(15.f), glm::radians(25.f));
		Lights::SpotLight * spotLight2 = engine->CreateSpotLight(glm::vec3(30.f, 15.f, 5.f), glm::vec3(1.f, 1.f, 1.f), 10000.f, 100.f, glm::normalize(glm::vec3(.5f, -0.5f, 1.f)), glm::radians(15.f), glm::radians(25.f));


		Lights::PointLight * ptLight2 = engine->CreatePointLight(glm::vec3(10.f, 10.f, 20.f), glm::vec3(1.f, 1.f, 1.f), 1000.f, 50.f);
		Lights::PointLight * ptLight3 = engine->CreatePointLight(glm::vec3(300.f, 50.f, 100.f), glm::vec3(1.f, 0.6f, 0.f), 10000.f, 100.f);
		Lights::PointLight * ptLight1 = engine->CreatePointLight(glm::vec3(30.f, 2.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 200.f, 30.f);
		Lights::PointLight * ptLight4 = engine->CreatePointLight(glm::vec3(-200.f, 200.f, -200.f), glm::vec3(1.f, 1.f, 1.f), 250000.f, 250.f);
		Lights::PointLight * ptLight5 = engine->CreatePointLight(glm::vec3(0.f, 30.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 20000.f, 50.f);


		mSunLight = engine->CreateDirectionalLight(glm::normalize(glm::vec3(1.f, -1.f, 0.f)), glm::vec3(1.f, 1.f, 1.f), 100.f);
		//Lights::DirectionalLight * dirLight1 = engine->CreateDirectionalLight(glm::normalize(glm::vec3(0.f, 0.f, 1.f)), glm::vec3(1.f, 1.f, 1.f), 1.8f);

	//setup camera
	mCamera = new Camera();
	//mCamera->LookAt(glm::vec3(10, 6.f, 20.f), glm::vec3(0, 4.f, 0.f), glm::vec3(0, 1, 0));
	mCamera->LookAt(glm::vec3(10, 10.f, 20.f), glm::vec3(10.f, 10.f, 5.f), glm::vec3(0, 1, 0));
	engine->SetCamera(mCamera);

	SetupViewportAndProjection();

	mCamera->Update();

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

void SimpleCamera::OnKeyDown(wchar_t key)
{
	float exposureInc = 0.01f;
	float gammaInc = 0.01f;
	switch (key)
	{
	case L' ':
		mUseFiltering = !mUseFiltering;
		break;

	case L'N':
	case L'n':
		Engine::GetInstance()->EnableDrawVertexNormal(!Engine::GetInstance()->IsDrawVertexNormalEnabled());
		//glutPostRedisplay();
		break;

	case L'B':
	case L'b':
		Engine::GetInstance()->EnableDrawGBufferNormal(!Engine::GetInstance()->IsDrawGBufferNormalEnabled());
		//glutPostRedisplay();
		break;

	case 'e':
		exposureInc = -exposureInc;
	case 'E':
		{
			float exposure = Engine::GetInstance()->GetExposure();
			exposure = glm::clamp(exposure + exposureInc, 0.01f, 10.0f);
			Engine::GetInstance()->SetExposure(exposure);
		}
		break;

	case 'g':
		gammaInc = -gammaInc;
	case 'G':
		{
			float gamma = Engine::GetInstance()->GetGamma();
			gamma = glm::clamp(gamma + gammaInc, 0.01f, 10.0f);
			Engine::GetInstance()->SetGamma(gamma);
		}
		break;

	case 'x':
	case 'X':
		{
			Engine::GetInstance()->SetWireFrame(!Engine::GetInstance()->GetWireFrame());
		}
		break;

	case 'l':
	case 'L':
	{
		Engine::GetInstance()->SetIsDrawLightPositionEnabled(!Engine::GetInstance()->IsDrawLightPositionEnabled());
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
		glm::ivec2 grid = Engine::GetInstance()->GetDrawGBufferNormalGrid();
		grid += glm::ivec2(1);
		grid = glm::clamp(grid, glm::ivec2(5, 5), glm::ivec2(50, 50));
		Engine::GetInstance()->SetDrawGBufferNormalGrid(grid.x, grid.y);

	}
	break;

	case 'V':
	{
		glm::ivec2 grid = Engine::GetInstance()->GetDrawGBufferNormalGrid();
		grid -= glm::ivec2(1);
		grid = glm::clamp(grid, glm::ivec2(5, 5), glm::ivec2(50, 50));
		Engine::GetInstance()->SetDrawGBufferNormalGrid(grid.x, grid.y);
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