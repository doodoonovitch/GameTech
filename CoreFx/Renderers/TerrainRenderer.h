#pragma once

#ifndef COREFX_RENDERERS_TERRAINRENDERER_H
#define COREFX_RENDERERS_TERRAINRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class TerrainRenderer : public RendererHelper<1>
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
		MaterialDesc()
			: Renderer::MaterialDesc()
			, mHeightMin(0.f)
			, mHeightMax(1.f)
			, mTexScale(1.f)
		{}

		MaterialDesc(const MaterialDesc & src)
			: Renderer::MaterialDesc(src)
			, mHeightMin(src.mHeightMin)
			, mHeightMax(src.mHeightMax)
			, mTexScale(src.mTexScale)
		{
		}

		MaterialDesc & SetHeightMin(GLfloat value)
		{
			mHeightMin = value;
			return *this;
		}

		MaterialDesc & SetHeightMax(GLfloat value)
		{
			mHeightMax = value;
			return *this;
		}

		MaterialDesc & SetTexScale(GLfloat value)
		{
			mTexScale = value;
			return *this;
		}


		GLfloat mHeightMin;
		GLfloat mHeightMax;
		GLfloat mTexScale;
	};

	typedef std::vector<MaterialDesc> MaterialDescList;

	struct Desc : public Renderer::Desc
	{
		Desc(GLint heightMapWidth, GLint heightMapDepth, const glm::vec3 & scale, GLfloat lowSlope = 0.7f, GLfloat highSlope = 0.6f)
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

	virtual void Update() override;
	virtual void Render() override;
	virtual void RenderWireFrame() override;


private:

	void BuildMaterialShader(std::string & generatedSource, const Desc & desc);
	static void GenerateGetMaterialByHeight(std::string & generatedSource, const MaterialDescList & matDescList, const TextureInfoList & texInfo);

	void LoadShaders(const Desc & desc);
	void LoadMainShader(const Desc & desc);
	void LoadWireFrameShader(const Desc & desc);
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

	enum EBinding
	{
		u_PerInstanceWorlMatrix,
		u_PerInstanceViewMatrix,
	};

	struct PerMapData
	{
		Maths::DualQuaternion mModelDQ;
	};


	const int FIRST_TEXTURE_SAMPLER_INDEX = 1;


	glm::ivec2 mMapSize;
	glm::ivec2 mPatchCount;
	glm::vec3 mScale;
	GLfloat mLowSlope;
	GLfloat mHighSlope;

	GLuint mHeightMapTextureId;
	GLint mMapCount;

	ShaderStorageBuffer mPrecomputeDataBuffer;
	ShaderStorageBuffer mLocationRawDataBuffer;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TERRAINRENDERER_H