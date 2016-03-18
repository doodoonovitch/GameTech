#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{


Texture2D::Texture2D(GLuint id)
	: mId(id)
{
}


Texture2D::~Texture2D()
{
}


TextureGroup::TextureGroup(GLuint textureId, TextureGroupId groupId, GLint layerCount)
	: Texture2D(textureId)
	, mGroupId(groupId)
	, mLayerCount(layerCount)
{

}

TextureGroup::~TextureGroup()
{

}

} // namespace CoreFx