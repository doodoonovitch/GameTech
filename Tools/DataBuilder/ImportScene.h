#ifndef DATABUILDER_IMPORTSCENE_H
#define DATABUILDER_IMPORTSCENE_H

#pragma once

namespace DataBuilder
{
// =======================================================================
// =======================================================================



class ImportScene
{
public:
	ImportScene(FbxManager* pManager, FbxScene* pScene);
	~ImportScene();

	inline FbxManager* GetManger() const { return mManager; }

private:

	void Initialize();
	void Release();

private:

	FbxManager* mManager;
	FbxScene* mScene;
};



// =======================================================================
// =======================================================================
} // namespace DataBuilder
#endif // DATABUILDER_IMPORTSCENE_H