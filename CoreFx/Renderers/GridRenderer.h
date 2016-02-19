#pragma once

#ifndef COREFX_RENDERERS_GRIDRENDERER_H
#define COREFX_RENDERERS_GRIDRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class GridRenderer : public SceneObjectRenderer<Renderables::Grid, 1>
{
public:
	GridRenderer(int width=10, int depth=10, size_t capacity = 4, size_t pageSize = 2);
	virtual ~GridRenderer();

	virtual void Render() override;

private:

	int m_vertexCount;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_GRIDRENDERER_H