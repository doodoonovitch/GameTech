#pragma once
#ifndef COREFX_SHADERSTORAGEBUFFER_H
#define COREFX_SHADERSTORAGEBUFFER_H


namespace CoreFx
{


class ShaderStorageBuffer
{
public:

	ShaderStorageBuffer();
	~ShaderStorageBuffer();

	inline bool IsCreated() const
	{
		return mBufferId != 0;
	}

	inline GLuint GetBufferId() const
	{
		return mBufferId;
	}

	inline GLsizeiptr GetSize() const
	{
		return mSize;
	}

	void CreateResource(GLenum usage, GLsizeiptr size, const void * data = nullptr);

	void ReleaseResource();

	void Resize(GLenum usage, GLsizeiptr size, const void * data = nullptr);

protected:

	GLuint mBufferId;
	GLsizeiptr mSize;
	
private:

};


} // namespace CoreFx
#endif // COREFX_SHADERSTORAGEBUFFER_H