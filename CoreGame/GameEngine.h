#pragma once
#ifndef COREGAME_GAMEENGINE_H
#define COREGAME_GAMEENGINE_H

namespace CoreGame
{





class GameEngine
{
	template<typename TGameEngine> friend class TGameProgram;

public:
	GameEngine(GameProgram & gameProgram);
	virtual ~GameEngine();

	void PostExitGame(int exitCode)
	{
		mGameProgram.PostExitGame(exitCode);
	}

protected:

	GameProgram & mGameProgram;
};







} // namespace CoreGame
#endif // COREGAME_GAMEENGINE_H