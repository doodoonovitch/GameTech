#pragma once

#ifndef COREFX_RENDERERS_TEXTPAGE_H
#define COREFX_RENDERERS_TEXTPAGE_H


namespace CoreFx
{
	namespace Renderers
	{

	class TextRenderer;

class TextPage
{
	friend class TextRenderer;

public:
	
	enum class ELocationType
	{
		AbsolutePixel,
		RelativeRatio
	};

protected:

	struct TextLine
	{
#pragma pack(push, 1)
		struct CharacterShaderData
		{
			GLuint mGlyphInfoIndex;
			GLuint mLocation;
			GLuint mColor;
		};
#pragma pack(pop)

		typedef std::vector<CharacterShaderData> DataBuffer;

		GLuint mFontIndex;
		GLuint mColor;
		glm::vec2 mLocation;
		glm::vec2 mSize;
		std::wstring mText;
		DataBuffer mDataBuffer;
		GLint mShaderBufferIndex;
		ELocationType mLocationType;

		TextLine()
			: mFontIndex(0)
			, mLocation(0)
			, mShaderBufferIndex(-1)
			, mLocationType(ELocationType::RelativeRatio)
		{}
	};

	typedef std::vector<TextLine> TextLineList;

protected:

	TextPage(bool isVisible, TextRenderer * textRenderer);
	~TextPage();
	

public:

	bool GetIsVisible() const { return mIsVisible; }
	void SetIsVisible(bool visible);

	inline size_t PushBackText(const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex, const glm::u8vec4 & color);
	inline size_t PushBackText(const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex, const glm::vec4 & color);
	inline size_t PushBackText(const glm::vec2 & location, ELocationType locationType, const std::wstring & text, GLuint fontIndex, const glm::u8vec4 & color);
	size_t PushBackText(const glm::vec2 & location, ELocationType locationType, const std::wstring & text, GLuint fontIndex, const glm::vec4 & color);

	inline bool UpdateText(size_t textLineIndex, const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex, const glm::u8vec4 & color);
	inline bool UpdateText(size_t textLineIndex, const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex, const glm::vec4 & color);
	inline bool UpdateText(size_t textLineIndex, const glm::vec2 & location, ELocationType locationType, const std::wstring & text, GLuint fontIndex, const glm::u8vec4 & color);
	bool UpdateText(size_t textLineIndex, const glm::vec2 & location, ELocationType locationType, const std::wstring & text, GLuint fontIndex, const glm::vec4 & color);

	bool UpdateTextString(size_t textLineIndex, const std::wstring & text, bool rebuild = true);
	inline bool UpdateTextLocation(size_t textLineIndex, const glm::ivec2 & location, bool rebuild = true);
	bool UpdateTextLocation(size_t textLineIndex, const glm::vec2 & location, ELocationType locationType, bool rebuild = true);
	bool UpdateTextFont(size_t textLineIndex, GLuint fontIndex, bool rebuild = true);
	inline bool UpdateTextColor(size_t textLineIndex, const glm::u8vec4 & color, bool rebuild = true);
	bool UpdateTextColor(size_t textLineIndex, const glm::vec4 & color, bool rebuild = true);

	size_t GetTextCount() const { return mTextLineList.size(); }

	void EraseText(size_t index);
	void PopBackText();
	void ClearText();

	int MeasureString(const std::wstring & text, GLuint fontIndex) const;
	int MeasureStringInPixel(const std::wstring & text, GLuint fontIndex) const;

protected:

	void BuildTextLine(TextLine & line);
	void Build();

	void InvalidateRendererShaderBufferIfVisible();

	inline void InternalUpdateText(TextLine & textLine, const glm::vec2 & location, ELocationType locationType, const std::wstring & text, GLuint fontIndex, const glm::vec4 & color);

private:

	TextRenderer * mRenderer;
	TextLineList mTextLineList;
	bool mIsVisible;
};

typedef std::weak_ptr<TextPage> TextPageWeakPtr;





















inline size_t TextPage::PushBackText(const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex, const glm::u8vec4 & color)
{
	glm::vec4 c(color);
	c /= 255;
	return PushBackText(location, text, fontIndex, c);
}

inline size_t TextPage::PushBackText(const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex, const glm::vec4 & color)
{
	glm::vec2 loc((GLfloat)location.x, (GLfloat)location.y);
	return PushBackText(loc, ELocationType::AbsolutePixel, text, fontIndex, color);
}

inline size_t TextPage::PushBackText(const glm::vec2 & location, ELocationType locationType, const std::wstring & text, GLuint fontIndex, const glm::u8vec4 & color)
{
	glm::vec4 c(color);
	c /= 255;
	return PushBackText(location, locationType, text, fontIndex, c);
}

inline bool TextPage::UpdateText(size_t textLineIndex, const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex, const glm::u8vec4 & color)
{
	glm::vec4 c(color);
	c /= 255;
	return UpdateText(textLineIndex, location, text, fontIndex, c);
}

inline bool TextPage::UpdateText(size_t textLineIndex, const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex, const glm::vec4 & color)
{
	glm::vec2 loc((GLfloat)location.x, (GLfloat)location.y);
	return UpdateText(textLineIndex, loc, ELocationType::AbsolutePixel, text, fontIndex, color);
}

inline bool TextPage::UpdateText(size_t textLineIndex, const glm::vec2 & location, ELocationType locationType, const std::wstring & text, GLuint fontIndex, const glm::u8vec4 & color)
{
	glm::vec4 c(color);
	c /= 255;
	return UpdateText(textLineIndex, location, locationType, text, fontIndex, c);
}

inline bool TextPage::UpdateTextLocation(size_t textLineIndex, const glm::ivec2 & location, bool rebuild)
{
	glm::vec2 loc((GLfloat)location.x, (GLfloat)location.y);
	return UpdateTextLocation(textLineIndex, loc, ELocationType::AbsolutePixel, rebuild);

}

inline bool TextPage::UpdateTextColor(size_t textLineIndex, const glm::u8vec4 & color, bool rebuild)
{
	glm::vec4 c(color);
	c /= 255;
	return UpdateTextColor(textLineIndex, c, rebuild);
}

inline void TextPage::InternalUpdateText(TextLine & textLine, const glm::vec2 & location, ELocationType locationType, const std::wstring & text, GLuint fontIndex, const glm::vec4 & color)
{
	textLine.mFontIndex = fontIndex;
	textLine.mColor = glm::packUnorm4x8(color);
	textLine.mText = text;
	textLine.mLocation = location;
	textLine.mLocationType = locationType;

	BuildTextLine(textLine);

	InvalidateRendererShaderBufferIfVisible();
}

	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TEXTPAGE_H