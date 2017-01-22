#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{


Renderer::Renderer(ERenderPass renderPass)
	: mRenderPass(renderPass)
	, mIsInitialized(false)
	, mIsEnabled(true)
	, mUseToGenEnvMap(false)
{
}

Renderer::~Renderer()
{
}

bool Renderer::AddTexture(const char * filename, TextureCategory category, TextureWrap wrapS, TextureWrap wrapT, bool invertY)
{
	assert(filename != nullptr);

	uint32_t width = 128;
	uint32_t height = 128;

	TextureManager::GetTiffImageSize(filename, width, height);

	uint16_t rendererId = (uint16_t)GetInstanceId();

	TextureInfo textureInfo(filename, rendererId, category, (GLsizei)width, (GLsizei)height, wrapS, wrapT, invertY);

	mTextureInfoList.push_back(textureInfo);

	return true;
}

void Renderer::AddTextures(const TextureDescList & textures)
{
	for (TextureDescList::const_iterator it = textures.begin(); it != textures.end(); ++it)
	{
		AddTexture(*it);
	}
}

void Renderer::BuildTextureMapping()
{
	GenericBuildTextureMapping(&mTextureMapping, mTextureInfoList);
}

void Renderer::GenericBuildTextureMapping(TextureMapping * texMap, const TextureInfoList & texInfoList)
{
	//const TextureInfoList & texInfoList = this->GetTextureInfoList();
	for (TextureInfoList::const_iterator texInfoListIter = texInfoList.begin(); texInfoListIter != texInfoList.end(); ++texInfoListIter)
	{
		const TextureInfo & texInfo = *texInfoListIter;
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

}

void Renderer::LoadTextures()
{
	TextureManager * textureManager = Engine::GetInstance()->GetTextureManager();

	for (TextureMappingList::iterator it = mTextureMapping.mMapping.begin(); it != mTextureMapping.mMapping.end(); ++it)
	{
		std::vector<TextureManager::LoadTextureGroupDesc> textureList;
		textureList.reserve(it->mTexInfoList.size());
		for (TexInfoPtrList::const_iterator it2 = it->mTexInfoList.begin(); it2 != it->mTexInfoList.end(); ++it2)
		{
			textureList.push_back(TextureManager::LoadTextureGroupDesc((*it2)->GetFilename(), (*it2)->mInvertY));
		}

		it->mTexture = textureManager->LoadTextureGroup(it->mTexInfoList.front()->GetGroupId(), textureList);
	}
}

} // namespace CoreFx
