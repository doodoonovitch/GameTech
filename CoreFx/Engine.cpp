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
	, mQuad(nullptr)
	, mDeferredFBO(0)
	, mDepthRBO(0)
	, mHdrFBO(0)
	, mHdrBuffer(0)
	, mGBufferWidth(1920)
	, mGBufferHeight(1080)
	, mViewportX(0)
	, mViewportY(0)
	, mViewportWidth(1920)
	, mViewportHeight(1080)
	, mExposure(1.0f)
	, mGamma(1.0f)
	, mInvGamma(1.0f)
	, mAmbientLight(0.1f, 0.1f, 0.1f, 0.f)
	, mDrawVertexNormalColor(1.f, 1.f, 0.f, 0.f)
	, mDrawPointLightColor(1.f, 1.f, 0.f, 0.f)
	, mDrawDirectionalLightColor(1.f, 0.f, 0.f, 0.f)
	, mDrawVertexNormalMagnitude(1.f)
	, mDrawLightMagnitude(0.6f)
	, mFirstLightIndexToDraw(0)
	, mLightToDrawCount(16)
	, mInitialized(false)
	, mIsDrawVertexNormalEnabled(false)
	, mDeferredShader("DeferredShader")
	, mToneMappingShader("ToneMappingShader")
	, mWireFrame(false)
{
	for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
	{
		mLights[i] = nullptr;
	}
	memset(mBufferIds, 0, sizeof(mBufferIds));
	memset(mGBuffers, 0, sizeof(mGBuffers));
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

		InternalReleaseGBuffers();

		SAFE_DELETE(mQuad);

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
		PRINT_GEN_TEXTUREBUFFER("[Engine]", mLightDescBuffer);

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
		PRINT_GEN_TEXTUREBUFFER("[Engine]", mLightDataBuffer);
	}
	// -----------------------------------------------------------------------

	// material and texture creation
	// -----------------------------------------------------------------------
	//InternalCreateTextures();
	InternalCreateMaterialBuffer();
	// -----------------------------------------------------------------------
	
	InternalInitializeQuadVAO();
	InternalInitializeDeferredPassShader();
	InternalInitializeToneMappingShader();
}

void Engine::InternalCreateGBuffers()
{
	std::cout << std::endl;
	std::cout << "Initialize Deferred framebuffers..." << std::endl;
	std::cout << "\t Size = " << mGBufferWidth << " x " << mGBufferHeight << std::endl;

	glGenFramebuffers(1, &mDeferredFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mDeferredFBO);
	glGenTextures(__gBuffer_count__, mGBuffers);
	GL_CHECK_ERRORS;

	PRINT_GEN_FRAMEBUFFER("[Engine]", mDeferredFBO);
	PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_PositionBuffer]);
	PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_DataBuffer]);
	

	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_PositionBuffer]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mGBufferWidth, mGBufferHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mGBuffers[gBuffer_PositionBuffer], 0);
	GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_DataBuffer]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, mGBufferWidth, mGBufferHeight, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, mGBuffers[gBuffer_DataBuffer], 0);
	GL_CHECK_ERRORS;

	//glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_DepthBuffer]);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mGBufferWidth, mGBufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mGBuffers[gBuffer_DepthBuffer], 0);
	//GL_CHECK_ERRORS;
	glGenRenderbuffers(1, &mDepthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mGBufferWidth, mGBufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRBO);
	GL_CHECK_ERRORS;

	PRINT_GEN_RENDERBUFFER("[Engine]", mDepthRBO);

	static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(__gBuffer_count__, drawBuffers); GL_CHECK_ERRORS;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "... failed!" << std::endl;
	std::cout << "...complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::InternalReleaseGBuffers()
{
	glDeleteRenderbuffers(1, &mDepthRBO);
	mDepthRBO = 0;
	glDeleteTextures(__gBuffer_count__, mGBuffers);
	memset(mGBuffers, 0, sizeof(mGBuffers));
	glDeleteFramebuffers(1, &mDeferredFBO);
	mDeferredFBO = 0;
}

void Engine::InternalCreateHdrBuffers()
{
	std::cout << std::endl;
	std::cout << "Initialize HDR framebuffers..." << std::endl;
	std::cout << "\t Size = " << mGBufferWidth << " x " << mGBufferHeight << std::endl;

	glGenFramebuffers(1, &mHdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mHdrFBO);
	glGenTextures(1, &mHdrBuffer);
	GL_CHECK_ERRORS;

	PRINT_GEN_FRAMEBUFFER("[Engine]", mHdrFBO);
	PRINT_GEN_TEXTURE("[Engine]", mHdrBuffer);

	glBindTexture(GL_TEXTURE_2D, mHdrBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mGBufferWidth, mGBufferHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mHdrBuffer, 0);
	GL_CHECK_ERRORS;

	glDrawBuffer(GL_COLOR_ATTACHMENT0); GL_CHECK_ERRORS;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "... failed!" << std::endl;
	std::cout << "...complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::InternalReleaseHdrBuffers()
{
	glDeleteTextures(1, &mHdrBuffer);
	mHdrBuffer = 0;
	glDeleteFramebuffers(1, &mHdrFBO);
	mHdrFBO = 0;
}

void Engine::SetViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight)
{
	bool recreateGBuffers = mGBufferWidth != gBufferWidth || mGBufferHeight != gBufferHeight;

	if (recreateGBuffers)
	{
		InternalReleaseGBuffers();
		InternalReleaseHdrBuffers();
	}

	mViewportX = viewportX;
	mViewportY = viewportY;
	mViewportWidth = viewportWidth;
	mViewportHeight = viewportHeight;
	mGBufferWidth = gBufferWidth;
	mGBufferHeight = gBufferHeight;

	glViewport(mViewportX, mViewportY, mViewportWidth, mViewportHeight);
}

void Engine::InternalInitializeQuadVAO()
{
	mQuad = new Renderables::VertexArrayObject<1>();

	GLfloat quadVertices[] =
	{
		// Positions        
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
	};

	mQuad->CreateBuffers();

	// Setup plane VAO
	glBindVertexArray(mQuad->GetVao());

	glBindBuffer(GL_ARRAY_BUFFER, mQuad->GetVbo(0));
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
	glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;
}

void Engine::InternalInitializeDeferredPassShader()
{
	std::cout << std::endl;
	std::cout << "Initialize Deferred light pass shader..." << std::endl;

	//setup shader

	// vertex shader
	mDeferredShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/light.vs.glsl");
	mDeferredShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/light.fs.glsl");

	//// fragment shader
	//std::cout << "Loading shader file : shaders/light.fs.glsl" << std::endl;
	//std::vector<std::string> lightFsGlsl(2);
	//Shader::MergeFile(lightFsGlsl[0], "shaders/light.fs.glsl");
	//std::string & textureFuncSource = lightFsGlsl[1];
	//Shader::GenerateTexGetFunction(textureFuncSource, (int)mLightPassTextureMapping.mMapping.size());
	//mDeferredShader.LoadFromString(GL_FRAGMENT_SHADER, lightFsGlsl);

	const char * uniformNames[__deferred_uniforms_count__] =
	{
		"u_gBufferPosition",
		"u_gBufferData",
		//"u_materialDataSampler",
		"u_lightDescSampler",
		"u_lightDataSampler"
	};

	mDeferredShader.CreateAndLinkProgram();

	mDeferredShader.Use();

	mDeferredShader.AddUniforms(uniformNames, __deferred_uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform1i(mDeferredShader.GetUniform(u_lightDescSampler), 0);
	glUniform1i(mDeferredShader.GetUniform(u_lightDataSampler), 1);
	glUniform1i(mDeferredShader.GetUniform(u_gBufferPosition), 2);
	glUniform1i(mDeferredShader.GetUniform(u_gBufferData), 3);

	//glUniform1i(mDeferredShader.GetUniform(u_materialDataSampler), 2);
	//for (int i = 0; i < (int)mLightPassTextureMapping.mMapping.size(); ++i)
	//{
	//	char uniformName[50];
	//	sprintf_s(uniformName, 50, "u_textureSampler[%i]", i);
	//	int uniformIndex = glGetUniformLocation(mDeferredShader.GetProgram(), uniformName); GL_CHECK_ERRORS;
	//	if (uniformIndex > 0)
	//	{
	//		glUniform1i(uniformIndex, i + FIRST_TEXTURE_SAMPLER_INDEX);	GL_CHECK_ERRORS;
	//		std::cout << "\t" << uniformName << " : " << uniformIndex << std::endl;
	//	}
	//}

	mDeferredShader.SetupFrameDataBlockBinding();
	mDeferredShader.UnUse();

	GL_CHECK_ERRORS;
	std::cout << "... done." << std::endl;
}

void Engine::InternalInitializeToneMappingShader()
{
	std::cout << std::endl;
	std::cout << "Initialize tone mapping shader..." << std::endl;

	//setup shader

	// vertex shader
	mToneMappingShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/light.vs.glsl");
	mToneMappingShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/tonemapping.fs.glsl");

	const char * uniformNames[__tonemapping_uniforms_count__] =
	{
		"u_Exposure",
		"u_InvGamma",
		"u_HdrBuffer"
	};

	mToneMappingShader.CreateAndLinkProgram();

	mToneMappingShader.Use();

	mToneMappingShader.AddUniforms(uniformNames, __tonemapping_uniforms_count__);

	glUniform1i(mToneMappingShader.GetUniform(u_HdrBuffer), 0);

	mToneMappingShader.UnUse();

	GL_CHECK_ERRORS;

	std::cout << "... done." << std::endl;
}


void Engine::InternalCreateMaterialBuffer()
{
	int bufferSize = 0;
	mRenderers->ForEach([&bufferSize](Renderer * renderer)
	{
		bufferSize += renderer->mMaterials.GetDataSize();
	});

	mMaterialBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, bufferSize, nullptr);
	PRINT_GEN_TEXTUREBUFFER("[Engine]", mMaterialBuffer);

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
	glBindBuffer(GL_UNIFORM_BUFFER, 0); GL_CHECK_ERRORS;

	PRINT_GEN_BUFFER("[Engine]", mBufferIds[FrameData_BufferId]);
}

void Engine::Initialize(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight)
{
	GetInstance()->InternalInitialize(viewportX, viewportY, viewportWidth, viewportHeight, gBufferWidth, gBufferHeight);
}

void Engine::Release()
{
	DebugOutput::GetInstance()->DisableDebugMessage();
	GetInstance()->InternalRelease();
	SAFE_DELETE(sInstance);
	SAFE_DELETE(Log::Logger::sInstance);
}

Engine* Engine::GetInstance()
{
	if (sInstance == nullptr)
	{
		Log::Logger::sInstance = new Log::Logger();
		DebugOutput::GetInstance()->EnableDebugMessage();
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
			light->SetIsModified(false);
		});
	}
}

void Engine::RenderObjects()
{
	if (mDeferredFBO == 0)
	{
		InternalCreateGBuffers();
		InternalCreateHdrBuffers();
	}

	// Fill light data buffer 
	// -----------------------------------------------------------------------
	glBindBuffer(GL_TEXTURE_BUFFER, mLightDataBuffer.GetBufferId()); 
	std::uint8_t * lightDataBuffer = (std::uint8_t *)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
	for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
	{
		mLights[i]->ForEach([&lightDataBuffer](Lights::Light * light)
		{
			GLsizei dataSize = light->GetDataSize();
			memcpy(lightDataBuffer, light->GetData(), dataSize);
			lightDataBuffer += dataSize;
		});
	}
	glUnmapBuffer(GL_TEXTURE_BUFFER);
	// -----------------------------------------------------------------------


	// Frame data buffer
	// -----------------------------------------------------------------------
	glBindBuffer(GL_UNIFORM_BUFFER, mBufferIds[FrameData_BufferId]); 

	std::uint8_t * buffer = (std::uint8_t *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, mFrameDataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);	
	assert(buffer != nullptr);

	glm::vec4 eyePos(mCamera->GetFrame()->GetPosition(), 1.f);
	glm::mat4 viewMatrix(mCamera->GetViewMatrix());
	glm::vec4 depthRangeFovAspect(mCamera->GetNearZ(), mCamera->GetFarZ(), mCamera->GetFovY(), mCamera->GetAspect());
	glm::vec4 bufferViewportSize(mGBufferWidth, mGBufferHeight, mViewportWidth, mViewportHeight);

	memcpy(buffer + mFrameDataUniformOffsets[u_ProjMatrix], glm::value_ptr(mCamera->GetProjectionMatrix()), sizeof(glm::mat4));
	memcpy(buffer + mFrameDataUniformOffsets[u_InvProjMatrix], glm::value_ptr(mCamera->GetInverseProjectionMatrix()), sizeof(glm::mat4));
	memcpy(buffer + mFrameDataUniformOffsets[u_ViewMatrix], glm::value_ptr(viewMatrix), sizeof(glm::mat4));
	memcpy(buffer + mFrameDataUniformOffsets[u_ViewDQ], &mCamera->GetViewDQ(), sizeof(Maths::DualQuat));
	memcpy(buffer + mFrameDataUniformOffsets[u_ViewPosition], glm::value_ptr(eyePos), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_AmbientLight], glm::value_ptr(mAmbientLight), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_VertexNormalColor], glm::value_ptr(mDrawVertexNormalColor), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_BufferViewportSize], glm::value_ptr(bufferViewportSize), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_DepthRangeFovYAspect], glm::value_ptr(depthRangeFovAspect), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_NormalMagnitude], &mDrawVertexNormalMagnitude, sizeof(glm::vec2));

	static const int lightUniformVarIndex[(int)Lights::Light::__light_type_count__] = { u_PointLightCount, u_SpotLightCount, u_DirectionalLightCount };
	for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
	{
		*((GLint*)(buffer + mFrameDataUniformOffsets[lightUniformVarIndex[i]])) = (GLint)mLights[i]->GetCount();
	}

	glUnmapBuffer(GL_UNIFORM_BUFFER); 
	// -----------------------------------------------------------------------

	if (GetWireFrame())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//static const GLuint uintZeros[] = { 0, 0, 0, 0 };
	//static const GLfloat floatZeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//static const GLfloat floatOnes[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mDeferredFBO); 

	glDisable(GL_BLEND); 

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glDepthMask(GL_TRUE); 
	glEnable(GL_DEPTH_TEST); 
	glClearColor(0.0f, 0.0f, 0.0f, 1);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->DebugRender();
	});

	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->Render();
	});

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// light pass
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mHdrFBO); 

	glClear(GL_COLOR_BUFFER_BIT); 

	glDepthMask(GL_FALSE); 
	glDisable(GL_DEPTH_TEST); 

	mDeferredShader.Use();
		glBindVertexArray(mQuad->GetVao());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_BUFFER, GetLightDescBuffer().GetTextureId());

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_BUFFER, GetLightDataBuffer().GetTextureId());

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_PositionBuffer]);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_DataBuffer]);

			//glActiveTexture(GL_TEXTURE2);
			//glBindTexture(GL_TEXTURE_BUFFER, mMaterialBuffer.GetTextureId());

			//for (int i = 0; i < (int)mLightPassTextureMapping.mMapping.size(); ++i)
			//{
			//	glActiveTexture(GL_TEXTURE0 + FIRST_TEXTURE_SAMPLER_INDEX + i);
			//	glBindTexture(GL_TEXTURE_2D_ARRAY, mLightPassTextureMapping.mMapping[i].mTexture->GetResourceId());
			//}

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	mDeferredShader.UnUse();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
	glClear(GL_COLOR_BUFFER_BIT); 

	mToneMappingShader.Use();

		glUniform1f(mToneMappingShader.GetUniform(u_Exposure), mExposure);
		glUniform1f(mToneMappingShader.GetUniform(u_InvGamma), mInvGamma);

		glBindVertexArray(mQuad->GetVao());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mHdrBuffer);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	mToneMappingShader.UnUse();

	//glBindFramebuffer(GL_READ_FRAMEBUFFER, mDeferredFBO); 
	//glBlitFramebuffer(0, 0, mGBufferWidth, mGBufferHeight, 0, 0, mGBufferWidth, mGBufferHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST); 

	////glEnable(GL_BLEND);
	////glBlendEquation(GL_FUNC_ADD);
	////glBlendFunc(GL_ONE, GL_ONE);
	//glEnable(GL_DEPTH_TEST); 

	//mRenderers->ForEach([](Renderer * renderer)
	//{
	//	renderer->DebugRender();
	//});
}

Lights::PointLight * Engine::CreatePointLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, GLfloat constantAttenuation, GLfloat linearAttenuation, GLfloat quadraticAttenuation)
{
	const int lightType = (int)Lights::Light::Point_Light;
	if (mLights[lightType]->GetCount() < MAX_LIGHT_COUNT)
	{
		Lights::PointLight *light = new Lights::PointLight(position, color, intensity, constantAttenuation, linearAttenuation, quadraticAttenuation);
		mLights[lightType]->Attach(light);
		return light;
	}
	else
	{
		std::cerr << "Cannot create a Point Light : max lights reached!" << std::endl;
		return nullptr;
	}
}

Lights::SpotLight * Engine::CreateSpotLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, const glm::vec3 & direction, float innerConeAngle, float outerConeAngle, GLfloat constantAttenuation, GLfloat linearAttenuation, GLfloat quadraticAttenuation)
{
	const int lightType = (int)Lights::Light::Spot_Light;
	if (mLights[lightType]->GetCount() < MAX_LIGHT_COUNT)
	{
		Lights::SpotLight *light = new Lights::SpotLight(position, color, intensity, direction, innerConeAngle, outerConeAngle, constantAttenuation, linearAttenuation, quadraticAttenuation);
		mLights[lightType]->Attach(light);
		return light;
	}
	else
	{
		std::cerr << "Cannot create a Point Light : max lights reached!" << std::endl;
		return nullptr;
	}
}


Lights::DirectionalLight * Engine::CreateDirectionalLight(const glm::vec3 & direction, glm::vec3 const & color, GLfloat intensity)
{
	const int lightType = (int)Lights::Light::Directional_Light;
	if (mLights[lightType]->GetCount() < MAX_LIGHT_COUNT)
	{
		Lights::DirectionalLight *light = new Lights::DirectionalLight(direction, color, intensity);
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

//void Engine::InternalCreateTextures()
//{
//	assert(mLightPassTextureMapping.mMapping.empty());
//	
//	mRenderers->ForEach([this](Renderer * renderer)
//	{
//		renderer->BuildTextureMapping(&mLightPassTextureMapping);
//		renderer->LoadTextures();
//	});
//
//	for (TextureMappingList::iterator it = mLightPassTextureMapping.mMapping.begin(); it != mLightPassTextureMapping.mMapping.end(); ++it)
//	{
//		std::vector<std::string> textureList;
//		textureList.reserve(it->mTexInfoList.size());
//		for (TexInfoPtrList::const_iterator it2 = it->mTexInfoList.begin(); it2 != it->mTexInfoList.end(); ++it2)
//		{
//			textureList.push_back((*it2)->GetFilename());
//		}
//
//		it->mTexture = mTextureManager->LoadTextureGroup(it->mTexInfoList.front()->GetGroupId(), textureList);
//	}
//
//	mRenderers->ForEach([](Renderer * renderer)
//	{
//		renderer->UpdateMaterialTextureIndex();
//	});
//
//}
	// =======================================================================
	// =======================================================================
} // namespace CoreFx
