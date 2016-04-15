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

	typedef std::uint8_t TextureIndex;
	static constexpr TextureIndex NoTexture = (TextureIndex)-1;

	struct MaterialDesc
	{
		MaterialDesc(const glm::vec3& diffuse, TextureIndex diffuseTextureIndex, const glm::vec3& specular, int8_t specularPower, TextureIndex specularTextureIndex, const glm::vec3& emissive, TextureIndex emissiveTextureIndex, TextureIndex normalTextureIndex)
			: mDiffuse(diffuse)
			, mSpecular(specular)
			, mEmissive(emissive)
			, mSpecularPower(specularPower)
			, mDiffuseTextureIndex(diffuseTextureIndex)
			, mSpecularTextureIndex(specularTextureIndex)
			, mEmissiveTextureIndex(emissiveTextureIndex)
			, mNormalTextureIndex(normalTextureIndex)
		{}

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


public:

	Renderer(GLuint propertyCount);
	virtual ~Renderer();

	virtual void Render() = 0;
	virtual void DebugRender() = 0;

	virtual void UpdateMaterialTextureIndex() {}

	const PropertyData & GetMaterials() const {	return mMaterials; }
	GLint GetMaterialBaseIndex() const { return mMaterialBaseIndex; }

	const TextureInfoList & GetTextureInfoList() const { return mTextures; }
	bool AddTexture(const char * filename, TextureCategory category, TextureWrap wrapS, TextureWrap wrapT);

	const TextureMapping & GetTextureMapping() const { return mTextureMapping; }

protected:

	PropertyData mMaterials;
	GLint mMaterialBaseIndex;
	TextureInfoList mTextures;
	TextureMapping mTextureMapping;

};




} // namespace CoreFx
#endif // COREFX_RENDERER_H