#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{


Texture::Texture(GLuint id, TextureClass texClass)
	: mId(id)
	, mTextureClass(texClass)
{
}


Texture::~Texture()
{
}


Texture2D::Texture2D(GLuint id)
	: Texture(id, TextureClass::Texture2D)
{
}


Texture2D::~Texture2D()
{
}


CubeMapTexture::CubeMapTexture(GLuint textureId)
	: Texture(textureId, TextureClass::CubeMapTexture)
{
}


CubeMapTexture::~CubeMapTexture()
{
}


TextureGroup::TextureGroup(GLuint textureId, TextureGroupId groupId, GLint layerCount)
	: Texture(textureId, TextureClass::TextureGroup)
	, mGroupId(groupId)
	, mLayerCount(layerCount)
{

}

TextureGroup::~TextureGroup()
{

}

} // namespace CoreFx