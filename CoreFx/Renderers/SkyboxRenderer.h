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
	SkyboxRenderer(const std::string & skyboxCubeMapTextureFilename, bool isHDR = false);
	SkyboxRenderer(CubeMapTexture const * skyboxCubeMapTexture, bool isHDR);
	virtual ~SkyboxRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

	bool IsHDR() const { return mIsHDR; }

private:

	void Initialize();

	enum EUniformIndex
	{
		u_SkyboxCubeMapSampler,

		__uniforms_count__
	};

	CubeMapTexture const * mCubeMapTexture;
	bool mIsHDR;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_SKYBOXRENDERER_H