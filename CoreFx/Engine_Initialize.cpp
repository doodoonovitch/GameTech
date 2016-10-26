#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	// =======================================================================
	// =======================================================================



void Engine::InternalInitializeFrameConstainers()
{
	mFrameContainers = new FrameContainerContainer(SceneObjectTypeId::__count__, nullptr);
}

void Engine::InternalInitialize(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight)
{
	if (!mInitialized)
	{
		InternalInitializeFrameConstainers();

		mPointLightPositionRenderer = new Renderers::PointLightPositionRenderer();
		mSpotLightPositionRenderer = new Renderers::SpotLightPositionRenderer();

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
		mComputes = new ComputeShaderContainer(64, 16);

		for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
		{
			mLights[i] = new LightContainer(1, 1);
		}

		mDrawGBufferNormalShader.LoadShaders();
		InternalCreateFrameDataBuffer(mDrawGBufferNormalShader.GetProgram());

		PRINT_MESSAGE("Initializing PointLightHelper Renderer...");
		PRINT_MESSAGE("... end.");

		mInitialized = true;
	}
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
	//PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_PositionBuffer]);
	PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_NormalBuffer]);
	PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_AlbedoAndStatus]);
	PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_SpecularRoughness]);
	PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_Emissive]);
	PRINT_GEN_TEXTURE("[Engine]", mGBuffers[gBuffer_DepthBuffer]);


	//glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_PositionBuffer]);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mGBufferWidth, mGBufferHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mGBuffers[gBuffer_PositionBuffer], 0);
	//GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_NormalBuffer]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mGBufferWidth, mGBufferHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mGBuffers[gBuffer_NormalBuffer], 0);
	GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_AlbedoAndStatus]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, mGBufferWidth, mGBufferHeight, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, mGBuffers[gBuffer_AlbedoAndStatus], 0);
	GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_SpecularRoughness]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mGBufferWidth, mGBufferHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, mGBuffers[gBuffer_SpecularRoughness], 0);
	GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_Emissive]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mGBufferWidth, mGBufferHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, mGBuffers[gBuffer_Emissive], 0);
	GL_CHECK_ERRORS;


	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_DepthBuffer]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, mGBufferWidth, mGBufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mGBuffers[gBuffer_DepthBuffer], 0);
	GL_CHECK_ERRORS;
	//glGenRenderbuffers(1, &mDepthRBO);
	//glBindRenderbuffer(GL_RENDERBUFFER, mDepthRBO);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, mGBufferWidth, mGBufferHeight);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRBO);
	//GL_CHECK_ERRORS;
	//PRINT_GEN_RENDERBUFFER("[Engine]", mDepthRBO);

	static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3/*, GL_COLOR_ATTACHMENT4*/ };
	glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(drawBuffers), drawBuffers); GL_CHECK_ERRORS;

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

	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_DepthBuffer]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mGBuffers[gBuffer_DepthBuffer], 0);
	GL_CHECK_ERRORS;

	//glBindRenderbuffer(GL_RENDERBUFFER, mDepthRBO);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRBO);
	//GL_CHECK_ERRORS;

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

	glBindBuffer(GL_TEXTURE_BUFFER, 0);

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





// =======================================================================
// =======================================================================
// =======================================================================





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





// =======================================================================
// =======================================================================
// =======================================================================





void Engine::InternalInitializeDeferredPassShader()
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize Deferred light pass shader.....");

	//setup shader

	// vertex shader
	mDeferredShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/light.vs.glsl");

	{
		std::vector<std::string> shaderFilenames(2);
		shaderFilenames[0] = "shaders/light.fs.glsl";
		shaderFilenames[1] = "shaders/UnpackFromGBuffer.incl.glsl";
		mDeferredShader.LoadFromFile(GL_FRAGMENT_SHADER, shaderFilenames);
	}

	//// fragment shader
	//std::cout << "Loading shader file : shaders/light.fs.glsl" << std::endl;
	//std::vector<std::string> lightFsGlsl(2);
	//Shader::MergeFile(lightFsGlsl[0], "shaders/light.fs.glsl");
	//std::string & textureFuncSource = lightFsGlsl[1];
	//Shader::GenerateTexGetFunction(textureFuncSource, (int)mLightPassTextureMapping.mMapping.size());
	//mDeferredShader.LoadFromString(GL_FRAGMENT_SHADER, lightFsGlsl);

	const char * uniformNames[__deferred_uniforms_count__] =
	{
		//"u_gBufferPosition",
		"u_gDepthMap",
		"u_gBufferNormal",
		"u_gBufferAlbedoAndStatus",
		"u_gBufferSpecularRoughness",
		"u_gBufferEmissive",
		//"u_materialDataSampler",
		"u_lightDescSampler",
		"u_lightDataSampler",
	};

	mDeferredShader.CreateAndLinkProgram();

	mDeferredShader.Use();

	mDeferredShader.AddUniforms(uniformNames, __deferred_uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform1i(mDeferredShader.GetUniform(u_lightDescSampler), 0);
	glUniform1i(mDeferredShader.GetUniform(u_lightDataSampler), 1);
	//glUniform1i(mDeferredShader.GetUniform(u_gBufferPosition), 2);
	glUniform1i(mDeferredShader.GetUniform(u_gDepthMap), 2);
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
	mToneMappingShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/screenquad.vs.glsl");
	mToneMappingShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/tonemapping.fs.glsl");

	const char * uniformNames[__tonemapping_uniforms_count__] =
	{
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
	mCopyShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/screenquad.vs.glsl");
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

void Engine::InternalInitializeViewTex2DArrayShader()
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize copy 2D texture array (debug purpose) shader.....");

	//setup shader

	// vertex shader
	mViewTex2DArrayShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/screenquad.vs.glsl");
	mViewTex2DArrayShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/copyTex2DArray.fs.glsl");

	mViewTex2DArrayShader.CreateAndLinkProgram();

	const char * uniformNames[__copytex2Darrayshader_uniforms_count__] =
	{
		"u_TextureSampler",
		"u_LayerIndex"
	};

	mViewTex2DArrayShader.Use();

	mViewTex2DArrayShader.AddUniforms(uniformNames, __copytex2Darrayshader_uniforms_count__);

	glUniform1i(mViewTex2DArrayShader.GetUniform(u_TextureSampler), 0);

	mViewTex2DArrayShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....done.");
	PRINT_END_SECTION;
}

void Engine::InternalInitializeShowDeferredBuffersShader()
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize Show Deferred Buffers shader (debugging purpose).....");

	//setup shader

	// vertex shader
	mShowDeferredBuffersShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/light.vs.glsl");

	{
		std::vector<std::string> shaderFilenames(2);
		shaderFilenames[0] = "shaders/ShowDeferredBuffers.fs.glsl";
		shaderFilenames[1] = "shaders/UnpackFromGBuffer.incl.glsl";
		mShowDeferredBuffersShader.LoadFromFile(GL_FRAGMENT_SHADER, shaderFilenames);
	}

	mShowDeferredBuffersShader.CreateAndLinkProgram();

	const char * uniformNames[(int)EShowDeferredShaderUniformIndex::__uniforms_count__] =
	{
		//"u_gBufferPosition",
		"u_gDepthMap",
		"u_gBufferNormal",
		"u_gBufferAlbedoAndStatus",
		"u_gBufferSpecularRoughness",
		"u_gBufferEmissive",
		"u_lightDescSampler",
		"u_lightDataSampler",
		"u_BufferToShow",
	};

	mShowDeferredBuffersShader.Use();

	mShowDeferredBuffersShader.AddUniforms(uniformNames, (int)EShowDeferredShaderUniformIndex::__uniforms_count__);

	glUniform1i(mShowDeferredBuffersShader.GetUniform((int)EShowDeferredShaderUniformIndex::u_lightDescSampler), 0);
	glUniform1i(mShowDeferredBuffersShader.GetUniform((int)EShowDeferredShaderUniformIndex::u_lightDataSampler), 1);
	//glUniform1i(mShowDeferredBuffersShader.GetUniform((int)EShowDeferredShaderUniformIndex::u_gBufferPosition), 2);
	glUniform1i(mShowDeferredBuffersShader.GetUniform((int)EShowDeferredShaderUniformIndex::u_gDepthMap), 2);
	glUniform1i(mShowDeferredBuffersShader.GetUniform((int)EShowDeferredShaderUniformIndex::u_gBufferNormal), 3);
	glUniform1i(mShowDeferredBuffersShader.GetUniform((int)EShowDeferredShaderUniformIndex::u_gBufferAlbedoAndStatus), 4);
	glUniform1i(mShowDeferredBuffersShader.GetUniform((int)EShowDeferredShaderUniformIndex::u_gBufferSpecularRoughness), 5);
	glUniform1i(mShowDeferredBuffersShader.GetUniform((int)EShowDeferredShaderUniformIndex::u_gBufferEmissive), 6);
	glUniform1i(mShowDeferredBuffersShader.GetUniform((int)EShowDeferredShaderUniformIndex::u_BufferToShow), mDeferredDebugState);


	mShowDeferredBuffersShader.UnUse();

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



	// =======================================================================
	// =======================================================================
} // namespace CoreFx
