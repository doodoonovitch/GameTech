#pragma once

#ifndef COREFX_RENDERERS_SKYBOXRENDERER_H
#define COREFX_RENDERERS_SKYBOXRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class SkyboxRenderer : public RendererHelper<1>
{
public:
	SkyboxRenderer(const std::string & skyboxCubeMapTextureFilename);
	virtual ~SkyboxRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

private:

	enum EUniformIndex
	{
		u_SkyboxCubeMapSampler,

		__uniforms_count__
	};

	CubeMapTexture const * mCubeMapTexture;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_SKYBOXRENDERER_H