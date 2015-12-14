#pragma once

#ifndef COREFX_RENDERABLES_GRID_H
#define RENDERABLES_GRID_H


namespace CoreFx
{
	namespace Renderables
	{


class Grid : public RenderableObject
{
public:
	Grid(int width=10, int depth=10);
	virtual ~Grid();

	void Render(const float* MVP);
};



	} // namespace Renderables
} // namespace CoreFx
#endif // RENDERABLES_GRID_H