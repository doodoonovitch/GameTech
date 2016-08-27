#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Renderers
	{

TextPage::TextPage()
	: mIsBuilt(false)
{
}


TextPage::~TextPage()
{
	mTextLineList.clear();
}


GLint TextPage::AddText(const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex)
{
	GLint ret = (GLint)mTextLineList.size();

	TextLine textLine;
	textLine.mFontIndex = fontIndex;
	textLine.mText = text;
	textLine.mLocation = location;

	mTextLineList.push_back(textLine);

	mIsBuilt = false;

	return ret;
}


	} // namespace Renderers
} // namespace CoreFx
