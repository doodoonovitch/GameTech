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


typedef uint64_t TextureGroupId;

enum class TextureWrap
{
	Clamp,					// GL_CLAMP
	Repeat,					// GL_REPEAT
	ClampToEdge,			// GL_CLAMP_TO_EDGE
	ClampToBorder,			// GL_CLAMP_TO_BORDER
	MirrorRepeat,			// GL_MIRRORED_REPEAT 
	MirrorClampToEdge,		// GL_MIRROR_CLAMP_TO_EDGE
};

enum class TextureCategory
{
	Ambient,
	Diffuse,
	Specular,
	NormalMap,
	HeightMap,
};


class TextureInfo
{
	friend class Engine;

public:
	
	TextureInfo()
		: mGroupId((TextureGroupId)-1)
	{}

	TextureInfo(const char * filename, uint16_t rendererId, TextureCategory category, GLsizei width, GLsizei height, TextureWrap wrapS, TextureWrap wrapT)
		: mGroupId(CreateGroupId(rendererId, category, width, height, wrapS, wrapT))
		, mFilename(filename)
	{
	}

	TextureInfo(const TextureInfo & src)
		: mGroupId(src.mGroupId)
		, mFilename(src.mFilename)
	{
	}

public:

	TextureGroupId GetGroupId() const { return mGroupId; }

	GLint GetSamplerIndex() const { return mSamplerIndex; }
	GLint GetLayerIndex() const { return mLayerIndex; }

	GLsizei GetWidth() const { return GetWidthFromGroupId(mGroupId); }
	GLsizei GetHeight() const { return GetHeightFromGroupId(mGroupId); }
	TextureWrap GetWrapS() const { return GetWrapSFromGroupId(mGroupId); }
	TextureWrap GetWrapT() const { return GetWrapTFromGroupId(mGroupId); }
	TextureCategory GetCategory() const { GetCategoryFromGroupId(mGroupId); }
	uint16_t GetRendererId() const { GetRendererIdFromGroupId(mGroupId); }

	const std::string & GetFilename() const { return mFilename; }

public:

	static TextureGroupId CreateGroupId(uint16_t rendererId, TextureCategory category, GLsizei width, GLsizei height, TextureWrap wrapS, TextureWrap wrapT)
	{
		return ((uint64_t)category << 54) | ((uint64_t)wrapT << 51) | ((uint64_t)wrapS << 48) | ((uint64_t)(rendererId & 0xFFFF) << 32)
			| ((uint64_t)(height & 0xFFFF) << 16) | ((uint64_t)(width & 0xFFFF));
	}

	static GLsizei GetWidthFromGroupId(TextureGroupId id)
	{
		return (GLsizei)(id & (TextureGroupId)0xFFFF);
	}

	static GLsizei GetHeightFromGroupId(TextureGroupId id)
	{
		return (GLsizei)((id >> 16) & (TextureGroupId)0xFFFF);
	}

	static TextureWrap GetWrapSFromGroupId(TextureGroupId id)
	{
		return (TextureWrap)((id >> 48) & (TextureGroupId)0x7);
	}

	static TextureWrap GetWrapTFromGroupId(TextureGroupId id)
	{
		return (TextureWrap)((id >> 51) & (TextureGroupId)0x7);
	}

	static TextureCategory GetCategoryFromGroupId(TextureGroupId id)
	{
		return (TextureCategory)((id >> 54) & 0x7);
	}

	static uint16_t GetRendererIdFromGroupId(TextureGroupId id)
	{
		return (uint16_t)((id >> 32) & 0xFFFF);
	}

private:


	GLint mSamplerIndex = -1;
	GLint mLayerIndex = -1;

	TextureGroupId mGroupId;
	std::string mFilename;

};

typedef std::vector<TextureInfo> TextureInfoList;




class TextureGroup : public Texture2D
{
	
protected:

	TextureGroup(GLuint textureId, TextureGroupId groupId, GLint layerCount);
	~TextureGroup();

public:

	inline GLint GetLayerCount() const { return mLayerCount; }


private:

	TextureGroupId mGroupId;
	GLint mLayerCount;


	friend class TextureManager;
	friend class Engine;
};

typedef std::vector<const TextureGroup *> TextureGroupList;

} // namespace CoreFx
#endif // COREFX_TEXTURE2D_H