#pragma once

#ifndef COREFX_RENDERERS_ICOSAHEDRONRERENDERER_H
#define COREFX_RENDERERS_ICOSAHEDRONRERENDERER_H


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

	enum EUniformIndex
	{
		u_InnerTessLevel,
		u_OuterTessLevel,
		u_perInstanceDataSampler,
		u_DrawColor,

		__uniforms_count__
	};

	enum VBOIndex
	{
		VBO_Vertex = 0,
		VBO_Index = 1,
	};

protected:

	virtual void LoadShaders(const char * vs = "shaders/IcosahedronShader.vs.glsl", const char * tcs = "shaders/IcosahedronShader.tcs.glsl", const char * tes = "shaders/IcosahedronShader.tes.glsl", const char * gs = "shaders/IcosahedronShader.gs.glsl", const char * fs = "shaders/IcosahedronShader.forward.fs.glsl");

	void Initialize(const char * vs = "shaders/IcosahedronShader.vs.glsl", const char * tcs = "shaders/IcosahedronShader.tcs.glsl", const char * tes = "shaders/IcosahedronShader.tes.glsl", const char * gs = "shaders/IcosahedronShader.gs.glsl", const char * fs = "shaders/IcosahedronShader.forward.fs.glsl");

	void InternalRender(GLuint instanceCount, GLuint instanceDataBufferId);
	void InternalRenderWireFrame(GLuint instanceCount, GLuint instanceDataBufferId);


protected:

	GLuint mIndexCount;
};


class IcosahedronRenderer : IcosahedronRendererBase
{
public:
	IcosahedronRenderer(GLuint capacity);
	virtual ~IcosahedronRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;


protected:

	typedef std::vector<glm::vec4> Vec4StdVector;

	Vec4StdVector mSphereList;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_ICOSAHEDRONRERENDERER_H