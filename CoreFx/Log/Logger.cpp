#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{

namespace Log
{
// =======================================================================
// =======================================================================

void PrintMessage(const char * messageFormat, va_list args)
{
	Logger::GetInstance()->Print("[Info] ");
	Logger::GetInstance()->Print(messageFormat, args);
	Logger::GetInstance()->Print("\n");
}

void PrintError(const char * messageFormat, va_list args)
{
	Logger::GetInstance()->Print("[Error] ");
	Logger::GetInstance()->Print(messageFormat, args);
	Logger::GetInstance()->Print("\n");
}

void PrintWarning(const char * messageFormat, va_list args)
{
	Logger::GetInstance()->Print("[Warning] ");
	Logger::GetInstance()->Print(messageFormat, args);
	Logger::GetInstance()->Print("\n");
}


void PrintMessage(const char * messageFormat, ...)
{
	va_list args;
	va_start(args, messageFormat);
	PrintMessage(messageFormat, args);
	va_end(args);
}

void PrintError(const char * messageFormat, ...)
{
	va_list args;
	va_start(args, messageFormat);
	PrintError(messageFormat, args);
	va_end(args);
}

void PrintWarning(const char * messageFormat, ...)
{
	va_list args;
	va_start(args, messageFormat);
	PrintWarning(messageFormat, args);
	va_end(args);
}

Logger * Logger::sInstance = nullptr;

void Logger::InitializeLogger()
{
	CoreFx::Log::Logger::sInstance = new CoreFx::Log::Logger();
	CoreFx::DebugOutput::GetInstance()->EnableDebugMessage();
}

void Logger::ReleaseLogger()
{
	CoreFx::DebugOutput::GetInstance()->DisableDebugMessage();
	CoreFx::DebugOutput::GetInstance()->EnableDebugMessage();
	SAFE_DELETE(CoreFx::Log::Logger::sInstance);
}

Logger::Logger()
{
}


Logger::~Logger()
{
}

void Logger::Print(const char* messageFormat, ...)
{
	va_list args;
	va_start(args, messageFormat);
	Print(messageFormat, args);
	va_end(args);
}

void Logger::Print(const char* messageFormat, va_list args)
{
	vprintf_s(messageFormat, args);
}

// =======================================================================
// =======================================================================
} // namepsace Log
} // namespace CoreFx
