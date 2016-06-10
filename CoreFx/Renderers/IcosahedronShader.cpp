#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



BasicIcosahedronShader::BasicIcosahedronShader(const char * shaderTitle)
	: Shader(shaderTitle)
{
}


BasicIcosahedronShader::~BasicIcosahedronShader()
{
}
 
void BasicIcosahedronShader::LoadShaders()
{
	std::cout << std::endl;
	std::cout << "Load 'Basic Icosahedron Shader' shader..." << std::endl;

	//setup shader
	LoadFromFile(GL_VERTEX_SHADER, "shaders/IcosahedronShader.vs.glsl");
	LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/IcosahedronShader.tcs.glsl");
	LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/IcosahedronShader.tes.glsl");
	LoadFromFile(GL_FRAGMENT_SHADER, "shaders/IcosahedronShader.forward.fs.glsl");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_InnerTessLevel",
		"u_OuterTessLevel",
		"u_DrawColor",
	};

	CreateAndLinkProgram();

	Use();
		AddUniforms(uniformNames, __uniforms_count__);


		SetupFrameDataBlockBinding();
	UnUse();

	GL_CHECK_ERRORS;


	std::cout << std::endl;

}

	} // namespace Renderers
} // namespace CoreFx