#include "stdafx.h"
#include "SimpleCamera.h"
#include <iostream>

using namespace std;
using namespace CoreFx;
using namespace Renderables;


namespace CoreGame
{




void SimpleCamera::OnRender(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_dt = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	Engine* engine = Engine::GetInstance();

	engine->UpdateObjects();
	engine->RenderObjects();

	glutSwapBuffers();
}

void SimpleCamera::OnShutdown()
{
	Engine::Release();

	cout << "Shutdown successfull" << endl;
}

void SimpleCamera::OnResize(int w, int h)
{
	//set the viewport size
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	//setup the projection matrix 
	m_pCamera->SetupProjection(45, (GLfloat)w / h);
}

void SimpleCamera::OnInit()
{
	GL_CHECK_ERRORS

		Engine::Initialize();

		Engine* engine = Engine::GetInstance();

		Renderers::GridRenderer * gridRenderer = new Renderers::GridRenderer(50, 50);
		engine->AttachRenderer(gridRenderer);

		Renderers::AxisRenderer * axisRenderer = new Renderers::AxisRenderer();
		engine->AttachRenderer(axisRenderer);

		int xCount = 30;
		int yCount = 30;
		int zCount = 30;
		Renderers::CubeRenderer * cubeRenderer = new Renderers::CubeRenderer("medias/cube.dds", 5, xCount * yCount * zCount, 1);
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
		Lights::PointLight * ptLight1 = engine->CreatePointLight(glm::vec4(0.4f, 0.4f, 0.4f, 0.4f), glm::vec3(0.f, 0.f, 0.f));
		Lights::DirectionalLight * dirLight1 = engine->CreateDirectionalLight(glm::vec4(0.4f, 0.4f, 0.4f, 0.f), glm::normalize(glm::vec3(0.2f, -1.f, 0.f)));

	//setup camera
	m_pCamera = new Camera();
	m_pCamera->LookAt(glm::vec3(10, 6.f, 12.f), glm::vec3(0, 4.f, 0.f), glm::vec3(0, 1, 0));
	engine->SetCamera(m_pCamera);

	glm::mat4 view1 = glm::lookAt(glm::vec3(10, 6.f, 12.f), glm::vec3(0, 4.f, 0.f), glm::vec3(0, 1, 0));
	m_pCamera->GetFrame()->BuildMatrix();
	glm::mat4 view2 = m_pCamera->GetViewMatrix();
	glm::mat4 view3 = m_pCamera->GetFrame()->GetMatrix();

	for (int i = 0; i < 4; ++i)
	{
		glm::vec4 & v = view1[i];
		printf("%f %f %f %f\n", v.x, v.y, v.z, v.w);
	}

	printf("\n");
	for (int i = 0; i < 4; ++i)
	{
		glm::vec4 & v = view2[i];
		printf("%f %f %f %f\n", v.x, v.y, v.z, v.w);
	}

	printf("\n");
	for (int i = 0; i < 4; ++i)
	{
		glm::vec4 & v = view3[i];
		printf("%f %f %f %f\n", v.x, v.y, v.z, v.w);
	}

	cout << "Initialization successfull" << endl;
}

SimpleCamera::SimpleCamera()
	: m_pCamera(nullptr)
{
}

SimpleCamera::~SimpleCamera()
{
	m_pCamera = nullptr;
}

//mosue click handler
void SimpleCamera::OnMouseDown(int button, int s, int x, int y)
{
	if (s == GLUT_DOWN)
	{
		m_oldX = x;
		m_oldY = y;
	}

	if (button == GLUT_MIDDLE_BUTTON)
		m_state = 0;
	else if (button == GLUT_RIGHT_BUTTON)
		m_state = 2;
	else
		m_state = 1;
}

//mosue move handler
void SimpleCamera::OnMouseMove(int x, int y)
{
	bool postRedisplay = false;
	if (m_state == 0)
	{
		m_dist = (y - m_oldY) / 60.0f;
		//m_pCamera->Zoom(m_dist);
		postRedisplay = true;
	}
	else if (m_state == 2)
	{
		float dy = float(y - m_oldY) / 100.0f;
		float dx = float(m_oldX - x) / 100.0f;
		if (m_useFiltering)
			filterMouseMoves(dx, dy);
		else
		{
			m_mouseX = dx;
			m_mouseY = dy;
		}

		m_pCamera->Strafe(m_mouseX);
		postRedisplay = true;
	}
	else
	{
		m_rY = float(y - m_oldY);
		m_rX = float(m_oldX - x);
		if (m_useFiltering)
			filterMouseMoves(m_rX, m_rY);
		else
		{
			m_mouseX = m_rX;
			m_mouseY = m_rY;
		}
		m_pCamera->Yaw(glm::radians(m_mouseX));
		m_pCamera->Pitch(glm::radians(m_mouseY));

		postRedisplay = true;
	}

	if (postRedisplay)
	{
		m_oldX = x;
		m_oldY = y;
		glutPostRedisplay();
	}
}

void SimpleCamera::OnKey(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		m_useFiltering = !m_useFiltering;
		break;

	case 'N':
	case 'n':
		Engine::GetInstance()->EnableDrawVertexNormal(!Engine::GetInstance()->IsDrawVertexNormalEnabled());
		glutPostRedisplay();
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

	m_mouseX = averageX / averageTotal;
	m_mouseY = averageY / averageTotal;

}

void SimpleCamera::OnIdle()
{
	bool bWalk = false, bStrafe = false;
	float dx = 0, dy = 0;


	if (GetAsyncKeyState(VK_ESC) & 0x8000)
	{
		glutLeaveMainLoop();
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
		m_pCamera->Walk(dy);
	}
	
	if (bStrafe)
	{
		m_pCamera->Strafe(dx);
	}

	if (bWalk || bStrafe)
		glutPostRedisplay();
}


} // namespace CoreGame