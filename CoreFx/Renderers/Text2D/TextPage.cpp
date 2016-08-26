#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Renderers
	{

TextPage::TextPage(GLuint characterCountBufferCapacity)
	: mCharCountBufferCapacity(0)
	, mIsBuilt(false)
{
	ResizeBuffer(characterCountBufferCapacity);
}


TextPage::~TextPage()
{
	mTextLineList.clear();
	mDataBuffer.clear();
}

void TextPage::ResizeBuffer(GLuint characterCount)
{
	mCharCountBufferCapacity = characterCount;
	mDataBuffer.reserve(mCharCountBufferCapacity * __dataindex_count__);
}

GLint TextPage::AddText(const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex)
{
	if (mDataBuffer.size() == mDataBuffer.capacity())
	{
		PRINT_WARNING("[TextPage] : Buffer capacity has been reached. The text will be ignored !");
		return -1;
	}

	GLint ret = (GLint)mTextLineList.size();

	TextLine textLine;
	textLine.mFontIndex = fontIndex;
	textLine.mText = text;
	textLine.mLocation = location;

	GLuint newBufferSize = (GLuint)(mDataBuffer.size() + (text.size() * __dataindex_count__));
	if (newBufferSize > (GLuint)mDataBuffer.size())
	{
		PRINT_WARNING("[TextPage] : Buffer capacity has been reached. The text will be truncated !");
		newBufferSize = (GLuint)mDataBuffer.capacity();
	}
	
	mDataBuffer.resize(newBufferSize);
	mTextLineList.push_back(textLine);

	mIsBuilt = false;

	return ret;
}

void TextPage::BuildBuffer(bool forceRebuild)
{
	if (mIsBuilt && !forceRebuild)
		return;

	GLuint index = 0;
	GLuint bufferSize = (GLuint)mDataBuffer.size();
	for (TextLineList::iterator it = mTextLineList.begin(); it != mTextLineList.end() && index < bufferSize; ++it)
	{
		TextLine & textLine = *it;
		GLuint charCount = (GLuint)textLine.mText.size();
		if (forceRebuild || !textLine.mIsBuilt)
		{
			for (std::wstring::const_iterator charIt = textLine.mText.begin(); charIt != textLine.mText.end() && index < bufferSize; ++charIt)
			{

				index += __dataindex_count__;
			}
			textLine.mIsBuilt = true;
		}
		else
		{
			index += charCount * __dataindex_count__;
		}
		
	}

	mIsBuilt = true;
}

	} // namespace Renderers
} // namespace CoreFx
