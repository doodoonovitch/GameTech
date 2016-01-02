#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	// =======================================================================
	// =======================================================================


	Engine* Engine::s_instance = nullptr;

Engine::Engine()
	: m_textureManager(nullptr)
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

		m_initialized = true;
	}
}

void Engine::InternalRelease()
{
	if (m_initialized)
	{
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


	// =======================================================================
	// =======================================================================
} // namespace CoreFx
