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

protected:

	virtual void Activate() = 0;
	virtual void Unactivate() = 0;

};


template<typename TGame>
class TGameProgram : public GameProgram
{
public:

	TGameProgram()
	{
		sGameEngineInstance = &mGameEngine;
	}

	~TGameProgram()
	{

	}

	virtual int RunProgram(int argc, char **argv, int windowWidth, int windowHeight, bool fullscreen, const char* title) override
	{
		MSG		msg;									// Windows Message Structure
		BOOL	done = FALSE;								// Bool Variable To Exit Loop

															// Ask The User Which Screen Mode They Prefer
		if (MessageBox(NULL, "Would You Like To Run In Fullscreen Mode?", "Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			fullscreen = FALSE;							// Windowed Mode
		}

		// Create Our OpenGL Window
		if (!CreateGLWindow(title, windowWidth, windowHeight, 32, fullscreen))
		{
			return -;									// Quit If Window Was Not Created
		}

		Activate();

		while (!done)									// Loop That Runs While done=FALSE
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
			{
				if (msg.message == WM_QUIT)				// Have We Received A Quit Message?
				{
					done = TRUE;							// If So done=TRUE
				}
				else									// If Not, Deal With Window Messages
				{
					TranslateMessage(&msg);				// Translate The Message
					DispatchMessage(&msg);				// Dispatch The Message
				}
			}
			else										// If There Are No Messages
			{
			}
		}

		Unactivate();

		// Shutdown
		KillGLWindow();									// Kill The Window
		return (msg.wParam);							// Exit The Program
	}

protected:

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

protected:

	/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
	*	title			- Title To Appear At The Top Of The Window				*
	*	width			- Width Of The GL Window Or Fullscreen Mode				*
	*	height			- Height Of The GL Window Or Fullscreen Mode			*
	*	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
	*	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
	// Code from Nehe Productions
	bool CreateGLWindow(const char* title, int width, int height, int bits, bool fullscreenflag)
	{
		GLuint		PixelFormat;				// Holds The Results After Searching For A Match
		WNDCLASS	wc;							// Windows Class Structure
		DWORD		dwExStyle;					// Window Extended Style
		DWORD		dwStyle;					// Window Style
		RECT		WindowRect;					// Grabs Rectangle Upper Left / Lower Right Values
		WindowRect.left = (long)0;				// Set Left Value To 0
		WindowRect.right = (long)width;			// Set Right Value To Requested Width
		WindowRect.top = (long)0;				// Set Top Value To 0
		WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

		fullscreen = fullscreenflag;			// Set The Global Fullscreen Flag

		hInstance = GetModuleHandle(NULL);					// Grab An Instance For Our Window
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// Redraw On Size, And Own DC For Window.
		wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
		wc.cbClsExtra = 0;									// No Extra Window Data
		wc.cbWndExtra = 0;									// No Extra Window Data
		wc.hInstance = hInstance;							// Set The Instance
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);				// Load The Default Icon
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
		wc.hbrBackground = NULL;							// No Background Required For GL
		wc.lpszMenuName = NULL;								// We Don't Want A Menu
		wc.lpszClassName = sWindowClassName;				// Set The Class Name

		if (!RegisterClass(&wc))									// Attempt To Register The Window Class
		{
			MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (fullscreen)												// Attempt Fullscreen Mode?
		{
			DEVMODE dmScreenSettings;								// Device Mode
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
			dmScreenSettings.dmPelsWidth = width;				// Selected Screen Width
			dmScreenSettings.dmPelsHeight = height;				// Selected Screen Height
			dmScreenSettings.dmBitsPerPel = bits;					// Selected Bits Per Pixel
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
				if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				{
					fullscreen = FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
				}
				else
				{
					// Pop Up A Message Box Letting User Know The Program Is Closing.
					MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
					return false;
				}
			}
		}

		if (fullscreen)												// Are We Still In Fullscreen Mode?
		{
			dwExStyle = WS_EX_APPWINDOW;								// Window Extended Style
			dwStyle = WS_POPUP;										// Windows Style
			ShowCursor(FALSE);										// Hide Mouse Pointer
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
			dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
		}

		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

																		// Create The Window
		if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
			sWindowClassName,					// Class Name
			title,								// Window Title
			dwStyle |							// Defined Window Style
			WS_CLIPSIBLINGS |					// Required Window Style
			WS_CLIPCHILDREN,					// Required Window Style
			0, 0,								// Window Position
			WindowRect.right - WindowRect.left,	// Calculate Window Width
			WindowRect.bottom - WindowRect.top,	// Calculate Window Height
			NULL,								// No Parent Window
			NULL,								// No Menu
			hInstance,							// Instance
			NULL)))								// Dont Pass Anything To WM_CREATE
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			bits,										// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			16,											// 16Bit Z-Buffer (Depth Buffer)  
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};

		if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!InitGL())									// Initialize GL Library (GLEW...)
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		ShowWindow(hWnd, SW_SHOW);						// Show The Window
		SetForegroundWindow(hWnd);						// Slightly Higher Priority
		SetFocus(hWnd);									// Sets Keyboard Focus To The Window

		return true;									// Success
	}

	bool InitGL()
	{
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
	}

	// Code from Nehe Productions
	GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
	{
		if (fullscreen)										// Are We In Fullscreen Mode?
		{
			ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
			ShowCursor(TRUE);								// Show Mouse Pointer
		}

		if (hRC)											// Do We Have A Rendering Context?
		{
			if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
			{
				MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			}

			if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
			{
				MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			}
			hRC = NULL;										// Set RC To NULL
		}

		if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
		{
			MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			hDC = NULL;										// Set DC To NULL
		}

		if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
		{
			MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			hWnd = NULL;										// Set hWnd To NULL
		}

		if (!UnregisterClass(sWindowClassName, hInstance))			// Are We Able To Unregister Class
		{
			MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			hInstance = NULL;									// Set hInstance To NULL
		}
	}

	// Code from Nehe Productions
	static LRESULT CALLBACK WndProc(
		HWND	hWnd,			// Handle For This Window
		UINT	uMsg,			// Message For This Window
		WPARAM	wParam,			// Additional Message Information
		LPARAM	lParam)			// Additional Message Information
	{
		switch (uMsg)									// Check For Windows Messages
		{
			case WM_ACTIVATE:							// Watch For Window Activate Message
			{
				if (!HIWORD(wParam))					// Check Minimization State
				{
					sGameEngineInstance->Activate();
				}
				else
				{
					sGameEngineInstance->UnActivate();
				}

				return 0;								// Return To The Message Loop
			}

			case WM_SYSCOMMAND:							// Intercept System Commands
			{
				switch (wParam)							// Check System Calls
				{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
					return 0;							// Prevent From Happening
				}
				break;									// Exit
			}

			case WM_CLOSE:								// Did We Receive A Close Message?
			{
				PostQuitMessage(0);						// Send A Quit Message
				return 0;								// Jump Back
			}

			case WM_SIZE:								// Resize The OpenGL Window
			{
				OnResize(LOWORD(lParam), HIWORD(lParam));	// LoWord=Width, HiWord=Height
				return 0;									// Jump Back
			}
		}

		// Pass All Unhandled Messages To DefWindowProc
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}


private:

	static void OnRender()
	{
		sGameEngineInstance->OnRender();
	}

	static void OnShutdown()
	{
		sGameEngineInstance->OnShutdown();
	}

	static void OnResize(int w, int h)
	{
		sGameEngineInstance->OnResize(w, h);
	}

	static void OnInit()
	{
		sGameEngineInstance->OnInit();
	}

	static void OnIdle()
	{
		sGameEngineInstance->OnIdle();
	}

	static void OnMouseDown(int button, int s, int x, int y)
	{
		sGameEngineInstance->OnMouseDown(button, s, x, y);
	}

	static void OnMouseMove(int x, int y)
	{
		sGameEngineInstance->OnMouseMove(x, y);
	}

	static void OnKey(unsigned char key, int x, int y)
	{
		sGameEngineInstance->OnKey(key, x, y);
	}

private:

	TGame mGameEngine;

	static TGame* sGameEngineInstance;
	static const char * sWindowClassName;
};

template <typename TGame> TGame* TGameProgram<TGame>::sGameEngineInstance = nullptr;
template <typename TGame> const char * TGameProgram<TGame>::sWindowClassName = "GameTechProgram";

} // namespace CoreGame

#endif// CORE_TRUNSAMPLE_H