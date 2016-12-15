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
#include <list>
#include <algorithm>
#include <iterator>
#include <functional>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <string>
#include <thread>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <ft2build.h>
#include FT_FREETYPE_H

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
#include "Maths\Helpers.h"
#include "Maths\SolarPosition.h"

#include "InstanceContainer.h"
#include "PropertyData.h"
#include "Texture2D.h"
#include "Shader.h"
#include "Frame.h"
#include "SceneObject.h"
#include "ShaderStorageBuffer.h"
#include "TextureBuffer.h"
#include "Renderer.h"
#include "ComputeShader.h"
#include "Renderables\VertexArrayObject.h"
#include "Renderers\DebugTools\DrawGBufferNormalShader.h"
#include "Renderers\RendererHelper.h"
#include "Renderers\DebugTools\IcosahedronRenderer.h"
#include "Renderers\DebugTools\ConeRenderer.h"
#include "Renderers\DebugTools\LightPositionRenderer.h"

#include "Lights\Light.h"
#include "Lights\PointLight.h"
#include "Lights\SpotLight.h"
#include "Lights\DirectionalLight.h"

#include "DDS.h"
#include "TextureManager.h"

#include "Engine.h"

#include "Geometry\Helpers.h"
#include "Geometry\ModelData.h"


#include "Camera.h"
#include "Renderables\Axis.h"
#include "Renderables\Grid.h"
#include "Renderables\Model.h"

#include "Renderers\DebugTools\GridRenderer.h"
#include "Renderers\DebugTools\AxisRenderer.h"
#include "Renderers\DebugTools\RadialGridRenderer.h"
#include "Renderers\TerrainRenderer.h"
#include "Renderers\PerlinNoiseOcean\PerlinNoiseOceanWaveProperty.h"
#include "Renderers\PerlinNoiseOcean\PerlinNoiseOceanHeightMapCS.h"
#include "Renderers\PerlinNoiseOcean\PerlinNoiseOceanRenderer.h"
#include "Renderers\PerlinNoiseOcean\RadialGridOceanRenderer.h"
#include "Renderers\DeepOcean\DeepOceanWaveProperty.h"
#include "Renderers\DeepOcean\DeepOceanHeightMapCS.h"
#include "Renderers\DeepOcean\DeepOceanRenderer.h"
#include "Renderers\GerstnerWaves\GerstnerWavesWaveProperty.h"
#include "Renderers\GerstnerWaves\GerstnerWavesHeightMapCS.h"
#include "Renderers\GerstnerWaves\GerstnerWavesRenderer.h"
#include "Renderers\SkyboxRenderer.h"
#include "Renderers\SkydomeRenderer.h"
#include "Renderers\CompassRenderer.h"
#include "Renderers\ModelRenderer.h"
#include "Renderers\Text2D\TextPage.h"
#include "Renderers\Text2D\TextGroup.h"
#include "Renderers\Text2D\TextRenderer.h"


#endif // COREFX_COREFX_H