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
	LoadFromFile(GL_VERTEX_SHADER, "shaders/vertex_normal.vs.glsl");
	LoadFromFile(GL_GEOMETRY_SHADER, "shaders/vertex_normal.gs.glsl");
	LoadFromFile(GL_FRAGMENT_SHADER, "shaders/vertex_normal.fs.glsl");

	const char * attributeName[__attributes_count__] =
	{
		"in_Position",
		"in_Normal"
	};

	const char * uniformNames[__uniforms_count__] =
	{
		"perInstanceDataSampler",
	};

	CreateAndLinkProgram();

	Use();
		AddAttributes(attributeName, __attributes_count__);

		AddUniforms(uniformNames, __uniforms_count__);

		//pass values of constant uniforms at initialization
		glUniform1i(GetUniform(perInstanceDataSampler), 0);

		SetupFrameDataBlockBinding();
	UnUse();

	GL_CHECK_ERRORS;


	std::cout << std::endl;

}

void DrawNormalShader::SetUniformValues() const
{
	//Engine * engine = Engine::GetInstance();

}

	} // namespace Renderers
} // namespace CoreFx