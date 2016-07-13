#pragma once

#ifndef COREFX_RENDERERS_SKYDOMERENDERER_H
#define COREFX_RENDERERS_SKYDOMERENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class SkydomeRenderer : public RendererHelper<2>
{
public:
	SkydomeRenderer(const std::string & skyCubeMapTextureFilename, const int rings = 128, const int segments = 128);
	virtual ~SkydomeRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

protected:

	void CreateGeometry(std::vector<glm::vec3> & vertices, std::vector<GLushort> & indices, GLfloat radius, const GLfloat height, const int rings, const int segments);

private:

	enum EUniformIndex
	{
		u_SkyboxCubeMapSampler,

		__uniforms_count__
	};

	CubeMapTexture const * mCubeMapTexture;
	GLsizei mIndiceCount;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_SKYDOMERENDERER_H