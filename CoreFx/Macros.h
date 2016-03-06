#ifndef COREFX_MACROS_H
#define COREFX_MACROS_H
#pragma once


#define GL_CHECK_ERRORS \
{ \
	GLenum err = glGetError(); \
	if (err != GL_NO_ERROR) \
	{ \
		std::cerr << "Error " << err << std::endl; \
	} \
}

#define SAFE_DELETE(p) \
	delete p; \
	p = nullptr

#define SAFE_DELETE_ARRAY(p) \
	delete [] p; \
	p = nullptr



#endif // COREFX_MACROS_H