#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



BasicTessSphereShader::BasicTessSphereShader(const char * shaderTitle)
	: Shader(shaderTitle)
{
}


BasicTessSphereShader::~BasicTessSphereShader()
{
}
 
void BasicTessSphereShader::LoadShaders()
{
	std::cout << std::endl;
	std::cout << "Load 'Basic Tessellation Sphere' shader..." << std::endl;

	//setup shader
	LoadFromFile(GL_VERTEX_SHADER, "shaders/BasicTessSphereShader.vs.glsl");
	LoadFromFile(GL_TESS_CONTROL_SHADER, "shaders/BasicTessSphereShader.tcs.glsl");
	LoadFromFile(GL_TESS_EVALUATION_SHADER, "shaders/BasicTessSphereShader.tes.glsl");
	LoadFromFile(GL_FRAGMENT_SHADER, "shaders/BasicTessSphereShader.forward.fs.glsl");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_TessFactor",
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