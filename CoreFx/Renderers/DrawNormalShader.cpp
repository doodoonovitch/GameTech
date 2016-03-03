#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



DrawNormalShader::DrawNormalShader()
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

	CreateAndLinkProgram();

	Use();
		AddAttribute("in_Position");
		AddAttribute("in_Normal");

		AddUniform("u_NormalMagnitude");
		AddUniform("u_LightMagnitude");
		AddUniform("u_VertexNormalColor");
		AddUniform("u_PointLightColor");
		AddUniform("u_DirectionalLightColor");
		AddUniform("u_FirstLightIndex");
		AddUniform("u_DrawLightCount");		

		AddUniform("perInstanceDataSampler");
		AddUniform("lightDescSampler");
		AddUniform("lightDataSampler");

		//pass values of constant uniforms at initialization
		glUniform1i(GetUniform("perInstanceDataSampler"), 0);
		glUniform1i(GetUniform("lightDescSampler"), 1);
		glUniform1i(GetUniform("lightDataSampler"), 2);

		SetupFrameDataBlockBinding();
	UnUse();

	GL_CHECK_ERRORS;

	std::cout << std::endl;
	std::cout << "Vertex attribute index : " << std::endl;
	std::cout << "\t in_Position : " << GetAttribute("in_Position") << std::endl;
	std::cout << "\t in_Normal : " << GetAttribute("in_Normal") << std::endl;
	//std::cout << "\t in_TexUV : " << GetAttribute("in_TexUV") << std::endl;
	std::cout << std::endl;
	std::cout << "Uniform attribute index : " << std::endl;
	std::cout << "\t perInstanceDataSampler : " << GetUniform("perInstanceDataSampler") << std::endl;
	std::cout << "\t u_NormalMagnitude : " << GetUniform("u_NormalMagnitude") << std::endl;
	std::cout << "\t u_LightMagnitude : " << GetUniform("u_LightMagnitude") << std::endl;
	std::cout << "\t u_VertexNormalColor : " << GetUniform("u_VertexNormalColor") << std::endl;
	std::cout << "\t u_PointLightColor : " << GetUniform("u_PointLightColor") << std::endl;
	std::cout << "\t u_DirectionalLightColor : " << GetUniform("u_DirectionalLightColor") << std::endl;
	std::cout << "\t u_FirstLightIndex : " << GetUniform("u_FirstLightIndex") << std::endl;
	std::cout << "\t u_DrawLightCount : " << GetUniform("u_DrawLightCount") << std::endl;

	
	std::cout << std::endl;

}

void DrawNormalShader::SetUniformValues() const
{
	Engine * engine = Engine::GetInstance();

	glUniform1f(GetUniform("u_NormalMagnitude"), engine->GetDrawVertexNormalMagnitude());
	glUniform1f(GetUniform("u_LightMagnitude"), engine->GetDrawLightMagnitude());
	glUniform4fv(GetUniform("u_VertexNormalColor"), 1, glm::value_ptr(engine->GetDrawVertexNormalColor()));
	glUniform4fv(GetUniform("u_PointLightColor"), 1, glm::value_ptr(engine->GetDrawPointLightColor()));
	glUniform4fv(GetUniform("u_DirectionalLightColor"), 1, glm::value_ptr(engine->GetDrawDirectionalLightColor()));
	glUniform1i(GetUniform("u_FirstLightIndex"), engine->GetFirstLightIndexToDraw());	
	glUniform1i(GetUniform("u_DrawLightCount"), engine->GetLightToDrawCount());
}

	} // namespace Renderers
} // namespace CoreFx