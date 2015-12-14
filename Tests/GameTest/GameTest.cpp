// GameTest.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <iostream>

#include "GL\glew.h"
#include "GL\freeglut.h"

#include "Introspection.h"
#include "CoreFx.h"

#include "TGameProgram.h"
#include "Camera\SimpleCamera.h"

using namespace CoreFx;
using namespace CoreGame;

int main(int argc, char **argv)
{
	TGameProgram<Camera::SimpleCamera> simpleCameraSample;

	int err = simpleCameraSample.StartEngine(argc, argv, 1280, 960, false, "OpenGL First Window");
	if (err)
		return err;

	simpleCameraSample.Activate();

	glutMainLoop();


	return 0;
}

