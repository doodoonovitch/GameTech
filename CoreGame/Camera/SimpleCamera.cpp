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
		mCamera->SetupProjection(45, (GLfloat)mWindowWidth / mWindowHeight);
	}

	Engine::GetInstance()->SetViewport(0, 0, mWindowWidth, mWindowHeight, mWindowWidth, mWindowHeight);
}

void SimpleCamera::OnInit()
{
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

		int xCount = 30;
		int yCount = 30;
		int zCount = 30;
		const int materialCount = 8;
		Renderers::CubeRenderer * cubeRenderer = new Renderers::CubeRenderer(materialCount, xCount * yCount * zCount, 1);
		engine->AttachRenderer(cubeRenderer);

		cubeRenderer->AddTexture("medias/cube.ktx", TextureCategory::Ambient, TextureWrap::Clamp, TextureWrap::Clamp);
		cubeRenderer->AddTexture("medias/cube2.ktx", TextureCategory::Ambient, TextureWrap::Clamp, TextureWrap::Clamp);
		cubeRenderer->AddTexture("medias/cube_N.ktx", TextureCategory::NormalMap, TextureWrap::Clamp, TextureWrap::Clamp);
		cubeRenderer->AddTexture("medias/fallforestrocks01.ktx", TextureCategory::Ambient, TextureWrap::Clamp, TextureWrap::Clamp);
		cubeRenderer->AddTexture("medias/fallforestrocks01_n.ktx", TextureCategory::NormalMap, TextureWrap::Clamp, TextureWrap::Clamp);
		cubeRenderer->AddTexture("medias/bricks.ktx", TextureCategory::Ambient, TextureWrap::Clamp, TextureWrap::Clamp);
		cubeRenderer->AddTexture("medias/bricks_n.ktx", TextureCategory::NormalMap, TextureWrap::Clamp, TextureWrap::Clamp);
		cubeRenderer->AddTexture("medias/bricks1024.ktx", TextureCategory::Ambient, TextureWrap::Clamp, TextureWrap::Clamp);
		cubeRenderer->AddTexture("medias/bricks1024_s.ktx", TextureCategory::Specular, TextureWrap::Clamp, TextureWrap::Clamp);
		cubeRenderer->AddTexture("medias/bricks1024_n.ktx", TextureCategory::NormalMap, TextureWrap::Clamp, TextureWrap::Clamp);

		{
			Renderers::CubeRenderer::MaterialDesc mats[materialCount] =
			{
				Renderers::CubeRenderer::MaterialDesc(glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.f, 1.f, 1.f), 8, 0, Renderers::CubeRenderer::NoTexture, Renderers::CubeRenderer::NoTexture, 2),
				Renderers::CubeRenderer::MaterialDesc(glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 1.f), 32, 1, Renderers::CubeRenderer::NoTexture, Renderers::CubeRenderer::NoTexture, 2),

				Renderers::CubeRenderer::MaterialDesc(glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.4f, 0.4f, 0.4f), 64, 3, Renderers::CubeRenderer::NoTexture, Renderers::CubeRenderer::NoTexture, 4),
				Renderers::CubeRenderer::MaterialDesc(glm::vec3(.5f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 1.f), 64, 5, Renderers::CubeRenderer::NoTexture, Renderers::CubeRenderer::NoTexture, 6),

				Renderers::CubeRenderer::MaterialDesc(glm::vec3(.5f, .3f, .1f), glm::vec3(.5f, .5f, .5f), glm::vec3(.5f, .5f, .5f), 128, 7, Renderers::CubeRenderer::NoTexture, 8, 9),
				Renderers::CubeRenderer::MaterialDesc(glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 64, 0, Renderers::CubeRenderer::NoTexture, Renderers::CubeRenderer::NoTexture, 2),

				Renderers::CubeRenderer::MaterialDesc(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f, 1.f, 1.f), 128, 1, Renderers::CubeRenderer::NoTexture, Renderers::CubeRenderer::NoTexture, 2),
				Renderers::CubeRenderer::MaterialDesc(glm::vec3(1.f, 1.f, 0.f), glm::vec3(1.f, 1.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 256, 0, Renderers::CubeRenderer::NoTexture, Renderers::CubeRenderer::NoTexture, 2)
			};

			for (int i = 0; i < materialCount; ++i)
			{
				cubeRenderer->SetMaterial(i, mats[i]);
			}
		}


		const glm::vec3 XAxis(1.f, 0.f, 0.f);
		const glm::vec3 YAxis(0.f, 1.f, 0.f);
		const glm::vec3 ZAxis(0.f, 0.f, 1.f);

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
					cube->GetFrame()->SetPosition(glm::vec3(1.5f + 5.f * i, 1.f + 5.f * k, -1.5f + -5.f * j));

					glm::quat qRot = qX * qY * qZ;
					cube->GetFrame()->SetRotation(qRot);
				}
			}
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
		Lights::PointLight * ptLight1 = engine->CreatePointLight(glm::vec3(15.f, 2.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 10.f, 20.f, 10.f, 1.f, 0.14f, 0.07f);
		Lights::DirectionalLight * dirLight1 = engine->CreateDirectionalLight(glm::normalize(glm::vec3(0.2f, -1.f, 0.f)), glm::vec3(0.2f, 0.2f, 0.2f), 0.2f, 0.2f, 0.2f);
		Lights::PointLight * ptLight2 = engine->CreatePointLight(glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 5.f, 20.f, 20.f, 1.f, 0.7f, 1.8f);
		Lights::PointLight * ptLight3 = engine->CreatePointLight(glm::vec3(20.f, 7.f, 8.f), glm::vec3(1.f, 1.f, 1.f), 1.f, 1.f, 1.f, 1.f, 0.7f, 0.02f);

	//setup camera
	mCamera = new Camera();
	mCamera->LookAt(glm::vec3(10, 6.f, 12.f), glm::vec3(0, 4.f, 0.f), glm::vec3(0, 1, 0));
	engine->SetCamera(mCamera);

	SetupViewportAndProjection();

	engine->CreateDynamicResources();

	cout << "Initialization successfull" << endl;
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
	float dx = 0, dy = 0;


	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		PostExitGame(0);
		return;
	}

	if (GetAsyncKeyState(VK_Z) & 0x8000)
	{
		dy += (MOVE_SPEED);
		bWalk = true;
	}

	if (GetAsyncKeyState(VK_S) & 0x8000)
	{
		dy -= (MOVE_SPEED);
		bWalk = true;
	}

	if (GetAsyncKeyState(VK_Q) & 0x8000)
	{
		dx -= (MOVE_SPEED);
		bStrafe = true;
	}

	if (GetAsyncKeyState(VK_D) & 0x8000)
	{
		dx += (MOVE_SPEED);
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