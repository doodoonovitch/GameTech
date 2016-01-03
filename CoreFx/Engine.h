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

	static void Initialize();
	static void Release();

	static Engine* GetInstance();

	inline TextureManager * GetTextureManager() const 
	{
		assert(m_initialized);
		return m_textureManager;
	}

	void AddSceneObject(SceneObject* obj);

	void UpdateObjects();
	void RenderObjects();

	Camera* GetCamera() const
	{
		return m_camera;
	}

	void SetCamera(Camera * camera)
	{
		m_camera = camera;
	}

private:

	void InternalInitialize();
	void InternalRelease();

	Engine();
	~Engine();

	Engine(Engine const &) = delete;
	Engine & operator=(Engine const &) = delete;

private:

	typedef std::vector<SceneObject*> SceneObjectList;

	TextureManager * m_textureManager;
	SceneObjectList* m_sceneObjects;

	Camera* m_camera;

	bool m_initialized;

	static Engine* s_instance;
};


// =======================================================================
// =======================================================================
} // namespace CoreFx
#endif // COREFX_ENGINE_H