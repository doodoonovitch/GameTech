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

		SAFE_DELETE(mComputes);
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

		InternalReleaseFrameConstainers();

		mInitialized = false;
		mIsDrawVertexNormalEnabled = false;
	}
}

void Engine::InternalReleaseGBuffers()
{
	//glDeleteRenderbuffers(1, &mDepthRBO);
	//mDepthRBO = 0;
	glDeleteTextures(__gBuffer_count__, mGBuffers);
	memset(mGBuffers, 0, sizeof(mGBuffers));
	glDeleteFramebuffers(1, &mDeferredFBO);
	mDeferredFBO = 0;
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



	// =======================================================================
	// =======================================================================
} // namespace CoreFx
