#pragma once

#ifndef COREFX_RENDERABLES_GRID_H
#define COREFX_RENDERABLES_GRID_H


namespace CoreFx
{
	namespace Renderables
	{


class Grid : public SceneObject
{
public:
	Grid();
	virtual ~Grid();

private:

	int m_vertexCount;
};



	} // namespace Renderables
} // namespace CoreFx
#endif // COREFX_RENDERABLES_GRID_H