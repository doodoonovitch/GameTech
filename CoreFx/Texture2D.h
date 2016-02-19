#pragma once
#ifndef COREFX_TEXTURE2D_H
#define COREFX_TEXTURE2D_H


namespace CoreFx
{

	class TextureManager;

class Texture2D
{
public:

	inline GLuint GetId() const
	{
		return mId;
	}

private:

	Texture2D(GLuint id);
	~Texture2D();

	void SetId(GLuint id)
	{
		mId = id;
	}

private:

	GLuint mId;

	friend class TextureManager;
};


} // namespace CoreFx
#endif // COREFX_TEXTURE2D_H