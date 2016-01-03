#pragma once

#ifndef COREFX_RENDERABLES_AXIS_H
#define COREFX_RENDERABLES_AXIS_H


namespace CoreFx
{
	namespace Renderables
	{


class Axis : public RenderableObject<1>
{
public:
	Axis();
	virtual ~Axis();

	virtual void Render(glm::mat4 const & VP) override;

private:

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec4 color;
	};
};



	} // namespace Renderables
} // namespace CoreFx
#endif // COREFX_RENDERABLES_AXIS_H