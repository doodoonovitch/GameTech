#ifndef DATAFBXWRITER_DATAFBXWRITER_H
#define DATAFBXWRITER_DATAFBXWRITER_H

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DATAFBXWRITER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DATAFBXWRITER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DATAFBXWRITER_EXPORTS
#define DATAFBXWRITER_API __declspec(dllexport)
#else
#define DATAFBXWRITER_API __declspec(dllimport)
#endif


extern "C"
{
	DATAFBXWRITER_API void FBXPluginRegistration(FbxPluginContainer &pContainer, FbxModule pLibHandle);
}

// This class is exported from the DataFbxWriter.dll
class DataFbxWriter : public FbxPlugin 
{
	FBXSDK_PLUGIN_DECLARE(DataFbxWriter);

public:

	static const char *PLUGIN_NAME;
	static const char *PLUGIN_VERSION;

protected:

	explicit DataFbxWriter(const FbxPluginDef& pDefinition, FbxModule pLibHandle) : FbxPlugin(pDefinition, pLibHandle)
	{
	}

	virtual bool SpecificInitialize();
	virtual bool SpecificTerminate();
};



#endif // DATAFBXWRITER_DATAFBXWRITER_H