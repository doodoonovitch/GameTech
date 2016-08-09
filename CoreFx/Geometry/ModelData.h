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

	struct LoadOptions
	{
		LoadOptions & SetPreTransformVertices(bool value)
		{
			mPreTransformVertices = value;
			return *this;
		}

		LoadOptions & SetFlipWindingOrder(bool value)
		{
			mFlipWindingOrder = value;
			return *this;
		}

		LoadOptions & SetLogInfo(bool value)
		{
			mLogInfo = value;
			return *this;
		}

		bool mPreTransformVertices = false;
		bool mFlipWindingOrder = false;
		bool mLogInfo = false;
	};

	void LoadModel(const std::string & filepath, const std::string & textureBasePath, const LoadOptions & options);
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

	bool ParseNode(aiNode* node, const aiScene* scene, std::function<bool(aiNode* node, const aiScene* scene, int level)> processNodeFunc, std::function<bool(unsigned int meshIndex, const aiScene* scene, int level)> processMeshFunc, int level);
	static void PrintNodeMatrix(aiNode* node, int level, const char * indent);

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