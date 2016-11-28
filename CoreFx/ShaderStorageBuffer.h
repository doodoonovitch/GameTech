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
	void CreateResource(GLenum usage, GLsizeiptr itemCount, GLsizei itemSize, const void * data = nullptr, GLsizei stride = 0);

	void ReleaseResource();

	void Resize(GLenum usage, GLsizeiptr size, const void * data = nullptr);

	void BindBufferBase(GLuint bindingPointIndex)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPointIndex, GetBufferId());
	}

protected:

	GLuint mBufferId;
	GLsizeiptr mSize;
	
private:

};


} // namespace CoreFx
#endif // COREFX_SHADERSTORAGEBUFFER_H