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

	size_t PushBackText(const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex, const glm::u8vec4 & color);
	size_t PushBackText(const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex, const glm::vec4 & color);
	size_t PushBackText(const glm::vec2 & location, ELocationType locationType, const std::wstring & text, GLuint fontIndex, const glm::u8vec4 & color);
	size_t PushBackText(const glm::vec2 & location, ELocationType locationType, const std::wstring & text, GLuint fontIndex, const glm::vec4 & color);

	void EraseText(size_t index);
	void PopBackText();

protected:

	void BuildTextLine(TextLine & line);
	void Build();

	void InvalidateRendererShaderBufferIfVisible();

private:

	TextRenderer * mRenderer;
	TextLineList mTextLineList;
	bool mIsVisible;
};

typedef std::weak_ptr<TextPage> TextPageWeakPtr;


	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TEXTPAGE_H