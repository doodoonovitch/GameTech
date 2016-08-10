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
	: mHasBones(false)
	, mIsLoaded(false)
{
}


ModelData::~ModelData()
{
}

void ModelData::LoadModel(const std::string & filepath, const std::string & textureBasePath, const LoadOptions & options)
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
		mHasBones = false;
	}

	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Loading model '%s'...", filepath.c_str());

	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | /*aiProcess_RemoveRedundantMaterials | */(options.mPreTransformVertices ? aiProcess_PreTransformVertices : 0) | (options.mFlipWindingOrder ? aiProcess_FlipWindingOrder : 0));

	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		PRINT_ERROR("Cannot load model '%s'. (Assimp error : %s)", filepath.c_str(), importer.GetErrorString());
		goto LoadModelEnd;
	}

	if (options.mLogInfo)
	{
		PRINT_MESSAGE("--------------------");
		ParseNode(scene->mRootNode, scene, [](aiNode* node, const aiScene* /*scene*/, int level)
		{
			std::string indent(level + 1, '-');
			PRINT_MESSAGE("|%s[%li] Node : '%s' (children=%li, mesh=%li)", indent.c_str(), level, node->mName.C_Str(), node->mNumChildren, node->mNumMeshes);

			indent += "-";
			PRINT_MESSAGE("|%s[%li] Matrix :", indent, level);

			indent += "-";
			PrintNodeMatrix(node->mTransformation, level, indent.c_str());

			return true;

		}, [](unsigned int meshIndex, const aiScene* scene, int level)
		{
			aiMesh* mesh = scene->mMeshes[meshIndex];
			std::string indent(level + 1, '-');
			PRINT_MESSAGE("|%s[%li] Mesh %li : '%s' (bones=%li)", indent.c_str(), level, meshIndex, mesh->mName.C_Str(), mesh->mNumBones);

			indent += "-";
			std::string indent2 = indent + "-";
			std::string indent3 = indent2 + "-";
			for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
			{
				aiBone * bone = mesh->mBones[boneIndex];
				PRINT_MESSAGE("|%s[%li] Bone %li : '%s' (weights=li)", indent.c_str(), level, boneIndex, bone->mName.C_Str(), bone->mNumWeights);

				PRINT_MESSAGE("|%s[%li] Offset Matrix :", indent2.c_str(), level, boneIndex, bone->mName.C_Str());
				PrintNodeMatrix(bone->mOffsetMatrix, level, indent3.c_str());

				PRINT_MESSAGE("|%s[%li] Weights :", indent2.c_str(), level);
				for (unsigned int numWeight = 0; numWeight < bone->mNumWeights; ++numWeight)
				{
					const aiVertexWeight & weight = bone->mWeights[numWeight];
					PRINT_MESSAGE("|%s[%li] VertexId = %li, weight = %f", indent3.c_str(), level, weight.mVertexId, weight.mWeight);
				}

			}

			return true;
		}, 0);
		PRINT_MESSAGE("--------------------");
	}

	if(!scene->HasMaterials())
	{
		PRINT_ERROR("Cannot load model '%s' : the model should have materials!", filepath.c_str());
		goto LoadModelEnd;
	}

	GLuint totalVertexCount = 0;
	GLuint totalIndexCount = 0;
	if (!ParseNode(scene->mRootNode, scene, nullptr, [this, &totalVertexCount, &totalIndexCount, filepath](unsigned int meshIndex, const aiScene* scene, int /*level*/)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];

		mHasBones = mHasBones || mesh->HasBones();

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
	}, 0))
	{
		goto LoadModelEnd;
	}

	mVertexList.resize(totalVertexCount);
	mIndexList.resize(totalIndexCount);
	if (mHasBones)
	{
		mVertexBoneDataList.resize(totalVertexCount);
	}

	ProcessMaterials(scene, textureBasePath);

	GLuint meshInstanceNum = 0;
	if (!ParseNode(scene->mRootNode, scene, nullptr, [this, &meshInstanceNum, &options](unsigned int meshIndex, const aiScene* scene, int /*level*/)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		this->ProcessMesh(meshInstanceNum, mesh, scene, options.mFlipNormal);
		++meshInstanceNum;
		return true;
	}, 0))
	{
		goto LoadModelEnd;
	}

	mIsLoaded = true;

	PRINT_MESSAGE("\t* Vertex count : %li", mVertexList.size());
	PRINT_MESSAGE("\t* Index count : %li", mIndexList.size());
	PRINT_MESSAGE("\t* Has bones : %i", mHasBones);
	PRINT_MESSAGE("\t* Mesh count : %li", mMeshDrawInstanceList.size());
	{
		for (int i = 0; i < mMeshDrawInstanceList.size(); ++i)
		{
			const Renderer::DrawElementsIndirectCommand & v = mMeshDrawInstanceList[i];
			PRINT_MESSAGE("\t\t- Mesh %i : mElemCount=%li, mFirstIndex=%li, mBaseVertex=%li, mBaseInstance=%li", i, v.mElemCount, v.mFirstIndex, v.mBaseVertex, v.mBaseInstance);
		}
	}

	PRINT_MESSAGE("\t* Material count : %li", mMaterialList.size());
	{		
		for (int i = 0; i < mMaterialList.size(); ++i)
		{
			const Renderer::MaterialDesc & v = mMaterialList[i];
			PRINT_MESSAGE("\t\t- Material %i : Texture index (DSNE) = (%i, %i, %i, %i)", i, (int8_t)v.mDiffuseTextureIndex, (int8_t)v.mSpecularTextureIndex, (int8_t)v.mNormalTextureIndex, (int8_t)v.mEmissiveTextureIndex);
			PRINT_MESSAGE("\t\t\tDiffuse=(%f, %f, %f), Specular=(%f, %f, %f), Roughness=%f, Emissive=(%f, %f, %f)", v.mDiffuse.x, v.mDiffuse.y, v.mDiffuse.z, v.mSpecular.x, v.mSpecular.y, v.mSpecular.z, v.mRoughness, v.mEmissive.x, v.mEmissive.y, v.mEmissive.z);
		}
	}

	PRINT_MESSAGE("\t* Texture count : %li", mTextureList.size());
	{
		for (int i = 0; i < mTextureList.size(); ++i)
		{
			const Renderer::TextureDesc & v = mTextureList[i];
			PRINT_MESSAGE("\t\t- Texture %i : Category=%s, filename='%s'", i, TextureCategoryToString(v.mCategory), v.mFilename.c_str());
		}
	}


LoadModelEnd:
	PRINT_MESSAGE(".....loading model '%s' ended.", filepath.c_str());
	PRINT_END_SECTION;
}

void ModelData::PrintNodeMatrix(const aiMatrix4x4 & aiMat, int level, const char * indent)
{
	//glm::mat4 mat(aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1, aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2, aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3, aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4);
	glm::mat4 mat;
	SetMatrix(aiMat, mat);
	const glm::vec4 & c1 = mat[0];
	const glm::vec4 & c2 = mat[1];
	const glm::vec4 & c3 = mat[2];
	const glm::vec4 & c4 = mat[3];

	PRINT_MESSAGE("|%s[%li] %f, %f, %f, %f", indent, level, c1.x, c2.x, c3.x, c4.x);
	PRINT_MESSAGE("|%s[%li] %f, %f, %f, %f", indent, level, c1.y, c2.y, c3.y, c4.y);
	PRINT_MESSAGE("|%s[%li] %f, %f, %f, %f", indent, level, c1.z, c2.z, c3.z, c4.z);
	PRINT_MESSAGE("|%s[%li] %f, %f, %f, %f", indent, level, c1.w, c2.w, c3.w, c4.w);
}

void ModelData::SetMatrix(const aiMatrix4x4 & from, glm::mat4 & to)
{
	to[0][0] = from.a1; to[1][0] = from.a2;
	to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2;
	to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2;
	to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2;
	to[2][3] = from.d3; to[3][3] = from.d4;
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

		aiString name;
		mat->Get(AI_MATKEY_NAME, name);

		glm::vec3 diffuseColor(GetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE));
		glm::vec3 specularColor(GetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR));
		glm::vec3 emissiveColor(GetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE));

		float roughness;  
		//if (mat->Get(AI_MATKEY_SHININESS, roughness) != aiReturn_SUCCESS)
		{
			roughness = 1.f;
		}

		Renderer::TextureIndex diffuseTextureIndex = ProcessTextures(mDiffuseTextureList, mat, aiTextureType_DIFFUSE, textureBasePath);
		Renderer::TextureIndex specularTextureIndex = ProcessTextures(mSpecularTextureList, mat, aiTextureType_SPECULAR, textureBasePath);
		Renderer::TextureIndex emissiveTextureIndex = ProcessTextures(mEmissiveTextureList, mat, aiTextureType_EMISSIVE, textureBasePath);
		Renderer::TextureIndex normalTextureIndex = ProcessTextures(mNormalTextureList, mat, aiTextureType_NORMALS, textureBasePath);
		Renderer::TextureIndex roughnessTextureIndex = Renderer::NoTexture;

		mMaterialList.push_back(Renderer::MaterialDesc(diffuseColor, diffuseTextureIndex, specularColor, specularTextureIndex, roughness, roughnessTextureIndex, emissiveColor, emissiveTextureIndex, normalTextureIndex));

		PRINT_MESSAGE("\t-Parse material %i: Name='%s', Diffuse=(%f, %f, %f), Specular=(%f, %f, %f), Roughness=%f, Emissive=(%f, %f, %f)", i, name.C_Str(), diffuseColor.x, diffuseColor.y, diffuseColor.z, specularColor.x, specularColor.y, specularColor.z, roughness, emissiveColor.x, emissiveColor.y, emissiveColor.z);
	}
}

bool ModelData::ParseNode(aiNode* node, const aiScene* scene, std::function<bool(aiNode* node, const aiScene* scene, int level)> processNodeFunc, std::function<bool(unsigned int meshIndex, const aiScene* scene, int level)> processMeshFunc, int level)
{
	if (processNodeFunc != nullptr)
	{
		if (!processNodeFunc(node, scene, level))
			return false;
	}

	// Process each mesh located at the current node
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		if (!processMeshFunc(node->mMeshes[i], scene, level + 1))
			return false;
	}

	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		if (!ParseNode(node->mChildren[i], scene, processNodeFunc, processMeshFunc, level + 1))
			return false;
	}
	return true;
}

void ModelData::ProcessMesh(GLuint meshInstanceNum, aiMesh* mesh, const aiScene* /*scene*/, bool flipNormal)
{
	float c = flipNormal ? -1.f : 1.f;
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
		if (mesh->HasNormals())
		{
			vertex.mNormal.x = c * mesh->mNormals[i].x;
			vertex.mNormal.y = c * mesh->mNormals[i].y;
			vertex.mNormal.z = c * mesh->mNormals[i].z;
		}
		else
			vertex.mNormal = glm::vec3(0.f);

		// Tangent
		if (mesh->HasTangentsAndBitangents())
		{
			vertex.mTangent.x = c * mesh->mTangents[i].x;
			vertex.mTangent.y = c * mesh->mTangents[i].y;
			vertex.mTangent.z = c * mesh->mTangents[i].z;
		}
		else
			vertex.mTangent = glm::vec3(0.f);

		// Texture Coordinates
		if (mesh->HasTextureCoords(0)) // Does the mesh contain texture coordinates?
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

	if (mHasBones)
	{
		ProcessMeshBones(meshInstanceNum, mesh);
	}
}

void ModelData::ProcessMeshBones(GLuint meshInstanceNum, aiMesh * mesh)
{
	for (GLuint i = 0; i < mesh->mNumBones; i++)
	{
		aiBone * bone = mesh->mBones[i];
		GLuint boneIndex = 0;
		std::string boneName(bone->mName.C_Str());

		BoneMapping::const_iterator it = mBoneMapping.find(boneName);
		if (it == mBoneMapping.end()) 
		{
			// Allocate an index for a new bone
			boneIndex = (GLuint)mBoneDataList.size();
			Renderer::BoneData boneData;
			SetMatrix(bone->mOffsetMatrix, boneData.mOffsetMatrix);
			mBoneDataList.push_back(boneData);
			mBoneMapping[boneName] = boneIndex;
		}
		else 
		{
			boneIndex = it->second;
		}

		for (GLuint j = 0; j < bone->mNumWeights; j++) 
		{
			aiVertexWeight & weight = bone->mWeights[j];
			GLuint vertexId = mMeshDrawInstanceList[meshInstanceNum].mBaseVertex + weight.mVertexId;
			mVertexBoneDataList[vertexId].AddBoneData(boneIndex, weight.mWeight);
		}
	}
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
