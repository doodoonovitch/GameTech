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
		Renderers::CubeRenderer * cubeRenderer = new Renderers::CubeRenderer("medias/cube.dds", xCount * yCount * zCount, 1);
		engine->AttachRenderer(cubeRenderer);

		for (auto i = 0; i < xCount; ++i)
		{
			for (auto j = 0; j < yCount; ++j)
			{
				for (auto k = 0; k < zCount; ++k)
				{
					Cube * cube = cubeRenderer->CreateCube(0);
					cube->GetFrame()->SetPosition(glm::vec3(1.5f + 5.f * i, 1.f + 5.f * k, -1.5f + -5.f * j));
				}
			}
		}
		


	//setup camera
	m_pCamera = new Camera();
	m_pCamera->LookAt(glm::vec3(0, 4.f, 12.f), glm::vec3(0, 4.f, 0.f), glm::vec3(0, 1, 0));
	engine->SetCamera(m_pCamera);


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