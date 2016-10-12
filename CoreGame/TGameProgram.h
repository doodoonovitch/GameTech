#pragma once

#ifndef COREGAME_TRUNSAMPLE_H
#define COREGAME_TRUNSAMPLE_H

#include <strsafe.h>
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
		sGameProgram = this;
	}

	~TGameProgram()
	{

	}

	virtual int RunProgram(int argc, char **argv, const wchar_t* title, int monitorIndex, const RECT & winCoord, bool fullscreen, uint8_t bitsPerPixel = 32) override
	{
		MSG		msg;
		BOOL	done = FALSE;

		// Create Our OpenGL Window
		if (!CreateGLWindow(title, monitorIndex, winCoord, fullscreen, bitsPerPixel))
		{
			return -1;
		}

		{
			POINT pt;
			if (GetCursorPos(&pt))
			{
				mMouseX = (int)pt.x;
				mMouseY = (int)pt.y;
			}
		}

		OnInit();

		while (!done)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				switch (msg.message)
				{
				case WM_QUIT:
					done = TRUE;
					break;

				//case WM_KEYDOWN:
				//{
				//	bool wasPressed = (msg.lParam & (1L << 30)) != 0;
				//	int repeatCount = (int)(msg.lParam & 0x7FFF);
				//	bool altPressed = (msg.lParam & (1L << 29)) != 0;

				//	OnKeyDown(msg.wParam, wasPressed, repeatCount, altPressed);
				//}
				//break;

				//case WM_KEYUP:
				//{
				//	bool wasPressed = (msg.lParam & (1L << 30)) != 0;
				//	int repeatCount = (int)(msg.lParam & 0x7FFF);
				//	bool altPressed = (msg.lParam & (1L << 29)) != 0;

				//	OnKeyUp(msg.wParam, wasPressed, repeatCount, altPressed);
				//}
				//break;

				default:
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					break;
				}
			}
			else
			{
				mTimer.Tick([this, &done, &msg]
				{
					mGameEngine.SetDeltaTime(mTimer.GetElapsedSeconds());
					mGameEngine.SetTime(mTimer.GetTotalSeconds());

					mGameEngine.OnUpdate();
					mGameEngine.OnRender();

					SwapBuffers(mHDC);
				});
			}
		}

		OnShutdown();

		KillGLWindow();
		return ((int)msg.wParam);
	}

protected:

	bool CreateGLWindow(const wchar_t* title, int monitorIndex, const RECT & winRect, bool fullscreenflag, BYTE bitsPerPixel)
	{
		PRINT_BEGIN_SECTION;
		PRINT_MESSAGE("Application initialization.....");

		GLuint		PixelFormat;				// Holds The Results After Searching For A Match
		WNDCLASS	wc;							// Windows Class Structure
		DWORD		dwExStyle;					// Window Extended Style
		DWORD		dwStyle;					// Window Style

		mFullscreen = fullscreenflag;			
		mWinRect = winRect;

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
			//MessageBox(NULL, L"Failed To Register The Window Class.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			ErrorExit(L"Register the Window Class");
			return false;
		}

		if (mFullscreen)
		{
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmBitsPerPel = bitsPerPixel;
			dmScreenSettings.dmFields = DM_BITSPERPEL;

			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			{
				mFullscreen = false;
			}
		}

		if (mFullscreen)											// Are We Still In Fullscreen Mode?
		{
			dwExStyle = WS_EX_APPWINDOW;							// Window Extended Style
			dwStyle = WS_POPUP;										// Windows Style
			//ShowCursor(FALSE);									// Hide Mouse Pointer
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
			dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
		}

		if (!(mHWnd = CreateWindowEx(dwExStyle,	// Extended Style For The Window
			sWindowClassName,					// Class Name
			title,								// Window Title
			dwStyle |							// Defined Window Style
			WS_CLIPSIBLINGS |					// Required Window Style
			WS_CLIPCHILDREN,					// Required Window Style
			0, 0,								// Window Position
			mWinRect.right - mWinRect.left,		// Calculate Window Width
			mWinRect.bottom - mWinRect.top,		// Calculate Window Height
			NULL,								// No Parent Window
			NULL,								// No Menu
			mHInstance,							// Instance
			NULL)))								// Dont Pass Anything To WM_CREATE
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL, L"Window Creation Error.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		SetWindowPos(mHWnd, NULL, mWinRect.left, mWinRect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

		static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// nSize : Size Of This Pixel Format Descriptor
			1,											// nVersion : Version Number
			PFD_DRAW_TO_WINDOW |						// dwFlags :	Format Must Support Window
			PFD_SUPPORT_OPENGL |						//				Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							//				Must Support Double Buffering
			PFD_TYPE_RGBA,								// iPixelType : Request An RGBA Format
			bitsPerPixel,								// cColorBits : Select Our Color Depth
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

		if (!(mHDC = GetDC(mHWnd)))
		{
			KillGLWindow();
			MessageBox(NULL, L"Can't Create A GL Device Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!(PixelFormat = ChoosePixelFormat(mHDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
		{
			KillGLWindow();
			MessageBox(NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!SetPixelFormat(mHDC, PixelFormat, &pfd))
		{
			KillGLWindow();
			MessageBox(NULL, L"Can't Set The PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!(mHRC = wglCreateContext(mHDC)))				// Are We Able To Get A Rendering Context?
		{
			KillGLWindow();
			MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!wglMakeCurrent(mHDC, mHRC))					// Try To Activate The Rendering Context
		{
			KillGLWindow();	
			MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		if (!InitGL())									// Initialize GL Library (GLEW...)
		{
			KillGLWindow();
			MessageBox(NULL, L"Initialization Failed.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		ShowWindow(mHWnd, SW_SHOW);	
		SetForegroundWindow(mHWnd);
		SetFocus(mHWnd);

		PRINT_MESSAGE(".....Application initialization end");
		PRINT_END_SECTION;

		return true;
	}

	bool InitGL()
	{
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			PRINT_ERROR("Initialization has failed : %s", glewGetErrorString(err));
			return false;
		}

		PRINT_MESSAGE("\tUsing glew: %s", glewGetString(GLEW_VERSION));
		PRINT_MESSAGE("\tVendor: %s", glGetString(GL_VENDOR));
		PRINT_MESSAGE("\tRenderer: %s", glGetString(GL_RENDERER));
		PRINT_MESSAGE("\tVersion: %s", glGetString(GL_VERSION));
		PRINT_MESSAGE("\tGLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

		if (glewIsSupported("GL_VERSION_4_4"))
		{
			PRINT_MESSAGE("\tGLEW Version is 4.4");
		}
		else
		{
			PRINT_ERROR("\tGLEW 4.4 not supported");
			return false;
		}

		if (glewIsSupported("GL_ARB_shader_draw_parameters"))
		{
			PRINT_MESSAGE("\tGL_ARB_shader_draw_parameters is supported.");
		}
		else
		{
			PRINT_MESSAGE("\tGL_ARB_shader_draw_parameters is not supported!");
		}

		return true;
	}

	// Code from Nehe Productions
	GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
	{
		if (mFullscreen)									// Are We In Fullscreen Mode?
		{
			ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
			//ShowCursor(TRUE);								// Show Mouse Pointer
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
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
			{
				OnMouseButton(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;
			}

			case WM_MOUSEMOVE:
			{
				OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				return 0;
			}

			case WM_CHAR:
			{
				bool wasPressed = (lParam & (1L << 30)) != 0;
				int repeatCount = (int)(lParam & 0x7FFF);
				bool altPressed = (lParam & (1L << 29)) != 0;
				if ((lParam & (1L << 31)) != 0)
				{
					OnKeyUp((wchar_t)wParam, wasPressed, repeatCount, altPressed);
				}
				else
				{
					OnKeyDown((wchar_t)wParam, wasPressed, repeatCount, altPressed);
				}
				return 0;
			}
		}

		// Pass All Unhandled Messages To DefWindowProc
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}


private:

	static void OnInit()
	{
		sGameProgram->mGameEngine.OnInit();
	}

	static void OnShutdown()
	{
		sGameProgram->mGameEngine.OnShutdown();
	}

	static void OnUpdate()
	{
		sGameProgram->mGameEngine.OnUpdate();
	}

	static void OnRender()
	{
		sGameProgram->mGameEngine.OnRender();
	}

	static void OnWindowActivate()
	{
		sGameProgram->mGameEngine.OnWindowActivate();
	}

	static void OnWindowUnactivate()
	{
		sGameProgram->mGameEngine.OnWindowUnactivate();
	}

	static void OnWindowResize(int w, int h)
	{
		sGameProgram->mGameEngine.OnWindowResize(w, h);
	}

	static void OnIdle()
	{
		sGameProgram->mGameEngine.OnIdle();
	}

	static void OnMouseButton(WPARAM button, int x, int y)
	{
		sGameProgram->mMouseX = x;
		sGameProgram->mMouseY = y;
		sGameProgram->mMouseState.mFlags = (int)button;
		sGameProgram->mGameEngine.OnMouseButton(sGameProgram->mMouseState, x, y);
	}

	static void OnMouseMove(int x, int y)
	{
		sGameProgram->mMouseX = x;
		sGameProgram->mMouseY = y;
		sGameProgram->mGameEngine.OnMouseMove(x, y);
	}

	static void OnKeyDown(WPARAM key, bool wasPressed, int repeatCount, bool altPressed)
	{
		sGameProgram->mGameEngine.OnKeyDown(key, wasPressed, repeatCount, altPressed);
	}

	static void OnKeyUp(WPARAM key, bool wasPressed, int repeatCount, bool altPressed)
	{
		sGameProgram->mGameEngine.OnKeyUp(key, wasPressed, repeatCount, altPressed);
	}

protected:

	void ErrorExit(LPTSTR lpszFunction)
	{
		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;
		DWORD dw = GetLastError();

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

		// Display the error message and exit the process

		lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
		StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"), lpszFunction, dw, lpMsgBuf);
		MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);

		LocalFree(lpMsgBuf);
		LocalFree(lpDisplayBuf);

		ExitProcess(dw);
	}

private:

	TGameEngine mGameEngine;

	static TGameProgram<TGameEngine>* sGameProgram;
	static const wchar_t * sWindowClassName;
};

template <typename TGameEngine> TGameProgram<TGameEngine>* TGameProgram<TGameEngine>::sGameProgram = nullptr;
template <typename TGameEngine> const wchar_t * TGameProgram<TGameEngine>::sWindowClassName = L"GameTechProgram";

} // namespace CoreGame

#endif// CORE_TRUNSAMPLE_H