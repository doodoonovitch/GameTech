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

	static constexpr GLint BlendMaterialCount = 4;

	struct MaterialDesc : Renderer::MaterialDesc
	{
		MaterialDesc()
			: Renderer::MaterialDesc()
			, mMinHeight(0.f)
			, mMaxHeight(1.f)
			, mTexScale(1.f)
			, mMinSlope(0.f)
			, mMaxSlope(1.f)
			, mStrength(1.f)
		{}

		MaterialDesc(const MaterialDesc & src)
			: Renderer::MaterialDesc(src)
			, mMinHeight(src.mMinHeight)
			, mMaxHeight(src.mMaxHeight)
			, mTexScale(src.mTexScale)
			, mMinSlope(src.mMinSlope)
			, mMaxSlope(src.mMaxSlope)
			, mStrength(src.mStrength)
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

		MaterialDesc & SetStrength(GLfloat value)
		{
			mStrength = value;
			return *this;
		}

	protected:

		friend class TerrainRenderer;

		GLfloat mMinHeight;
		GLfloat mMaxHeight;
		GLfloat mTexScale;
		GLfloat mMinSlope;
		GLfloat mMaxSlope;
		GLfloat mStrength;
	};

	struct MapDesc
	{
		MapDesc()
		{
		}

		MapDesc(const char * filename, GLint heightMapTextureSize, bool invertY, const glm::vec3 & origin, const GLint matIndex[BlendMaterialCount])
			: mFilename(filename)
			, mHeightMapTextureSize(heightMapTextureSize)
			, mInvertY(invertY)
			, mOrigin(origin)
		{
			SetMaterials(matIndex);
		}

		MapDesc(const MapDesc & src)
			: mFilename(src.mFilename)
			, mHeightMapTextureSize(src.mHeightMapTextureSize)
			, mInvertY(src.mInvertY)
			, mOrigin(src.mOrigin)
			, mMatIndex(src.mMatIndex)
		{
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

		MapDesc & SetOrigin(const glm::vec3 & origin)
		{
			mOrigin = origin;
			return *this;
		}

		MapDesc & SetMaterials(const GLint matIndex[BlendMaterialCount])
		{
			for (GLint i = 0; i < BlendMaterialCount; ++i)
				mMatIndex[i] = matIndex[i];
			return *this;
		}



	protected:

		friend class TerrainRenderer;

		std::string mFilename;
		GLsizei mHeightMapTextureSize = 512;
		bool mInvertY = true;
		glm::vec3  mOrigin;
		glm::ivec4 mMatIndex;
	};

	typedef std::vector<MapDesc> MapDescList;

	typedef std::vector<MaterialDesc> MaterialDescList;

	struct Desc : public Renderer::Desc
	{
		Desc(GLsizei heightMapSize, const glm::vec3 & scale)
			: Renderer::Desc()
			, mHeightMapSize(heightMapSize)
			, mScale(scale)
		{ }


		bool CheckData() const;

		GLsizei mHeightMapSize;
		glm::vec3 mScale;

		MaterialDescList mMaterials;
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
		u_HeightMap,
		u_BlendMap,
		u_NormalMap,

		__uniforms_count__
	};

	enum EBinding
	{
		u_PerInstanceData,
		u_Materials
	};

#pragma pack(push, 1)
	struct TerrainPerInstanceData
	{
		glm::ivec4 mMatIndex;
		glm::vec3 mOrigin;
		GLfloat __padding__[1];
	};
#pragma pack(pop)


	const int FIRST_TEXTURE_SAMPLER_INDEX = 3;


	glm::ivec2 mPatchCount;
	glm::vec3 mScale;

	GLsizei mMapSize;

	GLint mMapCount;

	ShaderStorageBuffer mPerInstanceDataBuffer;
	ShaderStorageBuffer mMaterialBuffer;

	Texture2DArray const * mHeightMaps = nullptr;
	Texture2DArray const * mNormalMaps = nullptr;
	Texture2DArray const * mBlendMaps = nullptr;

	ModelRenderer::ShaderMaterialList mShaderMaterialList;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TERRAINRENDERER_H