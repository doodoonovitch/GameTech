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

	struct MapDesc
	{
		MapDesc(const char * filename, GLint heightMapTextureWidth, bool invertY)
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

	typedef std::vector<MapDesc> MapDescList;

	struct MaterialDesc
	{
		MaterialDesc(GLint textureIndex, GLfloat heightMin, GLfloat heightMax)
			: mTextureIndex(textureIndex)
			, mHeightMin(heightMin)
			, mHeightMax(heightMax)
		{}

		GLint mTextureIndex;
		GLfloat mHeightMin;
		GLfloat mHeightMax;
	};

	typedef std::vector<MaterialDesc> MaterialDescList;

	struct Desc : public Renderer::Desc
	{
		Desc(GLint heightMapWidth, GLint heightMapDepth, const glm::vec3 & scale, GLfloat slowSlopeMax = 0.35f, GLfloat hiSlopeMin = 0.30f)
			: Renderer::Desc()
			, mHeightMapWidth(heightMapWidth)
			, mHeightMapDepth(heightMapDepth)
			, mScale(scale)
			, mSlowSlopeMax(slowSlopeMax)
			, mHiSlopeMin(hiSlopeMin)
		{ }

		GLint mHeightMapWidth;
		GLint mHeightMapDepth;
		glm::vec3 mScale;
		GLfloat mSlowSlopeMax;
		GLfloat mHiSlopeMin;

		MapDescList mTerrains;
		MaterialDescList mLowSlopeMaterials;
		MaterialDescList mHiSlopeMaterials;
	};

public:
	TerrainRenderer(const Desc & desc);
	virtual ~TerrainRenderer();

	virtual void Render() override;
	virtual void DebugRender() override;

	virtual void UpdateMaterialTextureIndex() override;

private:

	void LoadShaders();
	void LoadHeightMap(const MapDescList & terrainDescList);

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