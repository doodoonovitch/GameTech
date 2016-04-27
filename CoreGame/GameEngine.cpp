#include "stdafx.h"
#include "CoreGame.h"

namespace CoreGame
{




GameEngine::GameEngine(GameProgram & gameProgram)
	: mGameProgram(gameProgram)
	, mDeltaTime(0.0)
	, mTime(0.0)
{
}


GameEngine::~GameEngine()
{
}



} // namespace CoreGame