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
	for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
	{
		mLights[i] = nullptr;
	}
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
		mGBufferWidth = gBufferWidth;
		mGBufferHeight = gBufferHeight;

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		//glFrontFace(GL_CCW);

		glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LESS);

		glViewport(mViewportX, mViewportY, mViewportWidth, mViewportHeight);

		mTextureManager = new TextureManager();
		mTextureManager->Initialize();

		mRenderers = new RendererContainer(64, 16);

		for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
		{
			mLights[i] = new LightContainer(1, 1);
		}
		
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

		for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
		{
			mLights[i]->ForEach([](Lights::Light* obj)
			{
				delete obj;
			});
			SAFE_DELETE(mLights[i]);
		}
		mLightDescBuffer.ReleaseResource();
		mLightDataBuffer.ReleaseResource();

		mTextureManager->Release();
		SAFE_DELETE(mTextureManager);

		glDeleteRenderbuffers(1, &mDepthBuffer);
		mDepthBuffer = 0;
		glDeleteTextures(__gBuffer_count__, mGBufferTex);
		memset(mGBufferTex, 0, sizeof(mGBufferTex));
		glDeleteFramebuffers(1, &mGBuffer);
		mGBuffer = 0;

		SAFE_DELETE(mDeferredLightPass);

		mInitialized = false;
		mIsDrawVertexNormalEnabled = false;
	}
}

void Engine::CreateDynamicResources()
{
	// Light desc buffer
	// -----------------------------------------------------------------------
	assert(!mLightDescBuffer.IsCreated());
	{		
		GLuint lightCount = 0;
		for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
		{
			lightCount += (GLuint)mLights[i]->GetCount();
		}

		GLsizeiptr bufferSize = (lightCount + 1) * sizeof(GLuint);

		mLightDescBuffer.CreateResource(GL_STATIC_DRAW, GL_R32UI, bufferSize, nullptr);

		glBindBuffer(GL_TEXTURE_BUFFER, mLightDescBuffer.GetBufferId()); GL_CHECK_ERRORS;
		GLuint * lightDescBuffer = (GLuint *)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);	GL_CHECK_ERRORS;
		lightDescBuffer[0] = (GLuint)lightCount;

		int index = 1;
		GLuint lightDataIndex = 0;
		for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
		{
			mLights[i]->ForEach([&index, &lightDataIndex, &lightDescBuffer](Lights::Light * light)
			{
				light->SetDataIndex(lightDataIndex);
				lightDataIndex += light->GetPropertyCount();

				lightDescBuffer[index++] = light->mLightDesc;
			});
		}

		glUnmapBuffer(GL_TEXTURE_BUFFER); GL_CHECK_ERRORS;
	}
	// -----------------------------------------------------------------------

	// Light data buffer
	// -----------------------------------------------------------------------
	assert(!mLightDataBuffer.IsCreated());
	{
		GLsizeiptr bufferSize = 0;
		for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
		{
			mLights[i]->ForEach([&bufferSize](Lights::Light * light)
			{
				bufferSize += light->GetDataSize();
			});
		}

		mLightDataBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, bufferSize, nullptr);
	}
	// -----------------------------------------------------------------------

	InternalCreateGBuffers();

	// material and texture creation
	// -----------------------------------------------------------------------
	InternalCreateTextures();
	InternalCreateMaterialBuffer();
	// -----------------------------------------------------------------------
	
	InitializeDeferredPassQuadShader();
}

void Engine::InternalCreateGBuffers()
{
	std::cout << std::endl;
	std::cout << "Initialize Deferred framebuffers..." << std::endl;
	std::cout << "\t Size = " << mGBufferWidth << " x " << mGBufferHeight << std::endl;

	glGenFramebuffers(1, &mGBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBuffer);
	glGenTextures(__gBuffer_count__, mGBufferTex);
	GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D, mGBufferTex[gBuffer_PositionBuffer]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mGBufferWidth, mGBufferHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mGBufferTex[gBuffer_PositionBuffer], 0);
	GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D, mGBufferTex[gBuffer_DataBuffer]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32UI, mGBufferWidth, mGBufferHeight, 0, GL_RG_INTEGER, GL_UNSIGNED_INT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, mGBufferTex[gBuffer_DataBuffer], 0);
	GL_CHECK_ERRORS;

	//glBindTexture(GL_TEXTURE_2D, mGBufferTex[gBuffer_DepthBuffer]);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mGBufferWidth, mGBufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mGBufferTex[gBuffer_DepthBuffer], 0);
	//GL_CHECK_ERRORS;
	glGenRenderbuffers(1, &mDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mGBufferWidth, mGBufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);
	GL_CHECK_ERRORS;

	static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawBuffers); GL_CHECK_ERRORS;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Engine::InitializeDeferredPassQuadShader()
{
	std::cout << std::endl;
	std::cout << "Initialize Deferred light pass shader..." << std::endl;

	mDeferredLightPass = new Renderables::RenderableObject<1>();

	//setup shader
	Shader & shader = mDeferredLightPass->GetShader();

	// vertex shader
	shader.LoadFromFile(GL_VERTEX_SHADER, "shaders/light.vs.glsl");

	// fragment shader
	std::cout << "Loading shader file : shaders/light.fs.glsl" << std::endl;
	std::vector<std::string> lightFsGlsl(2);
	Shader::MergeFile(lightFsGlsl[0], "shaders/light.fs.glsl");
	std::string & textureFuncSource = lightFsGlsl[1];
	Shader::GenerateTexGetFunction(textureFuncSource, (int)mLightPassTextureMapping.mMapping.size());
	shader.LoadFromString(GL_FRAGMENT_SHADER, lightFsGlsl);

	shader.CreateAndLinkProgram();

	shader.Use();
	shader.AddAttribute("in_Position");
	shader.AddAttribute("in_TexUV");

	shader.AddUniform("u_gBufferPosition");
	shader.AddUniform("u_gBufferData");
	shader.AddUniform("u_materialDataSampler");

	shader.AddUniform("u_lightDescSampler");
	shader.AddUniform("u_lightDataSampler");

	//pass values of constant uniforms at initialization
	glUniform1i(shader.GetUniform("u_gBufferPosition"), 0);
	glUniform1i(shader.GetUniform("u_gBufferData"), 1);

	glUniform1i(shader.GetUniform("u_materialDataSampler"), 2);
	glUniform1i(shader.GetUniform("u_lightDescSampler"), 3);
	glUniform1i(shader.GetUniform("u_lightDataSampler"), 4);

	for (int i = 0; i < (int)mLightPassTextureMapping.mMapping.size(); ++i)
	{
		char uniformName[50];
		sprintf_s(uniformName, 50, "u_textureSampler[%i]", i);
		int uniformIndex = glGetUniformLocation(shader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		if (uniformIndex > 0)
		{
			glUniform1i(uniformIndex, i + FIRST_TEXTURE_SAMPLER_INDEX);	GL_CHECK_ERRORS;
			std::cout << "\t" << uniformName << " : " << uniformIndex << std::endl;
		}
	}

	shader.SetupFrameDataBlockBinding();
	shader.UnUse();

	GL_CHECK_ERRORS;

	GLfloat quadVertices[] =
	{
		// Positions        // Texture Coords
		-1.0f, 1.0f, 0.0f, 
		-1.0f, -1.0f, 0.0f, 
		1.0f, 1.0f, 0.0f, 
		1.0f, -1.0f, 0.0f, 
	};

	mDeferredLightPass->CreateBuffers();

	// Setup plane VAO
	glBindVertexArray(mDeferredLightPass->GetVao());

		glBindBuffer(GL_ARRAY_BUFFER, mDeferredLightPass->GetVbo(0));
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;
}

void Engine::InternalCreateMaterialBuffer()
{
	int bufferSize = 0;
	mRenderers->ForEach([&bufferSize](Renderer * renderer)
	{
		bufferSize += renderer->mMaterials.GetDataSize();
	});

	mMaterialBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, bufferSize, nullptr);

	glBindBuffer(GL_TEXTURE_BUFFER, mMaterialBuffer.GetBufferId()); GL_CHECK_ERRORS;
	GLsizeiptr offset = 0;
	GLint baseIndex = 0;
	mRenderers->ForEach([&offset, &baseIndex](Renderer * renderer)
	{
		GLsizei dataSize = renderer->mMaterials.GetDataSize();
		if (dataSize > 0)
		{
			renderer->mMaterialBaseIndex = baseIndex;
			std::uint8_t * buffer = (std::uint8_t *)glMapBufferRange(GL_TEXTURE_BUFFER, offset, dataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT); GL_CHECK_ERRORS;
			memcpy(buffer, renderer->mMaterials.GetData(), dataSize);
			glUnmapBuffer(GL_TEXTURE_BUFFER); GL_CHECK_ERRORS;
			offset += dataSize;
			baseIndex += renderer->mMaterials.GetPropertyCount();
		}
	});
}

void Engine::InternalCreateFrameDataBuffer()
{
	GLuint program = mDrawVertexNormalShader.GetProgram();

	printf("Getting FrameData uniform block information...\n");
	glGetUniformIndices(program, __uniforms_count__, mFrameDataUniformNames, mFrameDataUniformIndices); GL_CHECK_ERRORS;
	glGetActiveUniformsiv(program, __uniforms_count__, mFrameDataUniformIndices, GL_UNIFORM_OFFSET, mFrameDataUniformOffsets); GL_CHECK_ERRORS;
	glGetActiveUniformsiv(program, __uniforms_count__, mFrameDataUniformIndices, GL_UNIFORM_SIZE, mFrameDataUniformSizes); GL_CHECK_ERRORS;

	//int lastItem = __uniforms_count__ - 1;
	//mFrameDataSize = mFrameDataUniformOffsets[lastItem] + mFrameDataUniformSizes[lastItem];
	GLuint frameDataUniformIndex = glGetUniformBlockIndex(program, "FrameData");
	glGetActiveUniformBlockiv(program, frameDataUniformIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &mFrameDataSize);

	printf("\n");
	printf("FrameData uniform block : \n");
	printf(" \t size : %i\n", mFrameDataSize);
	for (int i = 0; i < __uniforms_count__; ++i)
	{
		printf(" \t - %s : offset = %i, size = %i\n", mFrameDataUniformNames[i], mFrameDataUniformOffsets[i], mFrameDataUniformSizes[i]);
	}
	printf("\n");

	glGenBuffers(__BufferId_Count__, mBufferIds); GL_CHECK_ERRORS;
	glBindBuffer(GL_UNIFORM_BUFFER, mBufferIds[FrameData_BufferId]); GL_CHECK_ERRORS;
	glBufferData(GL_UNIFORM_BUFFER, mFrameDataSize, nullptr, GL_STATIC_DRAW); GL_CHECK_ERRORS;
	glBindBufferBase(GL_UNIFORM_BUFFER, FrameDataBuffer_BindingIndex, mBufferIds[FrameData_BufferId]); GL_CHECK_ERRORS;
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
	assert(mCamera != nullptr);
	assert(mCamera->GetFrame() != nullptr);
	mCamera->Update();

	for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
	{
		mLights[i]->ForEach([this](Lights::Light * light)
		{
			light->TransformInViewCoords(mCamera->GetViewMatrix());
		});
	}
}

void Engine::RenderObjects()
{
	// Fill light data buffer 
	// -----------------------------------------------------------------------
	glBindBuffer(GL_TEXTURE_BUFFER, mLightDataBuffer.GetBufferId()); GL_CHECK_ERRORS;

	GLsizeiptr offset = 0;
	for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
	{
		mLights[i]->ForEach([&offset](Lights::Light * light)
		{
			GLsizei dataSize = light->GetDataSize();
			std::uint8_t * lightDataBuffer = (std::uint8_t *)glMapBufferRange(GL_TEXTURE_BUFFER, offset, dataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT); GL_CHECK_ERRORS;
			memcpy(lightDataBuffer, light->GetData(), dataSize);
			offset += dataSize;
			glUnmapBuffer(GL_TEXTURE_BUFFER); GL_CHECK_ERRORS;
		});
	}
	// -----------------------------------------------------------------------


	// Frame data buffer
	// -----------------------------------------------------------------------
	glBindBuffer(GL_UNIFORM_BUFFER, mBufferIds[FrameData_BufferId]); GL_CHECK_ERRORS;

	std::uint8_t * buffer = (std::uint8_t *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, mFrameDataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);	GL_CHECK_ERRORS;
	assert(buffer != nullptr);

	memcpy(buffer + mFrameDataUniformOffsets[u_ProjMatrix], glm::value_ptr(mCamera->GetProjectionMatrix()), sizeof(glm::mat4));
	memcpy(buffer + mFrameDataUniformOffsets[u_ViewDQ], &mCamera->GetViewDQ(), sizeof(Maths::DualQuat));
	memcpy(buffer + mFrameDataUniformOffsets[u_AmbientLight], glm::value_ptr(mAmbientLight), sizeof(glm::vec4));

	static const int lightUniformVarIndex[(int)Lights::Light::__light_type_count__] = { u_PointLightCount, u_DirectionalLightCount };
	for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
	{
		*((GLint*)(buffer + mFrameDataUniformOffsets[lightUniformVarIndex[i]])) = (GLint)mLights[i]->GetCount();
	}

	glUnmapBuffer(GL_UNIFORM_BUFFER); GL_CHECK_ERRORS;
	// -----------------------------------------------------------------------

	//static const GLuint uintZeros[] = { 0, 0, 0, 0 };
	//static const GLfloat floatZeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//static const GLfloat floatOnes[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBuffer); GL_CHECK_ERRORS;

	glDisable(GL_BLEND); GL_CHECK_ERRORS;

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glDepthMask(GL_TRUE); GL_CHECK_ERRORS;
	glEnable(GL_DEPTH_TEST); GL_CHECK_ERRORS;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;

	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->Render();
	});

	// light pass
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); GL_CHECK_ERRORS;

	glClear(GL_COLOR_BUFFER_BIT); GL_CHECK_ERRORS;

	glDepthMask(GL_FALSE); GL_CHECK_ERRORS;
	glDisable(GL_DEPTH_TEST); GL_CHECK_ERRORS;

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

			for (int i = 0; i < (int)mLightPassTextureMapping.mMapping.size(); ++i)
			{
				glActiveTexture(GL_TEXTURE0 + FIRST_TEXTURE_SAMPLER_INDEX + i);
				glBindTexture(GL_TEXTURE_2D_ARRAY, mLightPassTextureMapping.mMapping[i].mTexture->GetResourceId());
			}

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	mDeferredLightPass->GetShader().UnUse();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer); GL_CHECK_ERRORS;
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); GL_CHECK_ERRORS;
	glBlitFramebuffer(0, 0, mGBufferWidth, mGBufferHeight, 0, 0, mGBufferWidth, mGBufferHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST); GL_CHECK_ERRORS;

	//glEnable(GL_BLEND);
	//glBlendEquation(GL_FUNC_ADD);
	//glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_DEPTH_TEST); GL_CHECK_ERRORS;

	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->DebugRender();
	});
}

Lights::PointLight * Engine::CreatePointLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat ambient, GLfloat diffuse, GLfloat specular, GLfloat constantAttenuation, GLfloat linearAttenuation, GLfloat quadraticAttenuation)
{
	const int lightType = (int)LightType::PointLight;
	if (mLights[lightType]->GetCount() < MAX_LIGHT_COUNT)
	{
		Lights::PointLight *light = new Lights::PointLight(position, color, ambient, diffuse, specular, constantAttenuation, linearAttenuation, quadraticAttenuation);
		mLights[lightType]->Attach(light);
		return light;
	}
	else
	{
		std::cerr << "Cannot create a Point Light : max lights reached!" << std::endl;
		return nullptr;
	}
}

Lights::DirectionalLight * Engine::CreateDirectionalLight(const glm::vec3 & direction, glm::vec3 const & color, GLfloat ambient, GLfloat diffuse, GLfloat specular)
{
	const int lightType = (int)LightType::DirectionalLight;
	if (mLights[lightType]->GetCount() < MAX_LIGHT_COUNT)
	{
		Lights::DirectionalLight *light = new Lights::DirectionalLight(direction, color, ambient, diffuse, specular);
		mLights[lightType]->Attach(light);
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
	mLights[light->GetLightType()]->Detach(light);
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

void Engine::InternalCreateTextures()
{
	assert(mLightPassTextureMapping.mMapping.empty());
	
	mRenderers->ForEach([this](Renderer * renderer)
	{
		const TextureInfoList & texInfoList = renderer->GetTextureInfoList();
		for (TextureInfoList::const_iterator texInfoListIter = texInfoList.begin(); texInfoListIter != texInfoList.end(); ++texInfoListIter)
		{
			TextureMapping * texMap;

			const TextureInfo & texInfo = *texInfoListIter;
			switch (texInfo.GetCategory())
			{
			case TextureCategory::Ambient:
			case TextureCategory::Diffuse:
			case TextureCategory::Specular:
				texMap = &mLightPassTextureMapping;
				break;

			default:
				texMap = &renderer->mTextureMapping;
				break;
			}

			TextureMappingList::iterator targetIt = std::find_if(texMap->mMapping.begin(), texMap->mMapping.end(), [&texInfo](const TextureMappingItem & item)
			{
				return item.mTexInfoList.front()->GetGroupId() == texInfo.GetGroupId();
			});

			TextureMappingItem * lpti;
			if (targetIt == texMap->mMapping.end())
			{
				texMap->mMapping.push_back(TextureMappingItem());
				lpti = &texMap->mMapping.back();
				lpti->mSamplerIndex = texMap->mSamplerCount;
				++texMap->mSamplerCount;
			}
			else
			{
				lpti = &(*targetIt);
			}

			texInfo.mSamplerIndex = lpti->mSamplerIndex;

			TexInfoPtrList::const_iterator it = std::find_if(lpti->mTexInfoList.begin(), lpti->mTexInfoList.end(), [&texInfo](const TextureInfo * item)
			{
				return item->GetFilename() == texInfo.GetFilename();
			});

			if (it == lpti->mTexInfoList.end())
			{
				texInfo.mLayerIndex = (GLint)lpti->mTexInfoList.size();
				lpti->mTexInfoList.push_back(&texInfo);
			}
			else
			{
				texInfo.mLayerIndex = (*it)->GetLayerIndex();
			}
		}

		for (TextureMappingList::iterator it = renderer->mTextureMapping.mMapping.begin(); it != renderer->mTextureMapping.mMapping.end(); ++it)
		{
			std::vector<std::string> textureList;
			textureList.reserve(it->mTexInfoList.size());
			for (TexInfoPtrList::const_iterator it2 = it->mTexInfoList.begin(); it2 != it->mTexInfoList.end(); ++it2)
			{
				textureList.push_back((*it2)->GetFilename());
			}

			it->mTexture = mTextureManager->LoadTextureGroup(it->mTexInfoList.front()->GetGroupId(), textureList);
		}
	});

	for (TextureMappingList::iterator it = mLightPassTextureMapping.mMapping.begin(); it != mLightPassTextureMapping.mMapping.end(); ++it)
	{
		std::vector<std::string> textureList;
		textureList.reserve(it->mTexInfoList.size());
		for (TexInfoPtrList::const_iterator it2 = it->mTexInfoList.begin(); it2 != it->mTexInfoList.end(); ++it2)
		{
			textureList.push_back((*it2)->GetFilename());
		}

		it->mTexture = mTextureManager->LoadTextureGroup(it->mTexInfoList.front()->GetGroupId(), textureList);
	}

	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->UpdateMaterialTextureIndex();
	});

}

/*
class TextureGroupLayerCount
{
public:

	std::map<std::string, GLint> mLayerIndexByName;
	GLint mLayerCount = 0;
	GLint mSamplerIndex;
};

typedef std::map<TextureGroupId, TextureGroupLayerCount> TextureGroupLayerCountMap;

void Engine::InternalCreateTextures()
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
				switch (texInfoIt->GetCategory())
				{
				case TextureCategory::Ambient:
				case TextureCategory::Diffuse:
				case TextureCategory::Specular:
					mapIt->second.mSamplerIndex = samplerIndex;
					++samplerIndex;
					break;

				case TextureCategory::NormalMap:
				case TextureCategory::HeightMap:
				default:
					mapIt->second.mSamplerIndex = -1;
					break;
				}
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

		//textureList.resize(1);
		//textureList[0] = "medias/cube_array.ktx";
		const TextureGroup* texGroup = mTextureManager->LoadTextureGroup(it->first, textureList);

		switch (texGroup->GetCategory())
		{
		case TextureCategory::Ambient:
		case TextureCategory::Diffuse:
		case TextureCategory::Specular:
			mTextureGroupList.push_back(texGroup);
			break;

		case TextureCategory::NormalMap:
		case TextureCategory::HeightMap:
		default:
			break;
		}		
	}

	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->UpdateMaterialTextureIndex();
	});
}
*/
	// =======================================================================
	// =======================================================================
} // namespace CoreFx
