#ifndef COREFX_MACROS_H
#define COREFX_MACROS_H
#pragma once


#define GL_CHECK_ERRORS \
{\
	GLenum err_GL_CHECK_ERRORS = glGetError();\
	if (err_GL_CHECK_ERRORS != GL_NO_ERROR) {\
	std::cerr << "Error " << err_GL_CHECK_ERRORS << " : " << gluErrorString(err_GL_CHECK_ERRORS) << std::endl;\
	assert(false);}\
}

#define SAFE_DELETE(p) \
	delete p; \
	p = nullptr

#define SAFE_DELETE_ARRAY(p) \
	delete [] p; \
	p = nullptr

//namespace CoreFx
//{
//	namespace Log
//	{
//		extern void PrintMessage(const char* messageFormat, ...);
//	}
//}

#ifndef DISABLE_PRINT_MESSAGE
#define PRINT_MESSAGE(messageFormat, ...) CoreFx::Log::PrintMessage(messageFormat, __VA_ARGS__);
#define PRINT_ERROR(messageFormat, ...) CoreFx::Log::PrintError(messageFormat, __VA_ARGS__);
#define PRINT_WARNING(messageFormat, ...) CoreFx::Log::PrintWarning(messageFormat, __VA_ARGS__);
#define PRINT_MESSAGE_VA(messageFormat, valist) CoreFx::Log::PrintMessage(messageFormat, valist);
#define PRINT_ERROR_VA(messageFormat, valist) CoreFx::Log::PrintError(messageFormat, valist);
#define PRINT_WARNING_VA(messageFormat, valist) CoreFx::Log::PrintWarning(messageFormat, valist);
#else
#define PRINT_MESSAGE(messageFormat, ...) {}
#define PRINT_ERROR(messageFormat, ...) {}
#define PRINT_WARNING(messageFormat, ...) {}
#define PRINT_MESSAGE_VA(messageFormat, valist) {}
#define PRINT_ERROR_VA(messageFormat, valist) {}
#define PRINT_WARNING_VA(messageFormat, valist) {}
#endif


#define PRINT_GEN_FRAMEBUFFER(prefix, buffer) PRINT_MESSAGE(prefix " Framebuffer " #buffer " : %i.", buffer)
#define PRINT_GEN_RENDERBUFFER(prefix, buffer) PRINT_MESSAGE(prefix " RenderBuffer " #buffer " : %i.", buffer)
#define PRINT_GEN_TEXTURE(prefix, buffer) PRINT_MESSAGE(prefix " Texbuffer " #buffer " : %i.", buffer)
#define PRINT_GEN_BUFFER(prefix, buffer) PRINT_MESSAGE(prefix " Buffer " #buffer " : %i.", buffer)
#define PRINT_GEN_TEXTUREBUFFER(prefix, buffer) PRINT_MESSAGE(prefix " Texture / Buffer " #buffer " : (%i, %i).", buffer.GetTextureId(), buffer.GetBufferId());
#define PRINT_GEN_VERTEXARRAY(prefix, buffer) PRINT_MESSAGE(prefix " Vertex " #buffer " : %i.", buffer)

#define PRINT_BEGIN_SECTION PRINT_MESSAGE("- - - - - - - - - - - - - - - - - - - -")
#define PRINT_END_SECTION PRINT_MESSAGE(". . .\n")

#endif // COREFX_MACROS_H