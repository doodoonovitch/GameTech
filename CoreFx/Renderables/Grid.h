#pragma once

#ifndef COREFX_RENDERABLES_GRID_H
#define COREFX_RENDERABLES_GRID_H


namespace CoreFx
{
	namespace Renderables
	{


class Grid : public RenderableObject<1>
{
public:
	Grid(int width=10, int depth=10);
	virtual ~Grid();

	virtual void Render(glm::mat4 const & VP) override;

private:

	int m_vertexCount;
};



	} // namespace Renderables
} // namespace CoreFx
#endif // COREFX_RENDERABLES_GRID_H