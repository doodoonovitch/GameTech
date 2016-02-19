#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Geometry
	{
	// =======================================================================
	// =======================================================================

const char* const MeshData::cNormal[4] = { "NX", "NY", "NZ", "NW" };
const char* const MeshData::cTangent[4] = { "TX", "TY", "TZ", "TW" };
const char* const MeshData::cBinormal[4] = { "BX", "BY", "BZ", "BW" };
const char* const MeshData::cXYZW[4] = { "X", "Y", "Z", "W" };
const char* const MeshData::cRGBA[4] = { "R", "G", "B", "A" };
const char* const MeshData::cUV[2] = { "U", "V" };


MeshData::MeshData(VertexType vertexType, int vertexCount, int subMeshCount)
	: mVertexCount(vertexCount)
	, mVertexType(vertexType)
	, mSubMeshDescList(subMeshCount)
{
	switch (mVertexType)
	{
	case VertexType::Position:
		mDataUnion.mVertexList = new VertexList(vertexCount, 0);
		break;

	case VertexType::PositionNormal:
		mDataUnion.mVertexNList = new VertexNList(vertexCount, 0);
		break;

	case VertexType::PositionNormalTangent:
		mDataUnion.mVertexNTList = new VertexNTList(vertexCount, 0);
		break;

	case VertexType::PositionNormalTangentBinormal:
		mDataUnion.mVertexNTBList = new VertexNTBList(vertexCount, 0);
		break;

	case VertexType::PositionUV:
		mDataUnion.mVertexUVList = new VertexUVList(vertexCount, 0);
		break;

	case VertexType::PositionNormalUV:
		mDataUnion.mVertexNUVList = new VertexNUVList(vertexCount, 0);;
		break;

	case VertexType::PositionNormalTangentUV:
		mDataUnion.mVertexNTUVList = new VertexNTUVList(vertexCount, 0);
		break;

	case VertexType::PositionNormalTangentBinormalUV:
		mDataUnion.mVertexNTBUVList = new VertexNTBUVList(vertexCount, 0);
		break;
	}
}


MeshData::~MeshData()
{
	ReleaseAll();
}

void MeshData::ReleaseAll()
{
	switch (mVertexType)
	{
	case VertexType::Position:
		SAFE_DELETE(mDataUnion.mVertexList);
		break;

	case VertexType::PositionNormal:
		SAFE_DELETE(mDataUnion.mVertexNList);
		break;

	case VertexType::PositionNormalTangent:
		SAFE_DELETE(mDataUnion.mVertexNTList);
		break;

	case VertexType::PositionNormalTangentBinormal:
		SAFE_DELETE(mDataUnion.mVertexNTBList);
		break;

	case VertexType::PositionUV:
		SAFE_DELETE(mDataUnion.mVertexUVList);
		break;

	case VertexType::PositionNormalUV:
		SAFE_DELETE(mDataUnion.mVertexNUVList);
		break;

	case VertexType::PositionNormalTangentUV:
		SAFE_DELETE(mDataUnion.mVertexNTUVList);
		break;

	case VertexType::PositionNormalTangentBinormalUV:
		SAFE_DELETE(mDataUnion.mVertexNTBUVList);
		break;
	}
	mSubMeshDescList.clear();
}

bool MeshData::Load(const std::wstring & filename, std::vector<MeshData*> & meshList, int & loadFailedMeshCount)
{
	FILE* file;
	errno_t errn = _wfopen_s(&file, filename.c_str(), L"r");
	if (errn != 0)
	{
		char buffer[80];
		strerror_s(buffer, 80, errno);
		std::cerr << "Fail to open the file '" << filename.c_str() << "'." << std::endl;
		std::cerr << "  Error " << errn << " : " << buffer << std::endl; 
		return false;
	}

	bool res = MeshData::Load(file, meshList, loadFailedMeshCount);
	fclose(file);

	return res;
}

bool MeshData::Load(const std::string& filename, std::vector<MeshData*> & meshList, int & loadFailedMeshCount)
{
	FILE* file;
	errno_t errn = fopen_s(&file, filename.c_str(), "r");
	if (errn != 0)
	{
		char buffer[80];
		strerror_s(buffer, 80, errno);
		std::cerr << "Fail to open the file '" << filename << "'." << std::endl;
		std::cerr << "  Error " << errn << " : " << buffer << std::endl;
		return false;
	}

	bool res = MeshData::Load(file, meshList, loadFailedMeshCount);
	fclose(file);

	return res;
}

bool MeshData::Load(FILE* file, std::vector<MeshData*> & meshList, int & loadFailedMeshCount)
{
	tinyxml2::XMLDocument xmlDoc;

	if (!xmlDoc.LoadFile(file))
	{
		return false;
	}

	return MeshData::InternalLoad(xmlDoc, meshList, loadFailedMeshCount);
}

#define MESH_LIST_NODE "Meshes"
#define MESH_NODE "Mesh"


bool MeshData::InternalLoad(tinyxml2::XMLDocument & xmlDoc, std::vector<MeshData*> & meshList, int & loadFailedMeshCount)
{
	loadFailedMeshCount = 0;

	tinyxml2::XMLElement * rootElem = xmlDoc.RootElement();

	if (rootElem == nullptr)
	{
		std::cerr << "Load mesh failed! (Could not get the root element)" << std::endl;
		return false;
	}

	tinyxml2::XMLElement * meshListElem = rootElem->FirstChildElement(MESH_LIST_NODE);
	if (meshListElem == nullptr)
	{
		std::cerr << "Load mesh failed! (Could not get the 'Meshes' element)" << std::endl;
		return false;
	}

	tinyxml2::XMLElement * meshElem = meshListElem->FirstChildElement(MESH_NODE);
	while (meshElem != nullptr)
	{
		MeshData * meshData = LoadMesh(meshElem);
		if (meshData != nullptr)
		{
			meshList.push_back(meshData);
		}
		else
		{
			++loadFailedMeshCount;
		}

		GET_NEXT_ELEM(meshElem, MESH_NODE);
	}

	return true;
}

#define MESH_SUBMESHES "Submeshes"
#define MESH_SUBMESH "Submesh"
#define MESH_VERTEX "Vertex"
#define MESH_MATERIALS "Materials"

#define MESH_NAME "Name"
#define MESH_ISTRIANGLEMESH "IsTriangleMesh"
#define MESH_ISNORMALSCCW "IsNormalsCCW"
#define MESH_FORMAT "Format"
#define MESH_SUBMESHCOUNT "SubmeshCount"
#define MESH_VERTEXCOUNT "VertexCount"
#define MESH_VERTEX_INDEX "Index"

#define SUBMESH_VERTEXCOUNT "VertexCount"
#define SUBMESH_MATERIAL "Material"

#define MESH_TEXTURES "Textures"



MeshData* MeshData::LoadMesh(tinyxml2::XMLElement * meshElem)
{
	MeshData* meshData = nullptr;

	const char* meshName = meshElem->Attribute(MESH_NAME);

	tinyxml2::XMLError xmlErr;

	bool isTriangleMesh;
	xmlErr = meshElem->QueryBoolAttribute(MESH_ISTRIANGLEMESH, &isTriangleMesh);
	if (XML_FAILED(xmlErr) || !isTriangleMesh)
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : it's not a triangle mesh!" << std::endl;
		goto LoadFailed;
	}

	bool isNormalsCCW;
	xmlErr = meshElem->QueryBoolAttribute(MESH_ISNORMALSCCW, &isNormalsCCW);
	if (XML_FAILED(xmlErr))
	{
		std::cerr << "Couldnot get the 'IsNormalsCCW' attribute from mesh '" << meshName << "'!" << std::endl;
	}

	const char* vertexFormat = meshElem->Attribute(MESH_FORMAT);
	if (vertexFormat == nullptr)
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : couldnot get the vertex format!" << std::endl;
		goto LoadFailed;
	}

	VertexType vertexType;
	if (!FindVertexType(vertexFormat, vertexType))
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : unsupported vertex format!" << std::endl;
		goto LoadFailed;
	}

	int subMeshCount;
	xmlErr = meshElem->QueryIntAttribute(MESH_SUBMESHCOUNT, &subMeshCount);
	if (XML_FAILED(xmlErr))
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : couldnot get the 'SubmeshCount' attribute!" << std::endl;
		goto LoadFailed;
	}

	int vertexCount;
	xmlErr = meshElem->QueryIntAttribute(MESH_VERTEXCOUNT, &vertexCount);
	if (XML_FAILED(xmlErr))
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : couldnot get the 'VertexCount' attribute!" << std::endl;
		goto LoadFailed;
	}

	if (subMeshCount <= 0)
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : 'SubmeshCount' couldnot be 0 (or less) !" << std::endl;
		goto LoadFailed;
	}

	if (vertexCount != (subMeshCount * 3))
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : 'VertexCount' must be equal 3 * 'SubmeshCount' !" << std::endl;
		goto LoadFailed;
	}

	meshData = new MeshData(vertexType, vertexCount, subMeshCount);
	
	tinyxml2::XMLElement * submeshesElem = meshElem->FirstChildElement(MESH_SUBMESHES);
	if (submeshesElem == nullptr)
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : Couldnot get the 'Submeshes' element node !" << std::endl;
		goto LoadFailed;
	}

	int vertexIndex = 0;
	int polygonGroupIndex = 0;

	glm::vec3* positionPtr;
	glm::vec3* normalPtr;
	glm::vec3* tangentPtr;
	glm::vec3* binormalPtr;
	glm::vec2* uvPtr;
	int vertexDataSize;
	meshData->GetVertexComponentPointers(positionPtr, normalPtr, tangentPtr, binormalPtr, uvPtr, vertexDataSize);
		
	tinyxml2::XMLElement * submeshElem = submeshesElem->FirstChildElement(MESH_SUBMESH);
	while (submeshElem != nullptr)
	{
		SubMeshDesc & subMeshDesc = meshData->mSubMeshDescList[polygonGroupIndex];

		subMeshDesc.mFirstIndex = vertexIndex;

		xmlErr = submeshElem->QueryIntAttribute(SUBMESH_VERTEXCOUNT, &subMeshDesc.mCount);
		if (XML_FAILED(xmlErr))
		{
			std::cerr << "The mesh '" << meshName << "' is ignored : couldnot get 'Submesh' " << polygonGroupIndex << " 'VertexCount' attribute!" << std::endl;
			goto LoadFailed;
		}

		if ((subMeshDesc.mCount + vertexIndex) > vertexCount)
		{
			std::cerr << "The mesh '" << meshName << "' is ignored : the 'Submesh' " << polygonGroupIndex << " vertex count' (" << subMeshCount << ") + already loaded vertex > total mesh vertex count (" << vertexCount << ")!" << std::endl;
			goto LoadFailed;
		}
				
		xmlErr = submeshElem->QueryIntAttribute(SUBMESH_MATERIAL, &subMeshDesc.mMaterialIndex);
		if (XML_FAILED(xmlErr))
		{
			std::cerr << "The mesh '" << meshName << "' is ignored : couldnot get 'Submesh' 'Material' attribute!" << std::endl;
			goto LoadFailed;
		}

		int subMeshVertexIndex = 0;
		tinyxml2::XMLElement * vertexElem = meshElem->FirstChildElement(MESH_VERTEX);
		while (vertexElem != nullptr && subMeshVertexIndex < subMeshDesc.mCount)
		{
			if (!MeshData::LoadVertexAttributes<glm::vec3, 3>(*positionPtr, vertexElem, MeshData::cXYZW))
			{
				std::cerr << "The mesh '" << meshName << "' is ignored : couldnot load vertex position (Sub mesh="<< polygonGroupIndex << ", vertex index=" << subMeshVertexIndex << ")!" << std::endl;
				goto LoadFailed;
			}
			positionPtr = (glm::vec3*)(((uint8_t*)positionPtr) + vertexDataSize);

			if (normalPtr != nullptr)
			{
				if (!MeshData::LoadVertexAttributes<glm::vec3, 3>(*normalPtr, vertexElem, MeshData::cNormal))
				{
					std::cerr << "The mesh '" << meshName << "' is ignored : couldnot load vertex normal (Sub mesh=" << polygonGroupIndex << ", vertex index=" << subMeshVertexIndex << ")!" << std::endl;
					goto LoadFailed;
				}
				normalPtr = (glm::vec3*)(((uint8_t*)normalPtr) + vertexDataSize);
			}

			if (tangentPtr != nullptr)
			{
				if (!MeshData::LoadVertexAttributes<glm::vec3, 3>(*tangentPtr, vertexElem, MeshData::cTangent))
				{
					std::cerr << "The mesh '" << meshName << "' is ignored : couldnot load vertex tangent (Sub mesh=" << polygonGroupIndex << ", vertex index=" << subMeshVertexIndex << ")!" << std::endl;
					goto LoadFailed;
				}
				tangentPtr = (glm::vec3*)(((uint8_t*)tangentPtr) + vertexDataSize);
			}		

			if (binormalPtr != nullptr)
			{
				if (!MeshData::LoadVertexAttributes<glm::vec3, 3>(*binormalPtr, vertexElem, MeshData::cBinormal))
				{
					std::cerr << "The mesh '" << meshName << "' is ignored : couldnot load vertex binormal (Sub mesh=" << polygonGroupIndex << ", vertex index=" << subMeshVertexIndex << ")!" << std::endl;
					goto LoadFailed;
				}
				binormalPtr = (glm::vec3*)(((uint8_t*)binormalPtr) + vertexDataSize);
			}

			if (uvPtr != nullptr)
			{
				if (!MeshData::LoadVertexAttributes<glm::vec2, 2>(*uvPtr, vertexElem, MeshData::cUV))
				{
					std::cerr << "The mesh '" << meshName << "' is ignored : couldnot load vertex UV (Sub mesh=" << polygonGroupIndex << ", vertex index=" << subMeshVertexIndex << ")!" << std::endl;
					goto LoadFailed;
				}
				uvPtr = (glm::vec2*)(((uint8_t*)uvPtr) + vertexDataSize);
			}

			++vertexIndex;
			GET_NEXT_ELEM(submeshElem, MESH_VERTEX);
		}

		++polygonGroupIndex;
		GET_NEXT_ELEM(submeshElem, MESH_SUBMESH);
	}

	tinyxml2::XMLElement * materialsElem = meshElem->FirstChildElement(MESH_MATERIALS);
	if (materialsElem == nullptr)
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : Couldnot get the 'Materials' element node !" << std::endl;
		goto LoadFailed;
	}

	if (!LoadMaterials(meshData->mMaterialList, materialsElem, meshName))
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : Couldnot load materials !" << std::endl;
		goto LoadFailed;
	}

	tinyxml2::XMLElement * texturesElem = meshElem->FirstChildElement(MESH_TEXTURES);
	if (texturesElem == nullptr)
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : Couldnot get the 'Textures' element node !" << std::endl;
		goto LoadFailed;
	}

	if (!LoadTextures(meshData->mTextureDescList, meshData->mSamplerDescList, texturesElem, meshName))
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : Couldnot load textures !" << std::endl;
		goto LoadFailed;
	}

	return meshData;

LoadFailed:
	if (meshData != nullptr)
	{
		delete meshData;
	}

	return nullptr;
}

bool MeshData::FindVertexType(const char* vertexFormat, VertexType& vertexType)
{
	const char* const cVertexFormatStr[] =
	{
		"Vertex",
		"VertexN",
		"VertexNT",
		"VertexNTB",
		"VertexUV",
		"VertexNUV",
		"VertexNTUV",
		"VertexNTBUV"
	};

	const VertexType cVertexType[] =
	{
		VertexType::Position,
		VertexType::PositionNormal,
		VertexType::PositionNormalTangent,
		VertexType::PositionNormalTangentBinormal,
		VertexType::PositionUV,
		VertexType::PositionNormalUV,
		VertexType::PositionNormalTangentUV,
		VertexType::PositionNormalTangentBinormalUV
	};

	for (int i = 0; i < 8; i++)
	{
		if (std::strcmp(cVertexFormatStr[i], vertexFormat) == 0)
		{
			vertexType = cVertexType[i];
			return true;
		}
	}

	return false;
}

void MeshData::GetVertexComponentPointers(glm::vec3*& position, glm::vec3*& normal, glm::vec3*& tangent, glm::vec3*& binormal, glm::vec2*& uv, int& vertexDataSize) const
{
	switch (mVertexType)
	{
	case VertexType::Position:
	{
		Vertex* ptr = mDataUnion.mVertexList->GetVertexBuffer();
		position = &ptr->mPosition;
		normal = nullptr;
		tangent = nullptr;
		binormal = nullptr;
		uv = nullptr;
		vertexDataSize = sizeof(*ptr);
	}
	break;

	case VertexType::PositionNormal:
	{
		VertexN* ptr = mDataUnion.mVertexNList->GetVertexBuffer();
		position = &ptr->mPosition;
		normal = &ptr->mNormal;
		tangent = nullptr;
		binormal = nullptr;
		uv = nullptr;
		vertexDataSize = sizeof(*ptr);
	}
	break;

	case VertexType::PositionNormalTangent:
	{
		VertexNT* ptr = mDataUnion.mVertexNTList->GetVertexBuffer();
		position = &ptr->mPosition;
		normal = &ptr->mNormal;
		tangent = &ptr->mTangent;
		binormal = nullptr;
		uv = nullptr;
		vertexDataSize = sizeof(*ptr);
	}
	break;

	case VertexType::PositionNormalTangentBinormal:
	{
		VertexNTB* ptr = mDataUnion.mVertexNTBList->GetVertexBuffer();
		position = &ptr->mPosition;
		normal = &ptr->mNormal;
		tangent = &ptr->mTangent;
		binormal = &ptr->mTangent;
		uv = nullptr;
		vertexDataSize = sizeof(*ptr);
	}
	break;

	case VertexType::PositionUV:
	{
		VertexUV* ptr = mDataUnion.mVertexUVList->GetVertexBuffer();
		position = &ptr->mPosition;
		normal = nullptr;
		tangent = nullptr;
		binormal = nullptr;
		uv = &ptr->mTexUV;
		vertexDataSize = sizeof(*ptr);
	}
	break;

	case VertexType::PositionNormalUV:
	{
		VertexNUV* ptr = mDataUnion.mVertexNUVList->GetVertexBuffer();
		position = &ptr->mPosition;
		normal = &ptr->mNormal;
		tangent = nullptr;
		binormal = nullptr;
		uv = &ptr->mTexUV;
		vertexDataSize = sizeof(*ptr);
	}
	break;

	case VertexType::PositionNormalTangentUV:
	{
		VertexNTUV* ptr = mDataUnion.mVertexNTUVList->GetVertexBuffer();
		position = &ptr->mPosition;
		normal = &ptr->mNormal;
		tangent = &ptr->mTangent;
		binormal = nullptr;
		uv = &ptr->mTexUV;
		vertexDataSize = sizeof(*ptr);
	}
	break;

	case VertexType::PositionNormalTangentBinormalUV:
	{
		VertexNTBUV* ptr = mDataUnion.mVertexNTBUVList->GetVertexBuffer();
		position = &ptr->mPosition;
		normal = &ptr->mNormal;
		tangent = &ptr->mTangent;
		binormal = &ptr->mTangent;
		uv = &ptr->mTexUV;
		vertexDataSize = sizeof(*ptr);
	}
	break;
	}
}

void MeshData::GetVertexComponentOffsets(void*& baseptr, ptrdiff_t& position, ptrdiff_t& normal, ptrdiff_t& tangent, ptrdiff_t& binormal, ptrdiff_t& uv, int& vertexDataSize) const
{
	baseptr = mDataUnion.mVertexList->GetVertexBuffer();
	position = 0;

	switch (mVertexType)
	{
	case VertexType::Position:
	{
		normal = -1;
		tangent = -1;
		binormal = -1;
		uv = -1;
		vertexDataSize = sizeof(Vertex);
	}
	break;

	case VertexType::PositionNormal:
	{
		normal = offsetof(VertexN, mNormal);
		tangent = -1;
		binormal = -1;
		uv = -1;
		vertexDataSize = sizeof(VertexN);
	}
	break;

	case VertexType::PositionNormalTangent:
	{
		normal = offsetof(VertexNT, mNormal);
		tangent = offsetof(VertexNT, mTangent);
		binormal = -1;
		uv = -1;
		vertexDataSize = sizeof(VertexNT);
	}
	break;

	case VertexType::PositionNormalTangentBinormal:
	{
		normal = offsetof(VertexNTB, mNormal);
		tangent = offsetof(VertexNTB, mTangent);
		binormal = offsetof(VertexNTB, mBinormal);
		uv = -1;
		vertexDataSize = sizeof(VertexNTB);
	}
	break;

	case VertexType::PositionUV:
	{
		normal = -1;
		tangent = -1;
		binormal = -1;
		uv = offsetof(VertexUV, mTexUV);
		vertexDataSize = sizeof(VertexUV);
	}
	break;

	case VertexType::PositionNormalUV:
	{
		normal = offsetof(VertexNUV, mNormal);
		tangent = -1;
		binormal = -1;
		uv = offsetof(VertexNUV, mTexUV);
		vertexDataSize = sizeof(VertexNUV);
	}
	break;

	case VertexType::PositionNormalTangentUV:
	{
		normal = offsetof(VertexNTUV, mNormal);
		tangent = offsetof(VertexNTUV, mTangent);
		binormal = -1;
		uv = offsetof(VertexNTUV, mTexUV);
		vertexDataSize = sizeof(VertexNTUV);
	}
	break;

	case VertexType::PositionNormalTangentBinormalUV:
	{
		normal = offsetof(VertexNTBUV, mNormal);
		tangent = offsetof(VertexNTBUV, mTangent);
		binormal = offsetof(VertexNTBUV, mBinormal);
		uv = offsetof(VertexNTBUV, mTexUV);
		vertexDataSize = sizeof(VertexNTBUV);
	}
	break;
	}
}

#define MESH_MATERIAL "Materials"

#define MATERIAL_AMBIENT "Ambient"
#define MATERIAL_DIFFUSE "Diffuse"
#define MATERIAL_EMISSIVE "Emissive"
#define MATERIAL_REFLECTION "Reflection"
#define MATERIAL_SPECULAR "Specular"
#define MATERIAL_NORMAL "Normal"
#define MATERIAL_TRANSPARENT "Transparent"
#define MATERIAL_TRANSPARENT_FACTOR "Factor"
#define MATERIAL_SHININESS "Shininess"
#define MATERIAL_COUNT "Count"
#define MATERIAL_TEXTURE "Texture"

bool MeshData::LoadMaterialProperty(glm::vec4 & matProperty, int & textureIndex, tinyxml2::XMLElement *& propertyElem, const char * propertyName, tinyxml2::XMLElement * materialElem, const char * meshName)
{
	matProperty.a = 1.f;

	propertyElem = materialElem->FirstChildElement(propertyName);
	if (propertyElem == nullptr)
	{
		matProperty.r = 0.f;
		matProperty.g = 0.f;
		matProperty.b = 0.f;

		return true;
	}

	if (!MeshData::LoadVertexAttributes<glm::vec4, 3>(matProperty, propertyElem, MeshData::cRGBA))
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : couldnot load material property '" << propertyName << "' values!" << std::endl;
		return false;
	}

	tinyxml2::XMLError xmlErr = propertyElem->QueryIntAttribute(MATERIAL_TEXTURE, &textureIndex);
	if (XML_FAILED(xmlErr))
	{
		textureIndex = -1;
	}

	return true;
}

bool MeshData::LoadMaterials(std::vector<MaterialData> & materials, tinyxml2::XMLElement * materialsElem, const char * meshName)
{
	int matCount;
	tinyxml2::XMLError xmlErr = materialsElem->QueryIntAttribute(MATERIAL_COUNT, &matCount);
	if (XML_FAILED(xmlErr))
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : couldnot get the materials 'Count' attribute!" << std::endl;
		goto LoadMaterialsFailed;
	}

	materials.resize(matCount);

	int matIndex = 0;
	tinyxml2::XMLElement * matElem = materialsElem->FirstChildElement(MESH_MATERIAL);
	while (matElem != nullptr)
	{
		MaterialData & mat = materials[matIndex];

		if (XML_FAILED(materialsElem->QueryAttribute(MATERIAL_SHININESS, &mat.mShininess)))
		{
			std::cerr << "The mesh '" << meshName << "' is ignored : couldnot get the material 'Shininess' attribute!" << std::endl;
			goto LoadMaterialsFailed;
		}

		tinyxml2::XMLElement * propertyElem;
		if (!MeshData::LoadMaterialProperty(mat.mAmbient, mat.mAbientTexture, propertyElem, MATERIAL_AMBIENT, matElem, meshName))
			goto LoadMaterialsFailed;

		if (!MeshData::LoadMaterialProperty(mat.mDiffuse, mat.mDiffuseTexture, propertyElem, MATERIAL_DIFFUSE, matElem, meshName))
			goto LoadMaterialsFailed;

		if (!MeshData::LoadMaterialProperty(mat.mEmissive, mat.mEmissiveTexture, propertyElem, MATERIAL_EMISSIVE, matElem, meshName))
			goto LoadMaterialsFailed;

		if (!MeshData::LoadMaterialProperty(mat.mReflection, mat.mReflectionTexture, propertyElem, MATERIAL_REFLECTION, matElem, meshName))
			goto LoadMaterialsFailed;

		if (!MeshData::LoadMaterialProperty(mat.mSpecular, mat.mSpecularTexture, propertyElem, MATERIAL_SPECULAR, matElem, meshName))
			goto LoadMaterialsFailed;

		glm::vec4 temp;
		if (!MeshData::LoadMaterialProperty(temp, mat.mNormalTexture, propertyElem, MATERIAL_NORMAL, matElem, meshName))
			goto LoadMaterialsFailed;

		glm::vec4 transparency;
		int tempTextureIndex;
		if (!MeshData::LoadMaterialProperty(transparency, tempTextureIndex, propertyElem, MATERIAL_TRANSPARENT, matElem, meshName))
			goto LoadMaterialsFailed;
		if (propertyElem == nullptr || XML_FAILED(materialsElem->QueryAttribute(MATERIAL_TRANSPARENT_FACTOR, &transparency.a)))
		{
			std::cerr << "The mesh '" << meshName << "' is ignored : couldnot get the material transparency 'Factor' attribute!" << std::endl;
			goto LoadMaterialsFailed;
		}
		mat.mTransparency = 1.0f - transparency.a;

		++matIndex;
		GET_NEXT_ELEM(matElem, MESH_MATERIAL);
	}

	if (matIndex != matCount)
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : couldnot load all the materials (loaded " << matIndex << " / " << matCount << ") !" << std::endl;
		goto LoadMaterialsFailed;
	}

	return true;

LoadMaterialsFailed:
	materials.clear();
	return false;
}

#define MESH_TEXTURE "Texture"

#define TEXTURE_COUNT "Count"

#define TEXTURE_INDEX "Index"
#define TEXTURE_FILENAME "Filename"
#define TEXTURE_WRAPU "WrapU"
#define TEXTURE_WRAPV "WrapV"
#define TEXTURE_SWAPUV "SwapUV"

enum
{
	WRAP_UV_CLAMP_TO_EDGE,
	WRAP_UV_CLAMP_TO_BORDER,
	WRAP_UV_REPEAT,
	WRAP_UV_MIRRORED_REPEAT,
	WRAP_UV_MIRROR_CLAMP_TO_EDGE
};

int WrapUVConvert(int wrapUV)
{
	switch (wrapUV)
	{
	case GL_CLAMP_TO_EDGE:
		return WRAP_UV_CLAMP_TO_EDGE;
		break;
	case GL_CLAMP_TO_BORDER:
		return WRAP_UV_CLAMP_TO_BORDER;
		break;
	case GL_MIRRORED_REPEAT:
		return WRAP_UV_MIRRORED_REPEAT;
		break;
	case GL_MIRROR_CLAMP_TO_EDGE:
		return WRAP_UV_MIRROR_CLAMP_TO_EDGE;
		break;

	default:
	case GL_REPEAT:
		return WRAP_UV_REPEAT;
		break;
	}
}

bool MeshData::LoadTextures(std::vector<TextureDesc> & textures, std::map<int, SamplerDesc> & samplerDescList, tinyxml2::XMLElement * texturesElem, const char * meshName)
{
	int count;
	tinyxml2::XMLError xmlErr = texturesElem->QueryIntAttribute(TEXTURE_COUNT, &count);
	if (XML_FAILED(xmlErr))
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : couldnot get the materials 'Count' attribute!" << std::endl;
		goto LoadTexturesFailed;
	}

	textures.resize(count);

	int nRead = 0;
	tinyxml2::XMLElement * texElem = texturesElem->FirstChildElement(MESH_TEXTURE);
	while (texElem != nullptr)
	{
		int index;

		if (XML_FAILED(texturesElem->QueryAttribute(TEXTURE_INDEX, &index)))
		{
			std::cerr << "The mesh '" << meshName << "' is ignored : couldnot get the texture 'Index' attribute!" << std::endl;
			goto LoadTexturesFailed;
		}

		if (!(index >= 0 && index < count))
		{
			std::cerr << "The mesh '" << meshName << "' is ignored : texture 'Index' has a wrong value (" << index << ")!" << std::endl;
			goto LoadTexturesFailed;
		}

		TextureDesc & texDesc = textures[index];

		const char * attText = nullptr;

		attText = texElem->Attribute(TEXTURE_FILENAME);
		if (attText == nullptr)
		{
			std::cerr << "The mesh '" << meshName << "' is ignored : couldnot get the texture filename !" << std::endl;
			goto LoadTexturesFailed;
		}

		texDesc.mFilename = attText;

		attText = texElem->Attribute(TEXTURE_WRAPU);
		if (attText == nullptr)
		{
			std::cerr << "The mesh '" << meshName << "' is ignored : couldnot get the texture 'WrapU' attribute !" << std::endl;
			goto LoadTexturesFailed;
		}

		int wrapU, wrapV;
		if (!TryParseWrapUV(attText, wrapU))
		{
			std::cerr << "The mesh '" << meshName << "' is ignored : texture 'WrapU' has a wrong value (" << attText << ")!" << std::endl;
			goto LoadTexturesFailed;
		}

		attText = texElem->Attribute(TEXTURE_WRAPV);
		if (attText == nullptr)
		{
			std::cerr << "The mesh '" << meshName << "' is ignored : couldnot get the texture 'WrapV' attribute !" << std::endl;
			goto LoadTexturesFailed;
		}

		if (!TryParseWrapUV(attText, wrapV))
		{
			std::cerr << "The mesh '" << meshName << "' is ignored : texture 'WrapV' has a wrong value (" << attText << ")!" << std::endl;
			goto LoadTexturesFailed;
		}

		int key = WrapUVConvert(wrapU) | WrapUVConvert(wrapV) << 16;
		auto it = samplerDescList.find(key);
		if (it == samplerDescList.end())
		{
			texDesc.mSamplerIndex = (int)samplerDescList.size();
			samplerDescList[key] = SamplerDesc(wrapU, wrapV, texDesc.mSamplerIndex);
		}
		else
		{
			texDesc.mSamplerIndex = it->second.mIndex;
		}

		++nRead;

		GET_NEXT_ELEM(texElem, MESH_TEXTURE);
	}

	if (nRead != count)
	{
		std::cerr << "The mesh '" << meshName << "' is ignored : " << count << " texture(s) expected but " << nRead << " loaded!" << std::endl;
		goto LoadTexturesFailed;
	}

	return true;

LoadTexturesFailed:
	textures.clear();
	return false;
}

bool MeshData::TryParseWrapUV(const char * text, int & value)
{
	if (std::strcmp(text, "Repeat"))
	{
		value = GL_REPEAT;
		return true;
	}

	if (std::strcmp(text, "Clamp"))
	{
		value = GL_CLAMP_TO_EDGE;
		return true;
	}

	return false;
}

	// =======================================================================
	// =======================================================================
	} // namespace Geometry
} // namespace CoreFx
