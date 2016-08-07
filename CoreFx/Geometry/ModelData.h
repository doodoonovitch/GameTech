#pragma once
#ifndef COREFX_GEOMETRY_MODELDATA_H
#define COREFX_GEOMETRY_MODELDATA_H

namespace CoreFx
{
	namespace Geometry
	{



class ModelData
{
public:

public:
	ModelData();
	virtual ~ModelData();

	void LoadModel(const std::string & filepath, const std::string & textureBasePath);
	bool IsLoaded() const { return mIsLoaded; }

	const Renderer::VertexDataVector & GetVertexList() const { return mVertexList; }
	const Renderer::IndexVector & GetIndexList() const { return mIndexList; }
	const Renderer::MaterialDescList & GetMaterialDescList() const { return mMaterialList; }
	const Renderer::TextureDescList & GetTextureDescList() const { return mTextureList; }
	const Renderer::DrawElementsIndirectCommandList & GetMeshDrawInstanceList() const { return mMeshDrawInstanceList; }

	Renderer::MaterialDescList & GetMaterialDescList() { return mMaterialList; }
	Renderer::TextureDescList & GetTextureDescList() { return mTextureList; }

protected:

	typedef std::map<std::string, int> TextureIndexMap;

	void ProcessMaterials(const aiScene* scene, const std::string & textureBasePath);
	void ProcessMesh(GLuint meshInstanceNum, aiMesh* mesh, const aiScene* scene);
	Renderer::TextureIndex ProcessTextures(TextureIndexMap & texMap, aiMaterial* mat, aiTextureType type, const std::string & textureBasePath);

	bool ParseNode(aiNode* node, const aiScene* scene, std::function<bool(aiMesh* mesh, const aiScene* scene)> processMeshFunc);

protected:

	Renderer::VertexDataVector mVertexList;
	Renderer::IndexVector mIndexList;
	Renderer::TextureDescList mTextureList;
	Renderer::MaterialDescList mMaterialList;
	Renderer::DrawElementsIndirectCommandList mMeshDrawInstanceList;
	TextureIndexMap mDiffuseTextureList;
	TextureIndexMap mSpecularTextureList;
	TextureIndexMap mEmissiveTextureList;
	TextureIndexMap mNormalTextureList;

	bool mIsLoaded;
};


	} // namespace Geometry
} // namespace Core
#endif // COREFX_GEOMETRY_MODELDATA_H