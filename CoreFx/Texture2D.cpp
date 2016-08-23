#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{

const char * TextureCategoryToString(TextureCategory cat)
{
	const char * textureCategoryStr[(int)TextureCategory::__count__] =
	{
		"Diffuse",
		"Specular",
		"NormalMap",
		"HeightMap",
		"Emissive",
		"Roughness"
	};

	return textureCategoryStr[(int)cat];
}


Texture::Texture(GLuint id, GLenum target, TextureClass texClass)
	: mId(id)
	, mTextureClass(texClass)
	, mTarget(target)
{
}


Texture::~Texture()
{
}


Texture2D::Texture2D(GLuint id, GLenum target)
	: Texture(id, target, TextureClass::Texture2D)
{
}


Texture2D::~Texture2D()
{
}


CubeMapTexture::CubeMapTexture(GLuint textureId, GLenum target)
	: Texture(textureId, target, TextureClass::CubeMapTexture)
{
}


CubeMapTexture::~CubeMapTexture()
{
}


TextureGroup::TextureGroup(GLuint textureId, TextureGroupId groupId, GLenum target, GLint layerCount)
	: Texture(textureId, target, TextureClass::TextureGroup)
	, mGroupId(groupId)
	, mLayerCount(layerCount)
{

}

TextureGroup::~TextureGroup()
{

}


Texture2DArray::Texture2DArray(GLuint textureId, GLenum target)
	: Texture(textureId, target, TextureClass::Texture2DArray)
{
}


Texture2DArray::~Texture2DArray()
{
}

} // namespace CoreFx