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
	, mDeferredLightPass(nullptr)
	, mGBuffer(0)
	, mDepthBuffer(0)
	, mGBufferWidth(1920)
	, mGBufferHeight(1080)
	, mViewportX(0)
	, mViewportY(0)
	, mViewportWidth(1920)
	, mViewportHeight(1080)
	, mAmbientLight(0.1f, 0.1f, 0.1f, 0.f)
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
	memset(mGBufferTex, 0, sizeof(mGBufferTex));
}


Engine::~Engine()
{
	InternalRelease();
}

void Engine::InternalInitialize(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight)
{
	if (!mInitialized)
	{
		mViewportX = viewportX;
		mViewportY = viewportY;
		mViewportWidth = viewportWidth;
		mViewportHeight = viewportHeight;

#ifdef FORWARD_RENDERING
		gBufferWidth;
		gBufferHeight;
#else
		CreateGBuffers(gBufferWidth, gBufferHeight);
		InitializeDeferredPassQuadShader();
#endif // FORWARD_RENDERING

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		//glFrontFace(GL_CCW);

		glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LESS);

		glViewport(mViewportX, mViewportY, mViewportWidth, mViewportHeight);

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
		memset(mBufferIds, 0, sizeof(mBufferIds));

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

#ifdef FORWARD_RENDERING
#else
		glDeleteTextures(__gBuffer_count__, mGBufferTex);
		memset(mGBufferTex, 0, sizeof(mGBufferTex));
		glDeleteFramebuffers(1, &mGBuffer);
		mGBuffer = 0;
		glDeleteRenderbuffers(1, &mDepthBuffer);
		mDepthBuffer = 0;

		SAFE_DELETE(mDeferredLightPass);
#endif // FORWARD_RENDERING

		mInitialized = false;
		mIsDrawVertexNormalEnabled = false;
	}
}

void Engine::CreateGBuffers(GLsizei gBufferWidth, GLsizei gBufferHeight)
{
	mGBufferWidth = gBufferWidth;
	mGBufferHeight = gBufferHeight;

	std::cout << std::endl;
	std::cout << "Initialize Deferred framebuffers..." << std::endl;
	std::cout << "\t Size = " << mGBufferWidth << " x " << mGBufferHeight << std::endl;

	glGenFramebuffers(1, &mGBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer);
	glGenTextures(__gBuffer_count__, mGBufferTex);
	GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D, mGBufferTex[gBuffer_PositionBuffer]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mGBufferWidth, mGBufferHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mGBufferTex[gBuffer_PositionBuffer], 0);
	GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D, mGBufferTex[gBuffer_DataBuffer]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, mGBufferWidth, mGBufferHeight, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, mGBufferTex[gBuffer_DataBuffer], 0);
	GL_CHECK_ERRORS;

	glGenRenderbuffers(1, &mDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mGBufferWidth, mGBufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);
	GL_CHECK_ERRORS;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::InitializeDeferredPassQuadShader()
{
	std::cout << std::endl;
	std::cout << "Initialize Deferred light pass shader..." << std::endl;

	mDeferredLightPass = new Renderables::RenderableObject<1>();

	//setup shader
	Shader & shader = mDeferredLightPass->GetShader();
	shader.LoadFromFile(GL_VERTEX_SHADER, "shaders/light.vs.glsl");
	shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/light.fs.glsl");

	shader.CreateAndLinkProgram();

	shader.Use();
	shader.AddAttribute("in_Position");
	shader.AddAttribute("in_TexUV");

	shader.AddUniform("u_gBufferPosition");
	shader.AddUniform("u_gBufferRGBA32UI");
	shader.AddUniform("u_materialDataSampler");

	shader.AddUniform("u_lightDescSampler");
	shader.AddUniform("u_lightDataSampler");

	shader.AddUniform("u_textureSampler[0]");

	//pass values of constant uniforms at initialization
	glUniform1i(shader.GetUniform("u_gBufferPosition"), 0);
	glUniform1i(shader.GetUniform("u_gBufferRGBA32UI"), 1);

	glUniform1i(shader.GetUniform("u_materialDataSampler"), 2);
	glUniform1i(shader.GetUniform("u_lightDescSampler"), 3);
	glUniform1i(shader.GetUniform("u_lightDataSampler"), 4);

	int uniformIndex = shader.GetUniform("u_textureSampler[0]");
	for (int i = 0; i < MAX_TEXTURE_SAMPLER; ++i)
	{
		glUniform1i(uniformIndex + i, i + FIRST_TEXTURE_SAMPLER_INDEX);
		GL_CHECK_ERRORS;
	}

	shader.SetupFrameDataBlockBinding();
	shader.UnUse();

	GL_CHECK_ERRORS;

	GLfloat quadVertices[] = 
	{
		// Positions        // Texture Coords
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	mDeferredLightPass->CreateBuffers();

	// Setup plane VAO
	glBindVertexArray(mDeferredLightPass->GetVao());

		glBindBuffer(GL_ARRAY_BUFFER, mDeferredLightPass->GetVbo(0));
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::UV_ATTRIBUTE);
		glVertexAttribPointer(Shader::UV_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;
}

void Engine::CreateMaterialBuffer()
{
	int bufferSize = 0;
	mRenderers->ForEach([&bufferSize](Renderer * renderer)
	{
		bufferSize += renderer->mMaterials.GetDataSize();
	});

	mMaterialBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, bufferSize, nullptr);

	glBindBuffer(GL_TEXTURE_BUFFER, mMaterialBuffer.GetBufferId());
	GLsizeiptr offset = 0;
	GLint baseIndex = 0;
	mRenderers->ForEach([&offset, &baseIndex](Renderer * renderer)
	{
		renderer->mMaterialBaseIndex = baseIndex;
		GLsizei dataSize = renderer->mMaterials.GetDataSize();
		std::uint8_t * buffer = (std::uint8_t *)glMapBufferRange(GL_TEXTURE_BUFFER, offset, dataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		GL_CHECK_ERRORS;
		memcpy(buffer, renderer->mMaterials.GetData(), dataSize);
		glUnmapBuffer(GL_TEXTURE_BUFFER);
		offset += dataSize;
		baseIndex += renderer->mMaterials.GetPropertyCount();
	});
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

void Engine::Initialize(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight)
{
	GetInstance()->InternalInitialize(viewportX, viewportY, viewportWidth, viewportHeight, gBufferWidth, gBufferHeight);
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


#ifdef FORWARD_RENDERING
#else

	if (!mMaterialBuffer.IsCreated())
	{
		CreateMaterialBuffer();
		CreateTextures();
	}

	static const GLuint uintZeros[] = { 0, 0, 0, 0 };
	static const GLfloat floatZeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const GLfloat floatOnes[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	{ // preparation for gbuffer rendering pass
		glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer);
		glDrawBuffers(2, drawBuffers);
	}

	glDisable(GL_BLEND);
#endif // FORWARD_RENDERING

	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->Render();
	});

#ifdef FORWARD_RENDERING
#else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		mDeferredLightPass->GetShader().Use();
			glBindVertexArray(mDeferredLightPass->GetVao());

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mGBufferTex[gBuffer_PositionBuffer]);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, mGBufferTex[gBuffer_DataBuffer]);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_BUFFER, mMaterialBuffer.GetTextureId());

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_BUFFER, GetLightDescBuffer().GetTextureId());

				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_BUFFER, GetLightDataBuffer().GetTextureId());

				for (int i = 0; i < (int)mTextureGroupList.size(); ++i)
				{
					glActiveTexture(GL_TEXTURE0 + FIRST_TEXTURE_SAMPLER_INDEX + i);
					glBindTexture(GL_TEXTURE_BUFFER, mTextureGroupList[i]->GetId());
				}

				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
		mDeferredLightPass->GetShader().UnUse();

		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
#endif // FORWARD_RENDERING

	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->EndFrame();
	});
}

Lights::PointLight * Engine::CreatePointLight(const glm::vec3 & position, glm::vec3 const & ambient, glm::vec3 const & diffuse, glm::vec3 const & specular, GLfloat constantAttenuation, GLfloat linearAttenuation, GLfloat quadraticAttenuation)
{
	if (mLights->GetCount() < MAX_LIGHT_COUNT)
	{
		Lights::PointLight *light = new Lights::PointLight(position, ambient, diffuse, specular, constantAttenuation, linearAttenuation, quadraticAttenuation);
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

Lights::DirectionalLight * Engine::CreateDirectionalLight(const glm::vec3 & direction, glm::vec3 const & ambient, glm::vec3 const & diffuse, glm::vec3 const & specular)
{
	if (mLights->GetCount() < MAX_LIGHT_COUNT)
	{
		Lights::DirectionalLight *light = new Lights::DirectionalLight(direction, ambient, diffuse, specular);
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

bool Engine::AttachRenderer(Renderer* renderer)
{
	return mRenderers->Attach(renderer);
}

bool Engine::DetachRenderer(Renderer* renderer)
{
	return mRenderers->Detach(renderer);
}

GLint Engine::AddMaterialsForDeferredRendering(const GLfloat * matProps, GLsizei matCount, GLsizei propPerMatCount)
{
		assert(matCount > 0);
		assert(propPerMatCount > 0);

		GLint index = (GLint)mMaterials.size();
		GLsizei propCount = matCount * propPerMatCount;
		GLsizei floatCount = propCount * 4;
		mMaterials.reserve(index + floatCount);
		memcpy(mMaterials.data() + index, matProps, floatCount * sizeof(GLfloat));
		return index / sizeof(GLfloat);
}


class TextureGroupLayerCount
{
public:

	std::map<std::string, GLint> mLayerIndexByName;
	GLint mLayerCount = 0;
	GLint mSamplerIndex;
};

typedef std::map<TextureGroupId, TextureGroupLayerCount> TextureGroupLayerCountMap;

void Engine::CreateTextures()
{
	int samplerIndex = 0;
	TextureGroupLayerCountMap layerCountByMap;
	mRenderers->ForEach([&samplerIndex, &layerCountByMap](Renderer * renderer)
	{
		TextureInfoList & list = renderer->mTextures;
		for (TextureInfoList::iterator texInfoIt = list.begin(); texInfoIt != list.end(); ++texInfoIt)
		{
			TextureGroupLayerCountMap::iterator mapIt = layerCountByMap.find(texInfoIt->GetGroupId());
			if (mapIt == layerCountByMap.end())
			{
				std::pair<TextureGroupLayerCountMap::iterator, bool> res = layerCountByMap.insert(TextureGroupLayerCountMap::value_type(texInfoIt->GetGroupId(), TextureGroupLayerCount()));
				mapIt = res.first;
				mapIt->second.mSamplerIndex = samplerIndex;
				++samplerIndex;
			}

			TextureGroupLayerCount & item = mapIt->second;
			std::map<std::string, GLint>::iterator it = item.mLayerIndexByName.find(texInfoIt->GetFilename());
			if (it == item.mLayerIndexByName.end())
			{
				item.mLayerIndexByName[texInfoIt->GetFilename()] = item.mLayerCount;
				texInfoIt->mLayerIndex = item.mLayerCount;
				++item.mLayerCount;
			}
			else
			{
				texInfoIt->mLayerIndex = it->second;
			}
			texInfoIt->mSamplerIndex = mapIt->second.mSamplerIndex;
		}
	});

	for (TextureGroupLayerCountMap::const_iterator it = layerCountByMap.begin(); it != layerCountByMap.end(); ++it)
	{
		const TextureGroupLayerCount & item = it->second;
		std::vector<std::string> textureList(item.mLayerCount);
		for (std::map<std::string, GLint>::const_iterator it2 = item.mLayerIndexByName.begin(); it2 != item.mLayerIndexByName.end(); ++it2)
		{
			textureList[it2->second] = it2->first;
		}

		const TextureGroup* texGroup = mTextureManager->LoadTextureGroup(it->first, textureList);
		mTextureGroupList.push_back(texGroup);
	}

	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->UpdateMaterialTextureIndex();
	});
}

	// =======================================================================
	// =======================================================================
} // namespace CoreFx
