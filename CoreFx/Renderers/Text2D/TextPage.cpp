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

void TextPage::SetIsVisible(bool visible)
{ 
	if (mIsVisible != visible && !mTextLineList.empty() && mRenderer->IsAnActivePage(this))
	{
		mRenderer->InvalidateShaderBuffer();
		mIsVisible = visible;
	}
}

void TextPage::InvalidateRendererShaderBufferIfVisible()
{
	if (mIsVisible && mRenderer->IsAnActivePage(this))
	{
		mRenderer->InvalidateShaderBuffer();
	}
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
	
	InternalUpdateText(textLine, location, locationType, text, fontIndex, color);

	return ret;
}

bool TextPage::UpdateText(size_t textLineIndex, const glm::vec2 & location, ELocationType locationType, const std::wstring & text, GLuint fontIndex, const glm::vec4 & color)
{
	if (textLineIndex >= mTextLineList.size())
	{
		PRINT_ERROR("[TextPage::UpdateText] The page doesnot contains text lines n° %l)!", mTextLineList.size());
		return false;
	}

	if (!mRenderer->IsValidFontIndex(fontIndex))
	{
		PRINT_ERROR("[TextPage::UpdateText] Undefined font index '%l'", fontIndex);
		return false;
	}

	TextLine & textLine = mTextLineList[textLineIndex];
	InternalUpdateText(textLine, location, locationType, text, fontIndex, color);
	return true;
}

bool TextPage::UpdateTextString(size_t textLineIndex, const std::wstring & text, bool rebuild)
{
	if (textLineIndex >= mTextLineList.size())
	{
		PRINT_ERROR("[TextPage::UpdateTextString] The page doesnot contains text lines n° %l)!", mTextLineList.size());
		return false;
	}

	TextLine & textLine = mTextLineList[textLineIndex];

	textLine.mText = text;

	if (rebuild)
	{
		BuildTextLine(textLine);
		InvalidateRendererShaderBufferIfVisible();
	}

	return true;
}

bool TextPage::UpdateTextLocation(size_t textLineIndex, const glm::vec2 & location, ELocationType locationType, bool rebuild)
{
	if (textLineIndex >= mTextLineList.size())
	{
		PRINT_ERROR("[TextPage::UpdateTextLocation] The page doesnot contains text lines n° %l)!", mTextLineList.size());
		return false;
	}

	TextLine & textLine = mTextLineList[textLineIndex];
	textLine.mLocation = location;
	textLine.mLocationType = locationType;

	if (rebuild)
	{
		BuildTextLine(textLine);
		InvalidateRendererShaderBufferIfVisible();
	}

	return true;
}

bool TextPage::UpdateTextFont(size_t textLineIndex, GLuint fontIndex, bool rebuild)
{
	if (textLineIndex >= mTextLineList.size())
	{
		PRINT_ERROR("[TextPage::UpdateTextFont] The page doesnot contains text lines n° %l)!", mTextLineList.size());
		return false;
	}

	TextLine & textLine = mTextLineList[textLineIndex];
	textLine.mFontIndex = fontIndex;

	if (rebuild)
	{
		BuildTextLine(textLine);
		InvalidateRendererShaderBufferIfVisible();
	}

	return true;
}

bool TextPage::UpdateTextColor(size_t textLineIndex, const glm::vec4 & color, bool rebuild)
{
	if (textLineIndex >= mTextLineList.size())
	{
		PRINT_ERROR("[TextPage::UpdateTextColor] The page doesnot contains text lines n° %l)!", mTextLineList.size());
		return false;
	}

	TextLine & textLine = mTextLineList[textLineIndex];
	textLine.mColor = glm::packUnorm4x8(color);

	if (rebuild)
	{
		BuildTextLine(textLine);
		InvalidateRendererShaderBufferIfVisible();
	}

	return true;
}


void TextPage::EraseText(size_t index)
{
	if (index >= mTextLineList.size())
	{
		PRINT_WARNING("[TextPage::PopBackText] The page is full (the page contains %l text lines)!", mTextLineList.size());
		return;
	}

	mTextLineList.erase(mTextLineList.begin() + index);

	InvalidateRendererShaderBufferIfVisible();
}

void TextPage::PopBackText()
{
	if (mTextLineList.empty())
	{
		PRINT_WARNING("[TextPage::PopBackText] The page is empty!")
	}

	mTextLineList.pop_back();

	InvalidateRendererShaderBufferIfVisible();
}

void TextPage::ClearText()
{
	if (mTextLineList.empty())
	{
		PRINT_WARNING("[TextPage::ClearText] The page is empty!")
	}

	mTextLineList.clear();

	InvalidateRendererShaderBufferIfVisible();
}

int TextPage::MeasureString(const std::wstring & text, GLuint fontIndex) const
{
	if (!mRenderer->IsValidFontIndex(fontIndex))
	{
		PRINT_ERROR("[TextPage::MeasureString] Undefined font index '%l'", fontIndex);
		return 0;
	}

	int width = 0;
	const TextRenderer::FontInfo & fi = mRenderer->GetFontInfo(fontIndex);

	for (std::wstring::const_iterator charIt = text.begin(); charIt != text.end(); ++charIt)
	{
		const TextRenderer::GlyphMetrics * gm = fi.GetGlyph(*charIt);
		if (gm == nullptr)
		{
			width += TextRenderer::GlyphMetrics::toPixel(fi.GetDefaultWidth());
		}
		else
		{
			width += TextRenderer::GlyphMetrics::toPixel(gm->mAdvance.x);
		}
	}

	return width;
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
