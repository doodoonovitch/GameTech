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
		MapDesc(const char * filename, GLint heightMapTextureWidth, bool invertY, const glm::vec3 & origin, const glm::quat & rotation)
			: mFilename(filename)
			, mHeightMapTextureWidth(heightMapTextureWidth)
			, mInvertY(invertY)
		{
			mModelDQ.SetRotationTranslation(rotation, origin);
		}

		std::string mFilename;
		GLint mHeightMapTextureWidth;
		bool mInvertY = true;

		Maths::DualQuaternion mModelDQ;
	};

	typedef std::vector<MapDesc> MapDescList;

	struct MaterialDesc : Renderer::MaterialDesc
	{
		MaterialDesc(GLfloat heightMin, GLfloat heightMax, 
			const glm::vec3& diffuse, TextureIndex diffuseTextureIndex, 
			const glm::vec3& specular, int8_t specularPower, TextureIndex specularTextureIndex, 
			const glm::vec3& emissive, TextureIndex emissiveTextureIndex, 
			TextureIndex normalTextureIndex, GLfloat texScale = 1.f)
			: Renderer::MaterialDesc(diffuse, diffuseTextureIndex, specular, specularPower, specularTextureIndex, emissive, emissiveTextureIndex, normalTextureIndex)
			, mHeightMin(heightMin)
			, mHeightMax(heightMax)
			, mTexScale(texScale)
		{}

		MaterialDesc(const MaterialDesc & src)
			: Renderer::MaterialDesc(src)
			, mHeightMin(src.mHeightMin)
			, mHeightMax(src.mHeightMax)
			, mTexScale(src.mTexScale)
		{
		}

		GLfloat mHeightMin;
		GLfloat mHeightMax;
		GLfloat mTexScale;
	};

	typedef std::vector<MaterialDesc> MaterialDescList;

	struct Desc : public Renderer::Desc
	{
		Desc(GLint heightMapWidth, GLint heightMapDepth, const glm::vec3 & scale, GLfloat lowSlope = 0.5f, GLfloat highSlope = 0.57f)
			: Renderer::Desc()
			, mHeightMapWidth(heightMapWidth)
			, mHeightMapDepth(heightMapDepth)
			, mScale(scale)
			, mLowSlope(lowSlope)
			, mHighSlope(highSlope)
		{ }

		GLint mHeightMapWidth;
		GLint mHeightMapDepth;
		glm::vec3 mScale;
		GLfloat mLowSlope;
		GLfloat mHighSlope;

		MapDescList mTerrains;
		MaterialDescList mLowSlopeMaterials;
		MaterialDescList mHighSlopeMaterials;
	};

public:
	TerrainRenderer(const Desc & desc);
	virtual ~TerrainRenderer();

	virtual void Render() override;
	virtual void DebugRender() override;


private:

	void BuildMaterialShader(std::string & generatedSource, const Desc & desc);
	static void GenerateGetMaterialByHeight(std::string & generatedSource, const MaterialDescList & matDescList, const TextureInfoList & texInfo);

	void LoadShaders(const Desc & desc);
	void LoadHeightMap(const MapDescList & terrainDescList);
	//void UpdateMaterialTextureIndex(const Desc & desc);

private:

	enum EUniformIndex
	{
		u_PatchCount,
		u_MapSize,
		u_Scale,
		u_TexScale,
		u_HeightMap,
		u_PerMapDataSampler,

		__uniforms_count__
	};

	enum EUniformIndex2
	{
		u_NormalMagnitude,
		u_VertexNormalColor,

		__uniforms2_count__
	};


	struct PerMapData
	{
		Maths::DualQuaternion mModelDQ;
	};


	const int FIRST_TEXTURE_SAMPLER_INDEX = 2;


	glm::ivec2 mMapSize;
	glm::ivec2 mPatchCount;
	glm::vec3 mScale;
	GLfloat mLowSlope;
	GLfloat mHighSlope;

	GLuint mHeightMapTextureId;
	GLint mMapCount;

	Shader mDrawNormalShader;
	TextureBuffer mModelMatrixBuffer;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TERRAINRENDERER_H