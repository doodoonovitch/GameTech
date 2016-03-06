#include "stdafx.h"
#include "SimpleCamera.h"
#include <iostream>

using namespace std;
using namespace CoreFx;
using namespace Renderables;


namespace CoreGame
{



void SimpleCamera::OnRender(GLFWwindow * /*window*/, double deltaTime)
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Engine* engine = Engine::GetInstance();

	engine->UpdateObjects();
	engine->RenderObjects();
}

void SimpleCamera::OnShutdown()
{
	Engine::Release();

	cout << "Shutdown successfull" << endl;
}


void SimpleCamera::OnInit()
{
	//GL_CHECK_ERRORS

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		//glFrontFace(GL_CCW);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		GL_CHECK_ERRORS

		GLint viewportDims[4];
		glGetIntegerv(GL_VIEWPORT, viewportDims);
		printf("Viewport : %i, %i, %i x %i.\n", viewportDims[0], viewportDims[1], viewportDims[2], viewportDims[3]);

		Engine::Initialize();

		Engine* engine = Engine::GetInstance();

		Renderers::GridRenderer * gridRenderer = new Renderers::GridRenderer(50, 50);
		engine->AttachRenderer(gridRenderer);

		Renderers::AxisRenderer * axisRenderer = new Renderers::AxisRenderer();
		engine->AttachRenderer(axisRenderer);

		int xCount = 30;
		int yCount = 30;
		int zCount = 30;
		Renderers::CubeRenderer * cubeRenderer = new Renderers::CubeRenderer("medias/cube.ktx", 5, xCount * yCount * zCount, 1);
		engine->AttachRenderer(cubeRenderer);

		cubeRenderer->SetMaterial(0, glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.f, 1.f, 1.f), 8, 0, -1, -1, -1);
		cubeRenderer->SetMaterial(1, glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 1.f), 32, 0, -1, -1, -1);
		cubeRenderer->SetMaterial(2, glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 64, 0, -1, -1, -1);
		cubeRenderer->SetMaterial(3, glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f, 1.f, 1.f), 128, 0, -1, -1, -1);
		cubeRenderer->SetMaterial(4, glm::vec3(1.f, 1.f, 0.f), glm::vec3(1.f, 1.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 256, 0, -1, -1, -1);


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

					Cube * cube = cubeRenderer->CreateCube(i % 5);
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
		Lights::PointLight * ptLight1 = engine->CreatePointLight(glm::vec3(15.f, 2.f, 0.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 1.f, 0.14f, 0.07f);
		Lights::DirectionalLight * dirLight1 = engine->CreateDirectionalLight(glm::normalize(glm::vec3(0.2f, -1.f, 0.f)), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.2f, 0.2f, 0.2f));
		Lights::PointLight * ptLight2 = engine->CreatePointLight(glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 0.f), glm::vec3(1.f, 1.f, 1.f), 1.f, 0.7f, 1.8f);

	//setup camera
	mCamera = new Camera();
	mCamera->LookAt(glm::vec3(10, 6.f, 12.f), glm::vec3(0, 4.f, 0.f), glm::vec3(0, 1, 0));
	engine->SetCamera(mCamera);


	cout << "Initialization successfull" << endl;
}

SimpleCamera::SimpleCamera()
	: mCamera(nullptr)
{
}

SimpleCamera::~SimpleCamera()
{
	mCamera = nullptr;
}



void SimpleCamera::filterMouseMoves(float dx, float dy)
{
	for (int i = MOUSE_HISTORY_BUFFER_SIZE - 1; i > 0; --i)
	{
		mMouseHistory[i] = mMouseHistory[i - 1];
	}

	// Store current mouse entry at front of array.
	mMouseHistory[0] = glm::vec2(dx, dy);

	float averageX = 0.0f;
	float averageY = 0.0f;
	float averageTotal = 0.0f;
	float currentWeight = 1.0f;

	// Filter the mouse.
	for (int i = 0; i < MOUSE_HISTORY_BUFFER_SIZE; ++i)
	{
		glm::vec2 tmp = mMouseHistory[i];
		averageX += tmp.x * currentWeight;
		averageY += tmp.y * currentWeight;
		averageTotal += 1.0f * currentWeight;
		currentWeight *= MOUSE_FILTER_WEIGHT;
	}

	mMouseX = averageX / averageTotal;
	mMouseY = averageY / averageTotal;

}

void SimpleCamera::OnUpdate(GLFWwindow * window, double deltaTime)
{
	bool bWalk = false, bStrafe = false;
	float dx = 0, dy = 0;


	if (IsKeyPressed(window, GLFW_KEY_Z))
	{
		dy += (MOVE_SPEED);
		bWalk = true;
	}

	if (IsKeyPressed(window, GLFW_KEY_S))
	{
		dy -= (MOVE_SPEED);
		bWalk = true;
	}

	if (IsKeyPressed(window, GLFW_KEY_Q))
	{
		dx -= (MOVE_SPEED);
		bStrafe = true;
	}

	if (IsKeyPressed(window, GLFW_KEY_D))
	{
		dx += (MOVE_SPEED);
		bStrafe = true;
	}

	if (bWalk)
	{
		mCamera->Walk(dy * (float)deltaTime);
	}
	
	if (bStrafe)
	{
		mCamera->Strafe(dx * (float)deltaTime);
	}

	//if (bWalk || bStrafe)
	//	glutPostRedisplay();
}

void SimpleCamera::OnWindowPos(GLFWwindow* /*window*/, int /*x*/, int /*y*/)
{

}

void SimpleCamera::OnWindowSize(GLFWwindow* window, int width, int height)
{

}
void SimpleCamera::OnFramebufferSize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	mCamera->SetupProjection(45, (GLfloat)width / (GLfloat)height);
}

void SimpleCamera::OnWindowClose(GLFWwindow* window)
{
	glfwSetWindowShouldClose(window, true);
}

void SimpleCamera::OnWindowRefresh(GLFWwindow* window)
{
	//glfwMakeContextCurrent(window);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glfwSwapBuffers(window);
}

void SimpleCamera::OnWindowFocus(GLFWwindow* /*window*/, int /*focused*/)
{

}

void SimpleCamera::OnWindowIconify(GLFWwindow* /*window*/, int iconified)
{
	mPause = iconified != GL_FALSE;
}

void SimpleCamera::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		mOldX = (float)x;
		mOldY = (float)y;
	}

	switch (button)
	{
	case GLFW_MOUSE_BUTTON_MIDDLE:
		mState = 0;
		break;

	case GLFW_MOUSE_BUTTON_RIGHT:
		mState = 2;
		break;

	case GLFW_MOUSE_BUTTON_LEFT:
		mState = 1;
		break;
	}
}

void SimpleCamera::OnCursorPosition(GLFWwindow * window, double xx, double yy)
{
	float x = (float)xx;
	float y = (float)yy;

	bool postRedisplay = false;
	if (mState == 0)
	{
		mDist = (y - mOldY) / 60.0f;
		//mCamera->Zoom(mDist);
		postRedisplay = true;
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
		postRedisplay = true;
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

		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			mCamera->Yaw(glm::radians(mMouseX));
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			mCamera->Pitch(glm::radians(mMouseY));
		}

		postRedisplay = true;
	}

	if (postRedisplay)
	{
		mOldX = x;
		mOldY = y;
		//glutPostRedisplay();
	}

}

void SimpleCamera::OnCursorEnter(GLFWwindow* /*window*/, int /*entered*/)
{

}

void SimpleCamera::OnScroll(GLFWwindow* /*window*/, double /*x*/, double /*y*/)
{

}

void SimpleCamera::OnKey(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void SimpleCamera::OnChar(GLFWwindow* /*window*/, unsigned int codepoint)
{
	switch (codepoint)
	{
	case L' ':
		mUseFiltering = !mUseFiltering;
		break;

	case L'N':
	case L'n':
		Engine::GetInstance()->EnableDrawVertexNormal(!Engine::GetInstance()->IsDrawVertexNormalEnabled());
		//glutPostRedisplay();
		break;
	}
}

void SimpleCamera::OnCharMods(GLFWwindow* /*window*/, unsigned int /*codepoint*/, int /*mods*/)
{

}

void SimpleCamera::OnDrop(GLFWwindow* /*window*/, int /*count*/, const char** /*paths*/)
{

}

void SimpleCamera::OnMonitor(GLFWmonitor* /*monitor*/, int /*event*/)
{

}

bool SimpleCamera::SelectScreenResolution(int & windowWidth, int & windowHeight, bool & fullscreen, GLFWmonitor *& monitor)
{
	fullscreen = false;
	return true;
}


} // namespace CoreGame