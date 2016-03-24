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



#endif // COREFX_MACROS_H