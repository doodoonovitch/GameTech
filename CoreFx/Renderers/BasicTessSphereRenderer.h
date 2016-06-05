#pragma once

#ifndef COREFX_RENDERERS_BASICTESSSPHERERENDERER_H
#define COREFX_RENDERERS_BASICTESSSPHERERENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class BasicTessSphereRendererBase : public RendererHelper<1, BasicTessSphereShader, Shader>
{
public:
	BasicTessSphereRendererBase(GLuint maxSphereCount);
	virtual ~BasicTessSphereRendererBase();

protected:

	void Initialize();
	void UpdateVertexArrayBuffer(std::uint8_t * buffer, GLuint vertexCount, GLuint vertexArrayCapacity);

protected:

	GLuint mVertexArrayCapacity;
};


class BasicTessSphereRenderer : BasicTessSphereRendererBase
{
public:
	BasicTessSphereRenderer(GLuint maxSphereCount);
	virtual ~BasicTessSphereRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

protected:

	void UpdateVertexArrayBuffer();

protected:

	typedef std::vector<glm::vec4> Vec4StdVector;

	Vec4StdVector mSphereList;
	bool mIsModified;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_BASICTESSSPHERERENDERER_H