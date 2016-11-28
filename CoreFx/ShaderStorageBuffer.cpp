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

void ShaderStorageBuffer::CreateResource(GLenum usage, GLsizeiptr itemCount, GLsizei itemSize, const void * data, GLsizei stride)
{
	assert(mBufferId == 0);

	mSize = itemCount * itemSize;

	bool autoSetBufferData = (data == nullptr) || (stride == 0) || (stride == itemSize);

	glGenBuffers(1, &mBufferId);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBufferId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mSize, autoSetBufferData ? data : nullptr, usage);
	GL_CHECK_ERRORS;

	if (!autoSetBufferData)
	{
		char * trg = (char *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		char * src = (char *)data;
		char * endPtr = trg + mSize;
		while (trg < endPtr)
		{
			memcpy(trg, src, itemSize);
			trg += itemSize;
			src += stride;
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

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