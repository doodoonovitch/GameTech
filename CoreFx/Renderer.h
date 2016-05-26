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

	enum ERenderPass
	{
		Deferred_Pass,
		Forward_Pass
	};

	typedef std::uint8_t TextureIndex;
	static constexpr TextureIndex NoTexture = (TextureIndex)-1;

	struct MaterialDesc
	{
		MaterialDesc(const glm::vec3& diffuse, TextureIndex diffuseTextureIndex, 
			const glm::vec3& specular, int8_t specularPower, TextureIndex specularTextureIndex, 
			const glm::vec3& emissive, TextureIndex emissiveTextureIndex, 
			TextureIndex normalTextureIndex)
			: mDiffuse(diffuse)
			, mSpecular(specular)
			, mEmissive(emissive)
			, mSpecularPower(specularPower)
			, mDiffuseTextureIndex(diffuseTextureIndex)
			, mSpecularTextureIndex(specularTextureIndex)
			, mEmissiveTextureIndex(emissiveTextureIndex)
			, mNormalTextureIndex(normalTextureIndex)
		{}

		MaterialDesc(const MaterialDesc & src)
			: mDiffuse(src.mDiffuse)
			, mSpecular(src.mSpecular)
			, mEmissive(src.mEmissive)
			, mSpecularPower(src.mSpecularPower)
			, mDiffuseTextureIndex(src.mDiffuseTextureIndex)
			, mSpecularTextureIndex(src.mSpecularTextureIndex)
			, mEmissiveTextureIndex(src.mEmissiveTextureIndex)
			, mNormalTextureIndex(src.mNormalTextureIndex)
		{
		}

		glm::vec3 mAmbient;
		glm::vec3 mDiffuse;
		glm::vec3 mSpecular;
		glm::vec3 mEmissive;
		int8_t mSpecularPower;
		TextureIndex mDiffuseTextureIndex;
		TextureIndex mSpecularTextureIndex;
		TextureIndex mNormalTextureIndex;
		TextureIndex mEmissiveTextureIndex;
	};

	typedef std::vector<MaterialDesc> MaterialDescList;

	struct TextureDesc
	{
		TextureDesc(const char * filename, TextureCategory category, TextureWrap wrapS, TextureWrap wrapT)
			: mFilename(filename)
			, mCategory(category)
			, mWrapS(wrapS)
			, mWrapT(wrapT)
		{
		}

		std::string mFilename;
		TextureCategory mCategory; 
		TextureWrap mWrapS;
		TextureWrap mWrapT;
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
	virtual void DebugRender() = 0;

	const PropertyData & GetMaterials() const {	return mMaterials; }
	GLint GetMaterialBaseIndex() const { return mMaterialBaseIndex; }

	const TextureInfoList & GetTextureInfoList() const { return mTextures; }
	bool AddTexture(const char * filename, TextureCategory category, TextureWrap wrapS, TextureWrap wrapT);
	bool AddTexture(const TextureDesc & desc)
	{
		return AddTexture(desc.mFilename.c_str(), desc.mCategory, desc.mWrapS, desc.mWrapT);
	}

	void AddTextures(const TextureDescList & textures);

	const TextureMapping & GetTextureMapping() const { return mTextureMapping; }

protected:

	void BuildTextureMapping(TextureMapping * lightPassTextureMapping = nullptr);
	void LoadTextures();

protected:

	PropertyData mMaterials;
	GLint mMaterialBaseIndex;
	TextureInfoList mTextures;
	TextureMapping mTextureMapping;
	ERenderPass mRenderPass;
};




} // namespace CoreFx
#endif // COREFX_RENDERER_H