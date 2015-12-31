#ifndef COREFX_COREFX_H
#define COREFX_COREFX_H

#pragma once

#pragma warning ( disable:4505 )

#define _USE_MATH_DEFINES
#include <cmath>


#include <GL/glew.h>
#include <GL/freeglut.h>


#pragma warning( push )
#pragma warning ( disable:4201 4310 )

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <SOIL.h>

#pragma warning(pop)


#include <iostream>
#include <map>

#define GL_CHECK_ERRORS \
{\
	GLenum err = glGetError();\
	if (err != GL_NO_ERROR) {\
	std::cerr << "Error " << err << " : " << gluErrorString(err) << std::endl;\
	assert(false);}\
}


#include "Frame.h"
#include "SceneObject.h"

#include "Shader.h"
#include "Camera.h"
#include "Renderables\RenderableObject.h"
#include "Renderables\Grid.h"
#include "Renderables\Cube.h"



#endif // COREFX_COREFX_H