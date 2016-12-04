#pragma once

#ifndef COREFX_RENDERERS_TEXTRENDERER_H
#define COREFX_RENDERERS_TEXTRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class TextRenderer : public RendererHelper<1>
{
public:

	struct PoliceDesc
	{
		std::string mFont;
		GLushort mCharacterWidth = 0;
		GLushort mCharacterHeight = 16;
		std::vector<GLuint> mCharacterSet;
	};

	typedef std::vector<PoliceDesc> PoliceDescList;

	class Desc 
	{
	public:
		glm::uvec2 mTextureSize = glm::uvec2(512);
		glm::uvec2 mScreenResolution = glm::uvec2(72);

		GLsizei mCharacterCountBufferCapacity = 1000;


		PoliceDescList mPoliceList;
	};

	struct GlyphMetrics
	{
		template <typename T> static T round(T value) { return (value + 32) & -64; }
		template <typename T> static T floor(T value) {	return value & -64; }
		template <typename T> static T ceiling(T value) { return (value + 64) & -64; }
		template <typename T> static T toPixel(T value) { return value / 64; }

		glm::ivec2 mSize;
		glm::ivec2 mBearing;
		glm::ivec2 mAdvance;
		GLuint mGlyphInfoBufferIndex;
#ifdef _DEBUG
		wchar_t mChar;
#endif // _DEBUG
	};

	typedef std::map<GLulong, GlyphMetrics> CharGlyphIndexMapping;

	class FontInfo
	{
	public:

		const std::string & GetFileName() const	{ return mFileName;	}
		const std::string & GetFamilyName() const { return mFamilyName; }
		const std::string & GetStyleName() const { return mStyleName; }

		GLushort GetLineHeight() const { return mLineHeight; }

		GLushort GetCharacterWidth() const { return mDesiredCharWidth; }
		GLushort GetCharacterHeight() const { return mDesiredCharHeight; }

		GLushort GetDefaultWidth() const { return mDefaultWidth; }

		const CharGlyphIndexMapping & GetMapping() const { return mMapping; }

		const GlyphMetrics * GetGlyph(GLulong character) const
		{
			CharGlyphIndexMapping::const_iterator it = mMapping.find(character);
			if (it == mMapping.end())
			{
				return nullptr;
			}
			else
			{
				return &it->second;
			}
		}

		GLuint GetUndefinedCharacterIndex() const { return mUndefinedCharIndex; }


	private:

		friend class TextRenderer;

		GLuint mIndex;

		GLuint mBufferEntryIndex;
		GLuint mUndefinedCharIndex;

		GLushort mLineHeight;
		GLushort mDefaultWidth;

		GLushort mDesiredCharWidth;
		GLushort mDesiredCharHeight;

		GLushort mStartLayerIndex;
		GLushort mEndLayerIndex;

		CharGlyphIndexMapping mMapping;

		std::string mFileName;
		std::string mFamilyName;
		std::string mStyleName;
	};

	typedef std::vector<FontInfo> FontInfoList;


#pragma pack(push, 1)
	struct ShaderGlyphInfo
	{
		GLuint mLayerIndexAndStatus = 0;
		GLuint mXTexUV;
		GLuint mYTexUV;
		GLuint mCharacterSize;
	};

	struct GlyphInfo : public ShaderGlyphInfo
	{
		enum ELayerIndexAndStatusMaskShift
		{
			LayerIndexMask = 0x0000FFFF,
			StatusMask = 0xFFFF0000,
		};

		enum class Status
		{
			Character,
			WhiteSpace = 1,
			Undefined = 2,
		};

		glm::uvec2 mLTCorner;
		glm::uvec2 mRBCorner;

		void Set(const glm::uvec2 leftTopCornerInPixel, const glm::uvec2 rightBottomCornerInPixel, const glm::ivec2 & textureSize, GLushort layerIndex, Status status)
		{
			mLayerIndexAndStatus = ((GLuint)layerIndex) | (((GLuint)status) << 16);
			SetCorners(leftTopCornerInPixel, rightBottomCornerInPixel, textureSize);
		}

		GLushort GetLayerIndex() const
		{
			return mLayerIndexAndStatus & LayerIndexMask;
		}

		void SetLayerIndex(GLushort layerIndex)
		{
			mLayerIndexAndStatus = ((GLuint)layerIndex) | (mLayerIndexAndStatus & StatusMask);
		}

		Status GetStatus() const
		{
			return (Status)(mLayerIndexAndStatus & StatusMask);
		}

		void SetStatus(Status status)
		{
			mLayerIndexAndStatus = (mLayerIndexAndStatus & LayerIndexMask) | (GLuint)status;
		}

		void SetCorners(const glm::uvec2 leftTopCornerInPixel, const glm::uvec2 rightBottomCornerInPixel, const glm::ivec2 & textureSize)
		{
			mLTCorner = leftTopCornerInPixel;
			mRBCorner = rightBottomCornerInPixel;
			glm::vec2 xTexUV(leftTopCornerInPixel.x - .5f, rightBottomCornerInPixel.x + .5f);
			glm::vec2 yTexUV(leftTopCornerInPixel.y - .5f, rightBottomCornerInPixel.y + .5f);
			xTexUV /= (textureSize.x - 1);
			yTexUV = glm::vec2(1.f) - (yTexUV / (textureSize.y - 1));
			mXTexUV = glm::packHalf2x16(xTexUV);
			mYTexUV = glm::packHalf2x16(yTexUV);
			glm::ivec2 size(mRBCorner.x - mLTCorner.x + 1, mRBCorner.y - mLTCorner.y + 1);
			mCharacterSize = (GLuint)(size.x & 0xFFFF) | (GLuint)((size.y & 0xFFFF) << 16);
		}

		GLushort GetCharacterWidth() const
		{
			return mCharacterSize & 0xFFFF;
		}
		
		GLushort GetCharacterHeight() const
		{
			return (mCharacterSize >> 16) & 0xFFFF;
		}

	};
#pragma pack(pop)

	typedef std::vector<GlyphInfo> GlyphInfoBuffer;



public:
	
	TextRenderer();
	virtual ~TextRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

	void Initialize(const Desc & desc);
	void ResizeBuffer(GLsizei characterCount);

	std::weak_ptr<TextPage> NewPage(bool isVisible);
	void DeletePage(std::weak_ptr<TextPage> page);
	GLsizei GetTextPageCount() const { return (GLsizei)mTextPageList.size(); }
	
	bool IsValidFontIndex(GLuint fontIndex) const { return fontIndex < (GLuint)mFontInfoList.size(); }
	const FontInfoList & GetFontInfoList() const { return mFontInfoList; }
	const FontInfo & GetFontInfo(GLuint fontIndex) const
	{ 
		if (!IsValidFontIndex(fontIndex))
		{
			PRINT_ERROR("Undefined font index '%l'", fontIndex);
			fontIndex = fontIndex % (GLuint)mFontInfoList.size();
		}
		return mFontInfoList[fontIndex];
	}

	const GlyphInfo & GetGlyphInfo(GLuint glyphIndex) const
	{
		if (!(glyphIndex < (GLuint)mGlyphInfoBuffer.size()))
		{
			PRINT_ERROR("Undefined glyph index '%l'", glyphIndex);
			glyphIndex = glyphIndex % (GLuint)mGlyphInfoBuffer.size();
		}
		return mGlyphInfoBuffer[glyphIndex];
	}

	void InvalidateShaderBuffer();

protected:

	typedef std::vector<std::shared_ptr<TextPage>> TextPageList;
	typedef std::vector<TextGroup> TextGroupList;

	static void TextPageDelete(TextPage * page);

protected:

	void InitializeShader();

	//typedef std::vector<FT_Face> FtFaceList;

	bool AddFont(FT_Library ftLibrary, const char * fontName, GLushort charWidth, GLushort charHeight, glm::uvec2 & topLeft, GLushort &layerIndex, const std::vector<GLuint> & characterSet);
	bool AddCharacterMetrics(FT_Face ftFace, FT_ULong ftCharCode, FT_UInt ftGlyphIndex, FontInfo & fi, GLushort & lineHeight, GLuint & bitmapHeight, glm::uvec2 & topLeft, GLushort & layerIndex);
	bool LoadTexture(FT_Library ftLibrary);

	void UpdateShaderStorageBuffer();

	friend class TextPage;

	void BuildPageList();

protected:

#pragma pack(push, 1)
	struct VAOBufferData
	{
		GLfloat mPosition[2];
		GLint mTexCoordIndex[2];
	};
#pragma pack(pop)

	enum EStorageBufferBinding
	{
		u_GlyphInfoBuffer,
		u_PerInstanceDataBuffer,
	};

	enum EAttributeIndex
	{
		u_FontTextureSampler,
		__attributes_count__
	};


	const Texture2DArray * mTexture = nullptr;
	glm::uvec2 mTextureSize = glm::uvec2(512);
	glm::uvec2 mScreenResolution = glm::uvec2(72);
	GLushort mLayerCount = 0;
	GLsizei mCharCountBufferCapacity = 1000;
	GLsizei mCharCount = 0;
	ShaderStorageBuffer mShaderBuffer;
	ShaderStorageBuffer mShaderGlyphInfoBuffer;

	GlyphInfoBuffer mGlyphInfoBuffer;
	FontInfoList mFontInfoList;
	
	TextPageList mTextPageList;
	TextGroupList mTextGroupList;


	bool mIsShaderBufferUpdated;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TEXTRENDERER_H