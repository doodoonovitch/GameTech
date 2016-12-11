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

	virtual int RunProgram(int argc, char **argv, const wchar_t* title, const wchar_t * displayDevice, const RECT & winCoord, bool fullscreen, uint8_t bitsPerPixel = 32) = 0;

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

	const RECT & GetClientRect() const
	{
		return mCliRect;
	}

	int GetWindowWidth() const
	{
		return mWinRect.right - mWinRect.left;
	}

	int GetWindowHeight() const
	{
		return mWinRect.bottom - mWinRect.top;
	}

	int GetClientWidth() const
	{
		return mCliRect.right - mCliRect.left;
	}

	int GetClientHeight() const
	{
		return mCliRect.bottom - mCliRect.top;
	}

	bool GetIsFullscreen() const { return mFullscreen; }

protected:

	HDC			mHDC = NULL;	// Private GDI Device Context
	HGLRC		mHRC = NULL;	// Permanent Rendering Context
	HWND		mHWnd = NULL;	// Holds Our Window Handle
	HINSTANCE	mHInstance = NULL;		// Holds The Instance Of The Application
	int			mMouseX = 0;
	int			mMouseY = 0;
	MouseState	mMouseState;
	RECT		mWinRect;
	RECT		mCliRect;

	Sys::StepTimer mTimer;
	Sys::DisplayMonitorHelper mMonitorHelper;
	bool		mFullscreen = false;
};





} // namespace CoreGame
#endif // COREGAME_GAMEPROGRAM_H