#pragma once

#ifndef COREFX_RENDERERS_GRIDRENDERER_H
#define COREFX_RENDERERS_GRIDRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class GridRenderer : public RendererHelper<Renderables::Grid, 1>
{
public:
	GridRenderer(int width=10, int depth=10);
	virtual ~GridRenderer();

	virtual void Render() override {};
	virtual void DebugRender() override;

private:

	int m_vertexCount;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_GRIDRENDERER_H