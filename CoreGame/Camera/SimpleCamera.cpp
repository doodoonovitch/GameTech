#include "stdafx.h"
#include "CoreGame.h"
#include <iostream>

using namespace std;
using namespace CoreFx;
using namespace Renderables;


namespace CoreGame
{




void SimpleCamera::OnRender(double elapsedTime)
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mDeltaTime = (float)elapsedTime;

	Engine* engine = Engine::GetInstance();

	engine->UpdateObjects();
	engine->RenderObjects();
}

void SimpleCamera::OnShutdown()
{
	Engine::Release();

	cout << "Shutdown successfull" << endl;
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

		//Renderers::GridRenderer * gridRenderer = new Renderers::GridRenderer(50, 50);
		//engine->AttachRenderer(gridRenderer);

		Renderers::AxisRenderer * axisRenderer = new Renderers::AxisRenderer();
		engine->AttachRenderer(axisRenderer);

		{
			//const float XStart = 1.f;
			//const float YStart = 0.f;
			//const float ZStart = 1.f;
			//const float XMul = 5.f;
			//const float YMul = 5.f;
			//const float ZMul = -200.f;

			const float XStart = 1.5f;
			const float YStart = 1.0f;
			const float ZStart = -1.5f;
			const float XMul = 5.f;
			const float YMul = 5.f;
			const float ZMul = -5.f;

			int xCount = 30;
			int yCount = 30;
			int zCount = 30;

			Renderers::CubeRenderer::Desc desc;
			desc.mTextures.push_back(Renderer::TextureDesc("medias/cube.ktx", TextureCategory::Diffuse, TextureWrap::Clamp, TextureWrap::Clamp)); // 0
			desc.mTextures.push_back(Renderer::TextureDesc("medias/cube2.ktx", TextureCategory::Diffuse, TextureWrap::Clamp, TextureWrap::Clamp)); // 1
			desc.mTextures.push_back(Renderer::TextureDesc("medias/cube_N.ktx", TextureCategory::NormalMap, TextureWrap::Clamp, TextureWrap::Clamp)); // 2
			desc.mTextures.push_back(Renderer::TextureDesc("medias/fallforestrocks01.ktx", TextureCategory::Diffuse, TextureWrap::Clamp, TextureWrap::Clamp)); // 3
			desc.mTextures.push_back(Renderer::TextureDesc("medias/fallforestrocks01_n.ktx", TextureCategory::NormalMap, TextureWrap::Clamp, TextureWrap::Clamp)); // 4
			desc.mTextures.push_back(Renderer::TextureDesc("medias/bricks.ktx", TextureCategory::Diffuse, TextureWrap::Clamp, TextureWrap::Clamp)); // 5
			desc.mTextures.push_back(Renderer::TextureDesc("medias/bricks_n.ktx", TextureCategory::NormalMap, TextureWrap::Clamp, TextureWrap::Clamp)); // 6
			desc.mTextures.push_back(Renderer::TextureDesc("medias/bricks1024.ktx", TextureCategory::Diffuse, TextureWrap::Clamp, TextureWrap::Clamp)); // 7
			desc.mTextures.push_back(Renderer::TextureDesc("medias/bricks1024_s.ktx", TextureCategory::Specular, TextureWrap::Clamp, TextureWrap::Clamp)); // 8
			desc.mTextures.push_back(Renderer::TextureDesc("medias/bricks1024_n.ktx", TextureCategory::NormalMap, TextureWrap::Clamp, TextureWrap::Clamp)); // 9

			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(0.7f, .7f, .7f), 3, glm::vec3(1.f, 1.f, 1.f), 7, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 4));

			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(0.8f, 0.8f, 0.8f), 0, glm::vec3(1.f, 1.f, 1.f), 6, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 2));

			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(0.7f, .7f, .7f), 3, glm::vec3(1.f, 1.f, 1.f), 7, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 4));

			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(.5f, .3f, .1f), 5, glm::vec3(1.f, .6f, .2f), 8, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 6));


			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(.5f, .5f, .5f), 7, glm::vec3(.5f, .5f, .5f), 0, Renderers::CubeRenderer::NoTexture, glm::vec3(0.f, 8600.f, 5000.f), 8, 9));
			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(.5f, .5f, .5f), 7, glm::vec3(.5f, .5f, .5f), 7, 8, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 9));


			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(0.8f, 0.8f, 0.8f), 1, glm::vec3(1.f, 1.f, 1.f), 4, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 2));

			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(1.f, 0.f, 0.f), 0, glm::vec3(1.f, 1.f, 1.f), 8, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 2));

			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(0.f, 0.f, 1.f), 1, glm::vec3(1.f, 1.f, 1.f), 7, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 2));
			desc.mMaterials.push_back(Renderer::MaterialDesc(glm::vec3(1.f, 1.f, 0.f), 0, glm::vec3(1.f, 1.f, 1.f), 8, Renderers::CubeRenderer::NoTexture, glm::vec3(0), Renderers::CubeRenderer::NoTexture, 2));

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

		{
			float yScale = 100.f;

			Renderers::TerrainRenderer::Desc desc(512, 512, glm::vec3(1.f, yScale, 1.f));
			
			desc.mTerrains.push_back(Renderers::TerrainRenderer::MapDesc("medias/Terrain/Canyon_513x513.r32", 513, true, glm::vec3(0.f), glm::angleAxis(glm::radians(0.f), YAxis)));
			desc.mTerrains.push_back(Renderers::TerrainRenderer::MapDesc("medias/Terrain/SweetCanyon_513x513.r32", 513, true, glm::vec3(-512.f, 0.f, 0.f), glm::angleAxis(glm::radians(0.f), YAxis)));
			//desc.mTerrains.push_back(Renderers::TerrainRenderer::MapDesc("medias/Terrain/Canyon_513x513.r32", 513, true, glm::vec3(0.f), glm::angleAxis(glm::radians(180.f), YAxis)));
			//desc.mTerrains.push_back(Renderers::TerrainRenderer::MapDesc("medias/alps-valley-height-2048.raw", 2048, true, glm::vec3(0.f), glm::angleAxis(glm::radians(180.f), YAxis)));
			desc.mTerrains.push_back(Renderers::TerrainRenderer::MapDesc("medias/alps-valley-height-2048.raw", 2048, true, glm::vec3(-512.f, 0.f, -512.f), glm::angleAxis(glm::radians(0.f), YAxis)));

			desc.mTextures.push_back(Renderer::TextureDesc("medias/fallforestrocks01.ktx", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat));
			desc.mTextures.push_back(Renderer::TextureDesc("medias/snowstone01.ktx", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat));
			desc.mTextures.push_back(Renderer::TextureDesc("medias/pineforest03.ktx", TextureCategory::Diffuse, TextureWrap::Repeat, TextureWrap::Repeat));

			desc.mTextures.push_back(Renderer::TextureDesc("medias/fallforestrocks01_n.ktx", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat));
			desc.mTextures.push_back(Renderer::TextureDesc("medias/snowstone01_n.ktx", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat));
			desc.mTextures.push_back(Renderer::TextureDesc("medias/pineforest03_n.ktx", TextureCategory::NormalMap, TextureWrap::Repeat, TextureWrap::Repeat));

			desc.mLowSlopeMaterials.push_back(Renderers::TerrainRenderer::MaterialDesc(0.f * yScale, 0.95f * yScale, glm::vec3(1.f), 2, glm::vec3(1.f), 1, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture, 0.08f));
			desc.mLowSlopeMaterials.push_back(Renderers::TerrainRenderer::MaterialDesc(0.91f * yScale, 1.f * yScale, glm::vec3(1.f), 1, glm::vec3(1.f), 7, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture, 0.1f));
			desc.mHighSlopeMaterials.push_back(Renderers::TerrainRenderer::MaterialDesc(0.f * yScale, 0.95f * yScale, glm::vec3(1.f), 0, glm::vec3(1.f), 1, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture, 0.3f));
			desc.mHighSlopeMaterials.push_back(Renderers::TerrainRenderer::MaterialDesc(0.91f * yScale, 1.f * yScale, glm::vec3(1.f), 1, glm::vec3(1.f), 7, Renderer::NoTexture, glm::vec3(0), Renderer::NoTexture, Renderer::NoTexture, 0.1f));

			Renderers::TerrainRenderer * terrain = new Renderers::TerrainRenderer(desc);
			engine->AttachRenderer(terrain);
		}

		/*
		std::vector<Geometry::MeshData*> meshDataList;
		int loadFailedCount = 0;
		Geometry::MeshData::Load("Medias/test/Girl.xml", meshDataList, loadFailedCount);
		if (!meshDataList.empty())
		{
			Renderers::MeshRenderer * meshRenderer = new Renderers::MeshRenderer(*meshDataList.front());
			engine->AttachRenderer(meshRenderer);
		}
		for (auto it : meshDataList)
		{
			delete it;
		}
		meshDataList.clear();
		*/

	// Setup Lights
		Lights::SpotLight * spotLight1 = engine->CreateSpotLight(glm::vec3(12.f, 5.f, 5.f), glm::vec3(1.f, 1.f, 1.f), 200.f, glm::normalize(glm::vec3(.2f, .2f, -.5f)), glm::radians(15.f), glm::radians(25.f), 0.9f, 0.1f, .1f);
		Lights::SpotLight * spotLight2 = engine->CreateSpotLight(glm::vec3(50.f, 50.f, 50.f), glm::vec3(1.f, 1.f, 1.f), 200.f, glm::normalize(glm::vec3(-0.2f, -1.f, 0.f)), glm::radians(15.f), glm::radians(25.f), 0.9f, 0.1f, .1f);
		Lights::DirectionalLight * dirLight1 = engine->CreateDirectionalLight(glm::normalize(glm::vec3(0.2f, -1.f, 0.f)), glm::vec3(1.f, 1.f, 1.f), 0.8f);
		Lights::PointLight * ptLight2 = engine->CreatePointLight(glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 20.f, 1.f, 0.7f, 1.8f);
		Lights::PointLight * ptLight3 = engine->CreatePointLight(glm::vec3(20.f, 7.f, 8.f), glm::vec3(1.f, 1.f, 1.f), 1.f, 1.f, 0.7f, 0.02f);
		Lights::PointLight * ptLight1 = engine->CreatePointLight(glm::vec3(30.f, 2.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 20.f, 1.f, 0.14f, 0.07f);

	//setup camera
	mCamera = new Camera();
	mCamera->LookAt(glm::vec3(10, 6.f, 12.f), glm::vec3(0, 4.f, 0.f), glm::vec3(0, 1, 0));
	engine->SetCamera(mCamera);

	SetupViewportAndProjection();

	mCamera->Update();

	engine->CreateDynamicResources();

	cout << "Initialization successfull" << endl;


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

void SimpleCamera::OnUpdate(double elapsedTime)
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

	if (GetAsyncKeyState(VK_Q) & 0x8000)
	{
		dx -= (speed);
		bStrafe = true;
	}

	if (GetAsyncKeyState(VK_D) & 0x8000)
	{
		dx += (speed);
		bStrafe = true;
	}

	if (bWalk)
	{
		mCamera->Walk(dy * mDeltaTime);
	}
	
	if (bStrafe)
	{
		mCamera->Strafe(dx * mDeltaTime);
	}

	//if (bWalk || bStrafe)
	//	glutPostRedisplay();
}


} // namespace CoreGame