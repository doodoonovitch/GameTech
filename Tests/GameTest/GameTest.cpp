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

		do
		{
			int i = 1;

			wprintf(L"\t%i - Window Mode\n", i++);

			Sys::DisplayMonitorInfoList::const_iterator it;
			for (it = infoList.begin(); it != infoList.end(); ++it, ++i)
			{
				const Sys::DisplayMonitorInfo & info = *it;
				wprintf(L"\t%i - Fullscreen on monitor '%s' (%i, %i - %i x %i)\n", i, info.mName.c_str(), info.mVirtualScreen.left, info.mVirtualScreen.top, std::abs(info.mVirtualScreen.right - info.mVirtualScreen.left), std::abs(info.mVirtualScreen.bottom - info.mVirtualScreen.top));
			}

			wprintf(L"\t0 - Exit\n");

			wprintf(L"Please, enter your choice : ");
			wscanf_s(L"%i", &choice);
			wprintf(L"\n\n");

		} while (!(choice >= 0 && (choice - 1) <= (int)infoList.size()));

		if (choice == 0)
		{
			return 0;
		}

		if (choice == 1)
		{
			monitorIndex = helper.GetPrimaryMonitorIndex();
			const Sys::DisplayMonitorInfo & info = infoList[monitorIndex];
			win = info.mVirtualScreen;
			win.right = 1280;
			win.bottom = 960;
			fullscreen = false;
		}
		else
		{
			monitorIndex = choice - 2;
			const Sys::DisplayMonitorInfo & info = infoList[monitorIndex];
			win = info.mVirtualScreen;
			fullscreen = true;
		}
	}


	return simpleCameraSample.RunProgram(argc, argv, L"GameTech (first test)", monitorIndex, win, fullscreen);
}

