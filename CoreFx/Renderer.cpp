#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{


Renderer::Renderer(GLuint propertyCount)
	: mMaterials(propertyCount)
	, mMaterialBaseIndex(0)
{
}


Renderer::~Renderer()
{
}

bool Renderer::AddTexture(const char * filename, uint16_t rendererId, TextureCategory category, TextureWrap wrapS, TextureWrap wrapT)
{
	assert(filename != nullptr);

	TextureManager::KTX_header hd;
	if (!TextureManager::KTX_ReadHeader(filename, hd))
	{
		printf("Error: Cannot load texture header for '%s'.\n", filename);
		return false;
	}

#ifdef FORWARD_RENDERING
#else
	rendererId = 0;
#endif
	TextureInfo textureInfo(filename, rendererId, category, (GLsizei)hd.pixelWidth, (GLsizei)hd.pixelHeight, wrapS, wrapT);

	mTextures.push_back(textureInfo);

	return true;
}

} // namespace CoreFx
