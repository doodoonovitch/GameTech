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
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Load 'draw GBuffer normal' shader.....");

	//setup shader
	LoadFromFile(GL_VERTEX_SHADER, "shaders/drawGBufferNormals.vs.glsl");
	LoadFromFile(GL_GEOMETRY_SHADER, "shaders/drawGBufferNormals.gs.glsl");
	LoadFromFile(GL_FRAGMENT_SHADER, "shaders/drawGBufferNormals.forward.fs.glsl");

	const char * uniformNames[__uniforms_count__] =
	{
		"u_PatchCount",
		"u_gDepthMap",
		"u_gBufferNormal",
	};

	CreateAndLinkProgram();

	Use();
		AddUniforms(uniformNames, __uniforms_count__);

		//pass values of constant uniforms at initialization
		glUniform1i(GetUniform(u_gDepthMap), 0);
		glUniform1i(GetUniform(u_gBufferNormal), 1);

		SetupFrameDataBlockBinding();
	UnUse();

	GL_CHECK_ERRORS;


	PRINT_MESSAGE(".....'draw GBuffer normal' shader loaded!");
	PRINT_END_SECTION;
}

void DrawGBufferNormalShader::SetUniformValues() const
{
	//Engine * engine = Engine::GetInstance();

}

	} // namespace Renderers
} // namespace CoreFx