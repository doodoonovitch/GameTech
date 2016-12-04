#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	// =======================================================================
	// =======================================================================


void Engine::InternalReleaseFrameConstainers()
{
	if (mFrameContainers != nullptr)
	{
		for (FrameContainerContainer::iterator it = mFrameContainers->begin(); it != mFrameContainers->end(); ++it)
		{
			FrameContainer *& container = *it;
			SAFE_DELETE(container);
		}

		SAFE_DELETE(mFrameContainers);
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

		mComputes->DeleteAll();
		mRenderers->DeleteAll();
		mForwardRenderers->DeleteAll();
		mHUDRenderers->DeleteAll();

		SAFE_DELETE(mComputes);
		SAFE_DELETE(mRenderers);
		SAFE_DELETE(mForwardRenderers);
		SAFE_DELETE(mHUDRenderers);

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

		SAFE_DELETE(mQuad);

		InternalReleaseBuffersAndFBOs();

		InternalReleaseFrameConstainers();

		mInitialized = false;
		mIsDrawVertexNormalEnabled = false;
	}
}

void Engine::InternalReleaseBuffersAndFBOs()
{
	InternalReleaseSSAOBuffers();
	InternalReleaseHdrBuffers();
	InternalReleaseGBuffers();

	glDeleteFramebuffers(__fbo_count__, mFBOs);
	memset(mFBOs, 0, sizeof(mFBOs));
}

void Engine::InternalReleaseGBuffers()
{
	//glDeleteRenderbuffers(1, &mDepthRBO);
	//mDepthRBO = 0;
	glDeleteTextures((int)EGBuffer::__gBuffer_count__, mGBuffers);
	memset(mGBuffers, 0, sizeof(mGBuffers));
}

void Engine::InternalReleaseHdrBuffers()
{
	glDeleteTextures(1, &mHdrBuffer);
	mHdrBuffer = 0;
	glDeleteTextures(1, &mForwardBuffer);
	mForwardBuffer = 0;
}

void Engine::InternalReleaseSSAOBuffers()
{
	glDeleteTextures(ARRAY_SIZE_IN_ELEMENTS(mSSAOBuffers), mSSAOBuffers);
	memset(mSSAOBuffers, 0, sizeof(mSSAOBuffers));
}


	// =======================================================================
	// =======================================================================
} // namespace CoreFx
