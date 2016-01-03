#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	// =======================================================================
	// =======================================================================


	Engine* Engine::s_instance = nullptr;

Engine::Engine()
	: m_textureManager(nullptr)
	, m_sceneObjects(nullptr)
	, m_initialized(false)
{
}


Engine::~Engine()
{
	InternalRelease();
}

void Engine::InternalInitialize()
{
	if (!m_initialized)
	{
		m_textureManager = new TextureManager();
		m_textureManager->Initialize();

		m_sceneObjects = new SceneObjectList();

		m_initialized = true;
	}
}

void Engine::InternalRelease()
{
	if (m_initialized)
	{
		SAFE_DELETE(m_camera);

		for (auto o : *m_sceneObjects)
		{
			delete o;
		}

		SAFE_DELETE(m_sceneObjects);

		m_textureManager->Release();
		SAFE_DELETE(m_textureManager);

		m_initialized = false;
	}
}

void Engine::Initialize()
{
	GetInstance()->InternalInitialize();
}

void Engine::Release()
{
	GetInstance()->InternalRelease();
	SAFE_DELETE(s_instance);
}

Engine* Engine::GetInstance()
{
	if (s_instance == nullptr)
	{
		s_instance = new Engine();
	}

	assert(s_instance != nullptr);
	return s_instance;
}

void Engine::AddSceneObject(SceneObject* obj)
{
	assert(m_initialized);

	m_sceneObjects->push_back(obj);
}

void Engine::UpdateObjects()
{
	for (SceneObjectList::const_iterator it = m_sceneObjects->begin(); it != m_sceneObjects->end(); ++it)
	{
		SceneObject* obj = *it;
		Frame* frame = obj->GetFrame();
		if (frame != nullptr)
		{
			frame->BuildMatrix();
		}
	}
	assert(m_camera != nullptr);
	assert(m_camera->GetFrame() != nullptr);
	m_camera->GetFrame()->BuildMatrix();
}

void Engine::RenderObjects()
{
	glm::mat4 V = m_camera->GetViewMatrix();
	glm::mat4 P = m_camera->GetProjectionMatrix();
	glm::mat4 VP = P*V;

	for (SceneObjectList::const_iterator it = m_sceneObjects->begin(); it != m_sceneObjects->end(); ++it)
	{
		SceneObject* obj = *it;
		obj->Render(VP);
	}
}


	// =======================================================================
	// =======================================================================
} // namespace CoreFx
