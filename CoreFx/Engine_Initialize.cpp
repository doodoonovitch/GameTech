#include "stdafx.h"
#include "CoreFx.h"
#include <random>

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

		InternalInitializeViewportParams(viewportX, viewportY, viewportWidth, viewportHeight, gBufferWidth, gBufferHeight);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		mTextureManager = new TextureManager();
		mTextureManager->Initialize();

		mRenderers = new RendererContainer(64, 16);
		mForwardRenderers = new RendererContainer(64, 16);
		mComputes = new ComputeShaderContainer(64, 16);

		for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
		{
			mLights[i] = new LightContainer(1, 1);
		}

		PRINT_MESSAGE("Initializing PointLightHelper Renderer...");
		PRINT_MESSAGE("... end.");

		mInitialized = true;
	}
}

void Engine::InternalInitializeViewportParams(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight)
{
	mViewportX = viewportX;
	mViewportY = viewportY;
	mViewportWidth = viewportWidth;
	mViewportHeight = viewportHeight;
	mGBufferWidth = gBufferWidth;
	mGBufferHeight = gBufferHeight;
	mNoiseScale.x = (GLfloat)mGBufferWidth / 4.0f;
	mNoiseScale.y = (GLfloat)mGBufferHeight / 4.0f;

	InternalUpdateDrawGBufferNormalsPatchCount();

	glViewport(mViewportX, mViewportY, mViewportWidth, mViewportHeight);
	mOrthoProjMatrix = glm::ortho(static_cast<GLfloat>(mViewportX), static_cast<GLfloat>(mViewportX + mViewportWidth), static_cast<GLfloat>(mViewportY), static_cast<GLfloat>(mViewportY + mViewportHeight));
}

void Engine::InternalGenerateBuffersAndFBOs()
{
	glGenFramebuffers(__fbo_count__, mFBOs);
	InternalCreateGBuffers();
	InternalCreateHdrBuffers();
	InternalCreateSSAOBuffers();
}

void Engine::InternalCreateGBuffers()
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize Deferred framebuffers.....");
	PRINT_MESSAGE("Size = %li x %li", mGBufferWidth, mGBufferHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, mFBOs[Deferred_FBO]);
	glGenTextures(__gBuffer_count__, mGBuffers);
	GL_CHECK_ERRORS;

	PRINT_GEN_FRAMEBUFFER("[Engine]", mFBOs[Deferred_FBO]);
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

	glBindFramebuffer(GL_FRAMEBUFFER, mFBOs[HDR_FBO]);
	glGenTextures(1, &mHdrBuffer);
	GL_CHECK_ERRORS;

	PRINT_GEN_FRAMEBUFFER("[Engine]", mFBOs[HDR_FBO]);
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

	glBindFramebuffer(GL_FRAMEBUFFER, mFBOs[Forward_FBO]);
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

	PRINT_GEN_RENDERBUFFER("[Engine]", mFBOs[Forward_FBO]);

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

void Engine::InternalCreateSSAOBuffers()
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize SSAO buffers.....");

	glGenTextures(ARRAY_SIZE_IN_ELEMENTS(mSSAOBuffers), mSSAOBuffers);
	GL_CHECK_ERRORS;

	PRINT_GEN_TEXTURE("[Engine]", mSSAOBuffers[SSAOBuffer_Main]);
	PRINT_GEN_TEXTURE("[Engine]", mSSAOBuffers[SSAOBuffer_Temp]);
	PRINT_GEN_TEXTURE("[Engine]", mSSAOBuffers[SSAOBuffer_Kernel]);
	PRINT_GEN_TEXTURE("[Engine]", mSSAOBuffers[SSAOBuffer_Noise]);

	for (GLuint i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, mSSAOBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, mGBufferWidth, mGBufferHeight, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		GL_CHECK_ERRORS;
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Sample kernel
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel; 
	ssaoKernel.reserve(mSSAOKernelSize);

	for (GLuint i = 0; i < mSSAOKernelSize; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		GLfloat scale = GLfloat(i) / (GLfloat)mSSAOKernelSize;

		// Scale samples s.t. they're more aligned to center of kernel
		scale = glm::lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	glBindTexture(GL_TEXTURE_1D, mSSAOBuffers[SSAOBuffer_Kernel]);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_RGB16F, mSSAOKernelSize);
	GL_CHECK_ERRORS;
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, mSSAOKernelSize, GL_RGB, GL_FLOAT, ssaoKernel.data());
	GL_CHECK_ERRORS;
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_1D, 0);

	// Noise texture
	std::vector<glm::vec3> ssaoNoise;
	for (GLuint i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
		ssaoNoise.push_back(noise);
	}
	glBindTexture(GL_TEXTURE_2D, mSSAOBuffers[SSAOBuffer_Noise]);
	TextureManager::CreateTexStorage2D(GL_TEXTURE_2D, 4, 4, ssaoNoise.data(), false, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_RGB16F, GL_RGB, GL_FLOAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	//

	glBindFramebuffer(GL_FRAMEBUFFER, mFBOs[SSAO_FBO]);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mSSAOBuffers[SSAOBuffer_Main], 0);
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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	PRINT_END_SECTION;
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
		std::vector<std::string> shaderFilenames(3);
		shaderFilenames[0] = "shaders/light.fs.glsl";
		shaderFilenames[1] = "shaders/UnpackFromGBuffer.incl.glsl";
		shaderFilenames[2] = "shaders/light.pbr.fs.glsl";
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
		"u_gBufferSSAO",
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
	glUniform1i(mDeferredShader.GetUniform(u_gBufferSSAO), 7);
	
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
		"u_gBufferSSAO"
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
	glUniform1i(mShowDeferredBuffersShader.GetUniform((int)EShowDeferredShaderUniformIndex::u_gBufferSSAO), 7);
	
	glUniform1i(mShowDeferredBuffersShader.GetUniform((int)EShowDeferredShaderUniformIndex::u_BufferToShow), mDeferredDebugState);


	mShowDeferredBuffersShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....done.");
	PRINT_END_SECTION;
}

void Engine::InternalInitializeSSAOShader()
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize SSAO shader .....");

	//setup shader

	mSSAOShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/light.vs.glsl");
	mSSAOShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/ssao.fs.glsl");

	mSSAOShader.CreateAndLinkProgram();

	const char * uniformNames[(int)ESSAOShaderUniformIndex::__uniforms_count__] =
	{
		//"u_gBufferPosition",
		"u_gDepthMap",
		"u_gBufferNormal",
		"u_gBufferAlbedoAndStatus",
		"u_gNoiseMap",
		"u_gKernel",
		"u_KernelSize",
		"u_Radius",
		"u_NoiseScale",
	};

	mSSAOShader.Use();

	mSSAOShader.AddUniforms(uniformNames, (int)ESSAOShaderUniformIndex::__uniforms_count__);

	glUniform1i(mSSAOShader.GetUniform((int)ESSAOShaderUniformIndex::u_gNoiseMap), 0);
	glUniform1i(mSSAOShader.GetUniform((int)ESSAOShaderUniformIndex::u_gKernel), 1);
	//glUniform1i(mSSAOShader.GetUniform((int)ESSAOShaderUniformIndex::u_gBufferPosition), 2);
	glUniform1i(mSSAOShader.GetUniform((int)ESSAOShaderUniformIndex::u_gDepthMap), 2);
	glUniform1i(mSSAOShader.GetUniform((int)ESSAOShaderUniformIndex::u_gBufferNormal), 3);
	glUniform1i(mSSAOShader.GetUniform((int)ESSAOShaderUniformIndex::u_gBufferAlbedoAndStatus), 4);

	mSSAOShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....done.");
	PRINT_END_SECTION;
}

void Engine::InternalInitializeSSAOBlurShader()
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize SSAO Blur shader .....");

	//setup shader

	mSSAOBlurShader.LoadFromFile(GL_COMPUTE_SHADER, "shaders/ssao.GaussianBlur.cs.glsl");

	mSSAOBlurShader.CreateAndLinkProgram();

	const char * uniformNames[(int)ESSAOBlurShaderUniformIndex::__uniforms_count__] =
	{
		"u_ImageIn",
		"u_TexOffset",
	};

	mSSAOBlurShader.Use();

	mSSAOBlurShader.AddUniforms(uniformNames, (int)ESSAOBlurShaderUniformIndex::__uniforms_count__);

	glUniform1i(mSSAOBlurShader.GetUniform((int)ESSAOBlurShaderUniformIndex::u_ImageIn), 0);

	mSSAOBlurShader.UnUse();

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
