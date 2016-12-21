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

	struct MaterialDesc : Renderer::MaterialDesc
	{
		MaterialDesc()
			: Renderer::MaterialDesc()
			, mMinHeight(0.f)
			, mMaxHeight(1.f)
			, mTexScale(1.f)
			, mMinSlope(0.f)
			, mMaxSlope(1.f)
		{}

		MaterialDesc(const MaterialDesc & src)
			: Renderer::MaterialDesc(src)
			, mMinHeight(src.mMinHeight)
			, mMaxHeight(src.mMaxHeight)
			, mTexScale(src.mTexScale)
			, mMinSlope(src.mMinSlope)
			, mMaxSlope(src.mMaxSlope)
		{
		}

		MaterialDesc & SetElevation(GLfloat minHeight, GLfloat maxHeight)
		{
			mMinHeight = minHeight;
			mMaxHeight = maxHeight;
			return *this;
		}

		MaterialDesc & SetTexScale(GLfloat value)
		{
			mTexScale = value;
			return *this;
		}

		MaterialDesc & SetSlope(GLfloat minSlope, GLfloat maxSlope)
		{
			mMinSlope = minSlope;
			mMaxSlope = maxSlope;
			return *this;
		}

	protected:

		friend class TerrainRenderer;

		GLfloat mMinHeight;
		GLfloat mMaxHeight;
		GLfloat mTexScale;
		GLfloat mMinSlope;
		GLfloat mMaxSlope;
	};

	struct MapDesc
	{
		MapDesc()
		{
		}

		MapDesc(const char * filename, GLint heightMapTextureSize, bool invertY, const glm::vec3 & origin, const glm::quat & rotation)
			: mFilename(filename)
			, mHeightMapTextureSize(heightMapTextureSize)
			, mInvertY(invertY)
		{
			mModelDQ.SetRotationTranslation(rotation, origin);
		}

		MapDesc(const MapDesc & src)
			: mFilename(src.mFilename)
			, mHeightMapTextureSize(src.mHeightMapTextureSize)
			, mInvertY(src.mInvertY)
			, mModelDQ(src.mModelDQ)
		{
			for (int i = 0; i < 4; ++i)
			{
				mMaterials[i] = src.mMaterials[i];
			}
		}

		MapDesc & SetFilename(const std::string & filename)
		{
			mFilename = filename;
			return *this;
		}

		MapDesc & SetHeightMapTextureSize(GLsizei mapSize)
		{
			mHeightMapTextureSize = mapSize;
			return *this;
		}

		MapDesc & SetInvertY(bool value)
		{
			mInvertY = value;
			return *this;
		}

		MapDesc & SetOriginRotation(const glm::vec3 & origin, const glm::quat & rotation)
		{
			mModelDQ.SetRotationTranslation(rotation, origin);
			return *this;
		}

		MaterialDesc mMaterials[4];

	protected:

		friend class TerrainRenderer;

		std::string mFilename;
		GLsizei mHeightMapTextureSize = 512;
		bool mInvertY = true;

		Maths::DualQuaternion mModelDQ;
	};

	typedef std::vector<MapDesc> MapDescList;

	//typedef std::vector<MaterialDesc> MaterialDescList;

	struct Desc : public Renderer::Desc
	{
		Desc(GLsizei heightMapSize, const glm::vec3 & scale)
			: Renderer::Desc()
			, mHeightMapSize(heightMapSize)
			, mScale(scale)
		{ }

		GLsizei mHeightMapSize;
		glm::vec3 mScale;

		MapDescList mTerrains;
	};

public:
	TerrainRenderer(const Desc & desc);
	virtual ~TerrainRenderer();

	virtual void Update() override;
	virtual void Render() override;
	virtual void RenderWireFrame() override;


private:

	void LoadShaders(const Desc & desc);
	void LoadMainShader(const Desc & desc);
	void LoadWireFrameShader(const Desc & desc);
	bool LoadHeightMap(const Desc & desc);
	void UpdateMaterialTextureIndex(const Desc & desc);

	//void GenerateBlendMap(const MapDesc & desc);
	static GLfloat ComputeWeight(GLfloat value, GLfloat minExtend, GLfloat maxExtend);
	
private:

	enum EUniformIndex
	{
		u_PatchCount,
		u_MapSize,
		u_Scale,
		u_PerMapDataSampler,
		u_HeightMap,
		u_UvMap,
		u_BlendMap,

		__uniforms_count__
	};

	enum EBinding
	{
		u_PerInstanceWorlMatrix,
		u_PerInstanceViewMatrix,
		u_Materials
	};

	struct PerMapData
	{
		Maths::DualQuaternion mModelDQ;
	};


	const int FIRST_TEXTURE_SAMPLER_INDEX = 3;


	glm::ivec2 mPatchCount;
	glm::vec3 mScale;

	GLsizei mMapSize;

	//GLuint mHeightMapTextureId;
	GLint mMapCount;

	ShaderStorageBuffer mPrecomputeDataBuffer;
	ShaderStorageBuffer mLocationRawDataBuffer;
	ShaderStorageBuffer mMaterialBuffer;

	Texture2DArray const * mHeightMaps = nullptr;
	Texture2DArray const * mUvMaps = nullptr;
	Texture2DArray const * mBlendMaps = nullptr;

	GLuint mMaterialCount;
	ModelRenderer::ShaderMaterialList mShaderMaterialList;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TERRAINRENDERER_H