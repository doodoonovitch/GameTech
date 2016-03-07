#pragma once
#ifndef COREGAME_SIMPLECAMERA_H
#define COREGAME_SIMPLECAMERA_H

#include "CoreFx.h" 


namespace CoreGame
{


class SimpleCamera
{
public:

	SimpleCamera();
	~SimpleCamera();

	void OnRender();

	void OnShutdown();

	void OnResize(int nw, int nh);

	void OnInit();

	void OnIdle();

	void OnMouseDown(int button, int s, int x, int y);
	void OnMouseMove(int x, int y);
	void OnKey(unsigned char key, int x, int y);

private:

	void filterMouseMoves(float dx, float dy);

	void SetupViewportAndProjection();

	static const int VK_W = 0x57;
	static const int VK_S = 0x53;
	static const int VK_A = 0x41;
	static const int VK_D = 0x44;
	static const int VK_Q = 0x51;
	static const int VK_Z = 0x5a;

	static const int MOUSE_HISTORY_BUFFER_SIZE = 10;

	const float MOVE_SPEED = 0.250f;
	const float MOUSE_FILTER_WEIGHT = 0.75f;

	GLsizei mWindowWidth = 1280;
	GLsizei mWindowHeight = 960;

	//camera transformation variables
	int m_state = 0, m_oldX = 0, m_oldY = 0;
	float m_rX = 0.f, m_rY = 135.f, m_dist = 0.f;
	float m_dt = 0;

	CoreFx::Camera* m_pCamera;

	glm::vec2 m_mouseHistory[MOUSE_HISTORY_BUFFER_SIZE];

	float m_mouseX = 0, m_mouseY = 0; //filtered mouse values
	bool m_useFiltering = true;

};



} // namespace CoreGame
#endif // COREGAME_SIMPLECAMERA_H
