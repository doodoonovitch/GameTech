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

namespace CoreFx
{
	namespace Log
	{
		extern void PrintMessage(const char* messageFormat, ...);
	}
}

#ifndef DISABLE_PRINT_MESSAGE
#define PRINT_MESSAGE(messageFormat, ...) CoreFx::Log::PrintMessage(messageFormat, __VA_ARGS__);
#else
#define PRINT_MESSAGE(messageFormat, ...) {}
#endif

#ifndef DISABLE_PRINT_MESSAGE
#define PRINT_ERROR(messageFormat, ...) CoreFx::Log::PrintError(messageFormat, __VA_ARGS__);
#else
#define PRINT_ERROR(messageFormat, ...) {}
#endif

#define PRINT_GEN_FRAMEBUFFER(prefix, buffer) PRINT_MESSAGE(prefix " Framebuffer " #buffer " : %i.\n", buffer)
#define PRINT_GEN_RENDERBUFFER(prefix, buffer) PRINT_MESSAGE(prefix " RenderBuffer " #buffer " : %i.\n", buffer)
#define PRINT_GEN_TEXTURE(prefix, buffer) PRINT_MESSAGE(prefix " Texbuffer " #buffer " : %i.\n", buffer)
#define PRINT_GEN_BUFFER(prefix, buffer) PRINT_MESSAGE(prefix " Buffer " #buffer " : %i.\n", buffer)
#define PRINT_GEN_TEXTUREBUFFER(prefix, buffer) PRINT_MESSAGE(prefix " Texture / Buffer " #buffer " : (%i, %i).\n", buffer.GetTextureId(), buffer.GetBufferId());
#define PRINT_GEN_VERTEXARRAY(prefix, buffer) PRINT_MESSAGE(prefix " Vertex " #buffer " : %i.\n", buffer)


#endif // COREFX_MACROS_H