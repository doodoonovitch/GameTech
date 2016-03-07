#pragma once

#ifndef COREGAME_TRUNSAMPLE_H
#define COREGAME_TRUNSAMPLE_H

#include "CoreFx.h"



namespace CoreGame
{







template<typename TGameEngine>
class TGameProgram : public GameProgram
{
public:

	TGameProgram()
		: mGameEngine(*this)
	{
		sGameEngineInstance = &mGameEngine;
	}

	~TGameProgram()
	{

	}

	virtual int RunProgram(int argc, char **argv, int windowWidth, int windowHeight, bool fullscreen, const wchar_t* title) override
	{
		MSG		msg;									// Windows Message Structure
		BOOL	done = FALSE;								// Bool Variable To Exit Loop

															// Ask The User Which Screen Mode They Prefer
		if (MessageBox(NULL, L"Would You Like To Run In Fullscreen Mode?", L"Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			fullscreen = FALSE;							// Windowed Mode
		}

		BYTE bits = (BYTE)GetDeviceCaps(mHDC, BITSPIXEL);
		// Create Our OpenGL Window
		if (!CreateGLWindow(title, windowWidth, windowHeight, bits, fullscreen))
		{
			return -1;									// Quit If Window Was Not Created
		}

		OnInit();

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
				mGameEngine.OnUpdate(0.016);
				mGameEngine.OnRender(0.016);
				SwapBuffers(mHDC);
			}
		}

		OnShutdown();

		// Shutdown
		KillGLWindow();									// Kill The Window
		return (msg.wParam);							// Exit The Program
	}

protected:

	/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
	*	title			- Title To Appear At The Top Of The Window				*
	*	width			- Width Of The GL Window Or Fullscreen Mode				*
	*	height			- Height Of The GL Window Or Fullscreen Mode			*
	*	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
	*	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
	// Code from Nehe Productions
	bool CreateGLWindow(const wchar_t* title, int width, int height, uint8_t bits, bool fullscreenflag)
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

		mFullscreen = fullscreenflag;			// Set The Global Fullscreen Flag

		mHInstance = GetModuleHandle(NULL);					// Grab An Instance For Our Window
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// Redraw On Size, And Own DC For Window.
		wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
		wc.cbClsExtra = 0;									// No Extra Window Data
		wc.cbWndExtra = 0;									// No Extra Window Data
		wc.hInstance = mHInstance;							// Set The Instance
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);				// Load The Default Icon
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
		wc.hbrBackground = NULL;							// No Background Required For GL
		wc.lpszMenuName = NULL;								// We Don't Want A Menu
		wc.lpszClassName = sWindowClassName;				// Set The Class Name

		if (!RegisterClass(&wc))									// Attempt To Register The Window Class
		{
			MessageBox(NULL, L"Failed To Register The Window Class.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (mFullscreen)												// Attempt Fullscreen Mode?
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
				if (MessageBox(NULL, L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", L"GameTech", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				{
					mFullscreen = FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
				}
				else
				{
					// Pop Up A Message Box Letting User Know The Program Is Closing.
					MessageBox(NULL, L"Program Will Now Close.", L"ERROR", MB_OK | MB_ICONSTOP);
					return false;
				}
			}
		}

		if (mFullscreen)											// Are We Still In Fullscreen Mode?
		{
			dwExStyle = WS_EX_APPWINDOW;							// Window Extended Style
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
		if (!(mHWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
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
			mHInstance,							// Instance
			NULL)))								// Dont Pass Anything To WM_CREATE
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, L"Window Creation Error.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// nSize : Size Of This Pixel Format Descriptor
			1,											// nVersion : Version Number
			PFD_DRAW_TO_WINDOW |						// dwFlags :	Format Must Support Window
			PFD_SUPPORT_OPENGL |						//				Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							//				Must Support Double Buffering
			PFD_TYPE_RGBA,								// iPixelType : Request An RGBA Format
			bits,										// cColorBits : Select Our Color Depth
			8, 0,										// cRedBits & cRedShift : Red color bits
			8, 0,										// cGreenBits & cGreenShift : Green color bits
			8, 0,										// cBlueBits & cBlueShift : Blue color bits
			0,											// cAlphaBits : Alpha Bits
			0,											// cAlphaShift : Shift Bit Ignored
			0,											// cAccumBits : No Accumulation Buffer
			0, 0, 0, 0,									// cAccumRedBits, cAccumGreenBits, cAccumBlueBits, cAccumAlphaBits : Accumulation Bits Ignored
			24,											// cDepthBits: 24Bit Z-Buffer (Depth Buffer)  
			8,											// cStencilBits : 8bits Stencil Buffer
			0,											// cAuxBuffers : No Auxiliary Buffer
			PFD_MAIN_PLANE,								// iLayerType : Main Drawing Layer
			0,											// bReserved
			0, 0, 0										// dwLayerMask, dwVisibleMask, dwDamageMask : Layer Masks Ignored
		};

		if (!(mHDC = GetDC(mHWnd)))							// Did We Get A Device Context?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, L"Can't Create A GL Device Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!(PixelFormat = ChoosePixelFormat(mHDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!SetPixelFormat(mHDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, L"Can't Set The PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!(mHRC = wglCreateContext(mHDC)))				// Are We Able To Get A Rendering Context?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!wglMakeCurrent(mHDC, mHRC))					// Try To Activate The Rendering Context
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!InitGL())									// Initialize GL Library (GLEW...)
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, L"Initialization Failed.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		ShowWindow(mHWnd, SW_SHOW);						// Show The Window
		SetForegroundWindow(mHWnd);						// Slightly Higher Priority
		SetFocus(mHWnd);								// Sets Keyboard Focus To The Window

		return true;									// Success
	}

	bool InitGL()
	{
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
			return false;
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

		return true;
	}

	// Code from Nehe Productions
	GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
	{
		if (mFullscreen)									// Are We In Fullscreen Mode?
		{
			ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
			ShowCursor(TRUE);								// Show Mouse Pointer
		}

		if (mHRC)											// Do We Have A Rendering Context?
		{
			if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
			{
				MessageBox(NULL, L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			}

			if (!wglDeleteContext(mHRC))						// Are We Able To Delete The RC?
			{
				MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			}
			mHRC = NULL;										// Set RC To NULL
		}

		if (mHDC && !ReleaseDC(mHWnd, mHDC))					// Are We Able To Release The DC
		{
			MessageBox(NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			mHDC = NULL;										// Set DC To NULL
		}

		if (mHWnd && !DestroyWindow(mHWnd))					// Are We Able To Destroy The Window?
		{
			MessageBox(NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			mHWnd = NULL;										// Set mHWnd To NULL
		}

		if (!UnregisterClass(sWindowClassName, mHInstance))			// Are We Able To Unregister Class
		{
			MessageBox(NULL, L"Could Not Unregister Class.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			mHInstance = NULL;									// Set mHInstance To NULL
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
					OnWindowActivate();
				}
				else
				{
					OnWindowUnactivate();
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
				OnWindowResize(LOWORD(lParam), HIWORD(lParam));	// LoWord=Width, HiWord=Height
				return 0;									// Jump Back
			}

			case WM_LBUTTONDOWN:
			{
				OnMouseDown(MouseButton::Left, ButtonState::Pressed, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;
			}

			case WM_LBUTTONUP:
			{
				OnMouseDown(MouseButton::Left, ButtonState::Released, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;
			}

			case WM_RBUTTONDOWN:
			{
				OnMouseDown(MouseButton::Right, ButtonState::Pressed, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;
			}

			case WM_RBUTTONUP:
			{
				OnMouseDown(MouseButton::Right, ButtonState::Released, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;
			}

			case WM_MBUTTONDOWN:
			{
				OnMouseDown(MouseButton::Middle, ButtonState::Pressed, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;
			}

			case WM_MBUTTONUP:
			{
				OnMouseDown(MouseButton::Middle, ButtonState::Released, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;
			}

			case WM_MOUSEMOVE:
			{
				OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;
			}

			case WM_CHAR:
			{
				OnKeyDown(wParam);
			}
		}

		// Pass All Unhandled Messages To DefWindowProc
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}


private:

	static void OnInit()
	{
		sGameEngineInstance->OnInit();
	}

	static void OnShutdown()
	{
		sGameEngineInstance->OnShutdown();
	}

	static void OnUpdate()
	{
		sGameEngineInstance->OnUpdate();
	}

	static void OnRender()
	{
		sGameEngineInstance->OnRender();
	}

	static void OnWindowActivate()
	{
		sGameEngineInstance->OnWindowActivate();
	}

	static void OnWindowUnactivate()
	{
		sGameEngineInstance->OnWindowUnactivate();
	}

	static void OnWindowResize(int w, int h)
	{
		sGameEngineInstance->OnWindowResize(w, h);
	}

	static void OnIdle()
	{
		sGameEngineInstance->OnIdle();
	}

	static void OnMouseDown(MouseButton button, ButtonState state, int x, int y)
	{
		sGameEngineInstance->OnMouseDown(button, state, x, y);
	}

	static void OnMouseMove(int x, int y)
	{
		sGameEngineInstance->OnMouseMove(x, y);
	}

	static void OnKeyDown(wchar_t key)
	{
		sGameEngineInstance->OnKeyDown(key);
	}

private:

	TGameEngine mGameEngine;

	static TGameEngine* sGameEngineInstance;
	static const wchar_t * sWindowClassName;
};

template <typename TGameEngine> TGameEngine* TGameProgram<TGameEngine>::sGameEngineInstance = nullptr;
template <typename TGameEngine> const wchar_t * TGameProgram<TGameEngine>::sWindowClassName = L"GameTechProgram";

} // namespace CoreGame

#endif// CORE_TRUNSAMPLE_H