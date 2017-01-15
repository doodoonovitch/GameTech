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
	Logger::GetInstance()->Print("##### [Error] ##### ");
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

void PrintMatrix(const glm::mat4 & m)
{
	const glm::mat4::col_type & c0 = m[0];
	const glm::mat4::col_type & c1 = m[1];
	const glm::mat4::col_type & c2 = m[2];
	const glm::mat4::col_type & c3 = m[3];
	PrintMessage("\t%f \t%f \t%f \t%f", c0.x, c1.x, c2.x, c3.x);
	PrintMessage("\t%f \t%f \t%f \t%f", c0.y, c1.y, c2.y, c3.y);
	PrintMessage("\t%f \t%f \t%f \t%f", c0.z, c1.z, c2.z, c3.z);
	PrintMessage("\t%f \t%f \t%f \t%f", c0.w, c1.w, c2.w, c3.w);
}

Logger * Logger::sInstance = nullptr;

void Logger::InitializeLogger()
{
	CoreFx::Log::Logger::sInstance = new CoreFx::Log::Logger();
}

void Logger::ReleaseLogger()
{
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
	std::lock_guard<std::mutex> lock(mMutex);
	vprintf_s(messageFormat, args);
}

// =======================================================================
// =======================================================================
} // namepsace Log
} // namespace CoreFx
