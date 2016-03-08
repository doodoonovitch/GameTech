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
	RECT win;
	bool fullscreen;

	{
		int choice = 1;
		int i = 1;
		printf("\t%i - Window Mode\n", i++);

		Sys::DisplayMonitorHelper helper;

		const Sys::DisplayMonitorInfoList & infoList = helper.GetDisplayMonitorList();
		Sys::DisplayMonitorInfoList::const_iterator it;
		for (it = infoList.begin(); it != infoList.end(); ++it, ++i)
		{
			const Sys::DisplayMonitorInfo & info = *it;
			printf("\t%i - Fullscreen on monitor %i (%i, %i - %i x %i)\n", i, i, info.mVirtualScreen.left, info.mVirtualScreen.top, std::abs(info.mVirtualScreen.right - info.mVirtualScreen.left), std::abs(info.mVirtualScreen.bottom - info.mVirtualScreen.top));
		}

		printf("\t0 - Exit\n");

		printf("Please, enter your choice : ");
		scanf_s("%i", &choice);

		if (choice == 0 || (choice-1) >= (int)infoList.size() )
		{
			return 0;
		}

		if (choice == 1)
		{
			const Sys::DisplayMonitorInfo & info = helper.GetPrimaryMonitorInfo();
			win = info.mVirtualScreen;
			win.right = 1280;
			win.bottom = 960;
			fullscreen = false;
		}
		else
		{
			const Sys::DisplayMonitorInfo & info = infoList[choice - 1];
			win = info.mVirtualScreen;
			fullscreen = true;
		}
	}


	TGameProgram<SimpleCamera> simpleCameraSample;
	return simpleCameraSample.RunProgram(argc, argv, win, fullscreen, L"GameTech (first test)");
}

