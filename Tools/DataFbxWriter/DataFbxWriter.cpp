// DataFbxWriter.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DataFbxWriter.h"
#include "MeshWriter.h"


extern "C"
{

	static DataFbxWriter *s_pPlugin = nullptr;

DATAFBXWRITER_API void FBXPluginRegistration(FbxPluginContainer &pContainer, FbxModule pLibHandle)
{
	if (s_pPlugin == nullptr) {
		// Create the plug-in definition which contains the information about the plug-in
		FbxPluginDef pluginDef;
		pluginDef.mName = FbxString(DataFbxWriter::PLUGIN_NAME);
		pluginDef.mVersion = FbxString(DataFbxWriter::PLUGIN_VERSION);

		// Create an instance of the plug-in.  The DLL has the ownership of the plug-in
		s_pPlugin = DataFbxWriter::Create(pluginDef, pLibHandle);

		// Register the plug-in
		pContainer.Register(*s_pPlugin);
	}
}
}

/*static*/ const char *DataFbxWriter::PLUGIN_NAME = "gt-mesh";
/*static*/ const char *DataFbxWriter::PLUGIN_VERSION = "0.1.0";

FBXSDK_PLUGIN_IMPLEMENT(DataFbxWriter);

bool DataFbxWriter::SpecificInitialize()
{
	int FirstPluginID, RegistredCount;
	//GetData().mSDKManager->GetIOPluginRegistry()->RegisterReader(CreateMyOwnReader, GetMyOwnReaderInfo, FirstPluginID, RegistredCount, FillOwnReaderIOSettings);
	GetData().mSDKManager->GetIOPluginRegistry()->RegisterWriter(MeshWriter::CreateGameTechMeshWriter, MeshWriter::GameTechMeshFormatInfo, FirstPluginID, RegistredCount, MeshWriter::FillIOSettings);
	return true;
}

bool DataFbxWriter::SpecificTerminate()
{
	return true;
}
