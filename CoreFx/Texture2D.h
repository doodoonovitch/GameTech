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
		return m_id;
	}

private:

	Texture2D(GLuint id);
	~Texture2D();

	void SetId(GLuint id)
	{
		m_id = id;
	}

private:

	GLuint m_id;

	friend class TextureManager;
};


} // namespace CoreFx
#endif // COREFX_TEXTURE2D_H