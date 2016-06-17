#ifndef COREFX_COREFX_H
#define COREFX_COREFX_H

#pragma once

#pragma warning ( disable:4505 )

#define _USE_MATH_DEFINES
#include <cmath>


#include <GL/glew.h>
//#include <GL/freeglut.h>


#pragma warning( push )
#pragma warning ( disable:4201 4310 )

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <ktx.h>

#pragma warning(pop)


#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>
#include <cstdint>

#include "macros.h"

namespace CoreFx
{

	template<typename T> void SafeDelete(T*& p)
	{
		delete p;
		p = nullptr;
	}

	template<typename T> void SafeDeleteArray(T*& p)
	{
		delete[] p;
		p = nullptr;
	}

}; // namespace CoreFx

#include "Log\Logger.h"
#include "DebugOutput.h"

#include "Maths\DualQuaternion.h"

#include "InstanceContainer.h"
#include "PropertyData.h"
#include "Texture2D.h"
#include "Shader.h"
#include "Frame.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Renderables\VertexArrayObject.h"
#include "Renderers\DrawNormalShader.h"
#include "Renderers\DrawGBufferNormalShader.h"
#include "Renderers\RendererHelper.h"
#include "Renderers\IcosahedronRenderer.h"
#include "Renderers\ConeRenderer.h"

#include "TextureBuffer.h"

#include "Lights\Light.h"
#include "Lights\PointLight.h"
#include "Lights\SpotLight.h"
#include "Lights\DirectionalLight.h"

#include "TextureManager.h"

#include "Engine.h"

#include "Geometry\VertexData.h"
#include "Geometry\MaterialData.h"
#include "Geometry\MeshData.h"


#include "Camera.h"
#include "Renderables\Axis.h"
#include "Renderables\Grid.h"
#include "Renderables\Cube.h"
#include "Renderables\Mesh.h"

#include "Renderers\CubeRenderer.h"
#include "Renderers\GridRenderer.h"
#include "Renderers\AxisRenderer.h"
#include "Renderers\TerrainRenderer.h"
#include "Renderers\DeepOceanRenderer.h"
#include "Renderers\ShallowWaterRenderer.h"
#include "Renderers\SkyboxRenderer.h"

#endif // COREFX_COREFX_H