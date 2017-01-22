#pragma once
#ifndef COREFX_RENDERER_H
#define COREFX_RENDERER_H


namespace CoreFx
{

	class Engine;



class Renderer : public ObjectInstance
{
	friend class Engine;

public:


//#pragma pack(push, 1)
	/*__declspec(align(4)) */class VertexData
	{
	public:

		glm::vec3 mPosition;
		//float _pad1;
		glm::vec2 mTexUV;
		//float _pad4[2];
		glm::vec3 mNormal;
		//float _pad2;
		glm::vec3 mTangent;
		//float _pad3;
		int mMeshId;
	};

	/*__declspec(align(4)) */class DrawElementsIndirectCommand
	{
	public:
		GLuint mElemCount;
		GLuint mInstanceCount;
		GLuint mFirstIndex;
		GLint mBaseVertex;
		GLuint mBaseInstance;
		//GLuint __padding__[3];
	};

	enum
	{
		BonesPerVertex = 4
	};

	class VertexBoneData
	{
	public:

		GLint mBoneIds[BonesPerVertex];
		GLfloat mWeights[BonesPerVertex];

		VertexBoneData()
		{
			for (GLuint i = 0; i < BonesPerVertex; i++)
			{
				mBoneIds[i] = -1;
				mWeights[i] = 0.f;
			}
		}

		void AddBoneData(GLuint boneID, float weight)
		{
			for (GLuint i = 0; i < BonesPerVertex; i++) 
			{
				if (mBoneIds[i] == -1)
				{
					mBoneIds[i] = boneID;
					mWeights[i] = weight;
					return;
				}
			}

			PRINT_WARNING("[AddBoneData] Too many bones per vertex !")
		}

	};

	struct BoneData
	{
		glm::mat4 mOffsetMatrix;
	};

//#pragma pack(pop)

	typedef std::vector<VertexData> VertexDataVector;
	typedef std::vector<GLuint> IndexVector;
	typedef std::vector<DrawElementsIndirectCommand> DrawElementsIndirectCommandList;
	typedef std::vector<BoneData> BoneDataList;
	typedef std::vector<VertexBoneData> VertexBoneDataList;


	enum ERenderPass
	{
		Deferred_Pass,
		Forward_Pass,
		HUD_Pass,
	};

	typedef std::uint8_t TextureIndex;
	static constexpr TextureIndex NoTexture = (TextureIndex)-1;

	enum class EEnvMapType
	{
		EnvMap_1,
		EnvMap_2
	};

	struct MaterialDesc
	{
	protected:

		static void SetValue(GLfloat & memberVar, GLfloat value, GLfloat minValue, GLfloat maxValue)
		{			
			memberVar = glm::clamp(value, minValue, maxValue);
		}
		
	public:

		MaterialDesc()
			: mBaseColor(1.f)
			, mMetallic(1.f)
			, mPorosity(0.0f)
			, mRoughness(0.5f)
			, mEmissive(0.f)
			, mHeightOffset(0.f)
			, mHeightScale(1.f)
			, mBaseColorTextureIndex(NoTexture)
			, mMetallicTextureIndex(NoTexture)
			, mRoughnessTextureIndex(NoTexture)
			, mNormalTextureIndex(NoTexture)
			, mEmissiveTextureIndex(NoTexture)
			, mHeightTextureIndex(NoTexture)
			, mEnvMapType(EEnvMapType::EnvMap_2)
		{}

		MaterialDesc(const MaterialDesc & src)
			: mBaseColor(src.mBaseColor)
			, mMetallic(src.mMetallic)
			, mPorosity(src.mPorosity)
			, mRoughness(src.mRoughness)
			, mEmissive(src.mEmissive)
			, mHeightOffset(src.mHeightOffset)
			, mHeightScale(src.mHeightScale)
			, mBaseColorTextureIndex(src.mBaseColorTextureIndex)
			, mMetallicTextureIndex(src.mMetallicTextureIndex)
			, mRoughnessTextureIndex(src.mRoughnessTextureIndex)
			, mNormalTextureIndex(src.mNormalTextureIndex)
			, mEmissiveTextureIndex(src.mEmissiveTextureIndex)
			, mHeightTextureIndex(src.mHeightTextureIndex)
			, mEnvMapType(src.mEnvMapType)
		{
		}

		MaterialDesc & SetBaseColor(const glm::vec3 & value)
		{
			mBaseColor = value;
			return *this;
		}

		MaterialDesc & SetMetallic(GLfloat value)
		{
			SetValue(mMetallic, value, 0.f, 1.f);
			return *this;
		}

		MaterialDesc & SetPorosity(GLfloat value)
		{
			SetValue(mPorosity, value, 0.f, 1.f);
			return *this;
		}

		MaterialDesc & SetRoughness(GLfloat value)
		{
			SetValue(mRoughness, value, 0.f, 1.f);
			return *this;
		}

		MaterialDesc & SetEmissive(GLfloat value)
		{
			mEmissive = value;
			return *this;
		}

		MaterialDesc & SetHeightOffset(GLfloat value)
		{
			mHeightOffset = value;
			return *this;
		}

		MaterialDesc & SetHeightScale(GLfloat value)
		{
			mHeightScale = value;
			return *this;
		}

		MaterialDesc & SetBaseColorTextureIndex(TextureIndex value)
		{
			mBaseColorTextureIndex = value;
			return *this;
		}

		MaterialDesc & SetMetallicTextureIndex(TextureIndex value)
		{
			mMetallicTextureIndex = value;
			return *this;
		}

		MaterialDesc & SetRoughnessTextureIndex(TextureIndex value)
		{
			mRoughnessTextureIndex = value;
			return *this;
		}

		MaterialDesc & SetNormalTextureIndex(TextureIndex value)
		{
			mNormalTextureIndex = value;
			return *this;
		}

		MaterialDesc & SetEmissiveTextureIndex(TextureIndex value)
		{
			mEmissiveTextureIndex = value;
			return *this;
		}

		MaterialDesc & SetHeightTextureIndex(TextureIndex value)
		{
			mHeightTextureIndex = value;
			return *this;
		}

		MaterialDesc & SetEnvMapType(EEnvMapType envMapType)
		{
			mEnvMapType = envMapType;
			return *this;
		}

		glm::vec3 mBaseColor;
		GLfloat mMetallic;
		GLfloat mPorosity;
		GLfloat mRoughness;
		GLfloat mEmissive;
		GLfloat mHeightOffset;
		GLfloat mHeightScale;
		TextureIndex mBaseColorTextureIndex;
		TextureIndex mMetallicTextureIndex;
		TextureIndex mRoughnessTextureIndex;
		TextureIndex mNormalTextureIndex;
		TextureIndex mEmissiveTextureIndex;
		TextureIndex mHeightTextureIndex;
		EEnvMapType mEnvMapType;

	};

	typedef std::vector<MaterialDesc> MaterialDescList;

	struct TextureDesc
	{
		TextureDesc()
			: mCategory(TextureCategory::Diffuse)
			, mWrapS(TextureWrap::Repeat)
			, mWrapT(TextureWrap::Repeat)
			, mInvertY(true)
		{
		}

		TextureDesc(const char * filename, TextureCategory category, TextureWrap wrapS, TextureWrap wrapT, bool invertY = true)
			: mFilename(filename)
			, mCategory(category)
			, mWrapS(wrapS)
			, mWrapT(wrapT)
			, mInvertY(invertY)
		{
		}

		std::string mFilename;
		TextureCategory mCategory; 
		TextureWrap mWrapS;
		TextureWrap mWrapT;
		bool mInvertY;
	};

	typedef std::vector<TextureDesc> TextureDescList;

	struct Desc
	{
		TextureDescList mTextures;
	};

public:

	Renderer(ERenderPass renderPass = Deferred_Pass);

	virtual ~Renderer();

	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void RenderWireFrame() = 0;

	bool GetIsInitialized() const { return mIsInitialized; }
	bool GetIsEnabled() const { return mIsEnabled; }
	bool GetIsUsedToGenerateEnvMap() const { return mUseToGenEnvMap; }

	void SetIsEnabled(bool isEnabled) { mIsEnabled = isEnabled; }

	void SetIsUsedToGenerateEnvMap(bool value) { mUseToGenEnvMap = value; }

	ERenderPass GetRenderPass() const { return mRenderPass; }

	const TextureInfoList & GetTextureInfoList() const { return mTextureInfoList; }
	bool AddTexture(const char * filename, TextureCategory category, TextureWrap wrapS, TextureWrap wrapT, bool invertY);
	bool AddTexture(const TextureDesc & desc)
	{
		return AddTexture(desc.mFilename.c_str(), desc.mCategory, desc.mWrapS, desc.mWrapT, desc.mInvertY);
	}

	void AddTextures(const TextureDescList & textures);

	const TextureMapping & GetTextureMapping() const { return mTextureMapping; }

protected:

	void BuildTextureMapping();

	static void GenericBuildTextureMapping(TextureMapping * lightPassTextureMapping, const TextureInfoList & texInfoList);
	void LoadTextures();

protected:

	TextureInfoList mTextureInfoList;
	TextureMapping mTextureMapping;
	ERenderPass mRenderPass;
	bool mIsInitialized;
	bool mIsEnabled;
	bool mUseToGenEnvMap;

};




} // namespace CoreFx
#endif // COREFX_RENDERER_H