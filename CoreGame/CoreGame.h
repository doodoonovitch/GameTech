#pragma once

#ifndef COREGAME_COREGAME_H
#define COREGAME_COREGAME_H

#include "Introspection.h"
#include "CoreFx.h" 

#include <windowsx.h>

namespace CoreGame
{

	enum class MouseButton : int
	{
		Left = 1,
		Right = 2,
		Middle = 4,
	};

	enum class ButtonState : int
	{
		Released = 0,
		Pressed
	};

} // namespace CoreGame


#include "GameProgram.h"
#include "GameEngine.h"
#include "TGameProgram.h"
#include "Camera\SimpleCamera.h"

#endif // COREGAME_COREGAME_H