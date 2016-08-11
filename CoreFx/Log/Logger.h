#ifndef COREFX_LOG_LOGGER_H
#define COREFX_LOG_LOGGER_H

#pragma once

namespace CoreFx
{

class Engine;

namespace Log
{
// =======================================================================
// =======================================================================


class Logger
{
public:

	static Logger * GetInstance()
	{
		assert(sInstance != nullptr);
		return sInstance;
	}

	void Print(const char* messageFormat, ...);
	void Print(const char* messageFormat, va_list args);

	static void InitializeLogger();
	static void ReleaseLogger();

protected:
	Logger();
	virtual ~Logger();

	static Logger * sInstance;

};


extern void PrintMessage(const char * messageFormat, ...);
extern void PrintError(const char * messageFormat, ...);
extern void PrintWarning(const char * messageFormat, ...);
extern void PrintMessage(const char * messageFormat, va_list args);
extern void PrintError(const char * messageFormat, va_list args);
extern void PrintWarning(const char * messageFormat, va_list args);

// =======================================================================
// =======================================================================
} // namepsace Log
} // namespace CoreFx
#endif // COREFX_LOG_LOGGER_H