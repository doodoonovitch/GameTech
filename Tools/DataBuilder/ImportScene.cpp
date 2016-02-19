#include "stdafx.h"
#include "ImportScene.h"

#define DESTROY(p) \
	if (p != nullptr) \
	{ \
		p->Destroy(); \
		p = nullptr; \
	} 


namespace DataBuilder
{
// =======================================================================
// =======================================================================


ImportScene::ImportScene(FbxManager* pManager, FbxScene* pScene)
	: mManager(pManager)
	, mScene(pScene)
{
	Initialize();
}


ImportScene::~ImportScene()
{
	Release();
}


void ImportScene::Initialize()
{
}

void ImportScene::Release()
{
}

// =======================================================================
// =======================================================================
} // namespace DataBuilder
