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
	FtFaceList ftFaceList;
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
		AddFont(ftLibrary, ftFaceList, policeDesc.mFont.c_str(), policeDesc.mCharacterWidth, policeDesc.mCharacterHeight, topLeft, mLayerCount, policeDesc.mCharacterSet);
	}

	LoadTexture(ftFaceList);

	for (FT_Face ftFace : ftFaceList)
	{
		if (ftFace != nullptr)
		{
			FT_Done_Face(ftFace);
		}
	}


EndTextRendererInit:

	FT_Done_FreeType(ftLibrary);

	PRINT_MESSAGE(".....TextRenderer initialized!");
	PRINT_END_SECTION;
}

bool TextRenderer::AddFont(FT_Library ftLibrary, FtFaceList & ftFaceList, const char * fontName, GLushort charWidth, GLushort charHeight, glm::uvec2 & topLeft, GLushort & layerIndex, const std::vector<GLuint> & characterSet)
{
	PRINT_MESSAGE("Add font file '%s' :", fontName);

	bool result = false;

	FT_Error error;
	FT_Face ftFace = nullptr;

	GLint * lineHeight = nullptr;
	GLuint bitmapHeight = 0;

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

	ftFaceList.push_back(ftFace);

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
	bitmapHeight = 0;

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
			AddCharacterMetrics(ftFace, ftCharCode, ftGlyphIndex, fi, *lineHeight, bitmapHeight, topLeft, layerIndex);

			ftCharCode = FT_Get_Next_Char(ftFace, ftCharCode, &ftGlyphIndex);
		}
	}
	else
	{
		for (auto v : characterSet)
		{
			FT_ULong ftCharCode = v;
			ftGlyphIndex = FT_Get_Char_Index(ftFace, ftCharCode);
			AddCharacterMetrics(ftFace, ftCharCode, ftGlyphIndex, fi, *lineHeight, bitmapHeight, topLeft, layerIndex);
		}
	}

	fi.mEndLayerIndex = layerIndex;

	PRINT_MESSAGE("  - Name : %s", fi.mFamilyName.c_str());
	PRINT_MESSAGE("  - Style : %s", fi.mStyleName.c_str());
	PRINT_MESSAGE("  - layer : [%i, %i], TopLeft : (%i, %i)", fi.mStartLayerIndex, fi.mEndLayerIndex, topLeft.x, topLeft.y);
	PRINT_MESSAGE("  - character count : %li", fi.mMapping.size());

ExitAddFont:
	return result;
}

bool TextRenderer::AddCharacterMetrics(FT_Face ftFace, FT_ULong ftCharCode, FT_UInt ftGlyphIndex, FontInfo & fi, GLint & lineHeight, GLuint & bitmapHeight, glm::uvec2 & topLeft, GLushort & layerIndex)
{
	FT_Error error;

	//error = FT_Load_Char(ftFace, ' ', FT_LOAD_RENDER);
	//if (error != 0)
	//{
	//	PRINT_ERROR("Could not load glyph for the character code=%li! (error=%li)", ftCharCode, error);
	//	goto NextCharacter;
	//}

	error = FT_Load_Glyph(ftFace, ftGlyphIndex, FT_LOAD_DEFAULT);
	if (error != 0)
	{
		PRINT_ERROR("Could not load glyph for the character code=%li! (error=%li) (0x%x)", ftCharCode, ftCharCode, error);
		return false;
	}

	//error = FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);
	//if (error != 0)
	//{
	//	PRINT_ERROR("Could not render the glyph bitmap for the character code=%li (0x%x)! (error=%li)", ftCharCode, ftCharCode, error);
	//	return false;
	//}

	glm::uvec2 bitmapSize(GlyphMetrics::toPixel(ftFace->glyph->metrics.width), GlyphMetrics::toPixel(ftFace->glyph->metrics.height));

	if (bitmapSize.x == 0 || bitmapSize.y == 0)
	{
		PRINT_WARNING("Could not render the glyph bitmap for the character code=%li (0x%x)!", ftCharCode, ftCharCode);

		if (ftCharCode == ' ')
		{
			if (ftFace->glyph->metrics.vertAdvance > lineHeight)
				lineHeight = ftFace->glyph->metrics.vertAdvance;

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
		if (ftFace->glyph->metrics.vertAdvance > lineHeight)
			lineHeight = ftFace->glyph->metrics.vertAdvance;

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

		//glm::uvec2 bitmapSize(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows);

		if (bitmapSize.y > bitmapHeight)
			bitmapHeight = bitmapSize.y;

		glm::uvec2 bottomRight = topLeft + bitmapSize - glm::uvec2(1);
		if (bottomRight.x >= mTextureSize.x)
		{
			topLeft.x = 0;
			topLeft.y += (bitmapHeight + 2);
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

bool TextRenderer::LoadTexture(const FtFaceList & ftFaceList)
{
	assert(ftFaceList.size() == mFontInfoList.size());

	std::uint8_t * blankBuffer = nullptr;

	mTexture = Engine::GetInstance()->GetTextureManager()->CreateTexture2DArray(1, GL_R8, mTextureSize.x, mTextureSize.y, mLayerCount);

	glBindTexture(mTexture->GetTarget(), mTexture->GetResourceId()); GL_CHECK_ERRORS;

	for (size_t i = 0; i < mFontInfoList.size(); ++i)
	{
		FontInfo &fi = mFontInfoList[i];
		FT_Face ftFace = ftFaceList[i];

		GLsizei bufferSize = fi.mDesiredCharWidth * fi.mDesiredCharHeight;
		if (blankBuffer == nullptr)
			blankBuffer = (std::uint8_t*)malloc(bufferSize);
		else
			blankBuffer = (std::uint8_t*)realloc(blankBuffer, bufferSize);
		memset(blankBuffer, 0, bufferSize);

		CharGlyphIndexMapping & mapping = fi.mMapping;
		for (CharGlyphIndexMapping::iterator itMapping = mapping.begin(); itMapping != mapping.end(); ++itMapping)
		{
			GLulong ftCharCode = itMapping->first;
			GlyphMetrics & gm = itMapping->second;
			GlyphInfo & gi = mGlyphInfoBuffer[gm.mGlyphInfoBufferIndex];

			if ((GlyphInfo::Status)gi.mBitfields.mStatus == GlyphInfo::Status::None)
			{
				FT_Error error = FT_Load_Char(ftFace, ' ', FT_LOAD_RENDER);
				if (error != 0)
				{
					PRINT_ERROR("Could not load glyph for the character code=%li! (error=%li)", ftCharCode, error);
					glTexSubImage3D(mTexture->GetTarget(), 0, 0, 0, gi.mBitfields.mLayerIndex, ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows, 1, GL_R, GL_UNSIGNED_BYTE, ftFace->glyph->bitmap.buffer); GL_CHECK_ERRORS;
				}
			}
		}
	}

	glBindTexture(mTexture->GetTarget(), 0);

	if (blankBuffer!=nullptr)
		free(blankBuffer);

	return true;
}

	} // namespace Renderers
} // namespace CoreFx