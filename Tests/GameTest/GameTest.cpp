// GameTest.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <iostream>

#include "Introspection.h"
#include "CoreFx.h"

#include "TGameProgram.h"
#include "Camera\SimpleCamera.h"

using namespace CoreFx;
using namespace CoreGame;

int main(int argc, char **argv)
{
	TGameProgram<SimpleCamera> simpleCameraSample;

	int err = simpleCameraSample.Initialize("Game Test");
	if (err)
	{
		char c;
		scanf_s("%c", &c, 1);
		return err;
	}

	simpleCameraSample.GameLoop();

	return 0;
}

