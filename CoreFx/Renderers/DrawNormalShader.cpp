#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



DrawNormalShader::DrawNormalShader()
	: Shader("DrawNormalShader")
{
}


DrawNormalShader::~DrawNormalShader()
{
}
 
void DrawNormalShader::LoadShaders()
{
	std::cout << std::endl;
	std::cout << "Load 'draw vertex normal' shader..." << std::endl;

	//setup shader
	LoadFromFile(GL_VERTEX_SHADER, "shaders/draw_vertex_normal_shader.vert");
	LoadFromFile(GL_GEOMETRY_SHADER, "shaders/draw_vertex_normal_shader.geom");
	LoadFromFile(GL_FRAGMENT_SHADER, "shaders/draw_vertex_normal_shader.frag");

	const char * attributeName[__attributes_count__] =
	{
		"in_Position",
		"in_Normal"
	};

	const char * uniformNames[__uniforms_count__] =
	{
	"u_NormalMagnitude",
	"u_LightMagnitude",
	"u_VertexNormalColor",
	"u_PointLightColor",
	"u_DirectionalLightColor",
	"u_FirstLightIndex",
	"u_DrawLightCount",
	"perInstanceDataSampler",
	"lightDescSampler",
	"lightDataSampler"
	};

	CreateAndLinkProgram();

	Use();
		AddAttributes(attributeName, __attributes_count__);

		AddUniforms(uniformNames, __uniforms_count__);

		//pass values of constant uniforms at initialization
		glUniform1i(GetUniform(perInstanceDataSampler), 0);
		glUniform1i(GetUniform(lightDescSampler), 1);
		glUniform1i(GetUniform(lightDataSampler), 2);

		SetupFrameDataBlockBinding();
	UnUse();

	GL_CHECK_ERRORS;


	std::cout << std::endl;

}

void DrawNormalShader::SetUniformValues() const
{
	Engine * engine = Engine::GetInstance();

	glUniform1f(GetUniform(u_NormalMagnitude), engine->GetDrawVertexNormalMagnitude());
	glUniform1f(GetUniform(u_LightMagnitude), engine->GetDrawLightMagnitude());
	glUniform4fv(GetUniform(u_VertexNormalColor), 1, glm::value_ptr(engine->GetDrawVertexNormalColor()));
	glUniform4fv(GetUniform(u_PointLightColor), 1, glm::value_ptr(engine->GetDrawPointLightColor()));
	glUniform4fv(GetUniform(u_DirectionalLightColor), 1, glm::value_ptr(engine->GetDrawDirectionalLightColor()));
	glUniform1i(GetUniform(u_FirstLightIndex), engine->GetFirstLightIndexToDraw());	
	glUniform1i(GetUniform(u_DrawLightCount), engine->GetLightToDrawCount());
}

	} // namespace Renderers
} // namespace CoreFx