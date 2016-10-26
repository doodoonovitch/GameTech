#pragma once

#ifndef COREFX_RENDERERS_DEBUGTOOLS_ICOSAHEDRONRERENDERER_H
#define COREFX_RENDERERS_DEBUGTOOLS_ICOSAHEDRONRERENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class IcosahedronRendererBase : public RendererHelper<2>
{
public:
	IcosahedronRendererBase(const char * shaderTitle = "IcosahedronRenderer", const char * wireFramShaderTitle = "IcosahedronWireFrameRendererBase");
	virtual ~IcosahedronRendererBase();

public:

	//enum EUniformIndex
	//{
	//	u_InnerTessLevel,
	//	u_OuterTessLevel,

	//	__uniforms_count__
	//};

	const GLuint mIndexCount = 60;

protected:

	void LoadShaders(Shader & shader, const char * gs = "shaders/IcosahedronShader.gs.glsl", const char * fs = "shaders/IcosahedronShader.forward.fs.glsl", const char * vs = "shaders/IcosahedronShader.vs.glsl", const char * tcs = "shaders/IcosahedronShader.tcs.glsl", const char * tes = "shaders/IcosahedronShader.tes.glsl");
	void InitializeVertexBuffer();
	//void InitializeUniforms(Shader & shader);

	//void InternalRender(Shader & shader, GLsizei instanceCount, GLuint instanceDataBufferId);
	//void InternalRender(GLsizei instanceCount, GLuint instanceDataBufferId);
	//void InternalRenderWireFrame(GLsizei instanceCount, GLuint instanceDataBufferId);


protected:

	enum VBOIndex
	{
		VBO_Vertex = 0,
		VBO_Index = 1,
	};
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_DEBUGTOOLS_ICOSAHEDRONRERENDERER_H