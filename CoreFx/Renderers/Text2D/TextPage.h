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

	enum DataIndex
	{
		DataIndex_X,
		DataIndex_Y,
		DataIndex_CharIndex,

		__dataindex_count__
	};

	typedef std::vector<GLuint> DataBuffer;



protected:
	TextPage(GLuint characterCountBufferCapacity = 1000);
	~TextPage();


public:

	void ResizeBuffer(GLuint characterCount);
	GLint AddText(const glm::ivec2 & location, const std::wstring & text, GLuint fontIndex);

protected:

	void BuildBuffer(bool forceRebuild = false);

private:

	GLuint mCharCountBufferCapacity;
	DataBuffer mDataBuffer;
	TextLineList mTextLineList;
	bool mIsBuilt;
};




	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TEXTPAGE_H