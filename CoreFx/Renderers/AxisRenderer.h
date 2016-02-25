#pragma once

#ifndef COREFX_RENDERERS_AXISRENDERER_H
#define COREFX_RENDERERS_AXISRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class AxisRenderer : public RendererHelper<Renderables::Axis, 1>
{
public:
	AxisRenderer();
	virtual ~AxisRenderer();

	virtual void BeginFrame() override {}
	virtual void EndFrame() override {}

	virtual void Render() override;

private:

	struct Vertex
	{
		__declspec(align(4)) glm::vec3 pos;
		__declspec(align(4)) glm::vec4 color;
	};
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_AXISRENDERER_H