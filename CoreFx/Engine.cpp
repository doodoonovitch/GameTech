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
	, mForwardRenderers(nullptr)
	, mCamera(nullptr)
	, mSkybox(nullptr)
	, mQuad(nullptr)
	, mSkydome(nullptr)
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
	, mExposure(0.1f)
	, mGamma(1.0f)
	, mInvGamma(1.0f)
	, mAmbientLight(0.1f, 0.1f, 0.1f, 0.f)
	, mWireFrameColor(1.0f, 0.576f, 0.0f, 1.0f)
	, mDrawVertexNormalColor(1.f, 1.f, 0.f, 0.f)
	, mDrawPointLightColor(1.f, 1.f, 0.f, 0.f)
	, mDrawDirectionalLightColor(1.f, 0.f, 0.f, 0.f)
	, mDrawVertexNormalMagnitude(0.2f)
	, mDrawLightMagnitude(0.6f)
	, mFirstLightIndexToDraw(0)
	, mDeferredShader("DeferredShader")
	, mToneMappingShader("ToneMappingShader")
	, mCopyShader("CopyShader")
	, mPointLightPositionRenderer(nullptr)
	, mSpotLightPositionRenderer(nullptr)
	, mDrawGBufferNormalGridSpan(20, 20)
	, mInitialized(false)
	, mIsDrawVertexNormalEnabled(false)
	, mIsDrawGBufferNormalEnabled(false)
	, mWireFrame(false)
	, mIsDrawLightPositionEnabled(false)
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
		mPointLightPositionRenderer = new PointLightPositionRenderer();
		mSpotLightPositionRenderer = new SpotLightPositionRenderer();

		mViewportX = viewportX;
		mViewportY = viewportY;
		mViewportWidth = viewportWidth;
		mViewportHeight = viewportHeight;
		mGBufferWidth = gBufferWidth;
		mGBufferHeight = gBufferHeight;

		InternalUpdateDrawGBufferNormalsPatchCount();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glViewport(mViewportX, mViewportY, mViewportWidth, mViewportHeight);
		mOrthoProjMatrix = glm::ortho(static_cast<GLfloat>(mViewportX), static_cast<GLfloat>(mViewportX + mViewportWidth), static_cast<GLfloat>(mViewportY), static_cast<GLfloat>(mViewportY + mViewportHeight));

		mTextureManager = new TextureManager();
		mTextureManager->Initialize();

		mRenderers = new RendererContainer(64, 16);
		mForwardRenderers = new RendererContainer(64, 16);

		for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
		{
			mLights[i] = new LightContainer(1, 1);
		}
		
		mDrawVertexNormalShader.LoadShaders();

		InternalCreateFrameDataBuffer(mDrawVertexNormalShader.GetProgram());

		mDrawGBufferNormalShader.LoadShaders();
		//InternalCreateFrameDataBuffer(mDrawGBufferNormalShader.GetProgram());

		PRINT_MESSAGE("Initializing PointLightHelper Renderer...");
		PRINT_MESSAGE("... end.");

		mInitialized = true;
	}
}

void Engine::InternalRelease()
{
	if (mInitialized)
	{
		SAFE_DELETE(mPointLightPositionRenderer);
		SAFE_DELETE(mSpotLightPositionRenderer);

		glDeleteBuffers(__BufferId_Count__, mBufferIds);
		memset(mBufferIds, 0, sizeof(mBufferIds));

		SAFE_DELETE(mCamera);

		SAFE_DELETE(mRenderers);
		SAFE_DELETE(mForwardRenderers);

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
			mLightDescIndexOffsets[i] = index;
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
	InternalInitializeCopyShader();
}

void Engine::InternalCreateGBuffers()
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize Deferred framebuffers.....");
	PRINT_MESSAGE("Size = %li x %li", mGBufferWidth, mGBufferHeight);

	glGenFramebuffers(1, &mDeferredFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mDeferredFBO);
	glGenTextures(__gBuffer_count__, mGBuffers);
	GL_CHECK_ERRORS;

	PRINT_GEN_FRAMEBUFFER("[Engine]", mDeferredFBO);
	PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_PositionBuffer]);
	PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_NormalBuffer]);
	PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_AlbedoAndStatus]);
	PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_SpecularRoughness]);
	PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_Emissive]);
	//PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_DepthBuffer]);


	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_PositionBuffer]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mGBufferWidth, mGBufferHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mGBuffers[gBuffer_PositionBuffer], 0);
	GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_NormalBuffer]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mGBufferWidth, mGBufferHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, mGBuffers[gBuffer_NormalBuffer], 0);
	GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_AlbedoAndStatus]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, mGBufferWidth, mGBufferHeight, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, mGBuffers[gBuffer_AlbedoAndStatus], 0);
	GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_SpecularRoughness]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mGBufferWidth, mGBufferHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, mGBuffers[gBuffer_SpecularRoughness], 0);
	GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_Emissive]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mGBufferWidth, mGBufferHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, mGBuffers[gBuffer_Emissive], 0);
	GL_CHECK_ERRORS;


	//glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_DepthBuffer]);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL, mGBufferWidth, mGBufferHeight, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mGBuffers[gBuffer_DepthBuffer], 0);
	//GL_CHECK_ERRORS;
	glGenRenderbuffers(1, &mDepthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, mGBufferWidth, mGBufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRBO);
	GL_CHECK_ERRORS;
	PRINT_GEN_RENDERBUFFER("[Engine]", mDepthRBO);

	static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(__gBuffer_count__, drawBuffers); GL_CHECK_ERRORS;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		PRINT_MESSAGE(".....failed!");
	}
	else
	{
		PRINT_MESSAGE("...complete!");
	}
	PRINT_END_SECTION;

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
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize HDR framebuffers.....");
	PRINT_MESSAGE("Size = %li x %li", mGBufferWidth, mGBufferHeight);

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
	{
		PRINT_MESSAGE(".....failed!");
	}
	else
	{
		PRINT_MESSAGE("...complete!");
	}
	PRINT_END_SECTION;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize Forward framebuffers.....");

	glGenFramebuffers(1, &mForwardFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mForwardFBO);
	glGenTextures(1, &mForwardBuffer);

	glBindTexture(GL_TEXTURE_2D, mForwardBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mGBufferWidth, mGBufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mForwardBuffer, 0);
	GL_CHECK_ERRORS;

	//glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_DepthBuffer]);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mGBuffers[gBuffer_DepthBuffer], 0);
	//GL_CHECK_ERRORS;

	glBindRenderbuffer(GL_RENDERBUFFER, mDepthRBO);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRBO);
	GL_CHECK_ERRORS;

	PRINT_GEN_RENDERBUFFER("[Engine]", mForwardFBO);

	static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers); GL_CHECK_ERRORS;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		PRINT_MESSAGE(".....failed!");
	}
	else
	{
		PRINT_MESSAGE("...complete!");
	}
	PRINT_END_SECTION;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::InternalReleaseHdrBuffers()
{
	glDeleteTextures(1, &mHdrBuffer);
	mHdrBuffer = 0;
	glDeleteTextures(1, &mForwardBuffer);
	mForwardBuffer = 0;
	glDeleteFramebuffers(1, &mHdrFBO);
	mHdrFBO = 0;
	glDeleteFramebuffers(1, &mForwardFBO);
	mForwardFBO = 0;
}

void Engine::SetViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight)
{
	if (mInitialized)
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
		mOrthoProjMatrix = glm::ortho(static_cast<GLfloat>(mViewportX), static_cast<GLfloat>(mViewportX + mViewportWidth), static_cast<GLfloat>(mViewportY), static_cast<GLfloat>(mViewportY + mViewportHeight));

		InternalUpdateDrawGBufferNormalsPatchCount();
	}
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
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize Deferred light pass shader.....");

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
		"u_gBufferNormal",
		"u_gBufferAlbedoAndStatus",
		"u_gBufferSpecularRoughness",
		"u_gBufferEmissive",
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
	glUniform1i(mDeferredShader.GetUniform(u_gBufferNormal), 3);
	glUniform1i(mDeferredShader.GetUniform(u_gBufferAlbedoAndStatus), 4);
	glUniform1i(mDeferredShader.GetUniform(u_gBufferSpecularRoughness), 5);
	glUniform1i(mDeferredShader.GetUniform(u_gBufferEmissive), 6);

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

	PRINT_MESSAGE(".....Deferred light pass shader initialized!");
	PRINT_END_SECTION;
}

void Engine::InternalInitializeToneMappingShader()
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize tone mapping shader.....");

	//setup shader

	// vertex shader
	mToneMappingShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/light.vs.glsl");
	mToneMappingShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/tonemapping.fs.glsl");

	const char * uniformNames[__tonemapping_uniforms_count__] =
	{
		"u_InvGamma",
		"u_HdrBuffer"
	};

	mToneMappingShader.CreateAndLinkProgram();

	mToneMappingShader.Use();

	mToneMappingShader.AddUniforms(uniformNames, __tonemapping_uniforms_count__);

	glUniform1i(mToneMappingShader.GetUniform(u_HdrBuffer), 0);

	mToneMappingShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....tone mapping shader initialized!");
	PRINT_END_SECTION;
}

void Engine::InternalInitializeCopyShader()
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize copy (post process) shader.....");

	//setup shader

	// vertex shader
	mCopyShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/light.vs.glsl");
	mCopyShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/copy.fs.glsl");

	mCopyShader.CreateAndLinkProgram();

	const char * uniformNames[__copyshader_uniforms_count__] =
	{
		"u_SourceBuffer"
	};

	mCopyShader.Use();

	mCopyShader.AddUniforms(uniformNames, __copyshader_uniforms_count__);

	glUniform1i(mCopyShader.GetUniform(u_SourceBuffer), 0);

	mCopyShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....done.");
	PRINT_END_SECTION;
}

void Engine::InternalCreateMaterialBuffer(RendererContainer * renderers, GLsizeiptr & offset, GLint & baseIndex)
{
	renderers->ForEach([&offset, &baseIndex](Renderer * renderer)
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

void Engine::InternalCreateMaterialBuffer()
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Create material buffers.....");

	int bufferSize = 0;
	mRenderers->ForEach([&bufferSize](Renderer * renderer)
	{
		bufferSize += renderer->mMaterials.GetDataSize();
	});

	mForwardRenderers->ForEach([&bufferSize](Renderer * renderer)
	{
		bufferSize += renderer->mMaterials.GetDataSize();
	});

	mMaterialBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, bufferSize, nullptr);
	PRINT_GEN_TEXTUREBUFFER("[Engine]", mMaterialBuffer);

	glBindBuffer(GL_TEXTURE_BUFFER, mMaterialBuffer.GetBufferId()); GL_CHECK_ERRORS;
	GLsizeiptr offset = 0;
	GLint baseIndex = 0;

	InternalCreateMaterialBuffer(mRenderers, offset, baseIndex);
	InternalCreateMaterialBuffer(mForwardRenderers, offset, baseIndex);

	PRINT_MESSAGE(".....done.");
	PRINT_END_SECTION;
}

void Engine::InternalCreateFrameDataBuffer(GLuint program)
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Create FrameData buffers.....");

	PRINT_MESSAGE("Getting FrameData uniform block information...");
	glGetUniformIndices(program, __uniforms_count__, mFrameDataUniformNames, mFrameDataUniformIndices); GL_CHECK_ERRORS;
	glGetActiveUniformsiv(program, __uniforms_count__, mFrameDataUniformIndices, GL_UNIFORM_OFFSET, mFrameDataUniformOffsets); GL_CHECK_ERRORS;
	glGetActiveUniformsiv(program, __uniforms_count__, mFrameDataUniformIndices, GL_UNIFORM_SIZE, mFrameDataUniformSizes); GL_CHECK_ERRORS;

	//int lastItem = __uniforms_count__ - 1;
	//mFrameDataSize = mFrameDataUniformOffsets[lastItem] + mFrameDataUniformSizes[lastItem];
	GLuint frameDataUniformIndex = glGetUniformBlockIndex(program, "FrameData");
	glGetActiveUniformBlockiv(program, frameDataUniformIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &mFrameDataSize);

	PRINT_MESSAGE("FrameData uniform block :");
	PRINT_MESSAGE(" \t size : %i", mFrameDataSize);
	for (int i = 0; i < __uniforms_count__; ++i)
	{
		PRINT_MESSAGE(" \t - %s : offset = %i, size = %i", mFrameDataUniformNames[i], mFrameDataUniformOffsets[i], mFrameDataUniformSizes[i]);
	}

	glGenBuffers(__BufferId_Count__, mBufferIds); GL_CHECK_ERRORS;
	glBindBuffer(GL_UNIFORM_BUFFER, mBufferIds[FrameData_BufferId]); GL_CHECK_ERRORS;
	glBufferData(GL_UNIFORM_BUFFER, mFrameDataSize, nullptr, GL_STATIC_DRAW); GL_CHECK_ERRORS;
	glBindBufferBase(GL_UNIFORM_BUFFER, FrameDataBuffer_BindingIndex, mBufferIds[FrameData_BufferId]); GL_CHECK_ERRORS;
	glBindBuffer(GL_UNIFORM_BUFFER, 0); GL_CHECK_ERRORS;

	PRINT_GEN_BUFFER("[Engine]", mBufferIds[FrameData_BufferId]);

	PRINT_MESSAGE(".....done.");
	PRINT_END_SECTION;
}

void Engine::Initialize(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight)
{
	GetInstance()->InternalInitialize(viewportX, viewportY, viewportWidth, viewportHeight, gBufferWidth, gBufferHeight);
}

void Engine::Release()
{
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
	memcpy(buffer + mFrameDataUniformOffsets[u_OrthoProjMatrix], glm::value_ptr(mOrthoProjMatrix), sizeof(glm::mat4));
	memcpy(buffer + mFrameDataUniformOffsets[u_ViewMatrix], glm::value_ptr(viewMatrix), sizeof(glm::mat4));
	memcpy(buffer + mFrameDataUniformOffsets[u_ViewDQ], &mCamera->GetViewDQ(), sizeof(Maths::DualQuat));
	memcpy(buffer + mFrameDataUniformOffsets[u_ViewPosition], glm::value_ptr(eyePos), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_AmbientLight], glm::value_ptr(mAmbientLight), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_WireFrameDrawColor], glm::value_ptr(mWireFrameColor), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_VertexNormalColor], glm::value_ptr(mDrawVertexNormalColor), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_BufferViewportSize], glm::value_ptr(bufferViewportSize), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_DepthRangeFovYAspect], glm::value_ptr(depthRangeFovAspect), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_TimeDeltaTime], mTimeDeltaTime, sizeof(GLfloat) * 2);
	memcpy(buffer + mFrameDataUniformOffsets[u_NormalMagnitude], &mDrawVertexNormalMagnitude, sizeof(GLfloat));
	memcpy(buffer + mFrameDataUniformOffsets[u_Exposure], &mExposure, sizeof(GLfloat));
	
	static const int lightUniformVarIndex[(int)Lights::Light::__light_type_count__] = { u_PointLightCount, u_SpotLightCount, u_DirectionalLightCount };
	for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
	{
		*((GLint*)(buffer + mFrameDataUniformOffsets[lightUniformVarIndex[i]])) = (GLint)mLights[i]->GetCount();
	}

	glUnmapBuffer(GL_UNIFORM_BUFFER); 
	// -----------------------------------------------------------------------

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// -----------------------------------------------------------------------
	// Geometry pass
	// -----------------------------------------------------------------------

	//static const GLuint uintZeros[] = { 0, 0, 0, 0 };
	//static const GLfloat floatZeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//static const GLfloat floatOnes[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mDeferredFBO); 

	glDisable(GL_BLEND); 

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);

	glDepthMask(GL_TRUE); 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilMask(0x00);

	mRenderers->ForEach([](Renderer * renderer)
	{
		renderer->Render();
	});

	// -----------------------------------------------------------------------
	// light pass
	// -----------------------------------------------------------------------

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
			glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_NormalBuffer]);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_AlbedoAndStatus]);

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_SpecularRoughness]);

			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_Emissive]);

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

	// -----------------------------------------------------------------------
	// tone mapping pass
	// -----------------------------------------------------------------------

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mForwardFBO);
	//glClear(GL_COLOR_BUFFER_BIT);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	mToneMappingShader.Use();

	glUniform1f(mToneMappingShader.GetUniform(u_InvGamma), mInvGamma);

	glBindVertexArray(mQuad->GetVao());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mHdrBuffer);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	mToneMappingShader.UnUse();

	// -----------------------------------------------------------------------
	// forward pass
	// -----------------------------------------------------------------------

	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mForwardFBO);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	if (mSkybox != nullptr)
	{
		//glDisable(GL_CULL_FACE);
		mSkybox->Render();
		//glEnable(GL_CULL_FACE);
	}
	else if (mSkydome != nullptr)
	{
		//glDisable(GL_CULL_FACE);
		mSkydome->Render();
		//glEnable(GL_CULL_FACE);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);

	//glDisable(GL_CULL_FACE);
	mForwardRenderers->ForEach([](Renderer * renderer)
	{
		renderer->Render();
	});
	//glEnable(GL_CULL_FACE);

	if (mIsDrawLightPositionEnabled)
	{
		mPointLightPositionRenderer->Render();
		mSpotLightPositionRenderer->Render();
	}

	// -----------------------------------------------------------------------
	// wire frame pass
	// -----------------------------------------------------------------------

	if (GetWireFrame())
	{
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		mRenderers->ForEach([](Renderer * renderer)
		{
			renderer->RenderWireFrame();
		});

		if(mSkybox != nullptr)
			mSkybox->RenderWireFrame();
		else if (mSkydome != nullptr)
			mSkydome->RenderWireFrame();

		if (mIsDrawLightPositionEnabled)
		{
			mPointLightPositionRenderer->RenderWireFrame();
			mSpotLightPositionRenderer->RenderWireFrame();
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// -----------------------------------------------------------------------
	// draw normal (from GBuffer) pass
	// -----------------------------------------------------------------------

	if (mIsDrawGBufferNormalEnabled)
	{
		//glEnable(GL_BLEND);
		//glBlendEquation(GL_FUNC_ADD);
		//glBlendFunc(GL_ONE, GL_ONE);
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		mDrawGBufferNormalShader.Use();

		glBindVertexArray(mQuad->GetVao());

		glUniform2iv(mDrawGBufferNormalShader.GetUniform(Renderers::DrawGBufferNormalShader::u_PatchCount), 1, glm::value_ptr(mDrawGBufferNormalPatchCount));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_PositionBuffer]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_NormalBuffer]);

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, mDrawGBufferNormalPatchCount.x * mDrawGBufferNormalPatchCount.y);

		glBindVertexArray(0);
		mDrawGBufferNormalShader.UnUse();
	}

	// -----------------------------------------------------------------------
	// copy to main framebuffer
	// -----------------------------------------------------------------------


	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	mCopyShader.Use();
	glBindVertexArray(mQuad->GetVao());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mForwardBuffer);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	mCopyShader.UnUse();

}

Lights::PointLight * Engine::CreatePointLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, GLfloat radius)
{
	const int lightType = (int)Lights::Light::Point_Light;
	if (mLights[lightType]->GetCount() < MAX_LIGHT_COUNT)
	{
		Lights::PointLight *light = new Lights::PointLight(position, color, intensity, radius);
		mLights[lightType]->Attach(light);
		//mPointLightHelperRenderer.mVboUpdateNeeded = true;
		return light;
	}
	else
	{
		PRINT_ERROR("Cannot create a Point Light : max lights reached!" );
		return nullptr;
	}
}

Lights::SpotLight * Engine::CreateSpotLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, GLfloat radius, const glm::vec3 & direction, float innerConeAngle, float outerConeAngle)
{
	const int lightType = (int)Lights::Light::Spot_Light;
	if (mLights[lightType]->GetCount() < MAX_LIGHT_COUNT)
	{
		Lights::SpotLight *light = new Lights::SpotLight(position, color, intensity, radius, direction, innerConeAngle, outerConeAngle);
		mLights[lightType]->Attach(light);
		return light;
	}
	else
	{
		PRINT_ERROR("Cannot create a Point Light : max lights reached!");
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
		PRINT_ERROR("Cannot create a Directional Light : max lights reached!");
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
	assert(renderer != nullptr);
	if (renderer == nullptr)
	{
		PRINT_ERROR("Cannot attach renderer : parameter null!");
		return false;
	}

	switch (renderer->GetRenderPass())
	{
	case Renderer::Deferred_Pass:
		return mRenderers->Attach(renderer);

	case Renderer::Forward_Pass:
		return mForwardRenderers->Attach(renderer);
	}

	return false;
}

bool Engine::DetachRenderer(Renderer* renderer)
{
	assert(renderer != nullptr);
	if (renderer == nullptr)
	{
		PRINT_ERROR("Cannot detach renderer : parameter null!");
		return false;
	}

	switch (renderer->GetRenderPass())
	{
	case Renderer::Deferred_Pass:
		return mRenderers->Detach(renderer);

	case Renderer::Forward_Pass:
		return mForwardRenderers->Detach(renderer);
	}

	return false;
}

bool Engine::AttachSkyboxRenderer(Renderers::SkyboxRenderer * skybox)
{
	mSkybox = skybox;
	return true;
}

bool Engine::DetachSkyboxRenderer(Renderers::SkyboxRenderer * skybox)
{
	if (mSkybox == skybox)
	{
		mSkybox = nullptr;
		return true;
	}

	return false;
}

bool Engine::AttachSkydomeRenderer(Renderers::SkydomeRenderer * skydome)
{
	mSkydome = skydome;
	return true;
}

bool Engine::DetachSkydomeRenderer(Renderers::SkydomeRenderer * skydome)
{
	if (mSkydome == skydome)
	{
		mSkydome = nullptr;
		return true;
	}

	return false;
}


void Engine::SetDrawGBufferNormalGrid(GLint drawEveryXPixels, GLint drawEveryYPixels)
{
	mDrawGBufferNormalGridSpan.x = drawEveryXPixels;
	mDrawGBufferNormalGridSpan.y = drawEveryYPixels;
	InternalUpdateDrawGBufferNormalsPatchCount();
}

void Engine::InternalUpdateDrawGBufferNormalsPatchCount()
{
	mDrawGBufferNormalPatchCount.x = mGBufferWidth / mDrawGBufferNormalGridSpan.x;
	mDrawGBufferNormalPatchCount.y = mGBufferHeight / mDrawGBufferNormalGridSpan.x;
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
// =======================================================================




Engine::PointLightPositionRenderer::PointLightPositionRenderer()
	: Renderers::IcosahedronRendererBase()
{
	InitializeVertexBuffer();
	LoadShaders(mShader, "shaders/IcosahedronShader.PointLight.gs.glsl", "shaders/IcosahedronShader.PointLight.forward.fs.glsl");
	InitializeUniforms(mShader);
	LoadShaders(mWireFrameShader, "shaders/IcosahedronShader.PointLight.gs.glsl", "shaders/IcosahedronShader.WireFrame.forward.fs.glsl");
	InitializeUniforms(mWireFrameShader);
}

Engine::PointLightPositionRenderer::~PointLightPositionRenderer()
{

}

void Engine::PointLightPositionRenderer::InitializeUniforms(Shader & shader)
{
	const char * uniformNames[__uniforms_count__] =
	{
		"u_InnerTessLevel",
		"u_OuterTessLevel",
		"u_LightDescSampler",
		"u_LightDataSampler",
		"u_LightOffset",
	};

	shader.Use();
	shader.AddUniforms(uniformNames, __uniforms_count__);

	glUniform1i(shader.GetUniform(u_LightDescSampler), 0);
	glUniform1i(shader.GetUniform(u_LightDataSampler), 1);
	glUniform1f(shader.GetUniform(u_InnerTessLevel), 1.f);
	glUniform1f(shader.GetUniform(u_OuterTessLevel), 1.f);

	shader.SetupFrameDataBlockBinding();
	shader.UnUse();

	GL_CHECK_ERRORS;
}

void Engine::PointLightPositionRenderer::InternalRender(Shader & shader, GLsizei instanceCount, GLint lightOffset, GLuint lightDescBufferId, GLuint lightDataBufferId)
{
	glDisable(GL_CULL_FACE);
	shader.Use();
	glBindVertexArray(mVaoID);

	glUniform1i(shader.GetUniform(u_LightOffset), lightOffset);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, lightDescBufferId);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, lightDataBufferId);

	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawElementsInstanced(GL_PATCHES, mIndexCount, GL_UNSIGNED_SHORT, 0, instanceCount);

	glBindVertexArray(0);
	shader.UnUse();
	glEnable(GL_CULL_FACE);
}

void Engine::PointLightPositionRenderer::Render()
{
	Engine * engine = Engine::GetInstance();
	auto lights = engine->mLights[Lights::Light::Point_Light];
	InternalRender(mShader, (GLsizei)lights->GetCount(), engine->mLightDescIndexOffsets[Lights::Light::Point_Light], engine->mLightDescBuffer.GetTextureId(), engine->mLightDataBuffer.GetTextureId());
}

void Engine::PointLightPositionRenderer::RenderWireFrame()
{
	Engine * engine = Engine::GetInstance();
	auto lights = engine->mLights[Lights::Light::Point_Light];
	InternalRender(mWireFrameShader, (GLsizei)lights->GetCount(), engine->mLightDescIndexOffsets[Lights::Light::Point_Light], engine->mLightDescBuffer.GetTextureId(), engine->mLightDataBuffer.GetTextureId());
}





// =======================================================================
// =======================================================================
// =======================================================================




Engine::SpotLightPositionRenderer::SpotLightPositionRenderer(GLuint numStrips)
	: Renderers::ConeRendererBase()
{
	InitializeVertexBuffer(numStrips);
	LoadShaders(mShader, "shaders/ConeShader.Light.gs.glsl", "shaders/ConeShader.Light.forward.fs.glsl");
	InitializeUniforms(mShader);
	LoadShaders(mWireFrameShader, "shaders/ConeShader.Light.gs.glsl", "shaders/IcosahedronShader.WireFrame.forward.fs.glsl");
	InitializeUniforms(mWireFrameShader);
}

Engine::SpotLightPositionRenderer::~SpotLightPositionRenderer()
{

}

void Engine::SpotLightPositionRenderer::InitializeUniforms(Shader & shader)
{
	const char * uniformNames[__uniforms_count__] =
	{
		"u_LightDescSampler",
		"u_LightDataSampler",
		"u_LightOffset",
	};

	shader.Use();
	shader.AddUniforms(uniformNames, __uniforms_count__);

	glUniform1i(shader.GetUniform(u_LightDescSampler), 0);
	glUniform1i(shader.GetUniform(u_LightDataSampler), 1);

	shader.SetupFrameDataBlockBinding();
	shader.UnUse();

	GL_CHECK_ERRORS;
}

void Engine::SpotLightPositionRenderer::InternalRender(Shader & shader, GLsizei instanceCount, GLint lightOffset, GLuint lightDescBufferId, GLuint lightDataBufferId)
{
	glDisable(GL_CULL_FACE);
	shader.Use();
	glBindVertexArray(mVaoID);

	glUniform1i(shader.GetUniform(u_LightOffset), lightOffset);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, lightDescBufferId);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, lightDataBufferId);

	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, mIndexCount, instanceCount);

	glBindVertexArray(0);
	shader.UnUse();
	glEnable(GL_CULL_FACE);
}

void Engine::SpotLightPositionRenderer::Render()
{
	Engine * engine = Engine::GetInstance();
	auto lights = engine->mLights[Lights::Light::Spot_Light];
	InternalRender(mShader, (GLsizei)lights->GetCount(), engine->mLightDescIndexOffsets[Lights::Light::Spot_Light], engine->mLightDescBuffer.GetTextureId(), engine->mLightDataBuffer.GetTextureId());
}

void Engine::SpotLightPositionRenderer::RenderWireFrame()
{
	Engine * engine = Engine::GetInstance();
	auto lights = engine->mLights[Lights::Light::Spot_Light];
	InternalRender(mWireFrameShader, (GLsizei)lights->GetCount(), engine->mLightDescIndexOffsets[Lights::Light::Spot_Light], engine->mLightDescBuffer.GetTextureId(), engine->mLightDataBuffer.GetTextureId());
}



	// =======================================================================
	// =======================================================================
} // namespace CoreFx
