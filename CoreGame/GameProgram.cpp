#include "stdafx.h"
#include "CoreGame.h"

namespace CoreGame
{


GameProgram::GameProgram()
{
	CoreFx::Log::Logger::InitializeLogger();
}


GameProgram::~GameProgram()
{
	CoreFx::Log::Logger::ReleaseLogger();
}




} // namespace CoreGame