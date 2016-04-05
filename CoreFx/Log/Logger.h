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
	friend class Engine;

public:

	static Logger * GetInstance()
	{
		assert(sInstance != nullptr);
		return sInstance;
	}

	void Print(const char* messageFormat, ...);
	void Print(const char* messageFormat, va_list args);


protected:
	Logger();
	virtual ~Logger();

	static Logger * sInstance;
};


extern void PrintMessage(const char * messageFormat, ...);

// =======================================================================
// =======================================================================
} // namepsace Log
} // namespace CoreFx
#endif // COREFX_LOG_LOGGER_H