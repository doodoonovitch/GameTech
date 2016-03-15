#pragma once
#ifndef COREGAME_GAMEPROGRAM_H
#define COREGAME_GAMEPROGRAM_H

namespace CoreGame
{






class GameProgram
{
public:

	GameProgram();
	virtual ~GameProgram();

	virtual int RunProgram(int argc, char **argv, const wchar_t* title, int monitorIndex, const RECT & winCoord, bool fullscreenflag, uint8_t bitsPerPixel = 32) = 0;

	void PostExitGame(int exitCode)
	{
		PostQuitMessage(exitCode);
	}

	const Sys::DisplayMonitorHelper GetDisplayMonitorHelper() const
	{
		return mMonitorHelper;
	}

	const Sys::StepTimer & GetTimer() const
	{
		return mTimer;
	}

	const RECT & GetWindowRect() const
	{
		return mWinRect;
	}

	int GetWindowWidth() const
	{
		return mWinRect.right - mWinRect.left;
	}

	int GetWindowHeight() const
	{
		return mWinRect.bottom - mWinRect.top;
	}

protected:

	HDC			mHDC = NULL;	// Private GDI Device Context
	HGLRC		mHRC = NULL;	// Permanent Rendering Context
	HWND		mHWnd = NULL;	// Holds Our Window Handle
	HINSTANCE	mHInstance = NULL;		// Holds The Instance Of The Application
	int			mMouseX = 0;
	int			mMouseY = 0;
	MouseState	mMouseState;
	RECT		mWinRect;

	Sys::StepTimer mTimer;
	Sys::DisplayMonitorHelper mMonitorHelper;
	bool		mFullscreen = false;
};





} // namespace CoreGame
#endif // COREGAME_GAMEPROGRAM_H