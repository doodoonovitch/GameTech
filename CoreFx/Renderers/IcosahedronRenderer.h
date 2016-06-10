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
	IcosahedronRendererBase(GLuint capacity);
	virtual ~IcosahedronRendererBase();

public:

	enum EUniformIndex
	{
		u_InnerTessLevel,
		u_OuterTessLevel,
		u_DrawColor,

		__uniforms_count__
	};

	enum VBOIndex
	{
		VBO_Vertex = 0,
		VBO_Index = 1,
	};

protected:

	virtual void LoadShaders();

	void Initialize();

protected:

	GLuint mVertexArrayCapacity;
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