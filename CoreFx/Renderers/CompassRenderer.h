#pragma once

#ifndef COREFX_RENDERERS_COMPASSRENDERER_H
#define COREFX_RENDERERS_COMPASSRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class CompassRenderer : public RendererHelper<1>
{
public:
	CompassRenderer();
	virtual ~CompassRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

private:

	static void UpdateUniforms(Shader const & shader);

	enum EUniformIndex
	{
		u_Translation,
		u_ViewDir,
		u_CompassTextureSampler,

		__uniforms_count__
	};


	Texture2D const * mCompassTexture;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_COMPASSRENDERER_H