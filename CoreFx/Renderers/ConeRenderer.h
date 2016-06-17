#pragma once

#ifndef COREFX_RENDERERS_CONERERENDERER_H
#define COREFX_RENDERERS_CONERERENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class ConeRendererBase : public RendererHelper<1>
{
public:
	ConeRendererBase(const char * shaderTitle = "ConeRenderer", const char * wireFramShaderTitle = "ConeWireFrameRendererBase");
	virtual ~ConeRendererBase();

public:

	//enum EUniformIndex
	//{
	//	u_InnerTessLevel,
	//	u_OuterTessLevel,

	//	__uniforms_count__
	//};

protected:

	GLuint mIndexCount;

	void LoadShaders(Shader & shader, const char * gs = "shaders/ConeShader.gs.glsl", const char * fs = "shaders/ConeShader.forward.fs.glsl", const char * vs = "shaders/ConeShader.vs.glsl");
	void InitializeVertexBuffer(GLuint numStrips);


protected:

};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_CONERERENDERER_H