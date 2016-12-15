#pragma once

#ifndef COREFX_RENDERERS_RADIALGRIDRENDERER_H
#define COREFX_RENDERERS_RADIALGRIDRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class RadialGridRenderer : public RendererHelper<2>
{
public:
	RadialGridRenderer(GLushort slides = 10, GLushort rings = 10, GLfloat maxDetail = 0.1f, GLfloat gridSize = 40000.f);
	virtual ~RadialGridRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

private:

	enum EUniformIndex
	{
		vGridSize,

		__uniforms_count__
	};

	GLsizei mVertexCount;
	GLsizei mIndexCount;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_RADIALGRIDRENDERER_H