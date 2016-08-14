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
			mUseMatrix = false;
			return *this;
		}

		LoadOptions & SetPreTransformVertices(const glm::mat4 & mat)
		{
			mPreTransformVertices = true;
			mUseMatrix = true;
			mMatrix = mat;
			return *this;
		}

		LoadOptions & SetFlipWindingOrder(bool value)
		{
			mFlipWindingOrder = value;
			return *this;
		}

		LoadOptions & SetFlipNormal(bool value)
		{
			mFlipNormal = value;
			return *this;
		}


		LoadOptions & SetGenSmoothNormal(bool value)
		{
			mGenSmoothNormal = value;
			return *this;
		}		

		LoadOptions & SetLogInfo(bool value)
		{
			mLogInfo = value;
			return *this;
		}

		LoadOptions & SetLogBoneInfo(bool value)
		{
			mLogBoneInfo = value;
			return *this;
		}

		bool mGenSmoothNormal = true;
		bool mPreTransformVertices = false;
		bool mFlipWindingOrder = false;
		bool mFlipNormal = false;
		bool mLogInfo = false;
		bool mLogBoneInfo = false;
		bool mUseMatrix = false;
		glm::mat4 mMatrix;
	};

	struct ModelMapping
	{
		ModelMapping(GLuint drawElementsIndirectCommandIndex, GLuint drawElementsIndirectCommandCount, bool hasBones)
			: mDrawCommandIndex(drawElementsIndirectCommandIndex)
			, mDrawCommandCount(drawElementsIndirectCommandCount)
			, mHasBones(hasBones)
		{
		}

		ModelMapping(const ModelMapping & src)
			: mDrawCommandIndex(src.mDrawCommandIndex)
			, mDrawCommandCount(src.mDrawCommandCount)
			, mHasBones(src.mHasBones)
		{
		}

		GLuint mDrawCommandIndex = 0;
		GLuint mDrawCommandCount = 0;
		bool mHasBones = false;
	};

	typedef std::vector<ModelMapping> ModelMappingList;


	struct DataContextBase
	{
		GLuint mVertexIndexBase;
		GLuint mIndexBase;
		GLuint mModelMappingIndexBase;
		GLuint mMeshInstanceIndexBase;
		GLuint mTextureIndexBase;
		GLuint mMaterialIndexBase;
		GLuint mBoneDataIndexBase;
		GLuint mVertexBoneDataIndexBase;
		GLuint mBoneMappingBaseIndex;
	};


	bool LoadModel(const std::string & filepath, const std::string & textureBasePath, const LoadOptions & options, DataContextBase * outDataContextBase = nullptr);

	const Renderer::VertexDataVector & GetVertexList() const { return mVertexList; }
	const Renderer::IndexVector & GetIndexList() const { return mIndexList; }
	const Renderer::MaterialDescList & GetMaterialDescList() const { return mMaterialList; }
	const Renderer::TextureDescList & GetTextureDescList() const { return mTextureList; }
	const Renderer::DrawElementsIndirectCommandList & GetMeshDrawInstanceList() const { return mMeshDrawInstanceList; }

	Renderer::MaterialDescList & GetMaterialDescList() { return mMaterialList; }
	Renderer::TextureDescList & GetTextureDescList() { return mTextureList; }

	const ModelMappingList & GetModelMappingList() const { return mModelMappingList; }
	ModelMappingList & GetModelMappingList() { return mModelMappingList; }

	bool CopyAndAddModel(GLuint sourceModelIndex, GLuint materialOffset);

protected:

	typedef std::map<std::string, int> TextureIndexMap;
	typedef std::map<std::string, GLuint> BoneMapping;
	typedef std::vector<BoneMapping> BonneMappingList;

	bool AddModel(const std::string & filepath, const std::string & textureBasePath, const LoadOptions & options, DataContextBase * outDataContextBase);

	void ProcessMaterials(const aiScene* scene, const std::string & textureBasePath);
	void ProcessMesh(GLuint meshInstanceNum, aiMesh* mesh, bool hasBones, const LoadOptions & options, const DataContextBase & dataCtxBase);
	void ProcessMeshBones(GLuint meshInstanceNum, aiMesh * mesh, const DataContextBase & dataCtxBase);
	Renderer::TextureIndex ProcessTextures(TextureIndexMap & texMap, aiMaterial* mat, aiTextureType type, const std::string & textureBasePath);

	bool ParseNode(aiNode* node, const aiScene* scene, std::function<bool(aiNode* node, const aiScene* scene, int level)> processNodeFunc, std::function<bool(unsigned int meshIndex, const aiScene* scene, int level)> processMeshFunc, int level);
	static void PrintNodeMatrix(const aiMatrix4x4 & aiMat, int level, const char * indent);
	static void SetMatrix(const aiMatrix4x4 & from, glm::mat4 & to);

protected:

	Renderer::VertexDataVector mVertexList;
	Renderer::IndexVector mIndexList;
	Renderer::TextureDescList mTextureList;
	Renderer::MaterialDescList mMaterialList;
	Renderer::DrawElementsIndirectCommandList mMeshDrawInstanceList;
	Renderer::BoneDataList mBoneDataList;
	Renderer::VertexBoneDataList mVertexBoneDataList;
	BonneMappingList mBoneMappingList;
	ModelMappingList mModelMappingList;

	TextureIndexMap mDiffuseTextureList;
	TextureIndexMap mSpecularTextureList;
	TextureIndexMap mEmissiveTextureList;
	TextureIndexMap mNormalTextureList;
};


	} // namespace Geometry
} // namespace Core
#endif // COREFX_GEOMETRY_MODELDATA_H