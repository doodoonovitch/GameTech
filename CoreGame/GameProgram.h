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

protected:

	HDC			mHDC = NULL;	// Private GDI Device Context
	HGLRC		mHRC = NULL;	// Permanent Rendering Context
	HWND		mHWnd = NULL;	// Holds Our Window Handle
	HINSTANCE	mHInstance = NULL;		// Holds The Instance Of The Application
	int			mMouseX = 0;
	int			mMouseY = 0;
	MouseState	mMouseState;

	Sys::DisplayMonitorHelper mMonitorHelper;
	bool		mFullscreen = false;
};





} // namespace CoreGame
#endif // COREGAME_GAMEPROGRAM_H