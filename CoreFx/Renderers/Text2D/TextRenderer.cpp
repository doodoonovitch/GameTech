#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



TextRenderer::TextRenderer()
	: RendererHelper<1>(0, "TextRenderer", "TextWireFrameRenderer", Renderer::Forward_Pass)
{

	/*
	const char * attributeNames[__attributes_count__] =
	{
			"u_Color"
	};

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/text.vs.glsl");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/text.forward.fs.glsl");
	mShader.CreateAndLinkProgram();
	mShader.Use();
		mShader.AddAttributes(attributeNames, __attributes_count__);
		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	GLfloat quadVertices[] =
	{
		// Positions        
		-1.0f, 1.0f, 
		-1.0f, -1.0f, 
		1.0f, 1.0f, 
		1.0f, -1.0f,
	};


	//setup vao and vbo stuff
	CreateBuffers();

	glBindVertexArray(mVaoID);

		glBindBuffer (GL_ARRAY_BUFFER, mVboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;	 

		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	mIsInitialized = true;
	*/

}


TextRenderer::~TextRenderer()
{
}

void TextRenderer::Render()
{
	mShader.Use();
		glBindVertexArray(mVaoID);
			glDrawArrays(GL_LINES, 0, 6);
		glBindVertexArray(0);
	mShader.UnUse();
}

void TextRenderer::RenderWireFrame()
{
	Render();
}

void TextRenderer::Initialize(Desc desc)
{
	//FtFaceList ftFaceList;
	mLayerCount = 0;
	glm::uvec2 topLeft(0);

	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize TextRenderer.....");

	mTextureSize = desc.mTextureSize;
	mScreenResolution = desc.mScreenResolution;

	PRINT_MESSAGE(" \t TextureSize = %i x %i", mTextureSize.x, mTextureSize.y);
	PRINT_MESSAGE(" \t Screen resolution = %i x %i", mScreenResolution.x, mScreenResolution.y);

	FT_Library ftLibrary = nullptr;

	FT_Error error = FT_Init_FreeType(&ftLibrary);
	if (error)
	{
		PRINT_ERROR("Error on initializing FreeType library! (Error=%li)", error);
		goto EndTextRendererInit;
	}

	for (const PoliceDesc & policeDesc : desc.mPoliceList)
	{
		AddFont(ftLibrary, policeDesc.mFont.c_str(), policeDesc.mCharacterWidth, policeDesc.mCharacterHeight, topLeft, mLayerCount, policeDesc.mCharacterSet);
	}
	++mLayerCount;

	LoadTexture(ftLibrary);
	PRINT_MESSAGE(" \t Texture Id = %i", mTexture->GetResourceId());

	//for (FT_Face ftFace : ftFaceList)
	//{
	//	if (ftFace != nullptr)
	//	{
	//		FT_Done_Face(ftFace);
	//	}
	//}

	Engine::GetInstance()->DisplayTexture2DArray(mTexture, 1);

	PRINT_MESSAGE(" \t Layer count = %i", mLayerCount);

EndTextRendererInit:

	FT_Done_FreeType(ftLibrary);

	PRINT_MESSAGE(".....TextRenderer initialized!");
	PRINT_END_SECTION;
}

bool TextRenderer::AddFont(FT_Library ftLibrary, const char * fontName, GLushort charWidth, GLushort charHeight, glm::uvec2 & topLeft, GLushort & layerIndex, const std::vector<GLuint> & characterSet)
{
	PRINT_MESSAGE("Add font file '%s' :", fontName);

	bool result = false;

	FT_Error error;
	FT_Face ftFace = nullptr;

	GLint * lineHeight = nullptr;
	GLuint currLineBitmapHeight = 0;

	if (ftLibrary == nullptr)
	{
		PRINT_ERROR("FreeType library is not initialized!", 0);
		goto ExitAddFont;
	}

	error = FT_New_Face(ftLibrary, fontName, 0, &ftFace);
	if (error != 0)
	{
		PRINT_ERROR("Could not load font! (error=%li)", error);
		goto ExitAddFont;
	}
	
	error = FT_Set_Char_Size(ftFace, charWidth * 64, charHeight * 64, mScreenResolution.x, mScreenResolution.y);
	if (error != 0)
	{
		PRINT_ERROR("Could not set pixel sizes! (error=%li)", error);
		goto ExitAddFont;
	}

	//ftFaceList.push_back(ftFace);

	GLuint index = (GLuint)mFontInfoList.size();
	mFontInfoList.resize(index + 1);
	FontInfo & fi = mFontInfoList[index];
	fi.mFileName = fontName;
	fi.mFamilyName = ftFace->family_name;
	fi.mStyleName = ftFace->style_name;
	fi.mStartLayerIndex = layerIndex;
	fi.mDesiredCharWidth = charWidth == 0 ? charHeight : charWidth;
	fi.mDesiredCharHeight = charHeight == 0 ? charWidth : charHeight;
	fi.mBufferEntryIndex = (GLuint)mGlyphInfoBuffer.size();
	fi.mUndefinedCharIndex = (GLuint)mGlyphInfoBuffer.size();
	fi.mLineHeight = 0;

	lineHeight = &fi.mLineHeight;

	//GlyphInfo gi0, gi1;
	//gi0.Set(511, 3784, 3482, 3038, 513, GlyphInfo::Status::Undefined);
	//memset(gi1.mPacked, 0, sizeof(gi1.mPacked));
	//gi1.mBitfields.mLeft = 511;
	//gi1.mBitfields.mTop = 3784;
	//gi1.mBitfields.mRight = 3482;
	//gi1.mBitfields.mBottom = 3038;
	//gi1.mBitfields.mLayerIndex = 513;
	//gi1.mBitfields.mStatus = (GLubyte)GlyphInfo::Status::Undefined;
	//assert(gi0.mPacked[0] == gi1.mPacked[0]);
	//assert(gi0.mPacked[1] == gi1.mPacked[1]);

	FT_UInt ftGlyphIndex;

	if (characterSet.empty())
	{
		FT_ULong ftCharCode = FT_Get_First_Char(ftFace, &ftGlyphIndex);
		while (ftGlyphIndex != 0)
		{
			AddCharacterMetrics(ftFace, ftCharCode, ftGlyphIndex, fi, *lineHeight, currLineBitmapHeight, topLeft, layerIndex);

			ftCharCode = FT_Get_Next_Char(ftFace, ftCharCode, &ftGlyphIndex);
		}
	}
	else
	{
		for (auto v : characterSet)
		{
			FT_ULong ftCharCode = v;
			ftGlyphIndex = FT_Get_Char_Index(ftFace, ftCharCode);
			AddCharacterMetrics(ftFace, ftCharCode, ftGlyphIndex, fi, *lineHeight, currLineBitmapHeight, topLeft, layerIndex);
		}
	}

	fi.mEndLayerIndex = layerIndex;

	PRINT_MESSAGE("  - Name : %s", fi.mFamilyName.c_str());
	PRINT_MESSAGE("  - Style : %s", fi.mStyleName.c_str());
	PRINT_MESSAGE("  - layer : [%i, %i], TopLeft : (%i, %i)", fi.mStartLayerIndex, fi.mEndLayerIndex, topLeft.x, topLeft.y);
	PRINT_MESSAGE("  - character count : %li", fi.mMapping.size());

ExitAddFont:
	if (ftFace != nullptr)
	{
		FT_Done_Face(ftFace);
	}
	return result;
}

bool TextRenderer::AddCharacterMetrics(FT_Face ftFace, FT_ULong ftCharCode, FT_UInt ftGlyphIndex, FontInfo & fi, GLint & lineHeightMax, GLuint & currLineBitmapHeight, glm::uvec2 & topLeft, GLushort & layerIndex)
{
	FT_Error error;

	error = FT_Load_Glyph(ftFace, ftGlyphIndex, FT_LOAD_DEFAULT);
	if (error != 0)
	{
		PRINT_ERROR("Could not load glyph for the character code=%li! (error=%li) (0x%x)", ftCharCode, ftCharCode, error);
		return false;
	}

	glm::uvec2 bitmapSize(GlyphMetrics::toPixel(ftFace->glyph->metrics.width), GlyphMetrics::toPixel(ftFace->glyph->metrics.height));

	if (bitmapSize.x == 0 || bitmapSize.y == 0)
	{
		PRINT_WARNING("Could not render the glyph bitmap for the character code=%li (0x%x)!", ftCharCode, ftCharCode);

		if (ftCharCode == ' ')
		{
			if (ftFace->glyph->metrics.vertAdvance > lineHeightMax)
				lineHeightMax = ftFace->glyph->metrics.vertAdvance;

			GlyphMetrics gm;
			gm.mAdvance = glm::u16vec2(ftFace->glyph->metrics.horiAdvance, ftFace->glyph->metrics.vertAdvance);
			gm.mSize = gm.mAdvance;
			gm.mBearing = glm::ivec2(0);
			gm.mGlyphInfoBufferIndex = (GLuint)mGlyphInfoBuffer.size();
#ifdef _DEBUG
			gm.mChar = (wchar_t)ftCharCode;
#endif // _DEBUG
			fi.mMapping[ftCharCode] = gm;

			GlyphInfo gi;
			gi.Set(0, 0, (GLushort)GlyphMetrics::toPixel(GlyphMetrics::floor(gm.mAdvance.x)), (GLushort)GlyphMetrics::toPixel(GlyphMetrics::floor(gm.mAdvance.y)), 0, GlyphInfo::Status::WhiteSpace);
			mGlyphInfoBuffer.push_back(gi);
		}
		return true;
	}

	{
		if (ftFace->glyph->metrics.vertAdvance > lineHeightMax)
			lineHeightMax = ftFace->glyph->metrics.vertAdvance;

		GlyphMetrics gm;
		gm.mAdvance = glm::u16vec2(ftFace->glyph->metrics.horiAdvance, ftFace->glyph->metrics.vertAdvance);
		gm.mSize = glm::ivec2(ftFace->glyph->metrics.width, ftFace->glyph->metrics.height);
		gm.mBearing = glm::i16vec2(ftFace->glyph->metrics.horiBearingX, ftFace->glyph->metrics.horiBearingY);
		gm.mGlyphInfoBufferIndex = (GLuint)mGlyphInfoBuffer.size();
#ifdef _DEBUG
		gm.mChar = (wchar_t)ftCharCode;
#endif // _DEBUG
		fi.mMapping[ftCharCode] = gm;

		GlyphInfo gi;

		if (bitmapSize.y > currLineBitmapHeight)
			currLineBitmapHeight = bitmapSize.y;

		if ((topLeft.y + currLineBitmapHeight + 2) >= mTextureSize.y)
		{
			topLeft.x = 0;
			topLeft.y = 0;
			++layerIndex;
			currLineBitmapHeight = bitmapSize.y;
		}

		glm::uvec2 bottomRight = topLeft + bitmapSize - glm::uvec2(1);
		if (bottomRight.x >= mTextureSize.x)
		{
			topLeft.x = 0;
			topLeft.y += (currLineBitmapHeight + 2);

			currLineBitmapHeight = bitmapSize.y;

			bottomRight.y = topLeft.y + bitmapSize.y - 1;
			if (bottomRight.y >= mTextureSize.y)
			{
				++layerIndex;

				topLeft.y = 0;
				bottomRight.y = topLeft.y + bitmapSize.y - 1;
			}
			bottomRight.x = topLeft.x + bitmapSize.x - 1;
		}
		gi.Set((GLushort)topLeft.x, (GLushort)topLeft.y, (GLushort)bottomRight.x, (GLushort)bottomRight.y, layerIndex, GlyphInfo::Status::None);
		mGlyphInfoBuffer.push_back(gi);
		topLeft.x = bottomRight.x + 2;
	}

	return true;
}

bool TextRenderer::LoadTexture(FT_Library ftLibrary)
{
	std::uint8_t * buffer = nullptr;
	GLsizei bufferSize = mTextureSize.x * mTextureSize.y;
	buffer = (std::uint8_t*)malloc(bufferSize);
	memset(buffer, 0, bufferSize);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	mTexture = Engine::GetInstance()->GetTextureManager()->CreateTexture2DArray(1, GL_R8, mTextureSize.x, mTextureSize.y, mLayerCount);

	glBindTexture(mTexture->GetTarget(), mTexture->GetResourceId()); GL_CHECK_ERRORS;

	GLushort layerIndex = 0;
	for (size_t i = 0; i < mFontInfoList.size(); ++i)
	{
		FontInfo &fi = mFontInfoList[i];
		FT_Face ftFace = nullptr;

		FT_Error error = FT_New_Face(ftLibrary, fi.mFileName.c_str(), 0, &ftFace);
		if (error != 0)
		{
			PRINT_ERROR("Could not load font! (error=%li)", error);
			continue;
		}

		error = FT_Set_Char_Size(ftFace, fi.mDesiredCharWidth * 64, fi.mDesiredCharHeight * 64, mScreenResolution.x, mScreenResolution.y);
		if (error != 0)
		{
			PRINT_ERROR("Could not set pixel sizes! (error=%li)", error);
			FT_Done_Face(ftFace);
			continue;
		}
		
		CharGlyphIndexMapping & mapping = fi.mMapping;
		for (CharGlyphIndexMapping::iterator itMapping = mapping.begin(); itMapping != mapping.end(); ++itMapping)
		{
			GLulong ftCharCode = itMapping->first;
			GlyphMetrics & gm = itMapping->second;
			GlyphInfo & gi = mGlyphInfoBuffer[gm.mGlyphInfoBufferIndex];

			if (gi.mBitfields.mLayerIndex > layerIndex)
			{
				glTexSubImage3D(mTexture->GetTarget(), 0, 0, 0, layerIndex, mTextureSize.x, mTextureSize.y, 1, GL_RED, GL_UNSIGNED_BYTE, buffer); GL_CHECK_ERRORS;

				layerIndex = gi.mBitfields.mLayerIndex;
				memset(buffer, 0, bufferSize);
			}

			if ((GlyphInfo::Status)gi.mBitfields.mStatus == GlyphInfo::Status::None)
			{
				error = FT_Load_Char(ftFace, ftCharCode, FT_LOAD_RENDER);
				if (error != 0)
				{
					PRINT_ERROR("Could not load glyph for the character code=%li! (error=%li)", ftCharCode, error);
				}
				else
				{
					assert(gi.mBitfields.mRight - gi.mBitfields.mLeft + 1 == ftFace->glyph->bitmap.width);
					assert(gi.mBitfields.mBottom - gi.mBitfields.mTop + 1 == ftFace->glyph->bitmap.rows);

					std::uint8_t * dst = buffer + ((mTextureSize.y - gi.mBitfields.mTop - 1) * mTextureSize.x) + gi.mBitfields.mLeft;
					std::uint8_t * src = ftFace->glyph->bitmap.buffer;
					for (auto y = gi.mBitfields.mTop; y <= gi.mBitfields.mBottom; ++y)
					{
						GLuint n = (GLuint)gi.mBitfields.mRight - (GLuint)gi.mBitfields.mLeft + 1;
							
						assert(ftFace->glyph->bitmap.width == n);
						assert(gi.mBitfields.mLeft + ftFace->glyph->bitmap.width <= mTextureSize.x);

						memcpy(dst, src, n);
						src += std::abs(ftFace->glyph->bitmap.pitch);
						dst -= mTextureSize.x;
					}
				}
			}
		}

		FT_Done_Face(ftFace);
	}

	glTexSubImage3D(mTexture->GetTarget(), 0, 0, 0, layerIndex, mTextureSize.x, mTextureSize.y, 1, GL_RED, GL_UNSIGNED_BYTE, buffer); GL_CHECK_ERRORS;


	glTexParameteri(mTexture->GetTarget(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(mTexture->GetTarget(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(mTexture->GetTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(mTexture->GetTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(mTexture->GetTarget(), 0);

	if (buffer!=nullptr)
		free(buffer);

	return true;
}

void TextRenderer::BuildPage(TextPage & page, bool forceRebuild)
{
	if (page.mIsBuilt && !forceRebuild)
		return;

	GLuint index = 0;
	GLuint bufferSize = (GLuint)page.mDataBuffer.size();
	glm::ivec2 cursor(0);

	for (TextPage::TextLineList::iterator it = page.mTextLineList.begin(); it != page.mTextLineList.end() && index < bufferSize; ++it)
	{
		TextPage::TextLine & textLine = *it;
		GLuint charCount = (GLuint)textLine.mText.size();
		if (forceRebuild || !textLine.mIsBuilt)
		{
			assert(textLine.mFontIndex < (GLuint)mFontInfoList.size());

			//const FontInfo & fi = mFontInfoList[textLine.mFontIndex];
			for (std::wstring::const_iterator charIt = textLine.mText.begin(); charIt != textLine.mText.end() && index < bufferSize; ++charIt)
			{

				index += TextPage::__dataindex_count__;
			}
			textLine.mIsBuilt = true;
		}
		else
		{
			index += charCount * TextPage::__dataindex_count__;
		}

	}

	page.mIsBuilt = true;

}

	} // namespace Renderers
} // namespace CoreFx