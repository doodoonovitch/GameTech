#ifndef DATAFBXWRITER_MESHWRITER_H
#define DATAFBXWRITER_MESHWRITER_H

#pragma once


class MeshWriter :
	public FbxWriter
{

public:

	MeshWriter(FbxManager &pManager, int id);
	virtual ~MeshWriter();

	virtual bool FileCreate(char * pFileName) override;
	virtual bool FileClose() override;
	virtual bool IsFileOpen() override;
	virtual void GetWriteOptions() override;
	virtual bool Write(FbxDocument * pDocument) override;
	virtual bool PreprocessScene(FbxScene & scene) override;
	virtual bool PostprocessScene(FbxScene & scene) override;

	static FbxWriter * CreateGameTechMeshWriter(FbxManager &manager, FbxExporter & exporter, int subID, int pluginID);
	static void * GameTechMeshFormatInfo(FbxWriter::EInfoRequest request, int id);
	static void FillIOSettings(FbxIOSettings &ios);

private:

	struct TextureInfo
	{
		TextureInfo()
			: mTexture(nullptr)
			, mIndex(-1)
		{}
		TextureInfo(const FbxTexture* texture, int index)
			: mTexture(texture)
			, mIndex(index)
		{}
		TextureInfo(const TextureInfo & src)
			: mTexture(src.mTexture)
			, mIndex(src.mIndex)
		{}

		TextureInfo& operator=(const TextureInfo & src)
		{
			mTexture = src.mTexture;
			mIndex = src.mIndex;
			return *this;
		}

		~TextureInfo() {}

		const FbxTexture* mTexture;
		int mIndex;
	};
	typedef std::map<std::string, TextureInfo> TextureMap;

	static const char * BoolToString(bool value)
	{
		return value ? "true" : "false";
	}
 
	bool PreprocessNodeRecursive(FbxNode* fbxNode);
	bool PreprocessNode(FbxNode* fbxNode);

	tinyxml2::XMLElement * ExportScene(FbxScene * scene);
	tinyxml2::XMLElement * ExportNodeRecursive(tinyxml2::XMLElement * parentNode, const FbxNode* fbxNode);
	tinyxml2::XMLElement * ExportNode(tinyxml2::XMLElement * parentNode, const FbxNode* fbxNode);

	tinyxml2::XMLElement * ExportMeshList(tinyxml2::XMLElement * parentElement);
	tinyxml2::XMLElement * ExportMesh(tinyxml2::XMLElement * parentElement, const FbxNode* meshNode);
	tinyxml2::XMLElement * ExportPolygonGroup(tinyxml2::XMLElement * parentElement, const FbxMesh* mesh, const std::vector<int> & polygonIndex, int materialId = -1);

	void WritePolygon(tinyxml2::XMLElement * parentElement, const FbxMesh* mesh, int & vertexIndex, int polygonIndex);

	tinyxml2::XMLElement * ExportMaterials(tinyxml2::XMLElement * parentElement, const FbxNode* meshNode, TextureMap & textureMap);
	tinyxml2::XMLElement * ExportMaterialProperty(tinyxml2::XMLElement * parentElement, const char * propertyName, const char * factorPropertyName, const FbxSurfaceMaterial * material, TextureMap & textureMap, bool premultiplyFactor = true);

	void WriteMaterialPropertyTexture(tinyxml2::XMLElement * matPropElem, const FbxTexture * texture, int blendMode, TextureMap & textureMap);


	template<typename TFbxDoubleX, int fieldCount>
	static void WriteFbxDoubleX(tinyxml2::XMLElement * elem, const TFbxDoubleX & value, const char* const * fieldNames)
	{
		for (int i = 0; i < fieldCount; ++i)
		{
			elem->SetAttribute(fieldNames[i], value[i]);
		}
	}

	void WriteVector3D(tinyxml2::XMLElement * elem, const FbxDouble3 & value) const;
	void WriteVector3D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const;
	void WriteVector4D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const;

	void WriteNormal3D(tinyxml2::XMLElement * elem, const FbxDouble3 & value) const;
	void WriteNormal3D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const;
	void WriteNormal4D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const;

	void WriteTangent3D(tinyxml2::XMLElement * elem, const FbxDouble3 & value) const;
	void WriteTangent3D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const;
	void WriteTangent4D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const;

	void WriteBinormal3D(tinyxml2::XMLElement * elem, const FbxDouble3 & value) const;
	void WriteBinormal3D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const;
	void WriteBinormal4D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const;

	void WriteRGB(tinyxml2::XMLElement * elem, const FbxDouble3 & value) const;
	void WriteRGBA(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const;
	void WriteUV(tinyxml2::XMLElement * elem, const FbxDouble2 & value) const;

	tinyxml2::XMLElement * ExportRGB(tinyxml2::XMLElement * parentElem, const FbxDouble3 & value, const char* elemName);

	typedef std::vector<const FbxNode*> FbxNodeVector;

	//struct TextureInfo
	//{
	//	TextureInfo() {}

	//	TextureInfo(int matIndex, int blendMode)
	//		: mMaterialIndex(matIndex), mBlendMode(blendMode)
	//	{}

	//	TextureInfo(const TextureInfo & src)
	//		: mMaterialIndex(src.mMaterialIndex), mBlendMode(src.mBlendMode)
	//	{}

	//	int mMaterialIndex;
	//	int mBlendMode;
	//};

	//typedef std::map<FbxTexture*, TextureInfo> FbxTextureMap;

	struct PolygonInfo
	{
		std::vector<int> mPolygonIndex;
	};

	struct MaterialMapping
	{
		std::vector<PolygonInfo> mMatMapping;
		std::vector<int> mNoMaterialPolygonIndex;
	};

	void BuildMaterialMapping(MaterialMapping& matMap, bool & isAllSame, const FbxMesh* mesh);


	template<typename TElement, typename TValue, int componentsCount>
	void WritePolygonProperties(tinyxml2::XMLElement * elem, const TElement * elemX, int vertexIndex, int controlPointIndex, const char* const attributeNames[])
	{
		if (elemX == nullptr)
			return;

		switch (elemX->GetMappingMode())
		{
		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (elemX->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				WriteFbxDoubleX<TValue, componentsCount>(elem, elemX->GetDirectArray().GetAt(vertexIndex), attributeNames);
				break;
			case FbxGeometryElement::eIndexToDirect:
			{
				int id = elemX->GetIndexArray().GetAt(vertexIndex);
				WriteFbxDoubleX<TValue, componentsCount>(elem, elemX->GetDirectArray().GetAt(id), attributeNames);
			}
			break;
			default:
				break; // other reference modes not shown here!
			}
		}
		break;

		case FbxGeometryElement::eByControlPoint:
		{
			switch (elemX->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				WriteFbxDoubleX<TValue, componentsCount>(elem, elemX->GetDirectArray().GetAt(controlPointIndex), attributeNames);
				break;
			case FbxGeometryElement::eIndexToDirect:
			{
				int id = elemX->GetIndexArray().GetAt(controlPointIndex);
				WriteFbxDoubleX<TValue, componentsCount>(elem, elemX->GetDirectArray().GetAt(id), attributeNames);
			}
			break;
			default:
				break; // other reference modes not shown here!
			}
			break;
		}
		break;

		default:
			break;

		}
	}


private:

	FbxScene* mScene;
	FILE* mFile;
	tinyxml2::XMLElement* mSceneNode;
	FbxString mFilename;
	tinyxml2::XMLDocument mXmlDoc;
	FbxNodeVector mMeshList;
	//FbxTextureMap mTextureMap;

	const char* mNormal[4] = { "NX", "NY", "NZ", "NW" };
	const char* mTangent[4] = { "TX", "TY", "TZ", "TW" };
	const char* mBinormal[4] = { "BX", "BY", "BZ", "BW" };
	const char* mXYZW[4] = { "X", "Y", "Z", "W" };
	const char* mRGBA[4] = { "R", "G", "B", "A" };
	const char* mUV[2] = { "U", "V" };

};


#endif // DATAFBXWRITER_MESHWRITER_H