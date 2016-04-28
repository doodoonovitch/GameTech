#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



DrawGBufferNormalShader::DrawGBufferNormalShader()
	: Shader("DrawGBufferNormalShader")
{
}


DrawGBufferNormalShader::~DrawGBufferNormalShader()
{
}
 
void DrawGBufferNormalShader::LoadShaders()
{
	std::cout << std::endl;
	std::cout << "Load 'draw GBuffer normal' shader..." << std::endl;

	//setup shader
	LoadFromFile(GL_VERTEX_SHADER, "shaders/drawGBufferNormals.vs.glsl");
	LoadFromFile(GL_GEOMETRY_SHADER, "shaders/drawGBufferNormals.gs.glsl");
	LoadFromFile(GL_FRAGMENT_SHADER, "shaders/drawGBufferNormals.forward.fs.glsl");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_gBufferPosition",
		"u_gBufferNormal",
	};

	CreateAndLinkProgram();

	Use();
		AddUniforms(uniformNames, __uniforms_count__);

		//pass values of constant uniforms at initialization
		glUniform1i(GetUniform(u_gBufferPosition), 0);
		glUniform1i(GetUniform(u_gBufferNormal), 1);

		SetupFrameDataBlockBinding();
	UnUse();

	GL_CHECK_ERRORS;


	std::cout << std::endl;

}

void DrawGBufferNormalShader::SetUniformValues() const
{
	//Engine * engine = Engine::GetInstance();

}

	} // namespace Renderers
} // namespace CoreFx