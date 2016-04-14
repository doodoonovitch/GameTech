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