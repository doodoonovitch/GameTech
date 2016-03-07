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

	return simpleCameraSample.RunProgram(argc, argv, 1280, 960, false, L"GameTech (first test)");
}

