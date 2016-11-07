#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{


ShaderStorageBuffer::ShaderStorageBuffer()
	: mBufferId(0)
	, mSize(0)
{
}


ShaderStorageBuffer::~ShaderStorageBuffer()
{
	ReleaseResource();
}


void ShaderStorageBuffer::CreateResource(GLenum usage, GLsizeiptr size, const void * data)
{
	assert(mBufferId == 0);

	mSize = size;

	glGenBuffers(1, &mBufferId);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBufferId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mSize, data, usage);
	GL_CHECK_ERRORS;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ShaderStorageBuffer::ReleaseResource()
{
	glDeleteBuffers(1, &mBufferId);
	mBufferId = 0;
}

void ShaderStorageBuffer::Resize(GLenum usage, GLsizeiptr size, const void * data)
{
	if (mSize == size)
		return;

	mSize = size;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBufferId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mSize, data, usage);
	GL_CHECK_ERRORS;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

} // namespace CoreFx