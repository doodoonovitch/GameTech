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

	{
		int choice = 0;
	
		const Sys::DisplayMonitorHelper & helper = simpleCameraSample.GetDisplayMonitorHelper();
		const Sys::DisplayMonitorInfoList & infoList = helper.GetDisplayMonitorList();

		int monitorCount = (int)infoList.size();

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
					Sys::DisplayMonitorInfoList::const_iterator it;
					for (it = infoList.begin(); it != infoList.end(); ++it, ++i)
					{
						const Sys::DisplayMonitorInfo & info = *it;
						wprintf(L"\t%i - %s on monitor '%s' (%i, %i - %i x %i)\n", i, modes[mode], info.mName.c_str(), info.mVirtualScreen.left, info.mVirtualScreen.top, std::abs(info.mVirtualScreen.right - info.mVirtualScreen.left), std::abs(info.mVirtualScreen.bottom - info.mVirtualScreen.top));
					}
				}

				wprintf(L"\t0 - Exit\n");

				wprintf(L"Please, enter your choice : ");
				wscanf_s(L"%i", &choice);
				wprintf(L"\n\n");

			} while (!( (choice >= 0) && (choice <= (monitorCount * 2))));
		}

		if (choice == 0)
		{
			return 0;
		}

		--choice;
		int mode = choice / monitorCount;
		monitorIndex = choice % monitorCount;
		
		if (mode == 0)
		{
			const Sys::DisplayMonitorInfo & info = infoList[monitorIndex];

			LONG w = std::abs(info.mVirtualScreen.right - info.mVirtualScreen.left);
			LONG h = std::abs(info.mVirtualScreen.bottom - info.mVirtualScreen.top);
			win = info.mVirtualScreen;
			win.right = info.mVirtualScreen.left + std::min((LONG)1280, w);
			win.bottom = info.mVirtualScreen.top + std::min((LONG)960, h);
			fullscreen = false;
		}
		else
		{
			const Sys::DisplayMonitorInfo & info = infoList[monitorIndex];
			win = info.mVirtualScreen;
			fullscreen = true;
		}
	}


	return simpleCameraSample.RunProgram(argc, argv, L"GameTech (first test)", monitorIndex, win, fullscreen);
}

