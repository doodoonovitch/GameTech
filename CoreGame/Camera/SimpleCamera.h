#pragma once
#ifndef COREGAME_SIMPLECAMERA_H
#define COREGAME_SIMPLECAMERA_H



namespace CoreGame
{


class SimpleCamera : public GameEngine
{
public:

	SimpleCamera(GameProgram & gameProgram);
	~SimpleCamera();


	void OnInit();
	void OnShutdown();

	void OnUpdate(double elapsedTime);
	void OnRender(double elapsedTime);

	void OnWindowResize(int nw, int nh);

	void OnMouseButton(MouseState mouseState, int x, int y);
	void OnMouseMove(int x, int y);
	void OnKeyDown(wchar_t key);

	void OnWindowActivate() {}
	void OnWindowUnactivate() {}

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

	const float MOUSE_FILTER_WEIGHT = 0.75f;
	const float DEFAULT_MOVE_SPEED = 10.f;

	float mMoveSpeed = DEFAULT_MOVE_SPEED;

	GLsizei mWindowWidth = 1280;
	GLsizei mWindowHeight = 960;

	//camera transformation variables
	int mState = 0, mOldX = 0, mOldY = 0;
	float mRotX = 0.f, mRotY = 135.f, mDist = 0.f;
	float mDeltaTime = 0;

	CoreFx::Camera* mCamera;

	glm::vec2 m_mouseHistory[MOUSE_HISTORY_BUFFER_SIZE];

	float mMouseX = 0, mMouseY = 0; //filtered mouse values
	bool mUseFiltering = true;

};



} // namespace CoreGame
#endif // COREGAME_SIMPLECAMERA_H
