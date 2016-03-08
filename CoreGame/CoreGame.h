#pragma once

#ifndef COREGAME_COREGAME_H
#define COREGAME_COREGAME_H

#include "Introspection.h"
#include "CoreFx.h" 

#include <windowsx.h>

namespace CoreGame
{

	class MouseState
	{
	public:

		enum ButtonId : int
		{
			Left		= 0x0001,
			Right		= 0x0002,
			Middle		= 0x0010,

			XButton1	= 0x0020,
			XButton2	= 0x0040,
			ControlKey	= 0x0008,
			ShiftKEY	= 0x0004,
		};
		
		MouseState()
			: mFlags(0)
		{
		}

		MouseState(int flags)
			: mFlags((int)flags)
		{
		}

		bool IsPressed(ButtonId button)
		{
			return (mFlags & ((int)button) )== (int)button;
		}

		bool IsLeftPressed()
		{
			return IsPressed(Left);
		}

		bool IsRightPressed()
		{
			return IsPressed(Right);
		}

		bool IsMiddlePressed()
		{
			return IsPressed(Middle);
		}

		bool IsXButton1Pressed()
		{
			return IsPressed(XButton1);
		}

		bool IsXButton2Pressed()
		{
			return IsPressed(XButton2);
		}

		bool IsControlKeyPressed()
		{
			return IsPressed(ControlKey);
		}

		bool IsShiftKeyPressed()
		{
			return IsPressed(ShiftKEY);
		}

		bool HasAButtonPressed()
		{
			return (mFlags & (Left | Right | Middle | XButton1 | XButton2)) != 0;
		}


		int mFlags;
	};

} // namespace CoreGame


#include "Sys\DisplayMonitorHelper.h"
#include "GameProgram.h"
#include "GameEngine.h"
#include "TGameProgram.h"
#include "Camera\SimpleCamera.h"

#endif // COREGAME_COREGAME_H