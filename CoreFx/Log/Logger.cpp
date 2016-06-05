#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
namespace Log
{
// =======================================================================
// =======================================================================

void PrintMessage(const char * messageFormat, ...)
{
	Logger::GetInstance()->Print("[Info] ");
	va_list args;
	va_start(args, messageFormat);
	Logger::GetInstance()->Print(messageFormat, args);
	va_end(args);
}

void PrintError(const char * messageFormat, ...)
{
	Logger::GetInstance()->Print("[Error] ");
	va_list args;
	va_start(args, messageFormat);
	Logger::GetInstance()->Print(messageFormat, args);
	va_end(args);
}


Logger * Logger::sInstance = nullptr;

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
