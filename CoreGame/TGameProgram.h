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

	virtual int StartProgram(int argc, char **argv, int windowWidth, int windowHeight, bool fullscreen, const char* title) = 0;
	virtual void Activate() = 0;
	virtual void Unactivate() = 0;

};


template<typename TGame>
class TGameProgram : public GameProgram
{
public:

	TGameProgram()
	{
		mGameEngineInstance = &mGameEngine;
	}

	~TGameProgram()
	{

	}

	virtual int StartProgram(int argc, char **argv, int windowWidth, int windowHeight, bool fullscreen, const char* title) override
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


	virtual void Activate()
	{
		TGameProgram<TGame>::OnInit();

		// register callbacks
		glutCloseFunc(TGameProgram<TGame>::OnShutdown);
		glutDisplayFunc(TGameProgram<TGame>::OnRender);
		glutReshapeFunc(TGameProgram<TGame>::OnResize);
		glutKeyboardFunc(TGameProgram<TGame>::OnKey);
		glutMouseFunc(TGameProgram<TGame>::OnMouseDown);
		glutMotionFunc(TGameProgram<TGame>::OnMouseMove);
		glutIdleFunc(TGameProgram<TGame>::OnIdle);
	}

	virtual void Unactivate()
	{
		glutCloseFunc(nullptr);
		glutDisplayFunc(nullptr);
		glutReshapeFunc(nullptr);
		glutKeyboardFunc(nullptr);
		glutMouseFunc(nullptr);
		glutMotionFunc(nullptr);
		glutIdleFunc(nullptr);

		TGameProgram<TGame>::OnShutdown();
	}

	static void OnRender()
	{
		mGameEngineInstance->OnRender();
	}

	static void OnShutdown()
	{
		mGameEngineInstance->OnShutdown();
	}

	static void OnResize(int w, int h)
	{
		mGameEngineInstance->OnResize(w, h);
	}

	static void OnInit()
	{
		mGameEngineInstance->OnInit();
	}

	static void OnIdle()
	{
		mGameEngineInstance->OnIdle();
	}

	static void OnMouseDown(int button, int s, int x, int y)
	{
		mGameEngineInstance->OnMouseDown(button, s, x, y);
	}

	static void OnMouseMove(int x, int y)
	{
		mGameEngineInstance->OnMouseMove(x, y);
	}

	static void OnKey(unsigned char key, int x, int y)
	{
		mGameEngineInstance->OnKey(key, x, y);
	}

private:

	TGame mGameEngine;

	static TGame* mGameEngineInstance;
};

template <typename TGame> TGame* TGameProgram<TGame>::mGameEngineInstance = nullptr;

} // namespace CoreGame

#endif// CORE_TRUNSAMPLE_H