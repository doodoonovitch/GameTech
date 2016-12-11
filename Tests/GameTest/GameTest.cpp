// GameTest.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <iostream>

#include "GL\glew.h"
#include "GL\freeglut.h"

#include "Introspection.h"
#include "CoreFx.h"
#include "CoreGame.h"

#include "TGameProgram.h"
#include "Camera\SimpleCamera.h"

using namespace CoreFx;
using namespace CoreGame;

bool getChoiceFromArgs(int argc, char **argv, int & choice)
{
	if (argc >= 2)
	{
		if (sscanf_s(argv[1], "%i", &choice) == 1)
			return true;
	}

	return false;
}

int main(int argc, char **argv)
{
	TGameProgram<SimpleCamera> simpleCameraSample;

	RECT win;
	bool fullscreen;
	int monitorIndex;

	const Sys::DisplayMonitorHelper & helper = simpleCameraSample.GetDisplayMonitorHelper();
	const Sys::DisplayMonitorList & monitorList = helper.GetMonitorList();

	int choice = 0;

	int monitorCount = (int)monitorList.size();

	const wchar_t * modes[] =
	{
		L"Window",
		L"Fullscreen"
	};

	if (!getChoiceFromArgs(argc, argv, choice))
	{
		do
		{
			int i = 1;

			for (int mode = 0; mode < ARRAY_SIZE_IN_ELEMENTS(modes); ++mode)
			{
				Sys::DisplayMonitorList::const_iterator it;
				for (it = monitorList.begin(); it != monitorList.end(); ++it, ++i)
				{
					const Sys::DisplayMonitor & monitor = *it;
					wprintf(L"\t%i - %s on '%s' (%i, %i - %i x %i, %ibpp, %iHz) : '%s'\n", i, modes[mode], monitor.mName.c_str(), monitor.mX, monitor.mY, monitor.mWidth, monitor.mHeight, monitor.mBitsPerPel, monitor.mDisplayFrequency, monitor.mLabel.c_str());
				}
			}

			wprintf(L"\t0 - Exit\n");

			wprintf(L"Please, enter your choice : ");
			wscanf_s(L"%i", &choice);
			wprintf(L"\n\n");

		} while (!((choice >= 0) && (choice <= (monitorCount * 2))));
	}

	if (choice == 0)
	{
		return 0;
	}

	--choice;
	int mode = choice / monitorCount;
	monitorIndex = choice % monitorCount;
	
	const Sys::DisplayMonitor & monitor = monitorList[monitorIndex];

	win.left = monitor.mX;
	win.top = monitor.mY;

	if (mode == 0)
	{
		win.right = win.left + std::min((LONG)1280, monitor.mWidth);
		win.bottom = win.top + std::min((LONG)960, monitor.mHeight);
		fullscreen = false;
	}
	else
	{
		win.right = win.left + monitor.mWidth;
		win.bottom = win.top + monitor.mHeight;
		fullscreen = true;
	}

	return simpleCameraSample.RunProgram(argc, argv, L"GameTech (first test)", helper.GetAdapterList()[monitor.mAdapterIndex].mName.c_str(), win, fullscreen, (uint8_t)monitor.mBitsPerPel);
}

