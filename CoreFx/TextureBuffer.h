#pragma once
#ifndef COREFX_TEXTUREBUFFER_H
#define COREFX_TEXTUREBUFFER_H


namespace CoreFx
{


class TextureBuffer
{
public:

	TextureBuffer();
	~TextureBuffer();

	inline GLuint GetTextureId() const
	{
		return mTextureId;
	}

	inline GLuint GetBufferId() const
	{
		return mBufferId;
	}

	void CreateResource(GLenum usage, GLenum internalformat, GLsizei size, const void * data = nullptr);

	void ReleaseResource();

protected:

	GLuint mTextureId;
	GLuint mBufferId;


private:

};


} // namespace CoreFx
#endif // COREFX_TEXTUREBUFFER_H