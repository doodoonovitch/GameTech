#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{


TextureBuffer::TextureBuffer()
	: mTextureId(0)
	, mBufferId(0)
{
}


TextureBuffer::~TextureBuffer()
{
	ReleaseResource();
}


void TextureBuffer::CreateResource(GLenum usage, GLenum internalformat, GLsizeiptr size, const void * data)
{
	assert(mBufferId == 0 && mTextureId == 0);

	glGenBuffers(1, &mBufferId);
	glBindBuffer(GL_TEXTURE_BUFFER, mBufferId);
	glBufferData(GL_TEXTURE_BUFFER, size, data, usage);
	GL_CHECK_ERRORS;

	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_BUFFER, mTextureId);
	glTexBuffer(GL_TEXTURE_BUFFER, internalformat, mBufferId);
	GL_CHECK_ERRORS;
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

void TextureBuffer::ReleaseResource()
{
	glDeleteTextures(1, &mTextureId);
	mTextureId = 0;
	glDeleteBuffers(1, &mBufferId);
	mBufferId = 0;
}

void TextureBuffer::Resize(GLenum usage, GLsizeiptr size, const void * data)
{
	glBindBuffer(GL_TEXTURE_BUFFER, mBufferId);
	glBufferData(GL_TEXTURE_BUFFER, size, data, usage);
	GL_CHECK_ERRORS;
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

} // namespace CoreFx