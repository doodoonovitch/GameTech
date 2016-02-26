#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	// =======================================================================
	// =======================================================================


	Engine* Engine::sInstance = nullptr;

Engine::Engine()
	: mTextureManager(nullptr)
	, mRenderers(nullptr)
	, mLights(nullptr)
	, mCamera(nullptr)
	, mAmbientLight(0.2f, 0.2f, 0.2f, 0.f)
	, mLightDataIndex(0)
	, mDrawVertexNormalColor(1.f, 0.41f, 0.f, 0.f)
	, mDrawPointLightColor(1.f, 1.f, 0.f, 0.f)
	, mDrawDirectionalLightColor(1.f, 0.f, 0.f, 0.f)
	, mDrawVertexNormalMagnitude(0.2f)
	, mDrawLightMagnitude(0.6f)
	, mInitialized(false)
	, mIsDrawVertexNormalEnabled(false)
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

		mRenderers = new RendererContainer(64, 16);

		mLights = new LightContainer(MAX_LIGHT_COUNT, 1);

		InternalCreateFrameDataBuffer();

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

		mLights->ForEach([](Lights::Light* obj)
		{
			delete obj;
		});
		SAFE_DELETE(mLights);
		mLightDataIndex = 0;

		mTextureManager->Release();
		SAFE_DELETE(mTextureManager);

		mInitialized = false;
		mIsDrawVertexNormalEnabled = false;
	}
}

void Engine::InternalCreateFrameDataBuffer()
{
	glGenBuffers(__BufferId_Count__, mBufferIds);
	glBindBuffer(GL_UNIFORM_BUFFER, mBufferIds[FrameData_BufferId]);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(FrameData), nullptr, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;
	glBindBufferBase(GL_UNIFORM_BUFFER, FrameDataBuffer_BindingIndex, mBufferIds[FrameData_BufferId]);
	GL_CHECK_ERRORS;
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

void Engine::UpdateObjects()
{
	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->BeginFrame();
	});

	assert(mCamera != nullptr);
	assert(mCamera->GetFrame() != nullptr);
	mCamera->GetFrame()->BuildMatrix();

	mLights->ForEach([this](Lights::Light * light)
	{
		light->TransformInViewCoords(mCamera->GetViewMatrix());
	});
}

void Engine::RenderObjects()
{
	glBindBuffer(GL_UNIFORM_BUFFER, mBufferIds[FrameData_BufferId]);
	FrameData* buffer = (FrameData*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(FrameData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);

	GL_CHECK_ERRORS;
	assert(buffer != nullptr);

	memcpy(glm::value_ptr(buffer->mView), glm::value_ptr(mCamera->GetViewMatrix()), sizeof(glm::mat4));
	memcpy(glm::value_ptr(buffer->mProj), glm::value_ptr(mCamera->GetProjectionMatrix()), sizeof(glm::mat4));
	memcpy(glm::value_ptr(buffer->vAmbientLight), glm::value_ptr(mAmbientLight), sizeof(glm::vec4));
	buffer->uLightCount = (GLuint)mLights->GetCount();

	{
		buffer->uLightCount = (GLuint)mLights->GetCount();

		std::uint8_t * lightDataBuffer = (std::uint8_t *) &buffer->vLightData[0];
		glm::ivec4 * lightDescBuffer = &buffer->uLightDesc[0];

		mLights->ForEach([&lightDataBuffer, &lightDescBuffer](Lights::Light * light)
		{
			int compIndex = light->GetInstanceId() & 3;
			int arrayIndex = light->GetInstanceId() >> 2;
			lightDescBuffer[arrayIndex][compIndex] = light->mLightDesc;

			GLsizei dataSize = light->GetDataSize();
			memcpy(lightDataBuffer, light->GetData(), dataSize);
			lightDataBuffer += dataSize;
		});
	}

	glUnmapBuffer(GL_UNIFORM_BUFFER);


	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->Render();
	});

	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->EndFrame();
	});

}

Lights::PointLight * Engine::CreatePointLight(const glm::vec4 & color, const glm::vec3 & position)
{
	if (mLights->GetCount() < MAX_LIGHT_COUNT)
	{
		Lights::PointLight *light = new Lights::PointLight(color, position);
		light->SetDataIndex(mLightDataIndex);
		mLightDataIndex += light->GetPropertyCount();
		mLights->Attach(light);
		return light;
	}
	else
	{
		std::cerr << "Cannot create a Point Light : max lights reached!" << std::endl;
		return nullptr;
	}
}

Lights::DirectionalLight * Engine::CreateDirectionalLight(const glm::vec4 & color, const glm::vec3 & direction)
{
	if (mLights->GetCount() < MAX_LIGHT_COUNT)
	{
		Lights::DirectionalLight *light = new Lights::DirectionalLight(color, direction);
		light->SetDataIndex(mLightDataIndex);
		mLightDataIndex += light->GetPropertyCount();
		mLights->Attach(light);
		return light;
	}
	else
	{
		std::cerr << "Cannot create a Directional Light : max lights reached!" << std::endl;
		return nullptr;
	}
}

void Engine::DeleteLight(Lights::Light * & light)
{
	if (light == nullptr)
		return;
	mLights->Detach(light);
	SAFE_DELETE(light);
}




	// =======================================================================
	// =======================================================================
} // namespace CoreFx
