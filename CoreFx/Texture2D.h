#pragma once
#ifndef COREFX_TEXTURE2D_H
#define COREFX_TEXTURE2D_H


namespace CoreFx
{

	class TextureManager;
	class Engine;

class Texture
{
public:

	inline GLuint GetResourceId() const
	{
		return mId;
	}

	GLenum GetTarget() const
	{
		return mTarget;
	}

protected:

	enum TextureClass
	{
		Texture1D,
		Texture2D,
		CubeMapTexture,
		TextureGroup,
		Texture2DArray,
	};

	Texture(GLuint id, GLenum target, TextureClass texClass);
	~Texture();

	TextureClass GetTextureClass() const
	{
		return mTextureClass;
	}

	void SetId(GLuint id)
	{
		mId = id;
	}

	void SetTarget(GLenum target)
	{
		mTarget = target;
	}

private:

	GLuint mId;
	TextureClass mTextureClass;
	GLenum mTarget;

	friend class TextureManager;
};

class Texture1D : public Texture
{
protected:

	Texture1D(GLuint id, GLenum target);
	~Texture1D();

private:

	friend class TextureManager;
};

class Texture2D : public Texture
{
protected:

	Texture2D(GLuint id, GLenum target);
	~Texture2D();

private:

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
	Diffuse,
	Metallic,
	NormalMap,
	HeightMap,
	Emissive,
	Roughness,

	__count__
};

extern const char * TextureCategoryToString(TextureCategory cat);

class TextureInfo
{
	friend class Engine;
	friend class Renderer;

public:
	
	TextureInfo()
		: mGroupId((TextureGroupId)-1)
	{}

	TextureInfo(const char * filename, uint16_t rendererId, TextureCategory category, GLsizei width, GLsizei height, TextureWrap wrapS, TextureWrap wrapT, bool invertY)
		: mGroupId(CreateGroupId(rendererId, category, width, height, wrapS, wrapT))
		, mFilename(filename)
		, mInvertY(invertY)
	{
	}

	TextureInfo(const TextureInfo & src)
		: mGroupId(src.mGroupId)
		, mFilename(src.mFilename)
		, mInvertY(src.mInvertY)
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
	TextureCategory GetCategory() const { return GetCategoryFromGroupId(mGroupId); }
	uint16_t GetRendererId() const { return GetRendererIdFromGroupId(mGroupId); }

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

	static GLenum FromTextureWrap(TextureWrap wrap)
	{
		switch (wrap)
		{
		case TextureWrap::Clamp:
			return GL_CLAMP;

		case TextureWrap::ClampToBorder:
			return GL_CLAMP_TO_BORDER;

		case TextureWrap::ClampToEdge:
			return GL_CLAMP_TO_EDGE;

		case TextureWrap::MirrorClampToEdge:
			return GL_MIRROR_CLAMP_TO_EDGE;

		case TextureWrap::MirrorRepeat:
			return GL_MIRRORED_REPEAT;

		case TextureWrap::Repeat:
			return GL_REPEAT;
		}

		return GL_CLAMP;
	}

private:


	mutable GLint mSamplerIndex = -1;
	mutable GLint mLayerIndex = -1;

	TextureGroupId mGroupId;
	std::string mFilename;
	bool mInvertY;

};

typedef std::vector<TextureInfo> TextureInfoList;




class TextureGroup : public Texture
{
	
protected:

	TextureGroup(GLuint textureId, TextureGroupId groupId, GLenum target, GLint layerCount);
	~TextureGroup();

public:

	inline TextureGroupId GetGroupId() const { return mGroupId; }

	inline GLint GetLayerCount() const { return mLayerCount; }

	GLsizei GetWidth() const { return TextureInfo::GetWidthFromGroupId(mGroupId); }
	GLsizei GetHeight() const { return TextureInfo::GetHeightFromGroupId(mGroupId); }
	TextureWrap GetWrapS() const { return TextureInfo::GetWrapSFromGroupId(mGroupId); }
	TextureWrap GetWrapT() const { return TextureInfo::GetWrapTFromGroupId(mGroupId); }
	TextureCategory GetCategory() const { return TextureInfo::GetCategoryFromGroupId(mGroupId); }
	uint16_t GetRendererId() const { return TextureInfo::GetRendererIdFromGroupId(mGroupId); }


private:

	TextureGroupId mGroupId;
	GLint mLayerCount;


	friend class TextureManager;
	friend class Engine;
};

typedef std::vector<const TextureGroup *> TextureGroupList;



class CubeMapTexture : public Texture
{
protected:

	CubeMapTexture(GLuint textureId, GLenum target);
	~CubeMapTexture();


private:

	friend class TextureManager;
	friend class Engine;
};

class Texture2DArray : public Texture
{
protected:

	Texture2DArray(GLuint textureId, GLenum target);
	~Texture2DArray();


private:

	friend class TextureManager;
	friend class Engine;
};





typedef std::vector<const TextureInfo *> TexInfoPtrList;
struct TextureMappingItem
{
	const TextureGroup * mTexture = nullptr;
	GLint mSamplerIndex = -1;
	TexInfoPtrList mTexInfoList;
};

typedef std::vector<TextureMappingItem> TextureMappingList;

struct TextureMapping
{
	int mSamplerCount = 0;
	TextureMappingList mMapping;
};

} // namespace CoreFx
#endif // COREFX_TEXTURE2D_H