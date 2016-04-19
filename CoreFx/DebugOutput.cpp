#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	// =======================================================================
	// =======================================================================

DebugOutput DebugOutput::sInstance;

DebugOutput::DebugOutput()
#ifdef _DEBUG
	: mSource(GL_DONT_CARE)
	, mType(GL_DONT_CARE)
	, mSeverity(GL_DONT_CARE)
#else
	: mSource(GL_DONT_CARE)
	, mType(GL_DEBUG_TYPE_ERROR)
	, mSeverity(GL_DEBUG_SEVERITY_HIGH)
#endif
	, mIsEnabled(false)
{
}

DebugOutput::~DebugOutput()
{
}

void DebugOutput::EnableDebugMessage()
{
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	if (major > 4 || (major == 4 && minor >= 3))
	{
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(CoreFx::DebugOutput::Callback, this);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_FALSE);
		glDebugMessageControl(mSource, mType, mSeverity, 0, NULL, GL_TRUE);
		mIsEnabled = true;
	}
}

void DebugOutput::DisableDebugMessage()
{
	if (mIsEnabled)
	{
		mIsEnabled = false;
		glDisable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(nullptr, nullptr);
	}
}

//if you want you can rename myCallback; This is just an example
void APIENTRY DebugOutput::Callback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei /*length*/,
	const GLchar *msg,
	GLvoid * data)
{
	DebugOutput * dbg = (DebugOutput*)data;
	if (dbg->IsEnabled())
	{
		//display warnings/errors however you like
		PRINT_MESSAGE("\n********** Debug Output **************\n");
		PRINT_MESSAGE("  source: %s.\n", getStringForSource(source));
		PRINT_MESSAGE("  id: %u.\n", id);
		PRINT_MESSAGE("  type: %s.\n", getStringForType(type));
		PRINT_MESSAGE("  severity: %s.\n", getStringForSeverity(severity));
		PRINT_MESSAGE("  debug call: %s.\n", msg);
		PRINT_MESSAGE("**************************************\n");
	}
}

const char * DebugOutput::getStringForType(GLenum type)
{
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		return"Error";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return "Deprecated behavior";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return "Undefined behavior";
	case GL_DEBUG_TYPE_PORTABILITY:
		return "Portability issue";
	case GL_DEBUG_TYPE_PERFORMANCE:
		return "Performance issue";
	case GL_DEBUG_TYPE_MARKER:
		return "Stream annotation";
	case GL_DEBUG_TYPE_OTHER_ARB:
		return "Other";
	default:
		assert(false);
		return "";
	}
}

const char * DebugOutput::getStringForSource(GLenum source)
{
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		return "API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return "Window system";
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return "Shader compiler";
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return "Third party";
	case GL_DEBUG_SOURCE_APPLICATION:
		return "Application";
	case GL_DEBUG_SOURCE_OTHER:
		return "Other";
	default:
		assert(false);
		return "";
	}
}

const char * DebugOutput::getStringForSeverity(GLenum severity)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		return "High";
	case GL_DEBUG_SEVERITY_MEDIUM:
		return "Medium";
	case GL_DEBUG_SEVERITY_LOW:
		return "Low";
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		return "Notification";
	default:
		assert(false);
		return("");
	}
}








	// =======================================================================
	// =======================================================================
} // namespace CoreFx
