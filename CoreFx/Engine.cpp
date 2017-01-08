#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	// =======================================================================
	// =======================================================================


	Engine* Engine::sInstance = nullptr;
	bool Engine::sIsUsedExtensionSupported[__UsedExtensions_count__ + 1];

Engine::Engine()
	: mTextureManager(nullptr)
	, mRenderers(nullptr)
	, mForwardRenderers(nullptr)
	, mHUDRenderers(nullptr)
	, mComputes(nullptr)
	, mFrameContainers(nullptr)
	, mCamera(nullptr)
	, mSkybox(nullptr)
	, mQuad(nullptr)
	, mSkydome(nullptr)
	, mGBufferWidth(1920)
	, mGBufferHeight(1080)
	, mViewportOrigin(0)
	, mViewportSize(1920, 1080)
	, mExposure(0.25f)
	, mGamma(1.0f)
	, mInvGamma(1.0f/mGamma)
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
	, mViewTex2DArrayShader("CopyTex2DArrayShader")
	, mShowDeferredBuffersShader("ShowDeferredBuffersShader")
	, mSSAOShader("SSAOShader")
	, mSSAOBlurShader("SSAOBlurShader")
	, mPreCompMatrixShader("PreComputeMatrixShader")
	, mPointLightPositionRenderer(nullptr)
	, mSpotLightPositionRenderer(nullptr)
	, mDrawGBufferNormalGridSpan(20, 20)
	, mInitialized(false)
	, mIsDrawVertexNormalEnabled(false)
	, mIsDrawGBufferNormalEnabled(false)
	, mWireFrame(false)
	, mIsDrawLightPositionEnabled(false)
	, mDisplayTexture(nullptr)
{
	for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
	{
		mLights[i] = nullptr;
	}
	memset(mBufferIds, 0, sizeof(mBufferIds));
	memset(mGBuffers, 0, sizeof(mGBuffers));
	memset(mFBOs, 0, sizeof(mFBOs));
	memset(mSSAOBuffers, 0, sizeof(mSSAOBuffers));
}


Engine::~Engine()
{
	InternalRelease();
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
		glBindBuffer(GL_TEXTURE_BUFFER, 0);
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

	mDrawGBufferNormalShader.LoadShaders();
	InternalCreateFrameDataBuffer(mDrawGBufferNormalShader.GetProgram());

	// material and texture creation
	// -----------------------------------------------------------------------
	//InternalCreateTextures();
	//InternalCreateMaterialBuffer();
	// -----------------------------------------------------------------------
	
	InternalInitializeQuadVAO();
	InternalInitializeSSAOShader();
	InternalInitializeSSAOBlurShader();
	InternalInitializeDeferredPassShader();
	InternalInitializeToneMappingShader();
	InternalInitializeCopyShader();
	InternalInitializeViewTex2DArrayShader();
	InternalInitializeShowDeferredBuffersShader();
	InternalInitializePreComputeMatrixShader();
}

void Engine::SetViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight)
{
	if (mInitialized)
	{
		bool recreateGBuffers = mGBufferWidth != gBufferWidth || mGBufferHeight != gBufferHeight;

		InternalInitializeViewportParams(viewportX, viewportY, viewportWidth, viewportHeight, gBufferWidth, gBufferHeight);

		if (recreateGBuffers)
		{
			InternalGenerateBuffersAndFBOs();
		}
	}
}

void Engine::Initialize(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight)
{
	GetInstance()->InternalInitialize(viewportX, viewportY, viewportWidth, viewportHeight, gBufferWidth, gBufferHeight);
}

void Engine::Release()
{
	CoreFx::DebugOutput::GetInstance()->DisableDebugMessage();
	SAFE_DELETE(sInstance);
}

Engine* Engine::GetInstance()
{
	if (sInstance == nullptr)
	{
		memset(sIsUsedExtensionSupported, sizeof(sIsUsedExtensionSupported), 0);
		sIsUsedExtensionSupported[ARB_shader_draw_parameters] = glewIsSupported("GL_ARB_shader_draw_parameters") != 0;

		sInstance = new Engine();
		CoreFx::DebugOutput::GetInstance()->EnableDebugMessage();
	}

	assert(sInstance != nullptr);
	return sInstance;
}

void Engine::UpdateObjects()
{
	assert(mCamera != nullptr);
	assert(mCamera->GetFrame() != nullptr);
	if (mCamera->GetFrame()->IsModified())
	{
		mCamera->Update();
		if (mEnvmapGen.mEnabled)
		{
			glm::vec3 pos = mCamera->GetPosition();
			for (int i = 0; i < 6; ++i)
			{
				mEnvmapGen.mCamera[i].SetPosition(pos);
				mEnvmapGen.mCamera[i].Update();
			}
		}
	}

	//for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
	//{
	//	mLights[i]->ForEach([this](Lights::Light * light)
	//	{
	//		light->TransformInViewCoords(mCamera->GetViewMatrix());
	//		light->SetIsModified(false);
	//	});
	//}
}

void Engine::InternalUpdateFrameDataUniformBuffer(const Camera & camera)
{
	// Frame data buffer
	// -----------------------------------------------------------------------
	glBindBuffer(GL_UNIFORM_BUFFER, mBufferIds[FrameData_BufferId]);

	std::uint8_t * buffer = (std::uint8_t *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, mFrameDataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
	assert(buffer != nullptr);

	glm::vec4 eyePos(camera.GetFrame()->GetPosition(), 1.f);
	glm::vec4 depthRangeFovAspect(camera.GetNearZ(), camera.GetFarZ(), camera.GetFovY(), camera.GetAspect());
	//glm::vec4 bufferViewportSize(mGBufferWidth, mGBufferHeight, mViewportSize.x, mViewportSize.y);
	//glm::vec4 leftRightTopBottom(0, mViewportSize.x, 0, mViewportSize.y);

	memcpy(buffer + mFrameDataUniformOffsets[u_ProjMatrix], glm::value_ptr(camera.GetProjectionMatrix()), sizeof(glm::mat4));
	memcpy(buffer + mFrameDataUniformOffsets[u_InvProjMatrix], glm::value_ptr(camera.GetInverseProjectionMatrix()), sizeof(glm::mat4));
	memcpy(buffer + mFrameDataUniformOffsets[u_OrthoProjMatrix], glm::value_ptr(mOrthoProjMatrix), sizeof(glm::mat4));
	memcpy(buffer + mFrameDataUniformOffsets[u_ViewMatrix], glm::value_ptr(camera.GetViewMatrix()), sizeof(glm::mat4));
	memcpy(buffer + mFrameDataUniformOffsets[u_InvViewMatrix], glm::value_ptr(camera.GetInverseViewMatrix()), sizeof(glm::mat4));
	memcpy(buffer + mFrameDataUniformOffsets[u_ViewProjMatrix], glm::value_ptr(camera.GetViewProjectionMatrix()), sizeof(glm::mat4));
	memcpy(buffer + mFrameDataUniformOffsets[u_ViewDQ], &mCamera->GetViewDQ(), sizeof(Maths::DualQuat));
	memcpy(buffer + mFrameDataUniformOffsets[u_ViewPosition], glm::value_ptr(eyePos), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_AmbientLight], glm::value_ptr(mAmbientLight), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_WireFrameDrawColor], glm::value_ptr(mWireFrameColor), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_VertexNormalColor], glm::value_ptr(mDrawVertexNormalColor), sizeof(glm::vec4));
	//memcpy(buffer + mFrameDataUniformOffsets[u_BufferViewportSize], glm::value_ptr(bufferViewportSize), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_NearFarFovYAspect], glm::value_ptr(depthRangeFovAspect), sizeof(glm::vec4));
	//memcpy(buffer + mFrameDataUniformOffsets[u_LeftRightTopBottom], glm::value_ptr(leftRightTopBottom), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_TimeDeltaTime], mTimeDeltaTime, sizeof(GLfloat) * 2);
	memcpy(buffer + mFrameDataUniformOffsets[u_NormalMagnitude], &mDrawVertexNormalMagnitude, sizeof(GLfloat));
	memcpy(buffer + mFrameDataUniformOffsets[u_Exposure], &mExposure, sizeof(GLfloat));
	memcpy(buffer + mFrameDataUniformOffsets[u_InvGamma], &mInvGamma, sizeof(GLfloat));


	static const int lightUniformVarIndex[(int)Lights::Light::__light_type_count__] = { u_PointLightCount, u_SpotLightCount, u_DirectionalLightCount };
	for (int i = 0; i < (int)Lights::Light::__light_type_count__; ++i)
	{
		*((GLint*)(buffer + mFrameDataUniformOffsets[lightUniformVarIndex[i]])) = (GLint)mLights[i]->GetCount();
	}

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// -----------------------------------------------------------------------
}

void Engine::RenderObjects()
{

	if (mDisplayTexture == nullptr)
	{
		if (mFBOs[Deferred_FBO] == 0)
		{
			InternalGenerateBuffersAndFBOs();
		}

		if (mPrecompEnvMapTexture == nullptr)
		{
			if (mSkydome != nullptr && mSkydome->UseCache())
			{
				mEnvMapTexture1 = mSkydome->GetCacheTexture();
				mIsEnvMapHDR1 = GL_TRUE;
				mHasEnvMap1 = GL_TRUE;
			}
			else if (mSkybox != nullptr)
			{
				mEnvMapTexture1 = mSkybox->GetTexture();
				mIsEnvMapHDR1 = (GLboolean)mSkybox->IsHDR();
				mHasEnvMap1 = GL_TRUE;
			}
			else
			{
				mEnvMapTexture1 = nullptr;
				mIsEnvMapHDR1 = GL_FALSE;
				mHasEnvMap1 = GL_FALSE;
			}
		}
		else
		{
			mEnvMapTexture1 = mPrecompEnvMapTexture;
			mHasEnvMap1 = GL_TRUE;
			mIsEnvMapHDR1 = GL_TRUE;
		}

		if (mEnvmapGen.mEnabled)
		{
			mHasEnvMap2 = GL_TRUE;
			mEnvMapTexture2 = mEnvmapGen.mTexture;
			mIsEnvMapHDR2 = GL_TRUE;
		}
		else
		{
			mHasEnvMap2 = mHasEnvMap1;
			mEnvMapTexture2 = mEnvMapTexture1;
			mIsEnvMapHDR2 = mIsEnvMapHDR1;
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
		glBindBuffer(GL_TEXTURE_BUFFER, 0);
		// -----------------------------------------------------------------------

		InternalUpdateFrameDataUniformBuffer(*mCamera);

		InternalComputePass();

		if (mSkydome != nullptr)
		{
			if (mSkydome->GetIsInitialized() && mSkydome->IsCacheInvalidated())
			{
				if (mSkydome->RenderCache())
				{
					if (mEnvmapGen.mEnabled)
					{
						glViewport(0, 0, mEnvmapGen.mTextureSize.x, mEnvmapGen.mTextureSize.y);
					}
					else
					{
						glViewport(mViewportOrigin.x, mViewportOrigin.y, mViewportSize.x, mViewportSize.y);
					}
				}
			}
		}

		glDisable(GL_BLEND);

		mRenderers->ForEach([](Renderer * renderer)
		{
			renderer->Update();
		});

		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		//glFinish();

		if (mEnvmapGen.mEnabled)
		{
			glViewport(0, 0, mEnvmapGen.mTextureSize.x, mEnvmapGen.mTextureSize.y);

			glm::vec3 pos = mCamera->GetPosition();
			for (int face = 0; face < 6; ++face)
			{
				mEnvmapGen.mCamera[face].SetPosition(pos);
				mEnvmapGen.mCamera[face].Update();

				InternalUpdateFrameDataUniformBuffer(mEnvmapGen.mCamera[face]);
				InternalEnvMapRenderObjects(face);
			}

			InternalUpdateFrameDataUniformBuffer(*mCamera);
			glViewport(mViewportOrigin.x, mViewportOrigin.y, mViewportSize.x, mViewportSize.y);
		}


		InternalRenderObjects();
	}
	else
	{
		InternalDisplayTexture();
	}
}

void Engine::InternalComputePass()
{
	mComputes->ForEach([](ComputeShader * cs)
	{
		if (cs->GetIsInitialized())
			cs->Compute();
	});
}

void Engine::InternalEnvMapRenderObjects(int face)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// -----------------------------------------------------------------------
	// Geometry pass
	// -----------------------------------------------------------------------

	//static const GLuint uintZeros[] = { 0, 0, 0, 0 };
	//static const GLfloat floatZeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//static const GLfloat floatOnes[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mEnvmapGen.mFBOs[(int)EnvmapGenData::EFBOIndex::Deferred_FBO_EnvmapGen]);

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);
	glStencilMask(0x00);

	mRenderers->ForEach([](Renderer * renderer)
	{
		if (renderer->GetIsInitialized() && renderer->GetIsUsedToGenerateEnvMap())
		{
			renderer->Render();
		}
	});

	// -----------------------------------------------------------------------
	// light pass
	// -----------------------------------------------------------------------

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mEnvmapGen.mFBOs[(int)EnvmapGenData::EFBOIndex::HDR_FBO_EnvmapGen]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mEnvmapGen.mTexture->GetResourceId(), 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glClear(GL_COLOR_BUFFER_BIT);

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	mDeferredShader.Use();

	glUniform1i(mDeferredShader.GetUniform(u_IsSSAOEnabled), GL_FALSE);	
	glUniform1i(mDeferredShader.GetUniform(u_HasEnvMap), mHasEnvMap1);
	glUniform1i(mDeferredShader.GetUniform(u_IsEnvMapHDR), mIsEnvMapHDR1);

	glBindVertexArray(mQuad->GetVao());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, GetLightDescBuffer().GetTextureId());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, GetLightDataBuffer().GetTextureId());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mEnvmapGen.mGBuffers[(int)EGBuffer::DepthBuffer]);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mEnvmapGen.mGBuffers[(int)EGBuffer::NormalBuffer]);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, mEnvmapGen.mGBuffers[(int)EGBuffer::UI32Buffer1]);

	if (mIsEnvMapHDR1)
	{
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(mEnvMapTexture1->GetTarget(), mEnvMapTexture1->GetResourceId());

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(mEnvMapTexture1->GetTarget(), mEnvMapTexture1->GetResourceId());
	}

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	mDeferredShader.UnUse();

	// -----------------------------------------------------------------------
	// HDR skydome / skybox render
	// -----------------------------------------------------------------------
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	if (mSkybox != nullptr)
	{
		if (/*mSkybox->IsHDR() &&*/ mSkybox->GetIsInitialized())
		{
			mSkybox->Render();
		}
	}
	else if (mSkydome != nullptr)
	{
		if (mSkydome->GetIsInitialized())
		{
			mSkydome->Render();
		}
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}



void Engine::InternalRenderObjects()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// -----------------------------------------------------------------------
	// Geometry pass
	// -----------------------------------------------------------------------

	//static const GLuint uintZeros[] = { 0, 0, 0, 0 };
	//static const GLfloat floatZeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//static const GLfloat floatOnes[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBOs[Deferred_FBO]);

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);
	glStencilMask(0x00);

	mRenderers->ForEach([](Renderer * renderer)
	{
		if (renderer->GetIsInitialized())
		{
			renderer->Render();
		}			
	});

	// -----------------------------------------------------------------------
	// SSAO pass
	// -----------------------------------------------------------------------

	if (mIsSSAOEnabled)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBOs[SSAO_FBO]);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		mSSAOShader.Use();

		glUniform1f(mSSAOShader.GetUniform((int)ESSAOShaderUniformIndex::u_Radius), mSSAORadius);
		glUniform1i(mSSAOShader.GetUniform((int)ESSAOShaderUniformIndex::u_KernelSize), mSSAOKernelSize);
		glUniform2fv(mSSAOShader.GetUniform((int)ESSAOShaderUniformIndex::u_NoiseScale), 1, glm::value_ptr(mNoiseScale));

		glBindVertexArray(mQuad->GetVao());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mSSAOBuffers[SSAOBuffer_Noise]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_1D, mSSAOBuffers[SSAOBuffer_Kernel]);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mGBuffers[(int)EGBuffer::DepthBuffer]);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mGBuffers[(int)EGBuffer::NormalBuffer]);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, mGBuffers[(int)EGBuffer::UI32Buffer1]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		mSSAOShader.UnUse();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// -----------------------------------------------------------------------
		// SSAO blur
		// -----------------------------------------------------------------------

		mSSAOBlurShader.Use();

		glUniform2i(mSSAOBlurShader.GetUniform((int)ESSAOBlurShaderUniformIndex::u_TexOffset), 1, 0);

		glBindImageTexture(0, mSSAOBuffers[SSAOBuffer_Temp], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16F);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mSSAOBuffers[SSAOBuffer_Main]);

		glDispatchCompute(mGBufferWidth, mGBufferHeight, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glUniform2i(mSSAOBlurShader.GetUniform((int)ESSAOBlurShaderUniformIndex::u_TexOffset), 0, 1);

		glBindImageTexture(0, mSSAOBuffers[SSAOBuffer_Main], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16F);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mSSAOBuffers[SSAOBuffer_Temp]);

		glDispatchCompute(mGBufferWidth, mGBufferHeight, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		mSSAOBlurShader.UnUse();
	}
	// -----------------------------------------------------------------------
	// 
	// -----------------------------------------------------------------------

	if (mDeferredDebugState == 0)
	{
		// -----------------------------------------------------------------------
		// light pass
		// -----------------------------------------------------------------------

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBOs[HDR_FBO]);

		glClear(GL_COLOR_BUFFER_BIT);

		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		mDeferredShader.Use();

		glUniform1i(mDeferredShader.GetUniform(u_IsSSAOEnabled), mIsSSAOEnabled);
		glUniform1i(mDeferredShader.GetUniform(u_HasEnvMap), mHasEnvMap2);
		glUniform1i(mDeferredShader.GetUniform(u_IsEnvMapHDR), mIsEnvMapHDR2);

		glBindVertexArray(mQuad->GetVao());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_BUFFER, GetLightDescBuffer().GetTextureId());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_BUFFER, GetLightDataBuffer().GetTextureId());

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mGBuffers[(int)EGBuffer::DepthBuffer]);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, mGBuffers[(int)EGBuffer::NormalBuffer]);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, mGBuffers[(int)EGBuffer::UI32Buffer1]);

		if (mHasEnvMap2)
		{
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(mEnvMapTexture1->GetTarget(), mEnvMapTexture1->GetResourceId());

			glActiveTexture(GL_TEXTURE6);
			glBindTexture(mEnvMapTexture2->GetTarget(), mEnvMapTexture2->GetResourceId());
		}

		if (mIsSSAOEnabled)
		{
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, mSSAOBuffers[SSAOBuffer_Main]);
		}

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		mDeferredShader.UnUse();

		// -----------------------------------------------------------------------
		// HDR skydome / skybox render
		// -----------------------------------------------------------------------
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		if (mSkybox != nullptr)
		{
			if (mSkybox->IsHDR() && mSkybox->GetIsInitialized())
			{
				mSkybox->Render();
			}
		}
		else if (mSkydome != nullptr)
		{
			if (mSkydome->GetIsInitialized())
			{
				mSkydome->Render();
			}
		}

		// -----------------------------------------------------------------------
		// tone mapping pass
		// -----------------------------------------------------------------------
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBOs[Forward_FBO]);
		//glClear(GL_COLOR_BUFFER_BIT);
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		mToneMappingShader.Use();

		glBindVertexArray(mQuad->GetVao());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mHdrBuffer);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		mToneMappingShader.UnUse();

		// -----------------------------------------------------------------------
		// forward pass
		// -----------------------------------------------------------------------

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_TRUE);

		//glDisable(GL_CULL_FACE);
		mForwardRenderers->ForEach([](Renderer * renderer)
		{
			if (renderer->GetIsInitialized())
				renderer->Render();
		});
		//glEnable(GL_CULL_FACE);

		if (mIsDrawLightPositionEnabled)
		{
			if (mPointLightPositionRenderer->GetIsInitialized())
				mPointLightPositionRenderer->Render();
			if (mSpotLightPositionRenderer->GetIsInitialized())
				mSpotLightPositionRenderer->Render();
		}

		if (mSkybox != nullptr)
		{
			if (!mSkybox->IsHDR() && mSkybox->GetIsInitialized())
			{
				mSkybox->Render();
			}
		}

		// -----------------------------------------------------------------------
		// wire frame pass
		// -----------------------------------------------------------------------

		if (GetWireFrame())
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_LINE_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

			glDepthMask(GL_FALSE);
			glDepthFunc(GL_LEQUAL);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glPolygonOffset(1.0f, 1.0f);

			mRenderers->ForEach([](Renderer * renderer)
			{
				if (renderer->GetIsInitialized())
				{
					renderer->RenderWireFrame();
				}
			});
			mForwardRenderers->ForEach([](Renderer * renderer)
			{
				if (renderer->GetIsInitialized())
					renderer->RenderWireFrame();
			});

			if (mSkybox != nullptr && mSkybox->GetIsInitialized())
			{
				mSkybox->RenderWireFrame();
			}
			else if (mSkydome != nullptr && mSkydome->GetIsInitialized())
			{
				mSkydome->RenderWireFrame();
			}


			if (mIsDrawLightPositionEnabled)
			{
				if (mPointLightPositionRenderer->GetIsInitialized())
				{
					mPointLightPositionRenderer->RenderWireFrame();
				}
				if (mSpotLightPositionRenderer->GetIsInitialized())
				{
					mSpotLightPositionRenderer->RenderWireFrame();
				}
			}

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glPolygonOffset(0.0f, 0.0f);
			glDisable(GL_LINE_SMOOTH);
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

			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_PositionBuffer]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mGBuffers[(int)EGBuffer::DepthBuffer]);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mGBuffers[(int)EGBuffer::NormalBuffer]);

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
	else
	{
		InternalRenderDeferredBuffers();
	}

	// -----------------------------------------------------------------------
	// draw HUD 
	// -----------------------------------------------------------------------

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glDisable(GL_CULL_FACE);
	mHUDRenderers->ForEach([](Renderer * renderer)
	{
		if (renderer->GetIsInitialized())
			renderer->Render();
	});

	if (GetWireFrame())
	{
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		mHUDRenderers->ForEach([](Renderer * renderer)
		{
			if (renderer->GetIsInitialized())
				renderer->RenderWireFrame();
		});

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_LINE_SMOOTH);
	}

	//glEnable(GL_CULL_FACE);
}

void Engine::InternalRenderDeferredBuffers()
{
	switch (mDeferredDebugState)
	{
	case (int)EDeferredDebug::ShowNormalBuffer:
	case (int)EDeferredDebug::ShowAlbedoBuffer:
	case (int)EDeferredDebug::ShowMetallicBuffer:
	case (int)EDeferredDebug::ShowRoughnessBuffer:
	case (int)EDeferredDebug::ShowDepthBuffer:
	case (int)EDeferredDebug::ShowSSAOBuffer:
	default:
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		break;

	case (int)EDeferredDebug::ShowPositionBuffer:
	case (int)EDeferredDebug::ShowEmissiveBuffer:
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFBOs[HDR_FBO]);
		break;
	}

	glClear(GL_COLOR_BUFFER_BIT);

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	mShowDeferredBuffersShader.Use();
	glBindVertexArray(mQuad->GetVao());

	glUniform1i(mShowDeferredBuffersShader.GetUniform((int)EShowDeferredShaderUniformIndex::u_BufferToShow), mDeferredDebugState);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mGBuffers[(int)EGBuffer::DepthBuffer]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mGBuffers[(int)EGBuffer::NormalBuffer]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mGBuffers[(int)EGBuffer::UI32Buffer1]);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mSSAOBuffers[SSAOBuffer_Main]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	mShowDeferredBuffersShader.UnUse();


	switch (mDeferredDebugState)
	{
	case (int)EDeferredDebug::ShowNormalBuffer:
	case (int)EDeferredDebug::ShowAlbedoBuffer:
	case (int)EDeferredDebug::ShowMetallicBuffer:
	case (int)EDeferredDebug::ShowRoughnessBuffer:
	case (int)EDeferredDebug::ShowDepthBuffer:
	case (int)EDeferredDebug::ShowSSAOBuffer:
	default:
		break;

	case (int)EDeferredDebug::ShowPositionBuffer:
	case (int)EDeferredDebug::ShowEmissiveBuffer:
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		mToneMappingShader.Use();

		glBindVertexArray(mQuad->GetVao());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mHdrBuffer);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		mToneMappingShader.UnUse();
		break;
	}
}

void Engine::InternalDisplayTexture()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	mViewTex2DArrayShader.Use();
	glBindVertexArray(mQuad->GetVao());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(mDisplayTexture->GetTarget(), mDisplayTexture->GetResourceId());

	glUniform1i(mViewTex2DArrayShader.GetUniform(u_LayerIndex), mDisplayTextureLayerIndex);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	mViewTex2DArrayShader.UnUse();
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

bool Engine::AttachComputeShader(ComputeShader* cs)
{
	assert(cs != nullptr);
	if (cs == nullptr)
	{
		PRINT_ERROR("Cannot attach compute shader : parameter null!");
		return false;
	}
	return mComputes->Attach(cs);
}

bool Engine::DetachComputeShader(ComputeShader* cs)
{
	assert(cs != nullptr);
	if (cs == nullptr)
	{
		PRINT_ERROR("Cannot detach compute shader : parameter null!");
		return false;
	}
	return mComputes->Detach(cs);
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

	case Renderer::HUD_Pass:
		return mHUDRenderers->Attach(renderer);
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

	case Renderer::HUD_Pass:
		return mHUDRenderers->Detach(renderer);
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

void Engine::DisplayTexture2DArray(const Texture * texture, GLint layerIndex)
{
	mDisplayTexture = texture;
	mDisplayTextureLayerIndex = layerIndex;
}

void Engine::UndisplayTexture2DArray()
{
	mDisplayTexture = nullptr;
}




// =======================================================================
// =======================================================================
// =======================================================================






Frame * Engine::CreateObjectLocation(SceneObjectType sceneObjectType, size_t defaultContainerCapacity, size_t defaultContainerPageSize)
{
	assert(mFrameContainers != nullptr);
	assert(sceneObjectType < mFrameContainers->size());
	FrameContainer *& container = (*mFrameContainers)[sceneObjectType];
	if (container == nullptr)
	{
		container = new FrameContainer(defaultContainerCapacity, defaultContainerPageSize);
	}
	Frame * frame = new Frame(sceneObjectType);
	container->Attach(frame);
	return frame;
}

void Engine::DeleteObjectLocation(Frame *& frame)
{
	if (frame == nullptr)
		return;

	SceneObjectType sceneObjectType = frame->GetSceneObjectType();
	assert(sceneObjectType < mFrameContainers->size());

	FrameContainer * container = (*mFrameContainers)[sceneObjectType];
	container->Detach(frame);
	SAFE_DELETE(frame);
}





// =======================================================================
// =======================================================================
// =======================================================================



void Engine::ComputeViewTransform(GLuint shaderStorageSourceId, GLuint shaderStorageTargetId, GLuint itemCount, GLuint sourceStride, GLuint targetStride)
{
	mPreCompMatrixShader.Use();

	glUniform1ui(mPreCompMatrixShader.GetUniform((int)EPreComputeMatrixShaderUniformIndex::u_SourceStride), sourceStride);
	glUniform1ui(mPreCompMatrixShader.GetUniform((int)EPreComputeMatrixShaderUniformIndex::u_TargetStride), targetStride);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (GLuint)EPreComputeMatrixShaderBindingIndex::u_SourceBuffer, shaderStorageSourceId);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (GLuint)EPreComputeMatrixShaderBindingIndex::u_TargetBuffer, shaderStorageTargetId);
	//glBindImageTexture((GLuint)EPreComputeMatrixShaderBindingIndex::u_TargetBuffer, shaderStorageTargetId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glDispatchCompute(itemCount, 1, 1);

	//glMemoryBarrier(GL_ALL_BARRIER_BITS);

	mPreCompMatrixShader.UnUse();
}


// =======================================================================
// =======================================================================

CubeMapTexture const * Engine::GetSkyCubeMap() const
{
	if (mSkydome != nullptr && mSkydome->UseCache())
	{
		return mSkydome->GetCacheTexture();
	}
	else if(mSkybox != nullptr)
	{
		return mSkybox->GetTexture();
	}
	else
	{
		return nullptr;
	}
}





// =======================================================================
// =======================================================================
// =======================================================================

	// =======================================================================
	// =======================================================================
} // namespace CoreFx
