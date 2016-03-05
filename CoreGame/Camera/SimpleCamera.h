#pragma once
#ifndef COREGAME_SIMPLECAMERA_H
#define COREGAME_SIMPLECAMERA_H

#include "CoreFx.h" 
#include "TGameProgram.h"

namespace CoreGame
{


class SimpleCamera
{
	template<typename TGame> friend class TGameProgram;

private:

	SimpleCamera();
	~SimpleCamera();

	void OnUpdate(GLFWwindow * window, double deltaTime);
	void OnRender(GLFWwindow * window, double deltaTime);


	void OnInit();
	void OnShutdown();


	bool SelectScreenResolution(int & windowWidth, int & windowHeight, bool & fullscreen, GLFWmonitor *& monitor);

	void OnWindowPos(GLFWwindow* window, int x, int y);
	void OnWindowSize(GLFWwindow* window, int width, int height);
	void OnFramebufferSize(GLFWwindow* window, int width, int height);
	void OnWindowClose(GLFWwindow* window);
	void OnWindowRefresh(GLFWwindow* window);
	void OnWindowFocus(GLFWwindow* window, int focused);
	void OnWindowIconify(GLFWwindow* window, int iconified);
	void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
	void OnCursorPosition(GLFWwindow* window, double x, double y);
	void OnCursorEnter(GLFWwindow* window, int entered);
	void OnScroll(GLFWwindow* window, double x, double y);
	void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);
	void OnChar(GLFWwindow* window, unsigned int codepoint);
	void OnCharMods(GLFWwindow* window, unsigned int codepoint, int mods);
	void OnDrop(GLFWwindow* window, int count, const char** paths);
	void OnMonitor(GLFWmonitor* monitor, int event);


private:

	inline bool IsKeyPressed(GLFWwindow* window, int key) const
	{
		return glfwGetKey(window, key) == GLFW_PRESS;
	}

	void filterMouseMoves(float dx, float dy);

	static const int VK_W = 0x57;
	static const int VK_S = 0x53;
	static const int VK_A = 0x41;
	static const int VK_D = 0x44;
	static const int VK_Q = 0x51;
	static const int VK_Z = 0x5a;

	static const int MOUSE_HISTORY_BUFFER_SIZE = 10;

	const float MOVE_SPEED = 0.250f;
	const float MOUSE_FILTER_WEIGHT = 0.75f;

	//camera transformation variables
	int mState = 0;
	float mOldX = 0.f, mOldY = 0.f;
	float mRotX = 0.f, mRotY = 135.f, mDist = 0.f;

	CoreFx::Camera* mCamera;

	glm::vec2 mMouseHistory[MOUSE_HISTORY_BUFFER_SIZE];

	float mMouseX = 0, mMouseY = 0; //filtered mouse values
	bool mUseFiltering = true;
	bool mPause = false;
};



} // namespace CoreGame
#endif // COREGAME_SIMPLECAMERA_H
