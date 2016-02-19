#pragma once

#ifndef COREFX_RENDERERS_AXISRENDERER_H
#define COREFX_RENDERERS_AXISRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class AxisRenderer : public SceneObjectRenderer<Renderables::Axis, 1>
{
public:
	AxisRenderer(size_t capacity = 2, size_t pageSize = 2);
	virtual ~AxisRenderer();

	virtual void Render() override;

private:

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec4 color;
	};
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_AXISRENDERER_H