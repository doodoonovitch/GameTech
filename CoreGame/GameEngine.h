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

	double GetDeltaTime() const { return mDeltaTime; }
	double GetTime() const { return mTime; }

	void SetDeltaTime(double value) { mDeltaTime = value; }
	void SetTime(double value) { mTime = value; }

protected:

	GameProgram & mGameProgram;
	double mDeltaTime;
	double mTime;
};







} // namespace CoreGame
#endif // COREGAME_GAMEENGINE_H