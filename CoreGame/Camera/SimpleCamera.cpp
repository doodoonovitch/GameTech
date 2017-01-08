#include "stdafx.h"
#include "CoreGame.h"
#include <iostream>

using namespace CoreFx;


namespace CoreGame
{


	glm::u8vec4 SimpleCamera::LightTextColor(234, 234, 234, 255); // #007F46
	glm::u8vec4 SimpleCamera::DarkTextColor(173, 96, 96, 255);
	glm::u8vec4 SimpleCamera::RedTextColor(198, 31, 25, 255);
	glm::u8vec4 SimpleCamera::BleuTextColor(51, 128, 183, 255);
	glm::u8vec4 SimpleCamera::GreenTextColor(0, 127, 70, 255);
	glm::u8vec4 SimpleCamera::OrangeTextColor(255, 106, 0, 255);

	glm::u8vec4 SimpleCamera::mLabelColor(234, 234, 234, 255); // #007F46
	glm::u8vec4 SimpleCamera::mValueColor(255, 106, 0, 255);

int ToMinutes(int hour, int minute) { return hour * 60 + minute; }

SimpleCamera::CityPositionInfo SimpleCamera::mCityList[(int)SimpleCamera::ECity::__count__] =
{
	{ 37.97945f, 23.71622f,			2016, 12, 7,	ToMinutes(5, 28),	ToMinutes(17, 5),		2, false,	L"Athens"},
	{ 11.5836300f, 165.3761700f,	2016, 12, 7,	ToMinutes(7, 6),	ToMinutes(18, 34),		12, true,	L"BikiniAtoll" },
	{ 35.0210700f, 135.7538500f,	2016, 12, 7,	ToMinutes(6, 52),	ToMinutes(16, 45),		9, true,	L"Kyoto" },
	{ 78.2166667f, 15.6333333f,		2016, 12, 7,	ToMinutes(11, 44),	ToMinutes(12, 41),		1, false,	L"Longyearbyen" },
	{ 64.18347f, -51.72157f,		2016, 12, 7,	ToMinutes(10, 2),	ToMinutes(14, 35),		-3, true,	L"Nuuk" },
	{ 59.9127300f, 10.7460900f,		2016, 12, 7,	ToMinutes(9, 3),	ToMinutes(15, 14),		1, false,	L"Oslo" },
	{ 48.8441311f, 2.3834446f,		2016, 12, 7,	ToMinutes(8, 30),	ToMinutes(16, 54),		1, true,	L"Paris"},
	{ 41.8919300f, 12.5113300f,		2016, 12, 7,	ToMinutes(7, 24),	ToMinutes(16, 39),		1, false,	L"Rome" },
	{ -33.8678500f, 151.2073200f,	2016, 12, 7,	ToMinutes(5, 37),	ToMinutes(19, 56),		11, true,	L"Sydney" },
	{ 1.2896700f, 103.8500700f,		2016, 12, 7,	ToMinutes(6, 55),	ToMinutes(18, 58),		8, false,	L"Singapore" },
	{ -51.700981f, -57.84919f,		2016, 12, 7,	ToMinutes(4, 29),	ToMinutes(20, 58),		-3, false,	L"Stanley" },
};



void SimpleCamera::OnRender()
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CoreFx::Engine* engine = CoreFx::Engine::GetInstance();

	if (!mFreezeTimer)
	{
		engine->SetDeltaTime((float)mDeltaTime);
		engine->SetTime((float)mTime);

		const int textBufferSize = 50;
		static wchar_t textBuffer[textBufferSize];

		auto fps = 1.0 / mDeltaTime;
		if (fps < 30)
			mFpsPage.lock()->UpdateTextColor(mFpsTextLineIndex, RedTextColor, false);
		else if(fps < 60)
			mFpsPage.lock()->UpdateTextColor(mFpsTextLineIndex, BleuTextColor, false);
		else
			mFpsPage.lock()->UpdateTextColor(mFpsTextLineIndex, GreenTextColor, false);

		UpdateValueTextString(mFpsPage, mFpsTextLineIndex, fps, L"%3.0f");
	}

	engine->UpdateObjects();
	engine->RenderObjects();
}

void SimpleCamera::OnShutdown()
{
	CoreFx::Engine::Release();

	PRINT_MESSAGE("Shutdown successfull");
}

void SimpleCamera::OnWindowResize(int /*w*/, int /*h*/)
{
	//mWindowWidth = (GLsizei)w;
	//mWindowHeight = (GLsizei)h;
	SetupViewportAndProjection();
}

void SimpleCamera::SetupViewportAndProjection()
{
	GLsizei w = mGameProgram.GetClientWidth();
	GLsizei h = mGameProgram.GetClientHeight();
	if (mCamera != nullptr)
	{
		assert(mCamera != nullptr);
		mCamera->SetupProjection(45, (GLfloat)w / h, mCamera->GetNearZ(), mCamera->GetFarZ());
	}

	if (mIsInitialized)
	{
		CoreFx::Engine::GetInstance()->SetViewport(0, 0, w, h, w, h);
	}
}

#define TERRAIN_SAMPLE
//#define DEEP_OCEAN_SAMPLE
//#define GERSTNER_WAVE_OCEAN_SAMPLE
//#define PERLIN_NOISE_OCEAN_SAMPLE
#define RADIAL_GRID_OCEAN_SAMPLE
#define SKYDOME_SAMPLE
//#define SKYBOX_SAMPLE
#define COMPASS_SAMPLE
#define MODEL_SAMPLE

#define TEXFOLDER "medias/Textures/"

struct InstancePerModelParams
{
	glm::uvec3 mInstanceCount = glm::uvec3(1, 1, 1);
	glm::vec3 mSpacing = glm::vec3(10.f, 10.f, 10.f);

	InstancePerModelParams()
	{
	}

	InstancePerModelParams(const InstancePerModelParams & src)
		: mInstanceCount(src.mInstanceCount)
		, mSpacing(src.mSpacing)
	{
	}

	InstancePerModelParams(const glm::uvec3 & instanceCount, const glm::vec3 & spacing)
		: mInstanceCount(instanceCount)
		, mSpacing(spacing)
	{
	}

	InstancePerModelParams(const glm::uvec3 & instanceCount)
		: mInstanceCount(instanceCount)
	{
	}
};

void SimpleCamera::OnInit()
{
	const glm::vec3 XAxis(1.f, 0.f, 0.f);
	const glm::vec3 YAxis(0.f, 1.f, 0.f);
	const glm::vec3 ZAxis(0.f, 0.f, 1.f);

	GL_CHECK_ERRORS;

	SetCity(ECity::Paris);

	{
		GLsizei w = (GLsizei)mGameProgram.GetClientWidth();
		GLsizei h = (GLsizei)mGameProgram.GetClientHeight();
		CoreFx::Engine::Initialize(0, 0, w, h, w, h);
	}

		CoreFx::Engine* engine = CoreFx::Engine::GetInstance();

		engine->SetExposure(0.06f);

		mCamera = new CoreFx::Camera();
		mCamera->SetupProjection(45.0f, 1.33333f, 1.f, 5000.f);
		mCamera->LookAt(glm::vec3(10, 10.f, 20.f), glm::vec3(10.f, 10.f, 5.f), glm::vec3(0, 1, 0));
		engine->SetCamera(mCamera);

		SetupViewportAndProjection();

		mCamera->Update();

		mIsInitialized = true;

		InitializeTextRenderer();

		//CoreFx::Renderers::RadialGridRenderer * gridRenderer = new CoreFx::Renderers::RadialGridRenderer(36, 100);
		//engine->AttachRenderer(gridRenderer);

		CoreFx::Renderers::AxisRenderer * axisRenderer = new CoreFx::Renderers::AxisRenderer();
		engine->AttachRenderer(axisRenderer);
		
		//engine->EnableDeferredDebug((Engine::EDeferredDebug)(mShowDeferredBufferState = (GLint)Engine::EDeferredDebug::ShowNormalBuffer));

#ifdef TERRAIN_SAMPLE
		{
			float yScale = 150.f;
			float xScale = 1.f;

			CoreFx::Renderers::TerrainRenderer::Desc desc(512, glm::vec3(xScale, yScale, xScale));

			desc.mMaterials.resize(4);

			Renderer::TextureDescList & texList = desc.mTextures;

#define USE_PBR_TEXTURES
#ifdef USE_PBR_TEXTURES

			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Rock & Stone/cliff-rockface1-color.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat));				// 0
			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Rock & Stone/wornredishroughrockface-color.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat));		// 1
			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Ground/mossy-ground1-albedo.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat));						// 2
			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Rock & Stone/worn-coral-color.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat));					// 3

			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Rock & Stone/cliff-rockface1-roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat));			// 4
			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Rock & Stone/wornredishroughrockface-roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat));	// 5
			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Ground/mossy-ground1-roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat));					// 6
			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Rock & Stone/worn-coral-roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat));				// 7

			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Rock & Stone/cliff-rockface1-height.tif", TextureCategory::HeightMap, TextureWrap::Repeat, TextureWrap::Repeat));			// 8
			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Rock & Stone/wornredishroughrockface-height.tif", TextureCategory::HeightMap, TextureWrap::Repeat, TextureWrap::Repeat));	// 9
			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Ground/mossy-ground1-height.tif", TextureCategory::HeightMap, TextureWrap::Repeat, TextureWrap::Repeat));					// 10
			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Rock & Stone/worn-coral-height.tif", TextureCategory::HeightMap, TextureWrap::Repeat, TextureWrap::Repeat));					// 11

			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Rock & Stone/cliff-rockface1-metallic.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat));			// 12
			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Rock & Stone/wornredishroughrockface-metallic.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat));	// 13
			texList.push_back(Renderer::TextureDesc(TEXFOLDER "Rock & Stone/worn-coral-metallic.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat));				// 14

			desc.mMaterials[0].SetElevation(0.f * yScale, 0.8f * yScale).SetTexScale(0.1f)
				.SetSlope(0.79f, 1.f)
				.SetStrength(0.5f)
				.SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(3)
				.SetMetallic(0.f).SetMetallicTextureIndex(14).SetRoughness(1.f).SetRoughnessTextureIndex(7)
				.SetHeightScale(1.f).SetHeightOffset(0.f).SetHeightTextureIndex(11);

			desc.mMaterials[1].SetElevation(0.01f * yScale, .6f * yScale).SetTexScale(0.1f)
				.SetSlope(0.79f, 1.f)
				.SetStrength(2.f)
				.SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(2)
				.SetMetallic(0).SetRoughness(1.f).SetRoughnessTextureIndex(6)
				.SetHeightScale(1.f).SetHeightOffset(0.f).SetHeightTextureIndex(10);

			desc.mMaterials[2].SetElevation(0.4f * yScale, 1.1f * yScale).SetTexScale(0.1f)
				.SetSlope(0.4f, 1.f)
				.SetStrength(1.f)
				.SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(1)
				.SetMetallic(1.f).SetMetallicTextureIndex(13).SetRoughness(1.f).SetRoughnessTextureIndex(5)
				.SetHeightScale(1.f).SetHeightOffset(0.f).SetHeightTextureIndex(9);

			desc.mMaterials[3].SetElevation(0.f * yScale, 1.1f * yScale).SetTexScale(0.1f)
				.SetSlope(0.f, .8f)
				.SetStrength(2.f)
				.SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(0)
				.SetMetallic(1).SetMetallicTextureIndex(12).SetRoughness(1.f).SetRoughnessTextureIndex(4)
				.SetHeightScale(1.f).SetHeightOffset(0.f).SetHeightTextureIndex(8);

#else //USE_PBR_TEXTURES

			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/diffus_219.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat));	// 0
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/diffus_108.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat));	// 1
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/diffus_104.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat));	// 2
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/diffus_45.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat));	// 3

			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/roughness_219.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat));	// 4
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/roughness_108.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat));	// 5
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/roughness_104.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat));	// 6
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/roughness_45.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat));	// 7

			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/height_219.tif", TextureCategory::HeightMap, TextureWrap::Repeat, TextureWrap::Repeat));		// 8
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/height_108.tif", TextureCategory::HeightMap, TextureWrap::Repeat, TextureWrap::Repeat));		// 9
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/height_104.tif", TextureCategory::HeightMap, TextureWrap::Repeat, TextureWrap::Repeat));		// 10
			desc.mTextures.push_back(Renderer::TextureDesc("medias/TerrainTextures/height_45.tif", TextureCategory::HeightMap, TextureWrap::Repeat, TextureWrap::Repeat));		// 11

			desc.mMaterials[0].SetElevation(0.f * yScale, 0.8f * yScale).SetTexScale(0.1f)
				.SetSlope(0.79f, 1.f)
				.SetStrength(0.5f)
				.SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(3)
				.SetMetallic(0).SetRoughness(.8f).SetRoughnessTextureIndex(7)
				.SetHeightOffset(0.f).SetHeightScale(0.9f).SetHeightTextureIndex(11);

			desc.mMaterials[1].SetElevation(0.01f * yScale, .6f * yScale).SetTexScale(0.1f)
				.SetSlope(0.79f, 1.f)
				.SetStrength(2.f)
				.SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(2)
				.SetMetallic(0).SetRoughness(.0f).SetRoughnessTextureIndex(6)
				.SetHeightOffset(0.2f).SetHeightScale(1.f).SetHeightTextureIndex(10);

			desc.mMaterials[2].SetElevation(0.4f * yScale, 1.1f * yScale).SetTexScale(0.1f)
				.SetSlope(0.4f, 1.f)
				.SetStrength(1.f)
				.SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(1)
				.SetMetallic(0.f).SetRoughness(.8f).SetRoughnessTextureIndex(5)
				.SetHeightOffset(0.2f).SetHeightScale(1.2f).SetHeightTextureIndex(9);

			desc.mMaterials[3].SetElevation(0.f * yScale, 1.1f * yScale).SetTexScale(0.1f)
				.SetSlope(0.f, .8f)
				.SetStrength(2.f)
				.SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(0)
				.SetMetallic(0).SetRoughness(.8f).SetRoughnessTextureIndex(4)
				.SetHeightOffset(0.2f).SetHeightScale(1.3f).SetHeightTextureIndex(8);

#endif // USE_PBR_TEXTURES

			Renderers::TerrainRenderer::MapDesc mapDesc("medias/Terrain/rangitoto-and-motutapu-islands-15m-dem.tif", 512, true, glm::vec3(xScale * -512.f * .5f, -1.f, xScale * -512.f), glm::value_ptr(glm::ivec4(0, 1, 2, 3)));
			desc.mTerrains.push_back(mapDesc);

			Renderers::TerrainRenderer * terrain = new Renderers::TerrainRenderer(desc);
			terrain->SetIsUsedToGenerateEnvMap(true);
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
#ifdef RADIAL_GRID_OCEAN_SAMPLE
		{
			Renderers::PerlinNoiseOcean::RadialGridOceanRenderer::Desc desc;
			desc.SetGrid(240, 600, .1f, 30000.f/*mCamera->GetFarZ()*/);

			mRadialGridOcean = new Renderers::PerlinNoiseOcean::RadialGridOceanRenderer(desc);
			mRadialGridOcean->SetIsUsedToGenerateEnvMap(true);
			engine->AttachRenderer(mRadialGridOcean);

			const wchar_t sOceanTexScale[] = L"Ocean TexScale : ";
			const wchar_t sOceanWaveAmplitude[] = L"Ocean Wave Amplitude : ";

			int col;
			auto page = mWaterRenderPage.lock();

			page->PushBackText(glm::ivec2(mEngineInfoCol, mEngineInfoRow), sOceanTexScale, (GLuint)EFont::Normal, mLabelColor);
			col = mEngineInfoCol + page->MeasureStringInPixel(sOceanTexScale, (GLuint)EFont::Normal);
			mOceanTexScaleTextLineIndex = page->PushBackText(glm::ivec2(col, mEngineInfoRow), L"xx.xx", (GLuint)EFont::NormalBold, mValueColor);
			mEngineInfoRow += mNormalLineHeight + Interline2;
			UpdateValueTextString(mWaterRenderPage, mOceanTexScaleTextLineIndex, mRadialGridOcean->GetTexScale(), OceanTexScaleFmt);

			page->PushBackText(glm::ivec2(mEngineInfoCol, mEngineInfoRow), sOceanWaveAmplitude, (GLuint)EFont::Normal, mLabelColor);
			col = mEngineInfoCol + page->MeasureStringInPixel(sOceanWaveAmplitude, (GLuint)EFont::Normal);
			mOceanTexWaveAmplitudeTextLineIndex = page->PushBackText(glm::ivec2(col, mEngineInfoRow), L"xx.xx", (GLuint)EFont::NormalBold, mValueColor);
			mEngineInfoRow += mNormalLineHeight + Interline2;
			UpdateValueTextString(mWaterRenderPage, mOceanTexWaveAmplitudeTextLineIndex, mRadialGridOcean->GetWaveAmplitude(), OceanWaveAmplitudeFmt);

		}
#endif // RADIAL_GRID_OCEAN_SAMPLE

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

#ifdef SKYBOX_SAMPLE
		{
			Renderers::SkyboxRenderer * skybox = new Renderers::SkyboxRenderer("medias/CubeMaps/Skybox1");
			engine->AttachSkyboxRenderer(skybox);
		}
#endif

#ifdef SKYDOME_SAMPLE
		{
			mSkydome = new Renderers::SkydomeRenderer(glm::vec3(7.0 / 255.0, 136.0 / 255.0, 143.0 / 255.0), true, 256);
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
//#define UFO_MODEL
//#define ASHTRAY_MODEL
//#define CONE_MODEL
#define PIPEBOMB_MODEL
//#define SLINGER_MODEL
//#define ROUGHAXE_MODEL
//#define SHIELD_MODEL
//#define BBUNIT_MODEL
//#define BARREL_MODEL
//#define SPONZA

//#define CUBESPHERE_MODEL
//#define LOW_CUBESPHERE_MODEL
//#define SPHERE_MODEL
//#define SPHERE001_MODEL
//#define SPHERE_300_POLY_MODEL
//#define SPHERE_1250_POLY_MODEL
#define SPHERE_5000_POLY_MODEL
//#define SPHERE_20000_POLY_MODEL
//#define SPHERE_79600_POLY_MODEL
//#define SPHERE_318400_POLY_MODEL
#if defined(CUBESPHERE_MODEL) \
			|| defined(LOW_CUBESPHERE_MODEL) \
			|| defined(SPHERE_MODEL) \
			|| defined(SPHERE001_MODEL) \
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

			std::vector<InstancePerModelParams> instancePerModel;

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

#ifdef SPHERE001_MODEL
				{
					glm::mat4 m = glm::scale(glm::vec3(.3f));
					opt.SetPreTransformVertices(m);
					modelData.LoadModel("Medias/Objects/Sphere/sphere001.obj", "Medias/Objects/Sphere", opt, &dataCtxBase);
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
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(4, 1, dataCtxBase.mModelMappingIndexBase + i)));

				matList.resize(dataCtxBase.mMaterialIndexBase + modelCount);
				
				// Gold
				matList[dataCtxBase.mMaterialIndexBase + 0].SetBaseColor(glm::vec3(0.95f, 0.64f, 0.54f)).SetBaseColorTextureIndex(Renderer::NoTexture).SetMetallic(1.f).SetMetallicTextureIndex(Renderer::NoTexture).SetRoughness(0.05f).SetNormalTextureIndex(Renderer::NoTexture).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);

				// Copper
				matList[dataCtxBase.mMaterialIndexBase + 1].SetBaseColor(glm::vec3(1.00f, 0.71f, 0.29f)).SetBaseColorTextureIndex(Renderer::NoTexture).SetMetallic(1.f).SetMetallicTextureIndex(Renderer::NoTexture).SetRoughness(0.1f).SetNormalTextureIndex(Renderer::NoTexture).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);

				// Silver
				matList[dataCtxBase.mMaterialIndexBase + 2].SetBaseColor(glm::vec3(0.95f, 0.93f, 0.88f)).SetBaseColorTextureIndex(Renderer::NoTexture).SetMetallic(1.f).SetMetallicTextureIndex(Renderer::NoTexture).SetRoughness(0.15f).SetNormalTextureIndex(Renderer::NoTexture).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);

				// Iron
				matList[dataCtxBase.mMaterialIndexBase + 3].SetBaseColor(glm::vec3(0.56f, 0.57f, 0.58f)).SetBaseColorTextureIndex(Renderer::NoTexture).SetMetallic(1.f).SetMetallicTextureIndex(Renderer::NoTexture).SetRoughness(0.2f).SetNormalTextureIndex(Renderer::NoTexture).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);

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
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/ArtoriasSword/Sword_metallic.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/ArtoriasSword/Sword_roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/ArtoriasSword/Sword_normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				const GLuint modelCount = 4;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::ivec3(5, 5, dataCtxBase.mModelMappingIndexBase + i)));

				matList.resize(dataCtxBase.mMaterialIndexBase + modelCount);
				matList[dataCtxBase.mMaterialIndexBase + 0].SetBaseColor(glm::vec3(0.56f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(.5f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				matList[dataCtxBase.mMaterialIndexBase + 1].SetBaseColor(glm::vec3(1.00f, 0.71f, 0.29f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(.5f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				matList[dataCtxBase.mMaterialIndexBase + 2].SetBaseColor(glm::vec3(0.95f, 0.64f, 0.54f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(.5f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				matList[dataCtxBase.mMaterialIndexBase + 3].SetBaseColor(glm::vec3(0.95f, 0.93f, 0.88f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(.5f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);

				for (GLuint i = 1; i < modelCount; ++i)
					modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, i);
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
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/Box/Textures/Met.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/Box/Textures/rough.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/Box/Textures/Nor.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				const GLuint modelCount = 3;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(6, 4, dataCtxBase.mModelMappingIndexBase + i)));

				matList.resize(dataCtxBase.mMaterialIndexBase + modelCount);

				matList[dataCtxBase.mMaterialIndexBase + 0].SetBaseColor(glm::vec3(0.952941f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(.5f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				matList[dataCtxBase.mMaterialIndexBase + 1].SetBaseColor(glm::vec3(0.95f, 0.64f, 0.54f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(.5f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				matList[dataCtxBase.mMaterialIndexBase + 2].SetBaseColor(glm::vec3(0.784314f, 0.f, 0.f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(.5f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);


				//matList[dataCtxBase.mMaterialIndexBase + 2] = Renderer::MaterialDesc(glm::vec3(0.784314f, 0.f, 0.f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.952941f), dataCtxBase.mTextureIndexBase + 1, 1.f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				//matList[dataCtxBase.mMaterialIndexBase + 3] = Renderer::MaterialDesc(glm::vec3(0.f, 0.784314f, 0.f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(0.952941f), dataCtxBase.mTextureIndexBase + 1, 1.f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);
				//matList[dataCtxBase.mMaterialIndexBase + 4] = Renderer::MaterialDesc(glm::vec3(0.4f), dataCtxBase.mTextureIndexBase + 0, glm::vec3(1.00f, 0.71f, 0.29f), dataCtxBase.mTextureIndexBase + 1, 1.f, dataCtxBase.mTextureIndexBase + 2, glm::vec3(0), Renderer::NoTexture, dataCtxBase.mTextureIndexBase + 3);

				for (GLuint i = 1; i < modelCount; ++i)
					modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, i);
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
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(1, 1, dataCtxBase.mModelMappingIndexBase + i)));
			}
#endif
#ifdef LARACROFT_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(2.f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/Lara_Croft_v1/Lara_Croft_v1.obj", "Medias/Objects/Lara_Croft_v1", opt, &dataCtxBase);

				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(1, 1, dataCtxBase.mModelMappingIndexBase + i)));
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
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/Shield/shield_metallic_map.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/Shield/shield_roughness_map.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/Shield/shield_normal_map.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				const GLuint modelCount = 5;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(3, 3, dataCtxBase.mModelMappingIndexBase + i)));

				matList.resize(dataCtxBase.mMaterialIndexBase + modelCount);

				matList[dataCtxBase.mMaterialIndexBase + 0].SetBaseColor(glm::vec3(0.56f, 0.57f, 0.58f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(.5f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				matList[dataCtxBase.mMaterialIndexBase + 1].SetBaseColor(glm::vec3(0.95f, 0.64f, 0.54f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(.5f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				matList[dataCtxBase.mMaterialIndexBase + 2].SetBaseColor(glm::vec3(1.00f, 0.71f, 0.29f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(.5f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				matList[dataCtxBase.mMaterialIndexBase + 3].SetBaseColor(glm::vec3(0.91f, 0.92f, 0.92f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(.5f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				matList[dataCtxBase.mMaterialIndexBase + 4].SetBaseColor(glm::vec3(0.95f, 0.93f, 0.88f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(.5f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);

				for (GLuint i = 1; i < modelCount; ++i)
					modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, i);
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
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/bb-unit/Body_Specular.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/bb-unit/Body_Roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/bb-unit/Body_Normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				texList[dataCtxBase.mTextureIndexBase + 4] = Renderer::TextureDesc("Medias/Objects/bb-unit/Head_Diffuse.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 5] = Renderer::TextureDesc("Medias/Objects/bb-unit/Head_Specular.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 6] = Renderer::TextureDesc("Medias/Objects/bb-unit/Head_Roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 7] = Renderer::TextureDesc("Medias/Objects/bb-unit/Head_Normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				const GLuint modelCount = 3;

				matList.resize(dataCtxBase.mMaterialIndexBase + modelCount * 4);

				matList[dataCtxBase.mMaterialIndexBase + 0].SetBaseColor(glm::vec3(1.f, 0.f, 0.f)).SetBaseColorTextureIndex(Renderer::NoTexture).SetMetallic(1.f).SetMetallicTextureIndex(Renderer::NoTexture).SetRoughness(1.f).SetRoughnessTextureIndex(Renderer::NoTexture).SetNormalTextureIndex(Renderer::NoTexture).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				matList[dataCtxBase.mMaterialIndexBase + 1].SetBaseColor(glm::vec3(0.56f, 0.57f, 0.58f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				matList[dataCtxBase.mMaterialIndexBase + 2].SetBaseColor(glm::vec3(0.56f, 0.57f, 0.58f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 4).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 5).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 6).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 7).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				matList[dataCtxBase.mMaterialIndexBase + 3].SetBaseColor(glm::vec3(0.56f, 0.57f, 0.58f)).SetBaseColorTextureIndex(Renderer::NoTexture).SetMetallic(1.f).SetMetallicTextureIndex(Renderer::NoTexture).SetRoughness(1.f).SetRoughnessTextureIndex(Renderer::NoTexture).SetNormalTextureIndex(Renderer::NoTexture).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);

				if (modelCount >= 2)
				{
					matList[dataCtxBase.mMaterialIndexBase + 4].SetBaseColor(glm::vec3(0.f, 1.f, 0.f)).SetBaseColorTextureIndex(Renderer::NoTexture).SetMetallic(1.f).SetMetallicTextureIndex(Renderer::NoTexture).SetRoughness(1.f).SetRoughnessTextureIndex(Renderer::NoTexture).SetNormalTextureIndex(Renderer::NoTexture).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
					matList[dataCtxBase.mMaterialIndexBase + 5].SetBaseColor(glm::vec3(0.95f, 0.64f, 0.54f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
					matList[dataCtxBase.mMaterialIndexBase + 6].SetBaseColor(glm::vec3(0.95f, 0.64f, 0.54f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 4).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 5).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 6).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 7).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
					matList[dataCtxBase.mMaterialIndexBase + 7].SetBaseColor(glm::vec3(0.95f, 0.64f, 0.54f)).SetBaseColorTextureIndex(Renderer::NoTexture).SetMetallic(1.f).SetMetallicTextureIndex(Renderer::NoTexture).SetRoughness(1.f).SetRoughnessTextureIndex(Renderer::NoTexture).SetNormalTextureIndex(Renderer::NoTexture).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				}

				if (modelCount >= 3)
				{
					matList[dataCtxBase.mMaterialIndexBase + 8].SetBaseColor(glm::vec3(0.f, 0.f, 1.f)).SetBaseColorTextureIndex(Renderer::NoTexture).SetMetallic(1.f).SetMetallicTextureIndex(Renderer::NoTexture).SetRoughness(1.f).SetRoughnessTextureIndex(Renderer::NoTexture).SetNormalTextureIndex(Renderer::NoTexture).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
					matList[dataCtxBase.mMaterialIndexBase + 9].SetBaseColor(glm::vec3(1.00f, 0.71f, 0.29f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
					matList[dataCtxBase.mMaterialIndexBase + 10].SetBaseColor(glm::vec3(1.00f, 0.71f, 0.29f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 4).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 5).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 6).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 7).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
					matList[dataCtxBase.mMaterialIndexBase + 11].SetBaseColor(glm::vec3(1.00f, 0.71f, 0.29f)).SetBaseColorTextureIndex(Renderer::NoTexture).SetMetallic(1.f).SetMetallicTextureIndex(Renderer::NoTexture).SetRoughness(1.f).SetRoughnessTextureIndex(Renderer::NoTexture).SetNormalTextureIndex(Renderer::NoTexture).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
				}
				
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(3, 1, dataCtxBase.mModelMappingIndexBase + i)));

				for (GLuint i = 1; i < modelCount; ++i)
					modelData.CopyAndAddModel(dataCtxBase.mModelMappingIndexBase, i * 4);
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
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/Barrel/Barrel_01_Metallic.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/Barrel/Barrel_01_Roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/Barrel/Barrel_01_Normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(4, 3, dataCtxBase.mModelMappingIndexBase + i)));

				matList.resize(dataCtxBase.mMaterialIndexBase + modelCount);

				matList[dataCtxBase.mMaterialIndexBase + 0].SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);
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
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/Cerberus/Textures/Cerberus_M.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/Cerberus/Textures/Cerberus_R.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/Cerberus/Textures/Cerberus_N.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				matList[dataCtxBase.mMaterialIndexBase + 0].SetBaseColor(glm::vec3(0.56f, 0.57f, 0.58f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);

				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(2, 1, dataCtxBase.mModelMappingIndexBase + i), glm::vec3(5.f, 5.f, 20.f)));
			}
#endif
#ifdef ASHTRAY_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(.01f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/AshtrayPBR/Ashtray.obj", "Medias/Objects/AshtrayPBR/Textures", opt, &dataCtxBase);
				
				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.resize(dataCtxBase.mTextureIndexBase + 4);
				texList[dataCtxBase.mTextureIndexBase + 0] = Renderer::TextureDesc("Medias/Objects/AshtrayPBR/Textures/Aschenbecher_01_Color.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/AshtrayPBR/Textures/Aschenbecher_01_Metallic.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/AshtrayPBR/Textures/Aschenbecher_01_Roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/AshtrayPBR/Textures/Aschenbecher_01_Normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				matList.resize(dataCtxBase.mMaterialIndexBase + 1);

				matList[dataCtxBase.mMaterialIndexBase + 0].SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);			

				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(1, 1, dataCtxBase.mModelMappingIndexBase + i)));
			}
#endif
#ifdef CONE_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(1.f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/ConePBR/Cone_01.obj", "Medias/Objects/ConePBR/Textures", opt, &dataCtxBase);

				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.resize(dataCtxBase.mTextureIndexBase + 4);
				texList[dataCtxBase.mTextureIndexBase + 0] = Renderer::TextureDesc("Medias/Objects/ConePBR/Textures/Cone_01_Color.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/ConePBR/Textures/Cone_01_Metallic.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/ConePBR/Textures/Cone_01_Roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/ConePBR/Textures/Cone_01_Normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				matList.resize(dataCtxBase.mMaterialIndexBase + 1);

				matList[dataCtxBase.mMaterialIndexBase + 0].SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);

				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(40, 40, dataCtxBase.mModelMappingIndexBase + i), glm::vec3(15.f)));
			}
#endif
#ifdef PIPEBOMB_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(1.f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/PipeBombPBR/PipeBomb.obj", "Medias/Objects/PipeBombPBR/Textures", opt, &dataCtxBase);

				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.resize(dataCtxBase.mTextureIndexBase + 4);
				texList[dataCtxBase.mTextureIndexBase + 0] = Renderer::TextureDesc("Medias/Objects/PipeBombPBR/Textures/Pipe_Bomb_01_Color.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/PipeBombPBR/Textures/Pipe_Bomb_01_Metallic.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/PipeBombPBR/Textures/Pipe_Bomb_01_Roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/PipeBombPBR/Textures/Pipe_Bomb_01_Normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				matList.resize(dataCtxBase.mMaterialIndexBase + 1);

				matList[dataCtxBase.mMaterialIndexBase + 0].SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);

				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(1, 1, dataCtxBase.mModelMappingIndexBase + i), glm::vec3(30.f)));
			}
#endif
#ifdef SLINGER_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(1.f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/SMG-60-MinigunPBR/Slinger_00.obj", "Medias/Objects/SMG-60-MinigunPBR/Textures", opt, &dataCtxBase);

				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.resize(dataCtxBase.mTextureIndexBase + 4);
				texList[dataCtxBase.mTextureIndexBase + 0] = Renderer::TextureDesc("Medias/Objects/SMG-60-MinigunPBR/Textures/Slinger_Base_Color.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/SMG-60-MinigunPBR/Textures/Slinger_Base_Metallic.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/SMG-60-MinigunPBR/Textures/Slinger_Base_Roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/SMG-60-MinigunPBR/Textures/Slinger_Base_NormalGL.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				matList.resize(dataCtxBase.mMaterialIndexBase + 1);

				matList[dataCtxBase.mMaterialIndexBase + 0].SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);

				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(40, 40, dataCtxBase.mModelMappingIndexBase + i), glm::vec3(30.f)));
			}
#endif
#ifdef ROUGHAXE_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(1.f));
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/RoughAxePBR/RoughAxe.fbx", "Medias/Objects/RoughAxePBR/Textures", opt, &dataCtxBase);

				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.resize(dataCtxBase.mTextureIndexBase + 8);
				texList[dataCtxBase.mTextureIndexBase + 0] = Renderer::TextureDesc("Medias/Objects/RoughAxePBR/Textures/RoughAxe_RoughWarAxeHandle_BaseColor.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/RoughAxePBR/Textures/RoughAxe_RoughWarAxeHandle_Metallic.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/RoughAxePBR/Textures/RoughAxe_RoughWarAxeHandle_Roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/RoughAxePBR/Textures/RoughAxe_RoughWarAxeHandle_NormalGL.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 4] = Renderer::TextureDesc("Medias/Objects/RoughAxePBR/Textures/RoughAxe_RoughWarAxeHead_BaseColor.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 5] = Renderer::TextureDesc("Medias/Objects/RoughAxePBR/Textures/RoughAxe_RoughWarAxeHead_Metallic.tif", TextureCategory::Metallic, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 6] = Renderer::TextureDesc("Medias/Objects/RoughAxePBR/Textures/RoughAxe_RoughWarAxeHead_Roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 7] = Renderer::TextureDesc("Medias/Objects/RoughAxePBR/Textures/RoughAxe_RoughWarAxeHead_NormalGL.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);

				matList.resize(dataCtxBase.mMaterialIndexBase + 2);

				matList[dataCtxBase.mMaterialIndexBase + 0].SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 4).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 5).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 6).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 7).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);

				matList[dataCtxBase.mMaterialIndexBase + 1].SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 3).SetEmissive(0.f).SetEmissiveTextureIndex(Renderer::NoTexture);

				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(40, 40, dataCtxBase.mModelMappingIndexBase + i), glm::vec3(30.f)));
			}
#endif
#ifdef UFO_MODEL
			{
				glm::mat4 m = glm::scale(glm::vec3(.1f));
				//opt.SetFlipWindingOrder(true);
				opt.SetPreTransformVertices(m);

				modelData.LoadModel("Medias/Objects/Ufo/Low_poly_UFO.obj", "Medias/Objects/Ufo/Textures", opt, &dataCtxBase);

				Renderer::MaterialDescList & matList = modelData.GetMaterialDescList();
				Renderer::TextureDescList & texList = modelData.GetTextureDescList();

				texList.resize(dataCtxBase.mTextureIndexBase + 7);
				texList[dataCtxBase.mTextureIndexBase + 0] = Renderer::TextureDesc("Medias/Objects/Ufo/Textures/ufo_diffuse.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 1] = Renderer::TextureDesc("Medias/Objects/Ufo/Textures/ufo_roughness.tif", TextureCategory::Roughness, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 2] = Renderer::TextureDesc("Medias/Objects/Ufo/Textures/ufo_normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 3] = Renderer::TextureDesc("Medias/Objects/Ufo/Textures/ufo_diffuse_glow.tif", TextureCategory::Emissive, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 4] = Renderer::TextureDesc("Medias/Objects/Ufo/Textures/ufo_diffuse2.tif", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 5] = Renderer::TextureDesc("Medias/Objects/Ufo/Textures/ufo_normal.tif", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat, false);
				texList[dataCtxBase.mTextureIndexBase + 6] = Renderer::TextureDesc("Medias/Objects/Ufo/Textures/ufo_diffuse2_glow.tif", TextureCategory::Emissive, TextureWrap::Repeat, TextureWrap::Repeat, false);

				matList.resize(dataCtxBase.mMaterialIndexBase + 3);

				matList[dataCtxBase.mMaterialIndexBase + 1].SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 0).SetMetallic(1.f).SetMetallicTextureIndex(Renderer::NoTexture).SetRoughness(1.f).SetRoughnessTextureIndex(dataCtxBase.mTextureIndexBase + 1).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 2).SetEmissive(100.f).SetEmissiveTextureIndex(dataCtxBase.mTextureIndexBase + 3);

				matList[dataCtxBase.mMaterialIndexBase + 2].SetBaseColor(glm::vec3(1.f)).SetBaseColorTextureIndex(dataCtxBase.mTextureIndexBase + 4).SetMetallic(0.f).SetMetallicTextureIndex(Renderer::NoTexture).SetRoughness(.5f).SetRoughnessTextureIndex(Renderer::NoTexture).SetNormalTextureIndex(dataCtxBase.mTextureIndexBase + 5).SetEmissive(100.f).SetEmissiveTextureIndex(dataCtxBase.mTextureIndexBase + 6);


				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(5, 1, dataCtxBase.mModelMappingIndexBase + i)));
			}
#endif
#ifdef SPONZA
			{
				glm::mat4 m = glm::scale(glm::vec3(0.5f));
				opt.SetPreTransformVertices(m);

				//modelData.LoadModel("Medias/Objects/sponza/sponza.obj", "Medias/Objects/sponza/textures", opt, &dataCtxBase);
				modelData.LoadModel("Medias/Objects/sponza/sponzaPBR.obj", "Medias/Objects/sponza/textures_pbr", opt, &dataCtxBase);

				const GLuint modelCount = 1;
				for (GLuint i = 0; i < modelCount; ++i)
					instancePerModel.push_back(InstancePerModelParams(glm::uvec3(1, 1, dataCtxBase.mModelMappingIndexBase + i)));
			}
#endif

			GLuint capacity = 0;
			GLuint modelCount = (GLuint)instancePerModel.size();
			for (std::vector<InstancePerModelParams>::const_iterator it = instancePerModel.begin(); it != instancePerModel.end(); ++it)
			{
				capacity += it->mInstanceCount.x * it->mInstanceCount.y;
			}

			Renderers::ModelRenderer * modelRenderer = Renderers::ModelRenderer::CreateFromModel(engine, modelData, capacity > 0 ? capacity : 1);
			if (modelRenderer != nullptr)
			{
				GLuint modelCount = (GLuint)modelData.GetModelMappingList().size();

				glm::vec3 position = glm::vec3(0.f, 10.f, 0.f);

				glm::vec3 rotAngle(0.f);
				glm::vec3 p(0.f);

				float rotX = glm::two_pi<float>() / (float)modelCount;

				for (GLuint i = 0; i < modelCount; ++i)
				{
					const InstancePerModelParams & modelParam = instancePerModel[i];
					const glm::uvec3 & modelInfo = modelParam.mInstanceCount;
					const glm::vec3 & spacing = modelParam.mSpacing;

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

							p.z += spacing.z;
						}

						p.y += spacing.y;
					}

					p.x += spacing.x;
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
		}

	// Setup Lights
		//Lights::SpotLight * spotLight1 = engine->CreateSpotLight(glm::vec3(65.f, 8.f, 15.f), glm::vec3(1.f, 1.f, 1.f), 50000.f, 10000.f, glm::normalize(glm::vec3(.3f, -0.7f, 1.f)), glm::radians(15.f), glm::radians(25.f));
		//Lights::SpotLight * spotLight2 = engine->CreateSpotLight(glm::vec3(15.f, 5.f, 10.f), glm::vec3(1.f, 1.f, 1.f), 10000.f, 100.f, glm::normalize(glm::vec3(.5f, 0.5f, -1.f)), glm::radians(15.f), glm::radians(25.f));


		Lights::PointLight * ptLight2 = engine->CreatePointLight(glm::vec3(-10.f, 10.f, -20.f), glm::vec3(1.f, 1.f, 1.f), 10000.f, 1000.f);
		Lights::PointLight * ptLight3 = engine->CreatePointLight(glm::vec3(300.f, 50.f, 40.f), glm::vec3(1.f, 1.f, 1.f), 100000.f, 50000.f);
		Lights::PointLight * ptLight1 = engine->CreatePointLight(glm::vec3(30.f, 2.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 200.f, 30.f);
		Lights::PointLight * ptLight4 = engine->CreatePointLight(glm::vec3(-200.f, 200.f, -200.f), glm::vec3(1.f, 1.f, 1.f), 250000.f, 250.f);
		Lights::PointLight * ptLight5 = engine->CreatePointLight(glm::vec3(20.f, 30.f, 5.f), glm::vec3(1.f, 1.f, 1.f), 20000.f, 100.f);


		mSunLight = engine->CreateDirectionalLight(glm::normalize(glm::vec3(1.f, -1.f, 0.f)), glm::vec3(1.f, 1.f, 1.f), 100.f);
		//Lights::DirectionalLight * dirLight1 = engine->CreateDirectionalLight(glm::normalize(glm::vec3(0.f, 0.f, 1.f)), glm::vec3(1.f, 1.f, 1.f), 1.8f);

	//setup camera

	UpdateSunPosition();
	UpdateSunPosTextPage();

	engine->CreateDynamicResources();

	PRINT_MESSAGE("Initialization successfull");

	//{
	//	//mCamera->Update();
	//	//const glm::mat4 & viewMat = mCamera->GetViewMatrix();
	//	//const glm::mat4 & invViewMat = mCamera->GetInverseViewMatrix();
	//	//PRINT_MESSAGE("View matrix :");
	//	//PRINT_MATRIX(viewMat);
	//	//PRINT_MESSAGE("Inverse View matrix :");
	//	//PRINT_MATRIX(invViewMat);

	//	const glm::vec3 X(1.f, 0.f, 0.f);
	//	const glm::vec3 Y(0.f, 1.f, 0.f);
	//	const glm::vec3 Z(0.f, 0.f, 1.f);
	//	const glm::vec3 O(0.f, 0.f, 0.f);

	//	glm::vec3 lookAtVectors[6] =
	//	{
	//		X,
	//		-X,
	//		Y, 
	//		-Y,
	//		Z,
	//		-Z
	//	};

	//	glm::vec3 upVectors[6] =
	//	{
	//		-Y, 
	//		Y,
	//		Z,
	//		-Z,
	//		-Y,
	//		-Y
	//	};

	//	Camera cubeMapCam[6];

	//	cubeMapCam[GL_TEXTURE_CUBE_MAP_POSITIVE_X - GL_TEXTURE_CUBE_MAP_POSITIVE_X].LookAt(O, X, -Y);
	//	cubeMapCam[GL_TEXTURE_CUBE_MAP_NEGATIVE_X - GL_TEXTURE_CUBE_MAP_POSITIVE_X].LookAt(O, -X, -Y);
	//	cubeMapCam[GL_TEXTURE_CUBE_MAP_POSITIVE_Y - GL_TEXTURE_CUBE_MAP_POSITIVE_X].LookAt(O, Y, Z);
	//	cubeMapCam[GL_TEXTURE_CUBE_MAP_NEGATIVE_Y - GL_TEXTURE_CUBE_MAP_POSITIVE_X].LookAt(O, -Y, -Z);
	//	cubeMapCam[GL_TEXTURE_CUBE_MAP_POSITIVE_Z - GL_TEXTURE_CUBE_MAP_POSITIVE_X].LookAt(O, Z, -Y);
	//	cubeMapCam[GL_TEXTURE_CUBE_MAP_NEGATIVE_Z - GL_TEXTURE_CUBE_MAP_POSITIVE_X].LookAt(O, -Z, -Y);

	//	glm::mat4 cubeMapMatrix[6], invCubeMapMatrix[6];

	//	glm::vec3 pos(1.f, 2.f, 3.f);

	//	glm::mat4 m = glm::lookAt(pos, glm::vec3(2.5f), upVectors[0]);

	//	for (int j = 0; j < 3; ++j)
	//	{
	//		pos += glm::vec3(1.f, -1.f, 2.f);

	//		PRINT_MESSAGE("");
	//		PRINT_MESSAGE("Origin = (%f, \t%f, \t%f)", pos.x, pos.y, pos.z);

	//		for (int i = 0; i < 6; ++i)
	//		{
	//			cubeMapCam[i].SetPosition(pos);
	//			cubeMapCam[i].Update();
	//			cubeMapMatrix[i] = glm::lookAt(pos, pos + lookAtVectors[i], upVectors[i]);
	//			invCubeMapMatrix[i] = glm::inverse(cubeMapMatrix[i]);

	//			PRINT_MESSAGE("");
	//			PRINT_MESSAGE("Face %i", i);
	//			PRINT_MESSAGE("cubeMapCam[%i] matrix :", i);
	//			PRINT_MATRIX(cubeMapCam[i].GetViewMatrix());
	//			PRINT_MESSAGE("cubeMapMatrix[%i] :", i);
	//			PRINT_MATRIX(cubeMapMatrix[i]);

	//			//PRINT_MESSAGE("cubeMapCam[%i] inverse matrix :", i);
	//			//PRINT_MATRIX(cubeMapCam[i].GetInverseViewMatrix());
	//			//PRINT_MESSAGE("invCubeMapMatrix[%i] :", i);
	//			//PRINT_MATRIX(invCubeMapMatrix[i]);
	//		}
	//	}

	//	//getchar();
	//}


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

	//{
	//	mCamera->Update();
	//	glm::vec4 p(20.f, 30.f, 5.f, 1.f);
	//	const glm::mat4 & P = mCamera->GetProjectionMatrix();
	//	const glm::mat4 & V = mCamera->GetViewMatrix();
	//	const glm::mat4 & VP = mCamera->GetViewProjectionMatrix();

	//	glm::vec4 pView = V * p;
	//	glm::vec4 pProj = VP * p;
	//	glm::vec4 pNDS = pProj / pProj.w;
	//	float linearDepth = CoreFx::Maths::Helpers::LinearizeDepth(pNDS.z, mCamera->GetNearZ(), mCamera->GetFarZ());

	//	PRINT_MESSAGE("pView = (%f, %f, %f, %f)", pView.x, pView.y, pView.z, pView.w);
	//	PRINT_MESSAGE("pProj = (%f, %f, %f, %f)", pProj.x, pProj.y, pProj.z, pProj.w);
	//	PRINT_MESSAGE("pNDS = (%f, %f, %f, %f)", pNDS.x, pNDS.y, pNDS.z, pNDS.w);
	//	PRINT_MESSAGE("linearDepth = %f", linearDepth);		
	//}

}

void SimpleCamera::InitializeTextRenderer()
{
	const char * fonts[] =
	{
		"Medias\\Fonts\\Hack-Regular.ttf",
		"Medias\\Fonts\\Hack-Bold.ttf",
		"Medias\\Fonts\\Hack-BoldItalic.ttf",
		"Medias\\Fonts\\INFROMAN.ttf",
		"Medias\\Fonts\\arial.ttf",
		"Medias\\Fonts\\OCRAEXT.TTF",
	};
	const int FontCount = ARRAY_SIZE_IN_ELEMENTS(fonts);

	const int fontSizes[] =
	{
		18, 36, 60
	};
	const int FontSizeCount = ARRAY_SIZE_IN_ELEMENTS(fontSizes);

	mTextRenderer = new Renderers::TextRenderer();
	Renderers::TextRenderer::Desc desc;
	desc.mPoliceList.resize((size_t)EFont::__fonts_count__);
	desc.mTextureSize = glm::ivec2(512);

	// Normal
	{
		Renderers::TextRenderer::PoliceDesc & p = desc.mPoliceList[(int)EFont::Normal];
		p.mFont = fonts[0];
		p.mCharacterHeight = fontSizes[0];
	}
	// NormalBold
	{
		Renderers::TextRenderer::PoliceDesc & p = desc.mPoliceList[(int)EFont::NormalBold];
		p.mFont = fonts[1];
		p.mCharacterHeight = fontSizes[0];
	}
	// NormalItalic
	{
		Renderers::TextRenderer::PoliceDesc & p = desc.mPoliceList[(int)EFont::NormalItalic];
		p.mFont = fonts[2];
		p.mCharacterHeight = fontSizes[0];
	}
	// Title
	{
		Renderers::TextRenderer::PoliceDesc & p = desc.mPoliceList[(int)EFont::Title];
		p.mFont = fonts[3];
		p.mCharacterHeight = fontSizes[2];
	}
	// Header
	{
		Renderers::TextRenderer::PoliceDesc & p = desc.mPoliceList[(int)EFont::Header];
		p.mFont = fonts[4];
		p.mCharacterHeight = fontSizes[1];
	}


	int fontIndex = 0;
	for (int i = 0; i < (int)EFont::__fonts_count__; ++i)
	{
		Renderers::TextRenderer::PoliceDesc & p = desc.mPoliceList[fontIndex++];
		for (GLuint i = L' '; i <= L'µ'; ++i)
			p.mCharacterSet.push_back(i);
	}

	mTextRenderer->Initialize(desc);

	mDefaultTextGroup = mTextRenderer->GetTextGroup((GLsizei)ETextGroup::Default);
	mShowDeferredBuffersTextGroup = mTextRenderer->NewTextGroup();
	mHelpInfoTextGroup = mTextRenderer->NewTextGroup();

	//mTestPage = mTextRenderer->NewPage(false);
	//mTestPage.lock()->PushBackText(glm::ivec2(300, 10), L"1234567890@ABCDEFGH@ ", (GLuint)EFont::Normal, glm::u8vec4(255, 106, 0, 255));
	//mTestPage.lock()->PushBackText(glm::ivec2(300, 100), L"[page1] Test text rendering...", (GLuint)EFont::NormalBold, glm::u8vec4(255, 106, 0, 255));
	//mTestPage.lock()->PushBackText(glm::ivec2(300, 200), L"[page1] Test 2 - Font 2...", (GLuint)EFont::NormalItalic, glm::u8vec4(0, 127, 70, 255));

	mHelpInfoPage = mTextRenderer->NewPage(true, mHelpInfoTextGroup);
	mFpsPage = mTextRenderer->NewPage(true);
	mFrameInfoPage = mTextRenderer->NewPage(true);
	mSunPosPage = mTextRenderer->NewPage(true);
	mWaterRenderPage = mTextRenderer->NewPage(true);
	mShowDeferredBuffersPage = mTextRenderer->NewPage(true, mShowDeferredBuffersTextGroup);

	CoreFx::Engine::GetInstance()->AttachRenderer(mTextRenderer);

	mLastActiveTextGroup = mTextRenderer->GetActiveTextGroup();

	InitializeTextPages();
}

void SimpleCamera::InitializeTextPages()
{
	const int Column1 = 50;
	const int Column2 = 100;
	const int Column3 = 180;
	const int Column4 = 300;

	const Renderers::TextRenderer::FontInfo & fiNormal = mTextRenderer->GetFontInfo((GLuint)EFont::Normal);
	const Renderers::TextRenderer::FontInfo & fiNormalBold = mTextRenderer->GetFontInfo((GLuint)EFont::NormalBold);
	const Renderers::TextRenderer::FontInfo & fiNormalItalic = mTextRenderer->GetFontInfo((GLuint)EFont::NormalItalic);
	const Renderers::TextRenderer::FontInfo & fiTitle = mTextRenderer->GetFontInfo((GLuint)EFont::Title);
	const Renderers::TextRenderer::FontInfo & fiHeader = mTextRenderer->GetFontInfo((GLuint)EFont::Header);

	mTitleLineHeight = Renderers::TextRenderer::GlyphMetrics::toPixel(fiTitle.GetLineHeight());
	mHeaderLineHeight = Renderers::TextRenderer::GlyphMetrics::toPixel(fiHeader.GetLineHeight());
	mNormalLineHeight = Renderers::TextRenderer::GlyphMetrics::toPixel(fiNormal.GetLineHeight());

	glm::u8vec4 color1 = OrangeTextColor;
	glm::u8vec4 color2 = RedTextColor;
	glm::u8vec4 color3 = GreenTextColor;
	glm::u8vec4 color4 = DarkTextColor;
	
	int row = 0;
	int col = 0;
	// ------------------------------------------------------------------------

	mFpsTextLineIndex = mFpsPage.lock()->PushBackText(glm::ivec2(0, 0), L"xx fps", (GLuint)EFont::NormalItalic, BleuTextColor);

	row = 50;

	{
		const wchar_t sExposure[] = L"Exposure : ";
		const wchar_t sGamma[] = L"Gamma : ";
		const wchar_t sSSAOStatus[] = L"SSAO : ";
		const wchar_t sSSAORadius[] = L"SSAO Radius : ";
		const wchar_t sSSAOKernel[] = L"SSAO Kernel : ";
		const wchar_t sEnvMapGenStatus[] = L"EnvMap generation : ";

		auto frameInfoPage = mFrameInfoPage.lock();

		frameInfoPage->PushBackText(glm::ivec2(mEngineInfoCol, mEngineInfoRow), sExposure, (GLuint)EFont::Normal, mLabelColor);
		col = mEngineInfoCol + frameInfoPage->MeasureStringInPixel(sExposure, (GLuint)EFont::Normal);
		mExposureTextLineIndex = mFrameInfoPage.lock()->PushBackText(glm::ivec2(col, mEngineInfoRow), L"xx.xx", (GLuint)EFont::NormalBold, mValueColor);
		mEngineInfoRow += mNormalLineHeight + Interline2;
		UpdateValueTextString(mFrameInfoPage, mExposureTextLineIndex, Engine::GetInstance()->GetExposure(), L"%2.2f");

		frameInfoPage->PushBackText(glm::ivec2(mEngineInfoCol, mEngineInfoRow), sGamma, (GLuint)EFont::Normal, mLabelColor);
		col = mEngineInfoCol + frameInfoPage->MeasureStringInPixel(sGamma, (GLuint)EFont::Normal);
		mGammaTextLineIndex = mFrameInfoPage.lock()->PushBackText(glm::ivec2(col, mEngineInfoRow), L"xx.xx", (GLuint)EFont::NormalBold, mValueColor);
		mEngineInfoRow += mNormalLineHeight + Interline2;
		UpdateValueTextString(mFrameInfoPage, mGammaTextLineIndex, Engine::GetInstance()->GetGamma(), L"%2.2f");

		frameInfoPage->PushBackText(glm::ivec2(mEngineInfoCol, mEngineInfoRow), sSSAOStatus, (GLuint)EFont::Normal, mLabelColor);
		col = mEngineInfoCol + frameInfoPage->MeasureStringInPixel(sSSAOStatus, (GLuint)EFont::Normal);
		mSSAOStatusIndex = mFrameInfoPage.lock()->PushBackText(glm::ivec2(col, mEngineInfoRow), Engine::GetInstance()->IsSSAOEnable() ? L"enabled" : L"disabled", (GLuint)EFont::NormalBold, mValueColor);
		mEngineInfoRow += mNormalLineHeight + Interline2;

		frameInfoPage->PushBackText(glm::ivec2(mEngineInfoCol, mEngineInfoRow), sSSAORadius, (GLuint)EFont::Normal, mLabelColor);
		col = mEngineInfoCol + frameInfoPage->MeasureStringInPixel(sSSAORadius, (GLuint)EFont::Normal);
		mSSAORadiusTextLineIndex = mFrameInfoPage.lock()->PushBackText(glm::ivec2(col, mEngineInfoRow), L"xx.xx", (GLuint)EFont::NormalBold, mValueColor);
		mEngineInfoRow += mNormalLineHeight + Interline2;
		UpdateValueTextString(mFrameInfoPage, mSSAORadiusTextLineIndex, Engine::GetInstance()->GetSSAORadius(), L"%2.2f");

		frameInfoPage->PushBackText(glm::ivec2(mEngineInfoCol, mEngineInfoRow), sSSAOKernel, (GLuint)EFont::Normal, mLabelColor);
		col = mEngineInfoCol + frameInfoPage->MeasureStringInPixel(sSSAOKernel, (GLuint)EFont::Normal);
		mSSAOKernelTextLineIndex = mFrameInfoPage.lock()->PushBackText(glm::ivec2(col, mEngineInfoRow), L"xxxxx", (GLuint)EFont::NormalBold, mValueColor);
		mEngineInfoRow += mNormalLineHeight + Interline2;
		UpdateValueTextString(mFrameInfoPage, mSSAOKernelTextLineIndex, Engine::GetInstance()->GetSSAOKernelSize(), L"%i");
		
		frameInfoPage->PushBackText(glm::ivec2(mEngineInfoCol, mEngineInfoRow), sEnvMapGenStatus, (GLuint)EFont::Normal, mLabelColor);
		col = mEngineInfoCol + frameInfoPage->MeasureStringInPixel(sEnvMapGenStatus, (GLuint)EFont::Normal);
		mEnvMapGenEnabledIndex = mFrameInfoPage.lock()->PushBackText(glm::ivec2(col, mEngineInfoRow), Engine::GetInstance()->IsEnvMapGenEnabled() ? L"enabled" : L"disabled", (GLuint)EFont::NormalBold, mValueColor);
		mEngineInfoRow += mNormalLineHeight + Interline2;		
	}
	
	// ------------------------------------------------------------------------

	row = 0;

	const wchar_t * Title = L"Commands...";
	int width = Renderers::TextRenderer::GlyphMetrics::toPixel(mHelpInfoPage.lock()->MeasureString(Title, (GLuint)EFont::Title));
	int titleColumn = (CoreFx::Engine::GetInstance()->GetViewPortWidth() - width) / 2;
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(titleColumn, row), Title, (GLuint)EFont::Title, color1);
	row += mTitleLineHeight + Interline0;
	//

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column1, row), L"Main commands", (GLuint)EFont::Header, color2);
	row += mHeaderLineHeight + Interline1;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"Mouse Left Button", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column4, row), L": camera rotation", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"Mouse Right Button", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column4, row), L": camera strafe", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	row += +Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[Z][S]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": move forward/backward", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[Q][D]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": move left/right", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[SHIFT]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": move faster", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[CTRL]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": lock camera pitch rotation", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[E][e]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": +/- exposure", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[G][g]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": +/- gamma", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[G][g]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": +/- gamma", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[+][-]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": +/- move speed", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[*]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": default move speed", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[<][>]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": +/- sun position", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[o][O]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": +/- SSAO radius", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[k][K]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": +/- SSAO kernel size", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[P][p]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": pause", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[ESC]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": quit", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	//

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column1, row), L"Debug-tools commands", (GLuint)EFont::Header, color2);
	row += mHeaderLineHeight + Interline1;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[F2]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": view deferred rendering buffers", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[F3]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": enable / disable SSAO", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[X][x]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": draw wireframes", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[B][b]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": draw normals", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[v][V]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": +/- normals density", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[,][?]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": +/- normal length", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column2, row), L"[L][l]", (GLuint)EFont::NormalBold, color3);
	mHelpInfoPage.lock()->PushBackText(glm::ivec2(Column3, row), L": draw lights position", (GLuint)EFont::Normal, color4);
	row += mNormalLineHeight + Interline2;

	// --------------------------------------------------------------

	row = 0;
	mShowDeferredBuffersPage.lock()->PushBackText(glm::vec2(-0.3f, -0.9f), Renderers::TextPage::ELocationType::RelativeRatio, L"Press F2 to continue...", (GLuint)EFont::Header, DarkTextColor);

	// --------------------------------------------------------------

	row = 0;


}

void SimpleCamera::UpdateSunPosTextPage()
{
	const int Column1 = 50;
	const int Column2 = 100;

	const glm::u8vec4 color1 = DarkTextColor;

	int row = 300;

	const int textBufferSize = 300;
	wchar_t textBuffer[textBufferSize];

	const CityPositionInfo & city = mCityList[(int)mCity];

	auto page = mSunPosPage.lock();

	page->ClearText();

	swprintf_s(textBuffer, textBufferSize, L"City : %s", city.mName.c_str());
	page->PushBackText(glm::ivec2(Column1, row), textBuffer, (GLuint)EFont::NormalBold, color1);
	row += mNormalLineHeight + Interline1;

	swprintf_s(textBuffer, textBufferSize, L"Latitude : %f, Longitude : %f", city.mLatitude, city.mLongitude);
	page->PushBackText(glm::ivec2(Column2, row), textBuffer, (GLuint)EFont::Normal, color1);
	row += mNormalLineHeight + Interline2;

	swprintf_s(textBuffer, textBufferSize, L"Timezone : %i, Day saving : %i", city.mTimeZone, city.mDaySaving);
	page->PushBackText(glm::ivec2(Column2, row), textBuffer, (GLuint)EFont::Normal, color1);
	row += mNormalLineHeight + Interline2;

	swprintf_s(textBuffer, textBufferSize, L"Sunrise : %i:%i, Sunset : %i:%i", (int)(city.mSunrise / 60), (int)(city.mSunrise % 60), (int)(city.mSunset / 60), (int)(city.mSunset % 60));
	page->PushBackText(glm::ivec2(Column2, row), textBuffer, (GLuint)EFont::Normal, color1);
	row += mNormalLineHeight + Interline2;

	swprintf_s(textBuffer, textBufferSize, L"Date (year-month-day): %i-%i-%i", city.mYear, city.mMonth, city.mDay);
	page->PushBackText(glm::ivec2(Column2, row), textBuffer, (GLuint)EFont::Normal, color1);
	row += mNormalLineHeight + Interline2;

	swprintf_s(textBuffer, textBufferSize, L"Time : %i:%i", (int)(mCurrentDayTime / 60), (int)(mCurrentDayTime % 60));
	mTimeTextLineIndex = page->PushBackText(glm::ivec2(Column2, row), textBuffer, (GLuint)EFont::Normal, color1);
	row += mNormalLineHeight + Interline2;
}

void SimpleCamera::UpdateSunPosTextPageTime()
{
	const int textBufferSize = 300;
	wchar_t textBuffer[textBufferSize];

	swprintf_s(textBuffer, textBufferSize, L"Time : %i:%i", (int)(mCurrentDayTime / 60), (int)(mCurrentDayTime % 60));
	mSunPosPage.lock()->UpdateTextString(mTimeTextLineIndex, textBuffer);
}

void SimpleCamera::UpdateShowDeferredBuffersText()
{
	static const wchar_t * sShowDeferredBuffersText[]
	{
		L"Normal Buffer",
		L"Albedo Buffer",
		L"Metallic Buffer",
		L"Roughness Buffer",
		L"Position Buffer",
		L"SSAO Buffer",
		L"Emissive Buffer",
		L"Depth Buffer"
	};

	auto pagePtr = mShowDeferredBuffersPage.lock();
	if (pagePtr->GetTextCount() == 2)
	{
		pagePtr->EraseText(1);
	}

	pagePtr->PushBackText(glm::vec2(-0.2f, 0.92f), Renderers::TextPage::ELocationType::RelativeRatio, sShowDeferredBuffersText[mShowDeferredBufferState-1], (GLuint)EFont::Header, OrangeTextColor);
}

SimpleCamera::SimpleCamera(GameProgram & gameProgram)
	: GameEngine(gameProgram)
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

		//if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000) == 0)
		//{
			mCamera->Yaw(glm::radians(mMouseX));
		//}
		if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) == 0)
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
	GLfloat texScale = 0.5f;
	GLfloat waveAmp = 0.5f;

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
			UpdateValueTextString(mFrameInfoPage, mExposureTextLineIndex, exposure, L"%2.2f");
		}
		break;

	case 'g':
		gammaInc = -gammaInc;
	case 'G':
		{
			float gamma = engine->GetGamma();
			gamma = glm::clamp(gamma + gammaInc, 0.01f, 10.0f);
			engine->SetGamma(gamma);
			UpdateValueTextString(mFrameInfoPage, mGammaTextLineIndex, gamma, L"%2.2f");
		}
		break;

	case 'h':
		texScale = -texScale;
	case 'H':
	{
		GLfloat newValue = mRadialGridOcean->GetTexScale() + texScale;
		mRadialGridOcean->SetTexScale(newValue);
		UpdateValueTextString(mWaterRenderPage, mOceanTexScaleTextLineIndex, mRadialGridOcean->GetTexScale(), OceanTexScaleFmt);
	}
	break;

	case 'j':
		waveAmp = -waveAmp;
	case 'J':
		{
			GLfloat newValue = mRadialGridOcean->GetWaveAmplitude() + waveAmp;
			mRadialGridOcean->SetWaveAmplitude(newValue);
			UpdateValueTextString(mWaterRenderPage, mOceanTexWaveAmplitudeTextLineIndex, mRadialGridOcean->GetWaveAmplitude(), OceanWaveAmplitudeFmt);
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
			UpdateValueTextString(mFrameInfoPage, mSSAORadiusTextLineIndex, engine->GetSSAORadius(), L"%2.2f");
		}
		break;

	case 'K':
	case 'k':
		{
			GLuint inc = key == 'k' ? +1 : -1;
			engine->SetSSAOKernelSize(max(16, engine->GetSSAOKernelSize() + inc));
			UpdateValueTextString(mFrameInfoPage, mSSAOKernelTextLineIndex, engine->GetSSAOKernelSize(), L"%i");
		}
		break;

	case 'p':
	case 'P':
		mFreezeTimer = !mFreezeTimer;
		break;

	case '<':
	case '>':
		{
			const CityPositionInfo & city = mCityList[(int)mCity];

			int inc = key == '<' ? mDayTimeIncMinutes : -mDayTimeIncMinutes;
			mCurrentDayTime += inc;
			if (mCurrentDayTime > city.mSunset)
				mCurrentDayTime = city.mSunset;
			else if (mCurrentDayTime < city.mSunrise)
				mCurrentDayTime = city.mSunrise;

			UpdateSunPosition();
			UpdateSunPosTextPageTime();
	}
		break;

	case 'c':
		SetCity((ECity)(((int)mCity + 1) % (int)ECity::__count__));
		UpdateSunPosition();
		UpdateSunPosTextPage();
		break;
	case 'C':
		SetCity((ECity)(((int)mCity - 1) % (int)ECity::__count__));
		UpdateSunPosition();
		UpdateSunPosTextPage();
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
	bool bWalk = false, bStrafe = false;
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

	if (GetAsyncKeyState(VK_Z) & 0x8000)
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

	static bool wasF1Pressed = false;
	if (GetAsyncKeyState(VK_F1) & 0x8000)
	{
		if (mShowDeferredBufferState == 0)
		{
			if (!wasF1Pressed)
			{
				if (mTextRenderer->IsActiveTextGroup((GLsizei)ETextGroup::HelpInfo))
				{
					mTextRenderer->SetActiveTextGroup(mLastActiveTextGroup);
				}
				else
				{
					mLastActiveTextGroup = mTextRenderer->GetActiveTextGroup();
					mTextRenderer->SetActiveTextGroup((GLsizei)ETextGroup::HelpInfo);
				}
			}
			wasF1Pressed = true;
		}
	}
	else
	{
		wasF1Pressed = false;
	}

	static bool wasF3Pressed = false;
	if (GetAsyncKeyState(VK_F3) & 0x8000)
	{
		if (mShowDeferredBufferState == 0)
		{
			if (!wasF3Pressed)
			{
				Engine::GetInstance()->EnableSSAO(!Engine::GetInstance()->IsSSAOEnable());
				mFrameInfoPage.lock()->UpdateTextString(mSSAOStatusIndex, Engine::GetInstance()->IsSSAOEnable() ? L"enabled" : L"disabled");
			}
			wasF3Pressed = true;
		}
	}
	else
	{
		wasF3Pressed = false;
	}

	static bool wasF2Pressed = false;
	if (GetAsyncKeyState(VK_F2) & 0x8000)
	{
		if (!wasF2Pressed)
		{
			mShowDeferredBufferState = (mShowDeferredBufferState + 1) % (GLint)Engine::EDeferredDebug::__count__;
			if (!Engine::GetInstance()->IsSSAOEnable() && (mShowDeferredBufferState == (GLint)Engine::EDeferredDebug::ShowSSAOBuffer))
			{
				mShowDeferredBufferState = (mShowDeferredBufferState + 1) % (GLint)Engine::EDeferredDebug::__count__;
			}

			if (mShowDeferredBufferState == 0)
			{
				Engine::GetInstance()->DisableDeferredDebug();
				mTextRenderer->SetActiveTextGroup(mLastActiveTextGroup);
			}
			else
			{
				UpdateShowDeferredBuffersText();
				mTextRenderer->SetActiveTextGroup((GLsizei)ETextGroup::ShowDeferredBuffers);
				Engine::GetInstance()->EnableDeferredDebug((Engine::EDeferredDebug)mShowDeferredBufferState);
			}
		}
		wasF2Pressed = true;
	}
	else
		wasF2Pressed = false;

	static bool wasF4Pressed = false;
	if (GetAsyncKeyState(VK_F4) & 0x8000)
	{
		if (mShowDeferredBufferState == 0)
		{
			if (!wasF4Pressed)
			{
				Engine::GetInstance()->EnableEnvMapGen(!Engine::GetInstance()->IsEnvMapGenEnabled());
				mFrameInfoPage.lock()->UpdateTextString(mEnvMapGenEnabledIndex, Engine::GetInstance()->IsEnvMapGenEnabled() ? L"enabled" : L"disabled");
			}
			wasF4Pressed = true;
		}
	}
	else
	{
		wasF4Pressed = false;
	}

	
	if (bWalk)
	{
		mCamera->Walk((float)(dy * mDeltaTime));
	}
	
	if (bStrafe)
	{
		mCamera->Strafe((float)(dx * mDeltaTime));
	}

	//if (bWalk || bStrafe)
	//	glutPostRedisplay();
}


void SimpleCamera::UpdateSunPosition()
{
	const CityPositionInfo & city = mCityList[(int)mCity];

	SYSTEMTIME tm = mSolarPosition.GetTime();

	tm.wHour = mCurrentDayTime / 60;
	tm.wMinute = mCurrentDayTime % 60;

	mSolarPosition.SetTime(tm);

	mSolarPosition.Update();

	glm::vec3 sunDir(glm::normalize(mSolarPosition.GetPosition()));
	if (mSunLight != nullptr)
		mSunLight->SetDirection(sunDir);
	if (mSkydome != nullptr)
		mSkydome->SetSunDirection(sunDir);

}

void SimpleCamera::SetCity(ECity cityIndex)
{
	mCity = cityIndex;

	SYSTEMTIME tm;
	const CityPositionInfo & city = mCityList[(int)mCity];
	mCurrentDayTime = city.mSunrise;
	tm.wYear = city.mYear; tm.wMonth = city.mMonth; tm.wDay = city.mDay;
	tm.wHour = city.mSunrise;
	tm.wMilliseconds = 0; tm.wSecond = 0; tm.wMinute = 0;

	mSolarPosition.SetLocation(city.mLatitude, city.mLongitude);
	mSolarPosition.SetTimeZone(city.mTimeZone, city.mDaySaving);
	mSolarPosition.SetTime(tm);

	PRINT_MESSAGE("============================================================");
	PRINT_MESSAGE("Position : %S", city.mName.c_str());
	PRINT_MESSAGE("\t-Latitude : %f, Longitude : %f", city.mLatitude, city.mLongitude);
	PRINT_MESSAGE("\t-Date (year-month-day): %i-%i-%i, Time : %i:%i", city.mYear, city.mMonth, city.mDay, (int)(mCurrentDayTime / 60), (int)(mCurrentDayTime % 60));
	PRINT_MESSAGE("\t-Timezone : %i, Day saving : %i", city.mTimeZone, city.mDaySaving);
	PRINT_MESSAGE("\t-Sunrise : %i:%i, Sunset : %i:%i", (int)(city.mSunrise / 60), (int)(city.mSunrise % 60), (int)(city.mSunset / 60), (int)(city.mSunset % 60));
	PRINT_MESSAGE("============================================================");

}


} // namespace CoreGame