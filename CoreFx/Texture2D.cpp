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


Texture::Texture(GLsizei width, GLsizei height, GLsizei depth, GLuint id, GLenum target, TextureClass texClass)
	: mId(id)
	, mSize(width, height, depth)
	, mTextureClass(texClass)
	, mTarget(target)
{
}


Texture::~Texture()
{
}


Texture1D::Texture1D(GLsizei width, GLuint id, GLenum target)
	: Texture(width, 1, 1, id, target, TextureClass::Texture1D)
{
}


Texture1D::~Texture1D()
{
}


Texture2D::Texture2D(GLsizei width, GLsizei height, GLuint id, GLenum target)
	: Texture(width, height, 1, id, target, TextureClass::Texture2D)
{
}


Texture2D::~Texture2D()
{
}


CubeMapTexture::CubeMapTexture(GLsizei width, GLsizei height, GLuint textureId, GLenum target)
	: Texture(width, height, 1, textureId, target, TextureClass::CubeMapTexture)
{
}


CubeMapTexture::~CubeMapTexture()
{
}


TextureGroup::TextureGroup(GLuint textureId, TextureGroupId groupId, GLenum target, GLint layerCount)
	: Texture(TextureInfo::GetWidthFromGroupId(groupId), TextureInfo::GetHeightFromGroupId(groupId), layerCount, textureId, target, TextureClass::TextureGroup)
	, mGroupId(groupId)
	, mLayerCount(layerCount)
{

}

TextureGroup::~TextureGroup()
{

}


Texture2DArray::Texture2DArray(GLsizei width, GLsizei height, GLsizei layerCount, GLuint textureId, GLenum target)
	: Texture(width, height, layerCount, textureId, target, TextureClass::Texture2DArray)
{
}


Texture2DArray::~Texture2DArray()
{
}

} // namespace CoreFx