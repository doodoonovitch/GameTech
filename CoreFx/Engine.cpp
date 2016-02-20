#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	// =======================================================================
	// =======================================================================


	Engine* Engine::sInstance = nullptr;

Engine::Engine()
	: mTextureManager(nullptr)
	, mSceneObjects(nullptr)
	, mRenderers(nullptr)
	, mCamera(nullptr)
	, mInitialized(false)
{
	memset(mBufferIds, 0, sizeof(mBufferIds));
}


Engine::~Engine()
{
	InternalRelease();
}

void Engine::InternalInitialize()
{
	if (!mInitialized)
	{
		mTextureManager = new TextureManager();
		mTextureManager->Initialize();

		mSceneObjects = new SceneObjectList();

		mRenderers = new RendererContainer(64, 16);

		glGenBuffers(__BufferId_Count__, mBufferIds);
		glBindBuffer(GL_UNIFORM_BUFFER, mBufferIds[FrameData_BufferId]);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(FrameData), nullptr, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;
		glBindBufferBase(GL_UNIFORM_BUFFER, FrameDataBuffer_BindingIndex, mBufferIds[FrameData_BufferId]);
		GL_CHECK_ERRORS;

		mInitialized = true;
	}
}

void Engine::InternalRelease()
{
	if (mInitialized)
	{
		glDeleteBuffers(__BufferId_Count__, mBufferIds);

		SAFE_DELETE(mCamera);

		SAFE_DELETE(mRenderers);

		for (auto o : *mSceneObjects)
		{
			delete o;
		}

		SAFE_DELETE(mSceneObjects);

		mTextureManager->Release();
		SAFE_DELETE(mTextureManager);

		mInitialized = false;
	}
}

void Engine::Initialize()
{
	GetInstance()->InternalInitialize();
}

void Engine::Release()
{
	GetInstance()->InternalRelease();
	SAFE_DELETE(sInstance);
}

Engine* Engine::GetInstance()
{
	if (sInstance == nullptr)
	{
		sInstance = new Engine();
	}

	assert(sInstance != nullptr);
	return sInstance;
}

void Engine::AddUnrenderedSceneObject(SceneObject* obj)
{
	assert(mInitialized);

	mSceneObjects->push_back(obj);
}

void Engine::UpdateObjects()
{
	for (SceneObjectList::const_iterator it = mSceneObjects->begin(); it != mSceneObjects->end(); ++it)
	{
		SceneObject* obj = *it;
		Frame* frame = obj->GetFrame();
		if (frame != nullptr)
		{
			frame->BuildMatrix();
		}
	}

	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->BeginFrame();
	});

	assert(mCamera != nullptr);
	assert(mCamera->GetFrame() != nullptr);
	mCamera->GetFrame()->BuildMatrix();
}

void Engine::RenderObjects()
{
	//mFrameData.mProj = mCamera->GetProjectionMatrix();
	//mFrameData.mView = mCamera->GetViewMatrix();

	glBindBuffer(GL_UNIFORM_BUFFER, mBufferIds[FrameData_BufferId]);
	FrameData* buffer = (FrameData*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(FrameData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
	memcpy(glm::value_ptr(buffer->mView), glm::value_ptr(mCamera->GetViewMatrix()), sizeof(glm::mat4));
	memcpy(glm::value_ptr(buffer->mProj), glm::value_ptr(mCamera->GetProjectionMatrix()), sizeof(glm::mat4));
	//buffer->mView = mCamera->GetViewMatrix();
	//buffer->mProj = mCamera->GetProjectionMatrix();

	glUnmapBuffer(GL_UNIFORM_BUFFER);


	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->Render();
	});

	//for (SceneObjectList::const_iterator it = m_sceneObjects->begin(); it != m_sceneObjects->end(); ++it)
	//{
	//	SceneObject* obj = *it;
	//	obj->Render(VP);
	//}

	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->EndFrame();
	});

}



	// =======================================================================
	// =======================================================================
} // namespace CoreFx
