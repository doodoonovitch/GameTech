#include "stdafx.h"
#include "SimpleCamera.h"
#include <iostream>

using namespace std;
using namespace CoreFx;
using namespace Renderables;


namespace CoreGame
{
	namespace Camera
	{




void SimpleCamera::OnRender(void)
{
	_dt = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glm::mat4 MV = _camera.GetViewMatrix();
	glm::mat4 P = _camera.GetProjectionMatrix();
	glm::mat4 MVP = P*MV;

	_grid->Render(glm::value_ptr(MVP));

	glutSwapBuffers();
}

void SimpleCamera::OnShutdown()
{
	//Destroy grid
	delete _grid;

	cout << "Shutdown successfull" << endl;
}

void SimpleCamera::OnResize(int w, int h)
{
	//set the viewport size
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	//setup the projection matrix 
	_camera.SetupProjection(45, (GLfloat)w / h);
}

void SimpleCamera::OnInit()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GL_CHECK_ERRORS

		//setup grid
		_grid = new Grid(10, 10);

	//setup camera
	_camera.SetPosition(glm::vec3(5, 5, 5));
	_camera.SetTarget(glm::vec3(0, 0, 0));


	cout << "Initialization successfull" << endl;
}

SimpleCamera::SimpleCamera()
{
}

SimpleCamera::~SimpleCamera()
{
}

//mosue click handler
void SimpleCamera::OnMouseDown(int button, int s, int x, int y)
{
	if (s == GLUT_DOWN)
	{
		_oldX = x;
		_oldY = y;
	}

	if (button == GLUT_MIDDLE_BUTTON)
		_state = 0;
	else if (button == GLUT_RIGHT_BUTTON)
		_state = 2;
	else
		_state = 1;
}

//mosue move handler
void SimpleCamera::OnMouseMove(int x, int y)
{
	if (_state == 0)
	{
		_dist = (y - _oldY) / 60.0f;
		_camera.Zoom(_dist);
	}
	else if (_state == 2)
	{
		float dy = float(y - _oldY) / 100.0f;
		float dx = float(_oldX - x) / 100.0f;
		if (useFiltering)
			filterMouseMoves(dx, dy);
		else
		{
			_mouseX = dx;
			_mouseY = dy;
		}

		_camera.Pan(_mouseX, _mouseY);
	}
	else
	{
		_rY = float(y - _oldY);
		_rX = float(_oldX - x);
		if (useFiltering)
			filterMouseMoves(_rX, _rY);
		else
		{
			_mouseX = _rX;
			_mouseY = _rY;
		}
		_camera.Rotate(_mouseX, _mouseY, 0);
	}
	_oldX = x;
	_oldY = y;

	glutPostRedisplay();
}

void SimpleCamera::OnKey(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		useFiltering = !useFiltering;
		break;
	}
	glutPostRedisplay();
}

void SimpleCamera::filterMouseMoves(float dx, float dy)
{
	for (int i = MOUSE_HISTORY_BUFFER_SIZE - 1; i > 0; --i)
	{
		_mouseHistory[i] = _mouseHistory[i - 1];
	}

	// Store current mouse entry at front of array.
	_mouseHistory[0] = glm::vec2(dx, dy);

	float averageX = 0.0f;
	float averageY = 0.0f;
	float averageTotal = 0.0f;
	float currentWeight = 1.0f;

	// Filter the mouse.
	for (int i = 0; i < MOUSE_HISTORY_BUFFER_SIZE; ++i)
	{
		glm::vec2 tmp = _mouseHistory[i];
		averageX += tmp.x * currentWeight;
		averageY += tmp.y * currentWeight;
		averageTotal += 1.0f * currentWeight;
		currentWeight *= MOUSE_FILTER_WEIGHT;
	}

	_mouseX = averageX / averageTotal;
	_mouseY = averageY / averageTotal;

}

void SimpleCamera::OnIdle()
{
	bool bPressed = false;
	float dx = 0, dy = 0;
	if (GetAsyncKeyState(VK_Z) & 0x8000)
	{
		dy += (MOVE_SPEED);
		bPressed = true;
	}

	if (GetAsyncKeyState(VK_S) & 0x8000)
	{
		dy -= (MOVE_SPEED);
		bPressed = true;
	}

	if (GetAsyncKeyState(VK_Q) & 0x8000)
	{
		dx -= (MOVE_SPEED);
		bPressed = true;
	}

	if (GetAsyncKeyState(VK_D) & 0x8000)
	{
		dx += (MOVE_SPEED);
		bPressed = true;
	}

	if (bPressed)
		_camera.Move(dx, dy);

	glutPostRedisplay();
}


	} // namespace Camera
} // namespace CoreGame