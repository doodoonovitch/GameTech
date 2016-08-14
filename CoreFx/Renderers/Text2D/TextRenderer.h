#pragma once

#ifndef COREFX_RENDERERS_TEXTRENDERER_H
#define COREFX_RENDERERS_TEXTRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class TextRenderer : public RendererHelper<1>
{
public:
	TextRenderer();
	virtual ~TextRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

private:

	enum EAttributeIndex
	{
		u_Color,
		__attributes_count__
	};
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TEXTRENDERER_H