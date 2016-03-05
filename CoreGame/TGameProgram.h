#pragma once

#ifndef COREGAME_TRUNSAMPLE_H
#define COREGAME_TRUNSAMPLE_H

#include "CoreFx.h"



namespace CoreGame
{



class GameProgram
{
public:

	GameProgram() {}
	virtual ~GameProgram() {}

	virtual void Activate() = 0;
	virtual void Unactivate() = 0;

};


template<typename TGame>
class TGameProgram : public GameProgram
{



public:

	TGameProgram()
		: mWindow(nullptr)
		, mLastTime(0)
	{
		sGameEngineInstance = &mGameEngine;
	}

	~TGameProgram()
	{
	}

	int Initialize(const char * title)
	{
		setlocale(LC_ALL, "");

		glfwSetErrorCallback(error_callback);

		if (!glfwInit())
		{
			exit(-1);
		}

		printf("Library initialized.\n");

		int windowWidth = 1280, windowHeight = 960;
		GLFWmonitor* monitor = nullptr; 
		bool fullscreen = false;

		if (!mGameEngine.SelectScreenResolution(windowWidth, windowHeight, fullscreen, monitor))
		{
			exit(-1);
		}

		glfwSetMonitorCallback(monitor_callback);


		if (fullscreen)
		{
			if (monitor == nullptr)
			{
				monitor = glfwGetPrimaryMonitor();
			}

			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);

			windowWidth = mode->width;
			windowHeight = mode->height;

			printf("Creating fullscreen window (%i x %i on %s)\n", windowWidth, windowHeight, glfwGetMonitorName(monitor));
		}
		else
		{
			printf("Creating windowed mode window (%i x %i)\n", windowWidth, windowHeight);
		}

		mWindow = glfwCreateWindow(windowWidth, windowHeight, title, monitor, NULL);
		if (mWindow == nullptr)
		{
			glfwTerminate();
			exit(-1);
		}

		glfwMakeContextCurrent(mWindow);

		glfwSwapInterval(1);

		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
			return (int)err;
		}

		std::cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << std::endl;
		std::cout << "\tVendor: " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "\tRenderer: " << glGetString(GL_RENDERER) << std::endl;
		std::cout << "\tVersion: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "\tGLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		if (glewIsSupported("GL_VERSION_4_5"))
		{
			std::cout << "\tGLEW Version is 4.5\n ";
		}
		else
		{
			std::cout << "\tGLEW 4.5 not supported\n ";
		}

		GL_CHECK_ERRORS

		glViewport(0, 0, windowWidth, windowHeight);

		GL_CHECK_ERRORS

		return 0;
	}

	void GameLoop()
	{
		Activate();

		mLastTime = glfwGetTime();

		while (!glfwWindowShouldClose(mWindow))
		{
			double oldTime = mLastTime;
			mLastTime = glfwGetTime();
			double deltaTime = mLastTime - oldTime;

			if (deltaTime >= mMinDeltaTime)
			{
				mGameEngine.OnUpdate(mWindow, deltaTime);
				mGameEngine.OnRender(mWindow, deltaTime);
			}
			else
			{
				mLastTime = oldTime;
			}

			glfwPollEvents();
		}

		Unactivate();

		glfwTerminate();
	}

protected:
	virtual void Activate() override
	{
		glfwSetWindowUserPointer(mWindow, this);

		glfwSetWindowPosCallback(mWindow, window_pos_callback);
		glfwSetWindowSizeCallback(mWindow, window_size_callback);
		glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
		glfwSetWindowCloseCallback(mWindow, window_close_callback);
		glfwSetWindowRefreshCallback(mWindow, window_refresh_callback);
		glfwSetWindowFocusCallback(mWindow, window_focus_callback);
		glfwSetWindowIconifyCallback(mWindow, window_iconify_callback);
		glfwSetMouseButtonCallback(mWindow, mouse_button_callback);
		glfwSetCursorPosCallback(mWindow, cursor_position_callback);
		glfwSetCursorEnterCallback(mWindow, cursor_enter_callback);
		glfwSetScrollCallback(mWindow, scroll_callback);
		glfwSetKeyCallback(mWindow, key_callback);
		glfwSetCharCallback(mWindow, char_callback);
		glfwSetCharModsCallback(mWindow, char_mods_callback);
		glfwSetDropCallback(mWindow, drop_callback);

		mGameEngine.OnInit();
	}

	virtual void Unactivate() override
	{
		glfwSetWindowUserPointer(mWindow, nullptr);

		glfwSetWindowPosCallback(mWindow, nullptr);
		glfwSetWindowSizeCallback(mWindow, nullptr);
		glfwSetFramebufferSizeCallback(mWindow, nullptr);
		glfwSetWindowCloseCallback(mWindow, nullptr);
		glfwSetWindowRefreshCallback(mWindow, nullptr);
		glfwSetWindowFocusCallback(mWindow, nullptr);
		glfwSetWindowIconifyCallback(mWindow, nullptr);
		glfwSetMouseButtonCallback(mWindow, nullptr);
		glfwSetCursorPosCallback(mWindow, nullptr);
		glfwSetCursorEnterCallback(mWindow, nullptr);
		glfwSetScrollCallback(mWindow, nullptr);
		glfwSetKeyCallback(mWindow, nullptr);
		glfwSetCharCallback(mWindow, nullptr);
		glfwSetCharModsCallback(mWindow, nullptr);
		glfwSetDropCallback(mWindow, nullptr);

		mGameEngine.OnShutdown();
	}

public:

	/*
	int StartEngine(int argc, char **argv, int windowWidth, int windowHeight, bool fullscreen, const char* title)
	{
		glutInit(&argc, argv);

		glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
		glutInitContextVersion(4, 2);
		glutInitContextFlags(GLUT_CORE_PROFILE | GLUT_DEBUG);
		glutInitContextProfile(GLUT_FORWARD_COMPATIBLE);
		glutInitWindowSize(windowWidth, windowHeight);
		glutCreateWindow(title);		

		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
			return (int)err;
		}

		std::cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << std::endl;
		std::cout << "\tVendor: " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "\tRenderer: " << glGetString(GL_RENDERER) << std::endl;
		std::cout << "\tVersion: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "\tGLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		if (glewIsSupported("GL_VERSION_4_5"))
		{
			std::cout << "\tGLEW Version is 4.5\n ";
		}
		else
		{
			std::cout << "\tGLEW 4.5 not supported\n ";
		}

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		//glFrontFace(GL_CCW);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		if (fullscreen)
		{
			glutFullScreen();
		}
			
		return 0;
	}
	*/


private:

	static void error_callback(int error, const char* description)
	{
		fprintf(stderr, "Error: %s\n", description);
	}

	static void window_pos_callback(GLFWwindow* window, int x, int y)
	{
		sGameEngineInstance->OnWindowPos(window, x, y);
	}

	static void window_size_callback(GLFWwindow* window, int width, int height)
	{
		sGameEngineInstance->OnWindowSize(window, width, height);
	}

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		sGameEngineInstance->OnFramebufferSize(window, width, height);
	}

	static void window_close_callback(GLFWwindow* window)
	{
		sGameEngineInstance->OnWindowClose(window);
	}

	static void window_refresh_callback(GLFWwindow* window)
	{
		sGameEngineInstance->OnWindowRefresh(window);
	}

	static void window_focus_callback(GLFWwindow* window, int focused)
	{
		sGameEngineInstance->OnWindowFocus(window, focused);
	}

	static void window_iconify_callback(GLFWwindow* window, int iconified)
	{
		sGameEngineInstance->OnWindowIconify(window, iconified);
	}

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		sGameEngineInstance->OnMouseButton(window, button, action, mods);
	}

	static void cursor_position_callback(GLFWwindow* window, double x, double y)
	{
		sGameEngineInstance->OnCursorPosition(window, x, y);
	}

	static void cursor_enter_callback(GLFWwindow* window, int entered)
	{
		sGameEngineInstance->OnCursorEnter(window, entered);
	}

	static void scroll_callback(GLFWwindow* window, double x, double y)
	{
		sGameEngineInstance->OnScroll(window, x, y);
	}

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		sGameEngineInstance->OnKey(window, key, scancode, action, mods);
	}

	static void char_callback(GLFWwindow* window, unsigned int codepoint)
	{
		sGameEngineInstance->OnChar(window, codepoint);
	}

	static void char_mods_callback(GLFWwindow* window, unsigned int codepoint, int mods)
	{
		sGameEngineInstance->OnCharMods(window, codepoint, mods);
	}

	static void drop_callback(GLFWwindow* window, int count, const char** paths)
	{
		sGameEngineInstance->OnDrop(window, count, paths);
	}

	static void monitor_callback(GLFWmonitor* monitor, int event)
	{
		sGameEngineInstance->OnMonitor(monitor, event);
	}


private:

	GLFWwindow* mWindow;
	double mLastTime;
	double mMinDeltaTime = 1.0 / 144.0;

	TGame mGameEngine;

	static TGame* sGameEngineInstance;
};

template <typename TGame> TGame* TGameProgram<TGame>::sGameEngineInstance = nullptr;

} // namespace CoreGame

#endif// CORE_TRUNSAMPLE_H