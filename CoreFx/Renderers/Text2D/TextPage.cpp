#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Renderers
	{

TextPage::TextPage(bool isVisible, TextRenderer * textRenderer)
	: mRenderer(textRenderer)
	, mIsVisible(isVisible)
{
}


TextPage::~TextPage()
{
	mTextLineList.clear();
	mRenderer = nullptr;
}

void TextPage::SetVisible(bool visible)
{ 
	if (mIsVisible != visible && !mTextLineList.empty())
	{
		mRenderer->InvalidateShaderBuffer();
		mIsVisible = visible;
	}
}

void TextPage::InvalidateRendererShaderBufferIfVisible()
{
	if (mIsVisible)
	{
		mRenderer->InvalidateShaderBuffer();
	}
}


size_t TextPage::PushBackText(const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex, const glm::u8vec4 & color)
{
	glm::vec4 c(color);
	c /= 255;
	return PushBackText(location, text, fontIndex, c);
}

size_t TextPage::PushBackText(const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex, const glm::vec4 & color)
{
	glm::vec2 loc((GLfloat)location.x, (GLfloat)location.y);
	return PushBackText(loc, ELocationType::AbsolutePixel, text, fontIndex, color);
}

size_t TextPage::PushBackText(const glm::vec2 & location, ELocationType locationType, const std::wstring & text, GLuint fontIndex, const glm::u8vec4 & color)
{
	glm::vec4 c(color);
	c /= 255;
	return PushBackText(location, locationType, text, fontIndex, c);
}

size_t TextPage::PushBackText(const glm::vec2 & location, ELocationType locationType, const std::wstring & text, GLuint fontIndex, const glm::vec4 & color)
{
	if (text.empty())
		return (size_t)-1;

	size_t ret = (size_t)mTextLineList.size();
	if (ret == (size_t)-1)
		return (size_t)-1;

	if (!mRenderer->IsValidFontIndex(fontIndex))
	{
		PRINT_ERROR("[TextPage::PushBackText] Undefined font index '%l'", fontIndex);
		return (size_t)-1;
	}

	mTextLineList.resize(mTextLineList.size() + 1);
	TextLine & textLine = mTextLineList.back();
	textLine.mFontIndex = fontIndex;
	textLine.mColor = glm::packUnorm4x8(color);
	textLine.mText = text;
	textLine.mLocation = location;
	textLine.mLocationType = locationType;
	
	BuildTextLine(textLine);

	InvalidateRendererShaderBufferIfVisible();

	return ret;
}

void TextPage::EraseText(size_t index)
{
	if (index >= mTextLineList.size())
	{
		PRINT_ERROR("[PopBackText] The page is full (the page contains %l text lines)!", mTextLineList.size());
		return;
	}

	mTextLineList.erase(mTextLineList.begin() + index);

	InvalidateRendererShaderBufferIfVisible();
}

void TextPage::PopBackText()
{
	if (mTextLineList.empty())
	{
		PRINT_ERROR("[PopBackText] The page is empty!")
	}

	mTextLineList.pop_back();

	InvalidateRendererShaderBufferIfVisible();
}


void TextPage::BuildTextLine(TextPage::TextLine & textLine)
{
	const glm::vec2 cUnit(1.f);

	Engine * engine = Engine::GetInstance();
	glm::vec2 vpSize(engine->GetViewPortSize());
	glm::vec2 vpOrig(engine->GetViewPortOrigin());

	TextPage::TextLine::DataBuffer & dataBuffer = textLine.mDataBuffer;
	dataBuffer.resize(textLine.mText.size());
	GLuint index = 0;

	const TextRenderer::FontInfo & fi = mRenderer->GetFontInfo(textLine.mFontIndex);
	glm::vec2 cursor = textLine.mLocation;
	if (textLine.mLocationType == TextPage::ELocationType::RelativeRatio)
	{
		cursor = (cursor + cUnit) * 0.5f * vpSize;
	}
	else
	{
		cursor.y = vpSize.y - cursor.y;
	}

	cursor += vpOrig;

	for (std::wstring::const_iterator charIt = textLine.mText.begin(); charIt != textLine.mText.end(); ++charIt)
	{
		const TextRenderer::GlyphMetrics * gm = fi.GetGlyph(*charIt);

		glm::vec2 pos;
		GLuint glyphIndex;

		TextPage::TextLine::CharacterShaderData & data = dataBuffer[index];
		
		if (gm == nullptr)
		{
			glm::vec2 offset(0, -TextRenderer::GlyphMetrics::toPixel(fi.GetCharacterHeight()));
			pos = cursor + offset;
			glyphIndex = fi.GetUndefinedCharacterIndex();
			
			cursor.x += TextRenderer::GlyphMetrics::toPixel(fi.GetDefaultWidth());
		}
		else
		{
			glm::vec2 offset(TextRenderer::GlyphMetrics::toPixel(gm->mBearing.x), -TextRenderer::GlyphMetrics::toPixel(gm->mSize.y - gm->mBearing.y));
			pos = cursor + offset;
			glyphIndex = gm->mGlyphInfoBufferIndex;
			cursor.x += TextRenderer::GlyphMetrics::toPixel(gm->mAdvance.x);
		}

		pos.y -= fi.GetCharacterHeight();

		data.mGlyphInfoIndex = glyphIndex;
		data.mLocation = glm::packHalf2x16(pos);
		data.mColor = textLine.mColor;

		++index;
	}

	textLine.mShaderBufferIndex = -1;
}

void TextPage::Build()
{
	for (TextPage::TextLineList::iterator it = mTextLineList.begin(); it != mTextLineList.end(); ++it)
	{
		TextPage::TextLine & textLine = *it;
		BuildTextLine(textLine);
	}
}


	} // namespace Renderers
} // namespace CoreFx
