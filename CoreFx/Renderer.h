#pragma once
#ifndef COREFX_RENDERER_H
#define COREFX_RENDERER_H


namespace CoreFx
{

	class Engine;
	class Renderer;
	typedef InstanceContainer<Renderer*> RendererContainer;



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
		Forward_Pass
	};

	typedef std::uint8_t TextureIndex;
	static constexpr TextureIndex NoTexture = (TextureIndex)-1;

	struct MaterialDesc
	{
		MaterialDesc()
			: mDiffuse(0.f)
			, mSpecular(0.f)
			, mEmissive(0.f)
			, mRoughness(0.f)
			, mDiffuseTextureIndex(NoTexture)
			, mSpecularTextureIndex(NoTexture)
			, mRoughnessTextureIndex(NoTexture)
			, mEmissiveTextureIndex(NoTexture)
			, mNormalTextureIndex(NoTexture)
		{}

		MaterialDesc(const glm::vec3& diffuse, TextureIndex diffuseTextureIndex, 
			const glm::vec3& specular, TextureIndex specularTextureIndex,
			GLfloat roughness, TextureIndex roughnessTextureIndex,
			const glm::vec3& emissive, TextureIndex emissiveTextureIndex, 
			TextureIndex normalTextureIndex)
			: mDiffuse(diffuse)
			, mSpecular(specular)
			, mEmissive(emissive)
			, mRoughness(roughness)
			, mDiffuseTextureIndex(diffuseTextureIndex)
			, mSpecularTextureIndex(specularTextureIndex)
			, mRoughnessTextureIndex(roughnessTextureIndex)
			, mEmissiveTextureIndex(emissiveTextureIndex)
			, mNormalTextureIndex(normalTextureIndex)
		{}

		MaterialDesc(const MaterialDesc & src)
			: mDiffuse(src.mDiffuse)
			, mSpecular(src.mSpecular)
			, mEmissive(src.mEmissive)
			, mRoughness(src.mRoughness)
			, mDiffuseTextureIndex(src.mDiffuseTextureIndex)
			, mSpecularTextureIndex(src.mSpecularTextureIndex)
			, mRoughnessTextureIndex(src.mRoughnessTextureIndex)
			, mEmissiveTextureIndex(src.mEmissiveTextureIndex)
			, mNormalTextureIndex(src.mNormalTextureIndex)
		{
		}

		glm::vec3 mAmbient;
		glm::vec3 mDiffuse;
		glm::vec3 mSpecular;
		glm::vec3 mEmissive;
		GLfloat mRoughness;
		TextureIndex mDiffuseTextureIndex;
		TextureIndex mSpecularTextureIndex;
		TextureIndex mRoughnessTextureIndex;
		TextureIndex mNormalTextureIndex;
		TextureIndex mEmissiveTextureIndex;
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

	Renderer(GLuint propertyCount, ERenderPass renderPass = Deferred_Pass);
	virtual ~Renderer();

	virtual void Render() = 0;
	virtual void RenderWireFrame() = 0;

	bool GetIsInitialized() const { return mIsInitialized; }

	ERenderPass GetRenderPass() const { return mRenderPass; }

	const PropertyData & GetMaterials() const {	return mMaterials; }
	GLint GetMaterialBaseIndex() const { return mMaterialBaseIndex; }

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

	PropertyData mMaterials;
	GLint mMaterialBaseIndex;
	TextureInfoList mTextureInfoList;
	TextureMapping mTextureMapping;
	ERenderPass mRenderPass;
	bool mIsInitialized;
};




} // namespace CoreFx
#endif // COREFX_RENDERER_H