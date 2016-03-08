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

	virtual int RunProgram(int argc, char **argv, const RECT & winCoord, bool fullscreen, const wchar_t* title) = 0;

	void PostExitGame(int exitCode)
	{
		PostQuitMessage(exitCode);
	}

protected:

	HDC			mHDC = NULL;	// Private GDI Device Context
	HGLRC		mHRC = NULL;	// Permanent Rendering Context
	HWND		mHWnd = NULL;	// Holds Our Window Handle
	HINSTANCE	mHInstance = NULL;		// Holds The Instance Of The Application
	int			mMouseX = 0;
	int			mMouseY = 0;
	MouseState	mMouseState;
	bool		mFullscreen = false;
};





} // namespace CoreGame
#endif // COREGAME_GAMEPROGRAM_H