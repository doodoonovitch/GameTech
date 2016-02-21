#ifndef COREFX_ENGINE_H
#define COREFX_ENGINE_H

#pragma once

namespace CoreFx
{
	// =======================================================================
	// =======================================================================

	class TextureManager;
	class SceneObject;
	class Camera;

class Engine
{
public:

	enum BindingIndex
	{
		FrameDataBuffer_BindingIndex = 0
	};

public:

	static void Initialize();
	static void Release();

	static Engine* GetInstance();

	inline TextureManager * GetTextureManager() const 
	{
		assert(mInitialized);
		return mTextureManager;
	}

	void AddUnrenderedSceneObject(SceneObject* obj);

	void UpdateObjects();
	void RenderObjects();

	Camera* GetCamera() const
	{
		return mCamera;
	}

	void SetCamera(Camera * camera)
	{
		mCamera = camera;
	}

	bool AttachRenderer(Renderer* renderer)
	{
		return mRenderers->Attach(renderer);
	}

	bool DetachRenderer(Renderer* renderer)
	{
		return mRenderers->Detach(renderer);
	}

private:

	void InternalInitialize();
	void InternalRelease();

	Engine();
	~Engine();

	Engine(Engine const &) = delete;
	Engine & operator=(Engine const &) = delete;

private:

	typedef InstanceContainer<Renderer> RendererContainer;
	typedef std::vector<SceneObject*> SceneObjectList;

	TextureManager * mTextureManager;
	SceneObjectList* mSceneObjects;
	RendererContainer* mRenderers;

	Camera* mCamera;

	enum BufferIdIndex
	{
		FrameData_BufferId,

		__BufferId_Count__
	};

	struct FrameData
	{
		glm::mat4 mView;
		glm::mat4 mProj;
	};

	GLuint mBufferIds[__BufferId_Count__];

	FrameData mFrameData;

	bool mInitialized;

	static Engine* sInstance;
};


// =======================================================================
// =======================================================================
} // namespace CoreFx
#endif // COREFX_ENGINE_H