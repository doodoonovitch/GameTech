#pragma once
#ifndef COREFX_TEXTURE2D_H
#define COREFX_TEXTURE2D_H


namespace CoreFx
{

	class TextureManager;
	class Engine;

class Texture2D
{
public:

	inline GLuint GetId() const
	{
		return mId;
	}

protected:

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


typedef uint32_t TextureGroupId;

enum class TextureWrap
{
	Clamp,					// GL_CLAMP
	Repeat,					// GL_REPEAT
	ClampToEdge,			// GL_CLAMP_TO_EDGE
	ClampToBorder,			// GL_CLAMP_TO_BORDER
	MirrorRepeat,			// GL_MIRRORED_REPEAT 
	MirrorClampToEdge,		// GL_MIRROR_CLAMP_TO_EDGE
};


class TextureGroup : public Texture2D
{
	
protected:

	TextureGroup(GLuint textureId, TextureGroupId groupId, GLint layerCount);
	~TextureGroup();

	inline GLint GetLayerCount() const { return mLayerCount; }


public:

	static TextureGroupId CreateGroupId(GLsizei width, GLsizei height, TextureWrap wrapS, TextureWrap wrapT)
	{
		return (uint32_t)height << 19 | (uint32_t)width << 6 | (uint32_t)wrapT << 3 | (uint32_t)wrapS;
	}

	static GLsizei GetWidthFromGroupId(TextureGroupId id)
	{
		return (GLsizei)((id >> 6) & (uint32_t)0x1FFF);
	}

	static GLsizei GetHeightFromGroupId(TextureGroupId id)
	{
		return (GLsizei)((id >> 19) & (uint32_t)0x1FFF);
	}

	static TextureWrap GetWrapSFromGroupId(TextureGroupId id)
	{
		return (TextureWrap)(id & (uint32_t)0x7);
	}

	static TextureWrap GetWrapTFromGroupId(TextureGroupId id)
	{
		return (TextureWrap)((id >> 3) & (uint32_t)0x7);
	}


private:

	TextureGroupId mGroupId;
	GLint mLayerCount;


	friend class TextureManager;
	friend class Engine;
};

} // namespace CoreFx
#endif // COREFX_TEXTURE2D_H