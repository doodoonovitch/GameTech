#pragma once
#ifndef COREFX_GEOMETRY_MESHDATA_H
#define COREFX_GEOMETRY_MESHDATA_H

#include <glm\glm.hpp>
#include <GL\glew.h>
#include "VertexData.h"

#define GET_NEXT_ELEM(meshElem, nodeName) meshElem = meshElem->NextSiblingElement(nodeName)
#define XML_SUCCEEDED(op) (op == tinyxml2::XML_SUCCESS)
#define XML_FAILED(op) (op != tinyxml2::XML_SUCCESS)

namespace CoreFx
{
	namespace Geometry
	{
	
typedef VertexBufferT<Vertex, GLushort> VertexList;
typedef VertexBufferT<VertexN, GLushort> VertexNList;
typedef VertexBufferT<VertexNT, GLushort> VertexNTList;
typedef VertexBufferT<VertexNTB, GLushort> VertexNTBList;
typedef VertexBufferT<VertexUV, GLushort> VertexUVList;
typedef VertexBufferT<VertexNUV, GLushort> VertexNUVList;
typedef VertexBufferT<VertexNTUV, GLushort> VertexNTUVList;
typedef VertexBufferT<VertexNTBUV, GLushort> VertexNTBUVList;

enum class VertexType
{
	Position,
	PositionNormal,
	PositionNormalTangent,
	PositionNormalTangentBinormal,
	PositionUV,
	PositionNormalUV,
	PositionNormalTangentUV,
	PositionNormalTangentBinormalUV,
};

struct SubMeshDesc
{
	int mFirstIndex;
	int mCount;
	int mMaterialIndex;
};

class MeshData
{
public:


public:

	MeshData(VertexType vertexType, int vertexCount, int polygonCount, int subMeshCount);
	~MeshData();

	MeshData(const MeshData &) = delete;
	MeshData & operator=(const MeshData &) = delete;

	VertexType GetVertexType() const
	{
		return mVertexType;
	}

	int GetVertexCount() const
	{
		return mVertexCount;
	}

	int GetPolygonCount() const
	{
		return mPolygonCount;
	}

	const std::vector<SubMeshDesc>& GetSubMeshDescList() const
	{
		return mSubMeshDescList;
	}

	const std::vector<MaterialData> & GetMaterialList() const
	{
		return mMaterialList;
	}

	void CopySubMeshDescList(std::vector<Geometry::SubMeshDesc>& target) const
	{
		target.reserve(mSubMeshDescList.size());
		std::copy(mSubMeshDescList.begin(), mSubMeshDescList.end(), std::back_inserter(target));
	}

	void CopyMaterialList(std::vector<Geometry::MaterialData>& target) const
	{
		target.reserve(mMaterialList.size());
		std::copy(mMaterialList.begin(), mMaterialList.end(), std::back_inserter(target));
	}

	void GetVertexComponentPointers(glm::vec3*& position, glm::vec3*& normal, glm::vec3*& tangent, glm::vec3*& binormal, glm::vec2*& uv, int& vertexDataSize) const;
	void GetVertexComponentOffsets(void*& baseptr, ptrdiff_t& position, ptrdiff_t& normal, ptrdiff_t& tangent, ptrdiff_t& binormal, ptrdiff_t& uv, int& vertexDataSize) const;


	static bool Load(const std::wstring & filename, std::vector<MeshData*> & meshList, int & loadFailedMeshCount);
	static bool Load(const std::string& filename, std::vector<MeshData*> & meshList, int & loadFailedMeshCount);
	static bool Load(FILE* file, std::vector<MeshData*> & meshList, int & loadFailedMeshCount);

private:


	static bool InternalLoad(tinyxml2::XMLDocument & xmlDoc, std::vector<MeshData*> & meshList, int & loadFailedMeshCount);
	static MeshData * LoadMesh(tinyxml2::XMLElement * meshElem);
	static bool FindVertexType(const char* vertexFormat, VertexType& vertexType);
	static bool LoadMaterials(std::vector<MaterialData> & materials, tinyxml2::XMLElement * materialsElem, const char * meshName);
	static bool LoadTextures(std::vector<TextureDesc> & textures, std::map<int, SamplerDesc> & samplerDescList, tinyxml2::XMLElement * texturesElem, const char * meshName);
	static bool TryParseWrapUV(const char * text, int & value);

	template<typename TVecX, int componentCount>
	static bool LoadVertexAttributes(TVecX& retValues, tinyxml2::XMLElement * vertexElem, const char* const * attributeNames)
	{
		for (int i = 0; i < componentCount; ++i)
		{
			if(vertexElem->QueryAttribute(attributeNames[i], &retValues[i]) != tinyxml2::XML_NO_ERROR)
				return false;
		}

		return true;
	}

	static bool LoadMaterialProperty(glm::vec4 & matProperty, int & textureIndex, tinyxml2::XMLElement *& propertyElem, const char * propertyName, tinyxml2::XMLElement * materialElem, const char * meshName);


	void ReleaseAll();

	static const char* const cNormal[4];
	static const char* const cTangent[4];
	static const char* const cBinormal[4];
	static const char* const cXYZW[4];
	static const char* const cRGBA[4];
	static const char* const cUV[2];


private:

	union MeshDataUnion
	{
		VertexList * mVertexList;
		VertexNList * mVertexNList;
		VertexNTList * mVertexNTList;
		VertexNTBList * mVertexNTBList;
		VertexUVList * mVertexUVList;
		VertexNUVList * mVertexNUVList;
		VertexNTUVList * mVertexNTUVList;
		VertexNTBUVList * mVertexNTBUVList;
	} mDataUnion;


	int mVertexCount;
	int mPolygonCount;
	VertexType mVertexType;

	std::vector<SubMeshDesc> mSubMeshDescList;
	std::vector<MaterialData> mMaterialList;
	std::vector<TextureDesc> mTextureDescList;
	std::map<int, SamplerDesc> mSamplerDescList;

};

	} // namespace Geometry
} // namespace Core
#endif // COREFX_GEOMETRY_MESHDATA_H