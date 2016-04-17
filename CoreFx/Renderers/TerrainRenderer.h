#pragma once

#ifndef COREFX_RENDERERS_TERRAINRENDERER_H
#define COREFX_RENDERERS_TERRAINRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


struct TerrainDesc
{
	TerrainDesc(const char * filename, GLint heightMapTextureWidth, bool invertY)
		: mFilename(filename)
		, mHeightMapTextureWidth(heightMapTextureWidth)
		, mInvertY(invertY)
	{}

	std::string mFilename;
	GLint mHeightMapTextureWidth;
	bool mInvertY = true;

	//glm::vec3 mPosition;
	//glm::quat mRotation;
};

typedef std::vector<TerrainDesc> TerrainDescList;

struct TerrainRendererDesc
{
	TerrainRendererDesc(GLint heightMapWidth, GLint heightMapDepth, const glm::vec3 & scale)
		: mHeightMapWidth(heightMapWidth)
		, mHeightMapDepth(heightMapDepth)
		, mScale(scale)
	{ }

	GLint mHeightMapWidth;
	GLint mHeightMapDepth;
	glm::vec3 mScale;

	TerrainDescList mTerrains;
};

class TerrainRenderer : public RendererHelper<Renderables::Grid, 1>
{
public:
	TerrainRenderer(const TerrainRendererDesc & desc);
	virtual ~TerrainRenderer();

	virtual void Render() override;
	virtual void DebugRender() override;

	virtual void UpdateMaterialTextureIndex() override;

private:

	void LoadShaders();
	void LoadHeightMap(const TerrainDescList & terrainDescList);

private:

	enum EUniformIndex
	{
		u_PatchCount,
		u_MapSize,
		u_Scale,
		u_TexScale,
		u_HeightMap,
		u_DiffuseMap,

		__uniforms_count__
	};

	enum EUniformIndex2
	{
		u_NormalMagnitude,
		u_VertexNormalColor,
		//perInstanceDataSampler,

		__uniforms2_count__
	};


	glm::ivec2 mMapSize;
	glm::ivec2 mPatchCount;
	glm::vec3 mScale;
	GLuint mHeightMapTextureId;
	Shader mDrawNormalShader;
	const TextureGroup * mDiffuseTextures;

};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TERRAINRENDERER_H