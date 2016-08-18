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
	TextRenderer();
	virtual ~TextRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

protected:

	enum EAttributeIndex
	{
		u_Color,
		__attributes_count__
	};

	struct GlyphInfo
	{
		enum class Status
		{
			None,
			WhiteSpace = 1,
			Undefined = 2,
		};

		union
		{
			struct S
			{
				std::uint64_t mLayerIndex : 12, mLeft : 12, mTop : 12, mRight : 12, mBottom : 12, mStatus:4;
			} mBitfields;
			GLuint mPacked[2];
		};

		void Set(GLushort left, GLushort top, GLushort right, GLushort bottom, GLushort layerIndex, Status status)
		{
			mPacked[0] = (layerIndex & 0xFFF) | ((left & 0xFFF) << 12) | ((top & 0xFFF) << 24);
			mPacked[1] = (((top & 0xFFF) >> 8) & 0xF) | ((right & 0xFFF) << 4) | ((bottom & 0xFFF) << 16) | ((((GLubyte)status) & 0xF) << 28);
		}
	};

	typedef std::vector<GlyphInfo> GlyphInfoBuffer;

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
	};

	typedef std::map<GLulong, GlyphMetrics> CharGlyphIndexMapping;

	struct FontInfo
	{
		GLuint mBufferEntryIndex;
		GLuint mUndefinedCharIndex;

		GLint mLineHeight;

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


protected:

	bool AddFont(FT_Library ftLibrary, const char * fontName, GLushort charWidth, GLushort charHeight, GLushort &layerIndex);


protected:

	glm::uvec2 mTextureSize = glm::uvec2(512);
	glm::uvec2 mScreenResolution = glm::uvec2(72);

	GlyphInfoBuffer mGlyphInfoBuffer;
	FontInfoList mFontInfoList;
	bool mIsInitialized;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TEXTRENDERER_H