#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{


	Renderer::Renderer(GLuint propertyCount, ERenderPass renderPass)
		: mMaterials(propertyCount)
		, mMaterialBaseIndex(0)
		, mRenderPass(renderPass)
{
}


Renderer::~Renderer()
{
}

bool Renderer::AddTexture(const char * filename, TextureCategory category, TextureWrap wrapS, TextureWrap wrapT)
{
	assert(filename != nullptr);

	uint32_t width = 128;
	uint32_t height = 128;

	TextureManager::GetTiffImageSize(filename, width, height);

	uint16_t rendererId = (uint16_t)GetInstanceId();

	TextureInfo textureInfo(filename, rendererId, category, (GLsizei)width, (GLsizei)height, wrapS, wrapT);

	mTextures.push_back(textureInfo);

	return true;
}

void Renderer::AddTextures(const TextureDescList & textures)
{
	for (TextureDescList::const_iterator it = textures.begin(); it != textures.end(); ++it)
	{
		AddTexture(*it);
	}
}

void Renderer::BuildTextureMapping(TextureMapping * lightPassTextureMapping) 
{
	const TextureInfoList & texInfoList = this->GetTextureInfoList();
	for (TextureInfoList::const_iterator texInfoListIter = texInfoList.begin(); texInfoListIter != texInfoList.end(); ++texInfoListIter)
	{
		TextureMapping * texMap;

		const TextureInfo & texInfo = *texInfoListIter;
		if (texInfo.GetRendererId() == 0 && lightPassTextureMapping != nullptr)
		{
			texMap = lightPassTextureMapping;
		}
		else
		{
			texMap = &mTextureMapping;
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

}

void Renderer::LoadTextures()
{
	TextureManager * textureManager = Engine::GetInstance()->GetTextureManager();

	for (TextureMappingList::iterator it = mTextureMapping.mMapping.begin(); it != mTextureMapping.mMapping.end(); ++it)
	{
		std::vector<std::string> textureList;
		textureList.reserve(it->mTexInfoList.size());
		for (TexInfoPtrList::const_iterator it2 = it->mTexInfoList.begin(); it2 != it->mTexInfoList.end(); ++it2)
		{
			textureList.push_back((*it2)->GetFilename());
		}

		it->mTexture = textureManager->LoadTextureGroup(it->mTexInfoList.front()->GetGroupId(), textureList);
	}
}

} // namespace CoreFx
