#include "stdafx.h"
#include "CoreFx.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace CoreFx
{
	namespace Geometry
	{
// =======================================================================
// =======================================================================


ModelData::ModelData()
	: mIsLoaded(false)
{
}


ModelData::~ModelData()
{
}

void ModelData::LoadModel(const std::string & filepath, const std::string & textureBasePath)
{
	if (mIsLoaded)
	{
		mVertexList.clear();
		mIndexList.clear();
		mTextureList.clear();
		mMaterialList.clear();
		mMeshDrawInstanceList.clear();
		mDiffuseTextureList.clear();
		mSpecularTextureList.clear();
		mEmissiveTextureList.clear();
		mNormalTextureList.clear();
		mIsLoaded = false;
	}

	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_RemoveRedundantMaterials);
	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		PRINT_ERROR("Cannot load model '%s'. (Assimp error : %s)", filepath.c_str(), importer.GetErrorString());
		return;
	}

	if(!scene->HasMaterials())
	{
		PRINT_ERROR("Cannot load model '%s' : the model should have materials!", filepath.c_str());
		return;
	}

	ProcessMaterials(scene, textureBasePath);

	GLuint totalVertexCount = 0;
	GLuint totalIndexCount = 0;
	if (!ParseNode(scene->mRootNode, scene, [this, &totalVertexCount, &totalIndexCount, filepath](aiMesh* mesh, const aiScene* /*scene*/)
	{
		if (mesh->mMaterialIndex < 0)
		{
			PRINT_ERROR("Cannot load model '%s' : the mesh '%s' should have a material!", filepath.c_str(), mesh->mName.C_Str());
			return false;
		}

		GLuint meshIndexCount = 0;
		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			meshIndexCount += face.mNumIndices;
		}


		Renderer::DrawElementsIndirectCommand meshDrawInstance;
		meshDrawInstance.mElemCount = meshIndexCount;
		meshDrawInstance.mInstanceCount = 1;
		meshDrawInstance.mFirstIndex = totalIndexCount;
		meshDrawInstance.mBaseVertex = totalVertexCount;
		meshDrawInstance.mBaseInstance = (GLuint)mesh->mMaterialIndex;//(GLuint)mMeshDrawInstanceList.size();
		mMeshDrawInstanceList.push_back(meshDrawInstance);

		totalVertexCount += mesh->mNumVertices;
		totalIndexCount += meshIndexCount;

		return true;
	}))
	{
		return;
	}

	mVertexList.resize(totalVertexCount);
	mIndexList.resize(totalIndexCount);

	GLuint meshInstanceNum = 0;
	if (!ParseNode(scene->mRootNode, scene, [this, &meshInstanceNum](aiMesh* mesh, const aiScene* scene)
	{
		this->ProcessMesh(meshInstanceNum, mesh, scene);
		++meshInstanceNum;
		return true;
	}))
	{
		return;
	}

	mIsLoaded = true;
}

static glm::vec3 GetMaterialColor(aiMaterial * mat, const char* pKey, unsigned int type, unsigned int idx)
{
	glm::vec3 color(1.f);

	aiColor4D aiC;

	if (mat->Get(pKey, type, idx, aiC) == aiReturn_SUCCESS)
	{
		color.x = aiC.r;
		color.y = aiC.g;
		color.z = aiC.b;
	}
	return color;
}

void ModelData::ProcessMaterials(const aiScene* scene, const std::string & textureBasePath)
{
	for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial * mat = scene->mMaterials[i];

		glm::vec3 diffuseColor(GetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE));
		glm::vec3 specularColor(GetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR));
		glm::vec3 emissiveColor(GetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE));

		Renderer::TextureIndex diffuseTextureIndex = ProcessTextures(mDiffuseTextureList, mat, aiTextureType_DIFFUSE, textureBasePath);
		Renderer::TextureIndex specularTextureIndex = ProcessTextures(mSpecularTextureList, mat, aiTextureType_SPECULAR, textureBasePath);
		Renderer::TextureIndex emissiveTextureIndex = ProcessTextures(mEmissiveTextureList, mat, aiTextureType_EMISSIVE, textureBasePath);
		Renderer::TextureIndex normalTextureIndex = ProcessTextures(mNormalTextureList, mat, aiTextureType_NORMALS, textureBasePath);

		mMaterialList.push_back(Renderer::MaterialDesc(diffuseColor, diffuseTextureIndex, specularColor, 1.f, specularTextureIndex, emissiveColor, emissiveTextureIndex, normalTextureIndex));
	}
}

bool ModelData::ParseNode(aiNode* node, const aiScene* scene, std::function<bool(aiMesh* mesh, const aiScene* scene)> processMeshFunc)
{
	// Process each mesh located at the current node
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		if (!processMeshFunc(mesh, scene))
			return false;
	}

	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		if (!ParseNode(node->mChildren[i], scene, processMeshFunc))
			return false;
	}
	return true;
}

void ModelData::ProcessMesh(GLuint meshInstanceNum, aiMesh* mesh, const aiScene* /*scene*/)
{
	const Renderer::DrawElementsIndirectCommand & meshDrawInstance = mMeshDrawInstanceList[meshInstanceNum];
	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Renderer::VertexData & vertex = mVertexList[i + meshDrawInstance.mBaseVertex];

		// Positions
		vertex.mPosition.x = mesh->mVertices[i].x;
		vertex.mPosition.y = mesh->mVertices[i].y;
		vertex.mPosition.z = mesh->mVertices[i].z;

		// Normals
		vertex.mNormal.x = mesh->mNormals[i].x;
		vertex.mNormal.y = mesh->mNormals[i].y;
		vertex.mNormal.z = mesh->mNormals[i].z;

		// Tangent
		vertex.mTangent.x = mesh->mNormals[i].x;
		vertex.mTangent.y = mesh->mNormals[i].y;
		vertex.mTangent.z = mesh->mNormals[i].z;

		// Texture Coordinates
		if (mesh->mTextureCoords[0] != nullptr) // Does the mesh contain texture coordinates?
		{
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vertex.mTexUV.x = mesh->mTextureCoords[0][i].x;
			vertex.mTexUV.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.mTexUV = glm::vec2(0.0f, 0.0f);
	}

	GLuint index = meshDrawInstance.mFirstIndex;

	// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
			mIndexList[index++] = face.mIndices[j];
	}

	// Process materials
	//if (mesh->mMaterialIndex >= 0)
	//{
	//	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	//}
}

Renderer::TextureIndex ModelData::ProcessTextures(TextureIndexMap & textureIndexMap, aiMaterial* mat, aiTextureType type, const std::string & textureBasePath)
{
	Renderer::TextureIndex textureIndex = Renderer::NoTexture;
	TextureCategory texCat;

	switch (type) // != || type != aiTextureType_SPECULAR || type != aiTextureType_NORMALS)
	{
	case aiTextureType_DIFFUSE:
		texCat = TextureCategory::Diffuse;
		break;

	case aiTextureType_SPECULAR:
		texCat = TextureCategory::Specular;
		break;

	case aiTextureType_NORMALS:
		texCat = TextureCategory::NormalMap;
		break;

	case aiTextureType_EMISSIVE:
		texCat = TextureCategory::Emissive;
		break;

	default:
		PRINT_ERROR("Only diffuse, specular and normals textures used (type = %i is not one of them).", type)
		return textureIndex;
	}

	//for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	if(mat->GetTextureCount(type) > 0)
	{
		aiString str;
		aiTextureMapMode mapMode;

		mat->GetTexture(type, 0, &str, nullptr, nullptr, nullptr, nullptr, &mapMode);
		std::string texFilename(str.C_Str());
		std::string texFilePath;

		std::string sep("/");
		std::string::size_type iPos = texFilename.find_last_of(sep);
		if(iPos == std::string::npos)
		{ 
			sep = "\\";
			iPos = texFilename.find_last_of(sep);
		}
			
		if (iPos != std::string::npos)
		{
			texFilename = texFilename.substr(iPos + 1);
		}

		iPos = texFilename.find_last_of('.');
		if (iPos != std::string::npos)
		{
			texFilename = texFilename.substr(0, iPos);
		}
		texFilename += ".tif";
		
		if (!textureBasePath.empty())
		{
			if (textureBasePath.back() == '/' || textureBasePath.back() == '\\')
				texFilePath = textureBasePath + texFilename;
			else
				texFilePath = textureBasePath + "/" + texFilename;
		}

		TextureIndexMap::const_iterator it = textureIndexMap.find(texFilename);
		if (it == textureIndexMap.end())
		{
			textureIndex = (Renderer::TextureIndex)textureIndexMap.size();
			textureIndexMap[texFilename] = (int)textureIndexMap.size();
			
			TextureWrap texWrap;

			switch (mapMode)
			{
			case aiTextureMapMode_Clamp:
				texWrap = TextureWrap::Clamp;
				break;

			case aiTextureMapMode_Wrap:
				texWrap = TextureWrap::Repeat;
				break;

			case aiTextureMapMode_Mirror:
				texWrap = TextureWrap::MirrorRepeat;
				break;

			default:
				texWrap = TextureWrap::ClampToBorder;
			}

			mTextureList.push_back(Renderer::TextureDesc(texFilePath.c_str(), texCat, texWrap, texWrap));

		}
		else
		{
			textureIndex = (Renderer::TextureIndex)it->second;
		}
	}
	return textureIndex;
}

// =======================================================================
// =======================================================================
	} // namespace Geometry
} // namespace CoreFx
