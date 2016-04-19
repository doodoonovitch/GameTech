#pragma once
#ifndef COREFX_DEBUGOUTPUT_H
#define COREFX_DEBUGOUTPUT_H


namespace CoreFx
{
	// =======================================================================
	// =======================================================================



class DebugOutput
{
public:

	static DebugOutput * GetInstance()
	{
		return &sInstance;
	}

	void EnableDebugMessage();
	void DisableDebugMessage();

	bool IsEnabled() const
	{
		return mIsEnabled;
	}

	//if you want you can rename myCallback; This is just an example
	static void APIENTRY Callback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar *msg,
		GLvoid *data);

private:

	DebugOutput();

	~DebugOutput();

	static const char * getStringForType(GLenum type);

	static const char * getStringForSource(GLenum source);

	static const char * getStringForSeverity(GLenum severity);

	static DebugOutput sInstance;

	GLenum mSource;
	GLenum mType;
	GLenum mSeverity;
	bool mIsEnabled;
};




	// =======================================================================
	// =======================================================================
} // namespace CoreFx
#endif // COREFX_DEBUGOUTPUT_H