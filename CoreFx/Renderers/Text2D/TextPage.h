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

	struct TextLine
	{
		GLuint mFontIndex;
		glm::ivec2 mLocation;
		std::wstring mText;
		bool mIsBuilt;

		TextLine()
			: mFontIndex(0)
			, mLocation(0)
			, mIsBuilt(false)
		{}
	};

	typedef std::vector<TextLine> TextLineList;

protected:
	TextPage();
	~TextPage();


public:

	GLint AddText(const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex);

protected:


private:

	TextLineList mTextLineList;
	bool mIsBuilt;
};




	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TEXTPAGE_H