#pragma once

#ifndef COREFX_RENDERERS_TERRAINRENDERER_H
#define COREFX_RENDERERS_TERRAINRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class TerrainRenderer : public RendererHelper<Renderables::Grid, 1>
{
public:
	TerrainRenderer(GLint heightMapWidth, GLint heightMapDepth, glm::vec3 const & scale);
	virtual ~TerrainRenderer();

	virtual void Render() override;
	virtual void DebugRender() override	{};

private:

	enum EUniformIndex
	{
		u_PatchCount,
		u_MapSize, 
		u_Scale,
		u_PatchPerTexture,

		__uniforms_count__
	};

	glm::ivec2 mMapSize;
	glm::ivec2 mPatchCount;
	glm::vec3 mScale;
	glm::int1 mPatchPerTexture;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TERRAINRENDERER_H