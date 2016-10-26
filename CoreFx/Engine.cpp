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
	, mComputes(nullptr)
	, mFrameContainers(nullptr)
	, mCamera(nullptr)
	, mSkybox(nullptr)
	, mQuad(nullptr)
	, mSkydome(nullptr)
	, mDeferredFBO(0)
	//, mDepthRBO(0)
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
	, mSSAOShader("SSAOComputeShader")
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
	InternalCreateMaterialBuffer();
	// -----------------------------------------------------------------------
	
	InternalInitializeQuadVAO();
	InternalInitializeDeferredPassShader();
	InternalInitializeToneMappingShader();
	InternalInitializeCopyShader();
	InternalInitializeViewTex2DArrayShader();
	InternalInitializeShowDeferredBuffersShader();
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
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
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
	glm::vec4 leftRightTopBottom(0, mViewportWidth, 0, mViewportHeight);

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
	memcpy(buffer + mFrameDataUniformOffsets[u_NearFarFovYAspect], glm::value_ptr(depthRangeFovAspect), sizeof(glm::vec4));
	memcpy(buffer + mFrameDataUniformOffsets[u_LeftRightTopBottom], glm::value_ptr(leftRightTopBottom), sizeof(glm::vec4));
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

	if (mDisplayTexture == nullptr)
	{
		InternalComputePass();
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

void Engine::InternalRenderObjects()
{
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
		if (renderer->GetIsInitialized())
		{
			renderer->Render();
		}			
	});

	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	if (mSkybox != nullptr)
	{
		if (mSkybox->GetIsInitialized())
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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	if (mDeferredDebugState == 0)
	{
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

		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_PositionBuffer]);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_DepthBuffer]);

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
					renderer->RenderWireFrame();
			});

			if (mSkybox != nullptr && mSkybox->GetIsInitialized())
				mSkybox->RenderWireFrame();
			else if (mSkydome != nullptr && mSkydome->GetIsInitialized())
				mSkydome->RenderWireFrame();

			if (mIsDrawLightPositionEnabled)
			{
				if (mPointLightPositionRenderer->GetIsInitialized())
					mPointLightPositionRenderer->RenderWireFrame();
				if (mSpotLightPositionRenderer->GetIsInitialized())
					mSpotLightPositionRenderer->RenderWireFrame();
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
			glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_DepthBuffer]);

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
	else
	{
		InternalRenderDeferredBuffers();
	}
}

void Engine::InternalRenderDeferredBuffers()
{
	switch (mDeferredDebugState)
	{
	case (int)EDeferredDebug::ShowNormalBuffer:
	case (int)EDeferredDebug::ShowAlbedoBuffer:
	case (int)EDeferredDebug::ShowSpecularBuffer:
	case (int)EDeferredDebug::ShowRoughnessBuffer:
	default:
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		break;

	case (int)EDeferredDebug::ShowPositionBuffer:
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mHdrFBO);
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
	glBindTexture(GL_TEXTURE_BUFFER, GetLightDescBuffer().GetTextureId());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, GetLightDataBuffer().GetTextureId());

	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_PositionBuffer]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mGBuffers[gBuffer_DepthBuffer]);

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
	mShowDeferredBuffersShader.UnUse();


	switch (mDeferredDebugState)
	{
	case (int)EDeferredDebug::ShowNormalBuffer:
	case (int)EDeferredDebug::ShowAlbedoBuffer:
	case (int)EDeferredDebug::ShowSpecularBuffer:
	case (int)EDeferredDebug::ShowRoughnessBuffer:
	default:
		break;

	case (int)EDeferredDebug::ShowPositionBuffer:
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






// =======================================================================
// =======================================================================






// =======================================================================
// =======================================================================
// =======================================================================

	// =======================================================================
	// =======================================================================
} // namespace CoreFx
