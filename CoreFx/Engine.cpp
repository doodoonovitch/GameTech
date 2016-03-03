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
	, mDrawVertexNormalColor(0.41f, 0.f, 1.f, 0.f)
	, mDrawPointLightColor(1.f, 1.f, 0.f, 0.f)
	, mDrawDirectionalLightColor(1.f, 0.f, 0.f, 0.f)
	, mDrawVertexNormalMagnitude(0.2f)
	, mDrawLightMagnitude(0.6f)
	, mFirstLightIndexToDraw(0)
	, mLightToDrawCount(16)
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

		mDrawVertexNormalShader.LoadShaders();

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
		mLightDescBuffer.ReleaseResource();
		mLightDataBuffer.ReleaseResource();

		mTextureManager->Release();
		SAFE_DELETE(mTextureManager);

		mInitialized = false;
		mIsDrawVertexNormalEnabled = false;
	}
}

void Engine::InternalCreateFrameDataBuffer()
{
	GLuint program = mDrawVertexNormalShader.GetProgram();

	printf("Getting FrameData uniform block information...\n");
	glGetUniformIndices(program, __uniforms_count__, mFrameDataUniformNames, mFrameDataUniformIndices);
	GL_CHECK_ERRORS;
	glGetActiveUniformsiv(program, __uniforms_count__, mFrameDataUniformIndices, GL_UNIFORM_OFFSET, mFrameDataUniformOffsets);
	GL_CHECK_ERRORS;
	glGetActiveUniformsiv(program, __uniforms_count__, mFrameDataUniformIndices, GL_UNIFORM_SIZE, mFrameDataUniformSizes);
	GL_CHECK_ERRORS;

	int lastItem = __uniforms_count__ - 1;
	mFrameDataSize = mFrameDataUniformOffsets[lastItem] + mFrameDataUniformSizes[lastItem];

	printf("\n");
	printf("FrameData uniform block : \n");
	printf(" \t size : %i\n", mFrameDataSize);
	for (int i = 0; i < __uniforms_count__; ++i)
	{
		printf(" \t - %s : offset = %i, size = %i\n", mFrameDataUniformNames[i], mFrameDataUniformOffsets[i], mFrameDataUniformSizes[i]);
	}
	printf("\n");

	glGenBuffers(__BufferId_Count__, mBufferIds);
	glBindBuffer(GL_UNIFORM_BUFFER, mBufferIds[FrameData_BufferId]);
	glBufferData(GL_UNIFORM_BUFFER, mFrameDataSize, nullptr, GL_STATIC_DRAW);
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
	mCamera->Update();

	mLights->ForEach([this](Lights::Light * light)
	{
		light->TransformInViewCoords(mCamera->GetViewMatrix());
	});
}

void Engine::RenderObjects()
{
	if (!mLightDescBuffer.IsCreated())
	{
		GLsizeiptr bufferSize = (mLights->GetCount() + 1) * sizeof(GLuint);
		mLightDescBuffer.CreateResource(GL_STATIC_DRAW, GL_R32UI, bufferSize, nullptr);

		glBindBuffer(GL_TEXTURE_BUFFER, mLightDescBuffer.GetBufferId());
		GLuint * lightDescBuffer = (GLuint *)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
		GL_CHECK_ERRORS;
		lightDescBuffer[0] = (GLuint)mLights->GetCount();

		mLights->ForEach([&lightDescBuffer](Lights::Light * light)
		{
			int index = light->GetInstanceId() + 1;
			lightDescBuffer[index] = light->mLightDesc;
		});

		glUnmapBuffer(GL_TEXTURE_BUFFER);
	}

	if (!mLightDataBuffer.IsCreated())
	{
		GLsizeiptr bufferSize = 0;
		mLights->ForEach([&bufferSize](Lights::Light * light)
		{
			bufferSize += light->GetDataSize();
		});

		mLightDataBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, bufferSize, nullptr);
	}

	glBindBuffer(GL_TEXTURE_BUFFER, mLightDataBuffer.GetBufferId());
	GLsizeiptr offset = 0;
	mLights->ForEach([&offset](Lights::Light * light)
	{
		GLsizei dataSize = light->GetDataSize();
		std::uint8_t * lightDataBuffer = (std::uint8_t *)glMapBufferRange(GL_TEXTURE_BUFFER, offset, dataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		GL_CHECK_ERRORS;
		memcpy(lightDataBuffer, light->GetData(), dataSize);
		offset += dataSize;
		glUnmapBuffer(GL_TEXTURE_BUFFER);
	});


	glBindBuffer(GL_UNIFORM_BUFFER, mBufferIds[FrameData_BufferId]);
	std::uint8_t * buffer = (std::uint8_t *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, mFrameDataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);

	GL_CHECK_ERRORS;
	assert(buffer != nullptr);

	memcpy(buffer + mFrameDataUniformOffsets[u_ProjMatrix], glm::value_ptr(mCamera->GetProjectionMatrix()), sizeof(glm::mat4));
	memcpy(buffer + mFrameDataUniformOffsets[u_ViewDQ], &mCamera->GetViewDQ(), sizeof(Maths::DualQuat));
	memcpy(buffer + mFrameDataUniformOffsets[u_AmbientLight], glm::value_ptr(mAmbientLight), sizeof(glm::vec4));
	*((GLint*)(buffer + mFrameDataUniformOffsets[u_LightCount])) = (GLint)mLights->GetCount();

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

Lights::PointLight * Engine::CreatePointLight(const glm::vec4 & color, const glm::vec3 & position, GLfloat constantAttenuation, GLfloat linearAttenuation, GLfloat quadraticAttenuation)
{
	if (mLights->GetCount() < MAX_LIGHT_COUNT)
	{
		Lights::PointLight *light = new Lights::PointLight(color, position, constantAttenuation, linearAttenuation, quadraticAttenuation);
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
