#include "stdafx.h"
#include "MeshWriter.h"



MeshWriter::MeshWriter(FbxManager &pManager, int id)
	: FbxWriter(pManager, id, FbxStatusGlobal::GetRef())
	, mScene(nullptr)
	, mFile(nullptr)
	, mSceneNode(nullptr)
{
}


MeshWriter::~MeshWriter()
{
	FileClose();
}


bool MeshWriter::FileCreate(char *pFileName) 
{
	FBXSDK_printf("[MeshWriter::FileCreate] File name '%s'.\n", pFileName);

	mFilename = FbxPathUtils::Clean(pFileName);
	FbxString path(FbxGetApplicationDirectory());
	if (!FbxPathUtils::Create(FbxPathUtils::GetFolderName(mFilename)))
		return (GetStatus().SetCode(FbxStatus::eFailure, "Cannot create folder!"), false);

	//<open your file here>
	if (fopen_s(&mFile, mFilename.Buffer(), "w") != 0)
	{
		mFile = nullptr;

		char buffer[80];
		strerror_s(buffer, 80, errno);

		FBXSDK_printf("[gtMeshWriter] Cannot open file '%s'. Error: '%s'.\n", mFilename.Buffer(), buffer);

		return false;
	}

	return (IsFileOpen());
}

bool MeshWriter::FileClose() 
{
	//<flush and close your file here>

	if (mFile == nullptr)
		return false;

	tinyxml2::XMLError err = mXmlDoc.SaveFile(mFile);
	if (err != tinyxml2::XML_NO_ERROR)
	{
		FBXSDK_printf("[gtMeshWriter] Cannot save the xml file '%s'. Error: '%i'.\n", mFilename.Buffer(), err);
	}

	fclose(mFile);
	mFile = nullptr;

	mXmlDoc.Clear();

	return true;
}

bool MeshWriter::IsFileOpen() 
{
	//<is your file open ? >
	return mFile != nullptr;
}

void MeshWriter::GetWriteOptions() 
{
}

bool MeshWriter::Write(FbxDocument *pDocument) 
{
	FBXSDK_printf("[MeshWriter::Write] ...\n");

	mScene = FbxCast<FbxScene>(pDocument);
	if (!mScene)
		return (GetStatus().SetCode(FbxStatus::eFailure, "Document not supported!"), false);

	if (!PreprocessScene(*mScene))
		return (false);

	FbxDocumentInfo *pSceneInfo = mScene->GetSceneInfo();

	// <write data in your file here>
	tinyxml2::XMLElement * sceneNode = ExportScene(mScene);
	if (sceneNode == nullptr)
	{
		return false;
	}

	mXmlDoc.InsertEndChild(sceneNode);

	if (!PostprocessScene(*mScene))
		return (false);

	return (true);
}


bool MeshWriter::PreprocessScene(FbxScene &scene) 
{
	mMeshList.clear();
	FbxNode *rootNode = scene.GetRootNode();
	return PreprocessNodeRecursive(rootNode);
}

bool MeshWriter::PreprocessNodeRecursive(FbxNode* fbxNode)
{
	if (!PreprocessNode(fbxNode))
		return false;

	const int lChildCount = fbxNode->GetChildCount();
	for (int i = 0; i < lChildCount; i++)
	{
		if (!PreprocessNodeRecursive(fbxNode->GetChild(i)))
			return false;
	}
	return true;
}

bool MeshWriter::PreprocessNode(FbxNode* fbxNode)
{
	const FbxNodeAttribute *fbxNodeAttribute = fbxNode->GetNodeAttribute();
	if (fbxNodeAttribute != nullptr)
	{
		switch (fbxNodeAttribute->GetAttributeType())
		{
		case FbxNodeAttribute::eUnknown:
		default:
			break;

		case FbxNodeAttribute::eNull:
			break;

		case FbxNodeAttribute::eMarker:
			break;

		case FbxNodeAttribute::eSkeleton:
			break;

		case FbxNodeAttribute::eMesh:
			mMeshList.push_back(fbxNode);
			//if (!PreprocessTextures((FbxMesh*)fbxNodeAttribute))
			//	return false;
			break;

		case FbxNodeAttribute::eNurbs:
			break;

		case FbxNodeAttribute::ePatch:
			break;

		case FbxNodeAttribute::eCamera:
			break;

		case FbxNodeAttribute::eCameraStereo:
			break;

		case FbxNodeAttribute::eCameraSwitcher:
			break;

		case FbxNodeAttribute::eLight:
			break;

		case FbxNodeAttribute::eOpticalReference:
			break;

		case FbxNodeAttribute::eOpticalMarker:
			break;

		case FbxNodeAttribute::eNurbsCurve:
			break;

		case FbxNodeAttribute::eTrimNurbsSurface:
			break;

		case FbxNodeAttribute::eBoundary:
			break;

		case FbxNodeAttribute::eNurbsSurface:
			break;

		case FbxNodeAttribute::eShape:
			break;

		case FbxNodeAttribute::eLODGroup:
			break;

		case FbxNodeAttribute::eSubDiv:
			break;

		case FbxNodeAttribute::eCachedEffect:
			break;

		case FbxNodeAttribute::eLine:
			break;
		}
	}

	return true;
}

//bool MeshWriter::PreprocessTextures(FbxMesh* mesh)
//{
//	int materialCount = mesh->GetNode()->GetSrcObjectCount<FbxSurfaceMaterial>();
//
//	if (materialCount > 0)
//	{
//		for (int matIndex = 0; matIndex < materialCount; ++matIndex)
//		{
//			FbxSurfaceMaterial* material = mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(matIndex);
//			if (material == nullptr)
//				continue;
//
//			int texIndex;
//			FBXSDK_FOR_EACH_TEXTURE(texIndex)
//			{
//				FbxProperty property = material->FindProperty(FbxLayerElement::sTextureChannelNames[texIndex]);
//				if (property.IsValid())
//				{
//					int texCount = property.GetSrcObjectCount<FbxTexture>();
//					for (int j = 0; j < texCount; ++j)
//					{
//						FbxLayeredTexture * layeredTexture = property.GetSrcObject<FbxLayeredTexture>(j);
//						if(layeredTexture != nullptr)
//						{
//							int lNbTextures = layeredTexture->GetSrcObjectCount<FbxTexture>();
//							for (int k = 0; k<lNbTextures; ++k)
//							{
//								FbxTexture* texture = layeredTexture->GetSrcObject<FbxTexture>(k);
//								if (texture)
//								{
//									if (mTextureMap.find(texture) != mTextureMap.end())
//									{
//										FbxLayeredTexture::EBlendMode blendMode;
//										layeredTexture->GetTextureBlendMode(k, blendMode);
//
//										mTextureMap[texture] = TextureInfo(matIndex, (int)blendMode);
//									}
//								}
//							}
//						}
//						else
//						{
//							FbxTexture* texture = property.GetSrcObject<FbxTexture>(j);
//							if (texture != nullptr)
//							{
//								if (mTextureMap.find(texture) == mTextureMap.end())
//								{
//									mTextureMap[texture] = TextureInfo(matIndex, -1);
//								}
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//
//	return true;
//}

bool MeshWriter::PostprocessScene(FbxScene &scene) 
{
	// ...
		return (true);
}

// Create your own writer - Your writer will get a pPluginID and pSubID.
FbxWriter * MeshWriter::CreateGameTechMeshWriter(FbxManager & manager, FbxExporter & exporter, int subID, int pluginID)
{
	FbxWriter *writer = FbxNew<MeshWriter>(manager, pluginID); // Use FbxNew instead of new, since FBX will take charge its deletion
	writer->SetIOSettings(exporter.GetIOSettings());
	return (writer);
}

// Get extension, description or version info about MyOwnWriter
void * MeshWriter::GameTechMeshFormatInfo(FbxWriter::EInfoRequest request, int id)
{
	static const char *sExt[] = { "gtmesh", 0 };
	static const char *sDesc[] = { "GameTech mesh (*.gtmesh)", 0 };
	static const char *sVersion[] = { "0.1", 0 };
	static const char *sInfoCompatible[] = { "-", 0 };
	static const char *sInfoUILabel[] = { "-", 0 };

	switch (request)
	{
	case FbxWriter::eInfoExtension:
		return (sExt);
	case FbxWriter::eInfoDescriptions:
		return (sDesc);
	case FbxWriter::eInfoVersions:
		return (sVersion);
	case FbxWriter::eInfoCompatibleDesc:
		return (sInfoCompatible);
	case FbxWriter::eInfoUILabel:
		return (sInfoUILabel);
	default:
		return (0);
	}
}

void MeshWriter::FillIOSettings(FbxIOSettings &pIOS) 
{
	// Here you can write your own FbxIOSettings and parse them.
	// Example at: http://help.autodesk.com/view/FBX/2015/ENU/?guid=__files_GUID_75CD0DC4_05C8_4497_AC6E_EA11406EAE26_htm
	// ...
}

tinyxml2::XMLElement * MeshWriter::ExportScene(FbxScene * scene)
{
	tinyxml2::XMLElement * sceneNode = mXmlDoc.NewElement("Scene");
	const char * sceneName = scene->GetName();
	sceneNode->SetAttribute("Name", sceneName);

	const FbxNode* rootNode = scene->GetRootNode();

	ExportMeshList(sceneNode);

	tinyxml2::XMLElement * testNode = mXmlDoc.NewElement("Tree");
	sceneNode->InsertEndChild(testNode);
	const int lChildCount = rootNode->GetChildCount();
	for (int i = 0; i < lChildCount; i++)
		ExportNodeRecursive(testNode, rootNode->GetChild(i));

	return sceneNode;
}

tinyxml2::XMLElement * MeshWriter::ExportNodeRecursive(tinyxml2::XMLElement * parentNode, const FbxNode* fbxNode)
{
	tinyxml2::XMLElement * node = ExportNode(parentNode, fbxNode);
	if (node == nullptr)
	{
		FBXSDK_printf("[gtMeshWriter] The node '%s' is not exported .\n", fbxNode->GetName());
	}
	
	for (int i = 0; i < fbxNode->GetChildCount(); i++) 
	{
		tinyxml2::XMLElement * childNode = ExportNodeRecursive(node, fbxNode->GetChild(i));
	}

	return node;
}

tinyxml2::XMLElement * MeshWriter::ExportNode(tinyxml2::XMLElement * parentNode, const FbxNode* fbxNode)
{
	tinyxml2::XMLElement * node = mXmlDoc.NewElement("Node");
	node->SetAttribute("Name", fbxNode->GetName());

	const FbxNodeAttribute *fbxNodeAttribute = fbxNode->GetNodeAttribute();
	if (fbxNodeAttribute != nullptr)
	{
		switch (fbxNodeAttribute->GetAttributeType())
		{
		case FbxNodeAttribute::eUnknown:
		default:
			node->SetAttribute("Type", "eUnknown");
			break;

		case FbxNodeAttribute::eNull:
			node->SetAttribute("Type", "eNull");
			break;

		case FbxNodeAttribute::eMarker:
			node->SetAttribute("Type", "eMarker");
			break;

		case FbxNodeAttribute::eSkeleton:
			node->SetAttribute("Type", "eSkeleton");
			break;

		case FbxNodeAttribute::eMesh:
		{
			node->SetAttribute("Type", "eMesh");
		}
		break;

		case FbxNodeAttribute::eNurbs:
			node->SetAttribute("Type", "eNurbs");
			break;

		case FbxNodeAttribute::ePatch:
			node->SetAttribute("Type", "ePatch");
			break;

		case FbxNodeAttribute::eCamera:
			node->SetAttribute("Type", "eCamera");
			break;

		case FbxNodeAttribute::eCameraStereo:
			node->SetAttribute("Type", "eCameraStereo");
			break;

		case FbxNodeAttribute::eCameraSwitcher:
			node->SetAttribute("Type", "eCameraSwitcher");
			break;

		case FbxNodeAttribute::eLight:
			node->SetAttribute("Type", "eLight");
			break;

		case FbxNodeAttribute::eOpticalReference:
			node->SetAttribute("Type", "eOpticalReference");
			break;

		case FbxNodeAttribute::eOpticalMarker:
			node->SetAttribute("Type", "eOpticalMarker");
			break;

		case FbxNodeAttribute::eNurbsCurve:
			node->SetAttribute("Type", "eNurbsCurve");
			break;

		case FbxNodeAttribute::eTrimNurbsSurface:
			node->SetAttribute("Type", "eTrimNurbsSurface");
			break;

		case FbxNodeAttribute::eBoundary:
			node->SetAttribute("Type", "eBoundary");
			break;

		case FbxNodeAttribute::eNurbsSurface:
			node->SetAttribute("Type", "eNurbsSurface");
			break;

		case FbxNodeAttribute::eShape:
			node->SetAttribute("Type", "eShape");
			break;

		case FbxNodeAttribute::eLODGroup:
			node->SetAttribute("Type", "eLODGroup");
			break;

		case FbxNodeAttribute::eSubDiv:
			node->SetAttribute("Type", "eSubDiv");
			break;

		case FbxNodeAttribute::eCachedEffect:
			node->SetAttribute("Type", "eCachedEffect");
			break;

		case FbxNodeAttribute::eLine:
			node->SetAttribute("Type", "eLine");
			break;
		}
	}
	parentNode->InsertEndChild(node);

	return node;
}


tinyxml2::XMLElement * MeshWriter::ExportMesh(tinyxml2::XMLElement * parentElement, const FbxNode* meshNode)
{
	tinyxml2::XMLElement * meshElem = mXmlDoc.NewElement("Mesh");
	meshElem->SetAttribute("Name", meshNode->GetName());

	parentElement->InsertEndChild(meshElem);

	FbxMesh* mesh = const_cast<FbxNode*>(meshNode)->GetMesh();

	meshElem->SetAttribute("IsTriangleMesh", BoolToString(mesh->IsTriangleMesh()));
	meshElem->SetAttribute("IsNormalsCCW", BoolToString(mesh->CheckIfVertexNormalsCCW()));
	meshElem->SetAttribute("VertexCount", mesh->GetPolygonVertexCount());
	meshElem->SetAttribute("PolygonCount", mesh->GetPolygonCount());

	bool hasUV = mesh->GetElementUVCount() > 0;
	bool hasNormal = mesh->GetElementNormalCount() > 0;
	bool hasTangent = mesh->GetElementTangentCount() > 0;
	bool hasBinormal = mesh->GetElementBinormalCount() > 0;

	std::string vertexFormat("Vertex");
	if (hasNormal)
		vertexFormat += "N";
	if (hasTangent)
		vertexFormat += "T";
	if (hasBinormal)
		vertexFormat += "B";
	if (hasUV)
		vertexFormat += "UV";

	meshElem->SetAttribute("Format", vertexFormat.c_str());

	TextureMap textureMap;
	ExportMaterials(meshElem, meshNode, textureMap);

	if (!textureMap.empty())
	{
		const char* wrapModes[] = { "Repeat", "Clamp" };

		tinyxml2::XMLElement * texListElem = mXmlDoc.NewElement("Textures");
		texListElem->SetAttribute("Count", (int)textureMap.size());
		meshElem->InsertEndChild(texListElem);

		for (auto it : textureMap)
		{
			tinyxml2::XMLElement * texElem = mXmlDoc.NewElement("Texture");
			texListElem->InsertEndChild(texElem);

			texElem->SetAttribute("Index", (int)it.second.mIndex);
			texElem->SetAttribute("Filename", it.first.c_str());

			texElem->SetAttribute("WrapU", wrapModes[it.second.mTexture->GetWrapModeU()]);
			texElem->SetAttribute("WrapV", wrapModes[it.second.mTexture->GetWrapModeV()]);

			texElem->SetAttribute("SwapUV", BoolToString(it.second.mTexture->GetSwapUV()));
		}
	}

	MaterialMapping matMap;
	bool isAllSame;
	BuildMaterialMapping(matMap, isAllSame, mesh);

	tinyxml2::XMLElement * matMapElem = mXmlDoc.NewElement("Submeshes");
	meshElem->InsertEndChild(matMapElem);

	if (isAllSame)
	{
		int matLayerCount = mesh->GetElementMaterialCount();
		if (matLayerCount <= 0)
		{
			matMapElem->SetAttribute("Mapping", "None");
		}
		else
		{
			FbxGeometryElementMaterial* materialElement = mesh->GetElementMaterial(0);
			int matId = materialElement->GetIndexArray().GetAt(0);

			tinyxml2::XMLElement * groupElem = mXmlDoc.NewElement("Submesh");
			matMapElem->InsertEndChild(groupElem);
			groupElem->SetAttribute("Material", matId);

			meshElem->SetAttribute("SubmeshCount", 1);

			int vertexIndex = 0;
			int polygonCount = mesh->GetPolygonCount();

			for (int i = 0; i < polygonCount; ++i)
			{
				WritePolygon(groupElem, mesh, vertexIndex, i >= 0 ? i : -1);
			}

			groupElem->SetAttribute("PolygonCount", polygonCount);
			groupElem->SetAttribute("VertexCount", vertexIndex);
		}
	}
	else
	{
		meshElem->SetAttribute("SubmeshCount", (unsigned)((matMap.mNoMaterialPolygonIndex.empty() ? 0 : 1) + matMap.mMatMapping.size() ));

		if (!matMap.mNoMaterialPolygonIndex.empty())
		{
			ExportPolygonGroup(matMapElem, mesh, matMap.mNoMaterialPolygonIndex , -1);
		}

		int matId = 0;
		for (auto it = matMap.mMatMapping.begin(); it != matMap.mMatMapping.end(); ++it)
		{
			ExportPolygonGroup(matMapElem, mesh, (*it).mPolygonIndex, matId++);
		}
	}

	return meshElem;
}

tinyxml2::XMLElement * MeshWriter::ExportPolygonGroup(tinyxml2::XMLElement * parentElement, const FbxMesh* mesh, const std::vector<int> & polygonIndex, int materialId)
{
	if (polygonIndex.empty())
		return nullptr;

	tinyxml2::XMLElement * groupElem = mXmlDoc.NewElement("Submesh");
	parentElement->InsertEndChild(groupElem);
	groupElem->SetAttribute("Material", materialId);

	int vertexIndex = 0;
	for (auto it = polygonIndex.begin(); it != polygonIndex.end(); ++it)
	{
		WritePolygon(groupElem, mesh, vertexIndex, *it);
	}

	groupElem->SetAttribute("PolygonCount", (int)polygonIndex.size());
	groupElem->SetAttribute("VertexCount", vertexIndex);

	return groupElem;
}

void MeshWriter::WritePolygon(tinyxml2::XMLElement * parentElement, const FbxMesh* mesh, int & vertexIndex, int polygonIndex)
{
	//tinyxml2::XMLElement * polyElem = mXmlDoc.NewElement("Polygon");
	//parentElement->InsertEndChild(polyElem);
	//polyElem->SetAttribute("Index", index);

	FbxVector4* controlPoints = mesh->GetControlPoints();

	int polygonSize = mesh->GetPolygonSize(polygonIndex);
	for (int j = 0; j < polygonSize; j++)
	{
		int controlPointIndex = mesh->GetPolygonVertex(polygonIndex, j);

		tinyxml2::XMLElement * elem = mXmlDoc.NewElement("Vertex");
		parentElement->InsertEndChild(elem);
		elem->SetAttribute("Index", vertexIndex);

		WriteVector3D(elem, controlPoints[controlPointIndex]);

		// UVs
		{
			if (mesh->GetElementUVCount() > 0)
			{
				const FbxGeometryElementUV* eUV = mesh->GetElementUV(0);
				WritePolygonProperties<FbxGeometryElementUV, FbxDouble2, 2>(elem, eUV, vertexIndex, controlPointIndex, mUV);
			}
		} // UVs

		// Normal
		{
			if (mesh->GetElementNormalCount() > 0)
			{
				const FbxGeometryElementNormal* eNormal = mesh->GetElementNormal(0);
				WritePolygonProperties<FbxGeometryElementNormal, FbxDouble4, 3>(elem, eNormal, vertexIndex, controlPointIndex, mNormal);
			}
		} // Normal

		  // Tangent
		{
			if (mesh->GetElementTangentCount() > 0)
			{
				const FbxGeometryElementTangent* eTangent = mesh->GetElementTangent(0);
				WritePolygonProperties<FbxGeometryElementTangent, FbxDouble4, 3>(elem, eTangent, vertexIndex, controlPointIndex, mTangent);
			}
		} // Tangent

		// Binormal
		{
			if (mesh->GetElementBinormalCount() > 0)
			{
				const FbxGeometryElementBinormal* eBinormal = mesh->GetElementBinormal(0);
				WritePolygonProperties<FbxGeometryElementBinormal, FbxDouble4, 3>(elem, eBinormal, vertexIndex, controlPointIndex, mBinormal);
			}
		} // Binormal


		++vertexIndex;
	} // for polygonSize
}

tinyxml2::XMLElement * MeshWriter::ExportMeshList(tinyxml2::XMLElement * parentElement)
{
	if (mMeshList.empty())
		return nullptr;

	tinyxml2::XMLElement * meshListElem = mXmlDoc.NewElement("Meshes");
	for(auto n : mMeshList)
	{
		ExportMesh(meshListElem, n);
	}

	if (meshListElem->NoChildren())
	{
		mXmlDoc.DeleteNode(meshListElem);
		return nullptr;
	}

	parentElement->InsertEndChild(meshListElem);

	return meshListElem;
}

tinyxml2::XMLElement * MeshWriter::ExportMaterialProperty(tinyxml2::XMLElement * parentElement, const char * propertyName, const char * factorPropertyName, const FbxSurfaceMaterial * material, TextureMap & textureMap, bool premultiplyFactor )
{
	const FbxProperty property = material->FindProperty(propertyName);
	const FbxProperty factorProperty = material->FindProperty(factorPropertyName);
	if (property.IsValid())
	{
		FbxDouble3 value = property.Get<FbxDouble3>();
		double factorValue(1.0);
		if (factorProperty.IsValid())
		{
			factorValue = factorProperty.Get<FbxDouble>();
			if (factorValue != 1 && premultiplyFactor)
			{
				value[0] *= factorValue;
				value[1] *= factorValue;
				value[2] *= factorValue;
			}
		}

		FbxString elemName(propertyName);
		int i = elemName.FindAndReplace("Color", "");

		tinyxml2::XMLElement * propElem = ExportRGB(parentElement, value, elemName.Buffer());
		if (!premultiplyFactor)
		{
			if (factorValue >= FBXSDK_DOUBLE_MIN)
				propElem->SetAttribute("Factor", factorValue);
			else
				propElem->SetAttribute("Factor", "0");
		}

		int textureCount = property.GetSrcObjectCount<FbxTexture>();
		for (int j = 0; j < textureCount; ++j)
		{
			//Here we have to check if it's layeredtextures, or just textures:
			FbxLayeredTexture * layeredTexture = property.GetSrcObject<FbxLayeredTexture>(j);
			if (layeredTexture != nullptr)
			{
				int textureCount = layeredTexture->GetSrcObjectCount<FbxTexture>();
				for (int k = 0; k<textureCount; ++k)
				{
					FbxTexture* texture = layeredTexture->GetSrcObject<FbxTexture>(k);
					if (texture != nullptr)
					{
						FbxLayeredTexture::EBlendMode blendMode;
						layeredTexture->GetTextureBlendMode(k, blendMode);
						WriteMaterialPropertyTexture(propElem, texture, blendMode, textureMap);
					}
				}
			}
			else
			{
				FbxTexture* texture = property.GetSrcObject<FbxTexture>(j);
				if (texture != nullptr)
				{
					WriteMaterialPropertyTexture(propElem, texture, texture->GetBlendMode(), textureMap);
				}
			}
		}

		return propElem;
	}

	return nullptr;
}

void MeshWriter::WriteMaterialPropertyTexture(tinyxml2::XMLElement * matPropElem, const FbxTexture * texture, int blendMode, TextureMap & textureMap)
{
	if (texture->GetClassId().Is(FbxFileTexture::ClassId))
	{
		const char* blendModes[] = { "Translucent", "Add", "Modulate", "Modulate2", "Over" };

		const FbxFileTexture * fileTexture = (const FbxFileTexture *)texture;

		std::string filename(FbxPathUtils::GetFileName(fileTexture->GetFileName()).Buffer());

		auto it = textureMap.find(filename);
		int textIndex;
		if (it == textureMap.end())
		{
			textIndex = (int)textureMap.size();
			textureMap[filename] = TextureInfo(texture, textIndex);
		}

		matPropElem->SetAttribute("BlendMode", blendModes[blendMode > 4 ? 4 : blendMode]);

		matPropElem->SetAttribute("Texture", textIndex);
	}
}



tinyxml2::XMLElement * MeshWriter::ExportMaterials(tinyxml2::XMLElement * parentElement, const FbxNode* meshNode, TextureMap & textureMap)
{
	tinyxml2::XMLElement * listElem = nullptr;

	int materialCount = meshNode->GetMaterialCount();

	if (materialCount > 0)
	{
		listElem = mXmlDoc.NewElement("Materials");
		listElem->SetAttribute("Count", materialCount);

		parentElement->InsertEndChild(listElem);

		for (int i = 0; i < materialCount; ++i)
		{
			FbxSurfaceMaterial* material = meshNode->GetMaterial(i);
			if (material == nullptr)
				continue;

			tinyxml2::XMLElement * elem = mXmlDoc.NewElement("Material");
			elem->SetAttribute("Index", i);
			elem->SetAttribute("Name", material->GetName());

			listElem->InsertEndChild(elem);

			//Get the implementation to see if it's a hardware shader.
			const FbxImplementation* implementation = GetImplementation(material, FBXSDK_IMPLEMENTATION_HLSL);
			FbxString implemenationType = "HLSL";
			if (!implementation)
			{
				implementation = GetImplementation(material, FBXSDK_IMPLEMENTATION_CGFX);
				implemenationType = "CGFX";
			}
			if (implementation)
			{
				elem->SetAttribute("ShadingModel", implemenationType.Buffer());
			}
			else
			{
				ExportMaterialProperty(elem, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, material, textureMap);
				ExportMaterialProperty(elem, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, material, textureMap);
				ExportMaterialProperty(elem, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, material, textureMap);

				if (material->GetClassId().Is(FbxSurfacePhong::ClassId))
				{
					elem->SetAttribute("ShadingModel", "Phong");

					FbxSurfacePhong * phongSurface = FbxCast<FbxSurfacePhong>(material);
					elem->SetAttribute("Shininess", phongSurface->Shininess.Get());

					ExportMaterialProperty(elem, FbxSurfaceMaterial::sReflection, FbxSurfaceMaterial::sReflectionFactor, material, textureMap);
					ExportMaterialProperty(elem, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, material, textureMap);
				}
				else if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
				{
					elem->SetAttribute("ShadingModel", "Lambert");
				}
				else
				{
					elem->SetAttribute("ShadingModel", "Unknown");
					FBXSDK_printf("[gtMeshWriter] Unknown type of material : '%s'.\n", material->GetClassId().GetName());
					continue;
				}

				ExportMaterialProperty(elem, FbxSurfaceMaterial::sTransparentColor, FbxSurfaceMaterial::sTransparencyFactor, material, textureMap, false);

				//ExportMaterialProperty(elem, FbxSurfaceMaterial::sVectorDisplacementColor, FbxSurfaceMaterial::sVectorDisplacementFactor, material, textureMap);
				//ExportMaterialProperty(elem, FbxSurfaceMaterial::sDisplacementColor, FbxSurfaceMaterial::sDisplacementFactor, material, textureMap);

				ExportMaterialProperty(elem, FbxSurfaceMaterial::sBump, FbxSurfaceMaterial::sBumpFactor, material, textureMap);
				ExportMaterialProperty(elem, FbxSurfaceMaterial::sNormalMap, "NormalMapFactor", material, textureMap);
			}
		}
	}

	return listElem;
}

//tinyxml2::XMLElement * MeshWriter::ExportTextureList(tinyxml2::XMLElement * parentElement)
//{
//	const char* cTextureUses[] = { "Standard", "ShadowMap", "LightMap",
//		"SphericalReflexionMap", "SphereReflexionMap", "BumpNormalMap" };
//	const char* cBlendModes[] = { "Translucent", "Add", "Modulate", "Modulate2" };
//
//
//	tinyxml2::XMLElement * listElem = nullptr;
//
//	if (!mTextureMap.empty())
//	{
//		listElem = mXmlDoc.NewElement("Textures");
//		listElem->SetAttribute("Count", (int)mTextureMap.size());
//
//		parentElement->InsertEndChild(listElem);
//
//		FbxTextureMap::const_iterator it;
//		for (it = mTextureMap.begin(); it != mTextureMap.end(); ++it)
//		{
//			tinyxml2::XMLElement * elem = mXmlDoc.NewElement("Texture");
//			elem->SetAttribute("MaterialIndex", it->second.mMaterialIndex);
//
//			listElem->InsertEndChild(elem);
//
//			FbxTexture* texture = it->first;
//			elem->SetAttribute("Name", texture->GetName());
//
//			elem->SetAttribute("Use", cTextureUses[texture->GetTextureUse()]);
//
//			if (it->second.mBlendMode >= 0)
//			{
//				elem->SetAttribute("Use", cBlendModes[it->second.mBlendMode]);
//			}
//
//			FbxFileTexture *fileTexture = FbxCast<FbxFileTexture>(texture);
//			FbxProceduralTexture *proceduralTexture = FbxCast<FbxProceduralTexture>(texture);
//
//			if (fileTexture != nullptr)
//			{
//				elem->SetAttribute("Type", "File");
//				elem->SetAttribute("Filename", fileTexture->GetFileName());
//			}
//			else if (proceduralTexture != nullptr)
//			{
//				elem->SetAttribute("Type", "Procedural");
//			}
//		}
//	}
//
//	return listElem;
//}

void MeshWriter::WriteVector3D(tinyxml2::XMLElement * elem, const FbxDouble3 & value) const
{
	WriteFbxDoubleX<FbxDouble3, 3>(elem, value, mXYZW);
}

void MeshWriter::WriteVector3D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const
{
	WriteFbxDoubleX<FbxDouble4, 3>(elem, value, mXYZW);
}

void MeshWriter::WriteVector4D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const
{
	WriteFbxDoubleX<FbxDouble4, 4>(elem, value, mXYZW);
}

void MeshWriter::WriteNormal3D(tinyxml2::XMLElement * elem, const FbxDouble3 & value) const
{
	WriteFbxDoubleX<FbxDouble3, 3>(elem, value, mNormal);
}

void MeshWriter::WriteNormal3D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const
{
	WriteFbxDoubleX<FbxDouble4, 3>(elem, value, mNormal);
}

void MeshWriter::WriteNormal4D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const
{
	WriteFbxDoubleX<FbxDouble4, 4>(elem, value, mNormal);
}

void MeshWriter::WriteTangent3D(tinyxml2::XMLElement * elem, const FbxDouble3 & value) const
{
	WriteFbxDoubleX<FbxDouble3, 3>(elem, value, mTangent);
}

void MeshWriter::WriteTangent3D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const
{
	WriteFbxDoubleX<FbxDouble4, 3>(elem, value, mTangent);
}

void MeshWriter::WriteTangent4D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const
{
	WriteFbxDoubleX<FbxDouble4, 4>(elem, value, mTangent);
}

void MeshWriter::WriteBinormal3D(tinyxml2::XMLElement * elem, const FbxDouble3 & value) const
{
	WriteFbxDoubleX<FbxDouble3, 3>(elem, value, mBinormal);
}

void MeshWriter::WriteBinormal3D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const
{
	WriteFbxDoubleX<FbxDouble4, 3>(elem, value, mBinormal);
}

void MeshWriter::WriteBinormal4D(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const
{
	WriteFbxDoubleX<FbxDouble4, 4>(elem, value, mBinormal);
}

void MeshWriter::WriteRGB(tinyxml2::XMLElement * elem, const FbxDouble3 & value) const
{
	WriteFbxDoubleX<FbxDouble3, 3>(elem, value, mRGBA);
}

void MeshWriter::WriteRGBA(tinyxml2::XMLElement * elem, const FbxDouble4 & value) const
{
	WriteFbxDoubleX<FbxDouble4, 4>(elem, value, mRGBA);
}

void MeshWriter::WriteUV(tinyxml2::XMLElement * elem, const FbxDouble2 & value) const
{
	WriteFbxDoubleX<FbxDouble2, 2>(elem, value, mUV);
}

tinyxml2::XMLElement * MeshWriter::ExportRGB(tinyxml2::XMLElement * parentElem, const FbxDouble3 & value, const char* elemName)
{
	tinyxml2::XMLElement * elem = mXmlDoc.NewElement(elemName);
	parentElem->InsertEndChild(elem);
	WriteRGB(elem, value);
	return elem;
}

void MeshWriter::BuildMaterialMapping(MaterialMapping& matMap, bool & isAllSame, const FbxMesh* mesh)
{
	isAllSame = true;

	int matLayerCount = mesh->GetElementMaterialCount();
	if (matLayerCount <= 0)
		return;

	int polygonCount = mesh->GetPolygonCount();

	for (int l = 0; l < mesh->GetElementMaterialCount(); l++)
	{

		const FbxGeometryElementMaterial* lMaterialElement = mesh->GetElementMaterial(l);
		if (lMaterialElement->GetMappingMode() == FbxGeometryElement::eByPolygon)
		{
			isAllSame = false;
			break;
		}
	}

	if (isAllSame)
		return;

	const FbxGeometryElementMaterial* materialElement = mesh->GetElementMaterial(0);

	for (int i = 0; i < polygonCount; i++)
	{
		int matId = materialElement->GetIndexArray().GetAt(i);
		FbxSurfaceMaterial* material = mesh->GetNode()->GetMaterial(matId);

		if (matId >= 0)
		{
			if ((int)matMap.mMatMapping.size() <= matId)
			{
				matMap.mMatMapping.resize(matId + 1);
			}

			PolygonInfo& polyInfo = matMap.mMatMapping[matId];
			polyInfo.mPolygonIndex.push_back(i);
		}
		else
		{
			matMap.mNoMaterialPolygonIndex.push_back(i);
		}
	}
}

