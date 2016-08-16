#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



TextRenderer::TextRenderer()
	: RendererHelper<1>(0, "TextRenderer", "TextWireFrameRenderer", Renderer::Forward_Pass)
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize TextRenderer.....");

	FT_Library ftLibrary = nullptr;

	FT_Error error = FT_Init_FreeType(&ftLibrary);
	if (error)
	{
		PRINT_ERROR("Error on initializing FreeType library! (Error=%li)", error);
		goto EndTextRendererInit;
	}


	GLushort layerIndex = 0;
	AddFont(ftLibrary, "Medias/Fonts/arialbd.ttf", 0, 48, layerIndex);

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

EndTextRendererInit:
	
	FT_Done_FreeType(ftLibrary);

	PRINT_MESSAGE(".....TextRenderer initialized!");
	PRINT_END_SECTION;
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

bool TextRenderer::AddFont(FT_Library ftLibrary, const char * fontName, GLushort charWidth, GLushort charHeight, GLushort & layerIndex)
{
	PRINT_MESSAGE("Add font file '%s' :", fontName);

	bool result = false;

	FT_Error error;
	FT_Face ftFace = nullptr;

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

	GLuint index = (GLuint)mFontInfoList.size();
	mFontInfoList.resize(index + 1);
	FontInfo & fi = mFontInfoList[index];
	fi.mFileName = fontName;
	fi.mFamilyName = ftFace->family_name;
	fi.mStyleName = ftFace->style_name;
	fi.mStartLayerIndex = layerIndex;
	fi.mCharWidth = charWidth == 0 ? charHeight : charWidth;
	fi.mCharHeight = charHeight == 0 ? charWidth : charHeight;
	fi.mBufferEntryIndex = (GLuint)mGlyphInfoBuffer.size();
	fi.mUndefinedCharIndex = (GLuint)mGlyphInfoBuffer.size();
	fi.mMin = glm::u16vec2(ftFace->bbox.xMin, ftFace->bbox.yMin);
	fi.mMax = glm::u16vec2(ftFace->bbox.xMax, ftFace->bbox.yMax);
	GLushort left = 0;
	GLushort top = 0;

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
	FT_ULong ftCharCode = FT_Get_First_Char(ftFace, &ftGlyphIndex);
	while(ftGlyphIndex != 0)
	{
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
			goto NextCharacter;
		}

		error = FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);
		if (error != 0)
		{
			PRINT_ERROR("Could not render the glyph bitmap for the character code=%li (0x%x)! (error=%li)", ftCharCode, ftCharCode, error);
			goto NextCharacter;
		}

		if (ftFace->glyph->bitmap.width == 0 || ftFace->glyph->bitmap.rows == 0)
		{
			PRINT_ERROR("Could not render the glyph bitmap for the character code=%li (0x%x)!", ftCharCode, ftCharCode);

			if (ftCharCode == ' ')
			{
				GlyphMetrics gm;
				gm.mSize = glm::u16vec2(fi.mCharWidth, fi.mCharHeight);
				gm.mAdvance = glm::i16vec2(fi.mCharWidth, fi.mCharHeight);
				gm.mBearing = glm::u16vec2(0, fi.mCharHeight);
				fi.mMapping[ftCharCode] = gm;

				GlyphInfo gi;
				gi.Set(0, 0, fi.mCharWidth - 1, fi.mCharHeight - 1, 0, GlyphInfo::Status::WhiteSpace);
				mGlyphInfoBuffer.push_back(gi);
			}
			goto NextCharacter;
		}
		
		{
			GlyphMetrics gm;
			gm.mSize = glm::u16vec2(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows);
			gm.mBearing = glm::i16vec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top);
			gm.mAdvance = glm::u16vec2(
				ftFace->glyph->advance.x % 64 > 0 ? (ftFace->glyph->advance.x >> 6) + 1 : ftFace->glyph->advance.x >> 6,
				ftFace->glyph->advance.y % 64 > 0 ? (ftFace->glyph->advance.y >> 6) + 1 : ftFace->glyph->advance.y >> 6);
			fi.mMapping[ftCharCode] = gm;

			GlyphInfo gi;
			GLushort right = (GLushort)(left + gm.mSize.x - 1);
			GLushort bottom = (GLushort)(top + gm.mSize.y - 1);
			if (right >= mTextureSize.x)
			{
				left = 0;
				top = bottom;
				bottom = (GLushort)(top + gm.mSize.y - 1);
				if (bottom >= mTextureSize.y)
				{
					++layerIndex;
					top = 0;
					bottom = (GLushort)(top + gm.mSize.y - 1);
				}
				right = (GLushort)(left + gm.mSize.x - 1);
			}
			gi.Set(left, top, right, bottom, layerIndex, GlyphInfo::Status::None);
			mGlyphInfoBuffer.push_back(gi);
		}
		
NextCharacter:
		ftCharCode = FT_Get_Next_Char(ftFace, ftCharCode, &ftGlyphIndex);
	}

	fi.mEndLayerIndex = layerIndex;

	PRINT_MESSAGE("  - Name : %s", fi.mFamilyName.c_str());
	PRINT_MESSAGE("  - Style : %s", fi.mStyleName.c_str());
	PRINT_MESSAGE("  - layer : [%i, %i]", fi.mStartLayerIndex, fi.mEndLayerIndex);
	PRINT_MESSAGE("  - character count : %li", fi.mMapping.size());

ExitAddFont:
	if (ftFace != nullptr)
	{
		FT_Done_Face(ftFace);
	}
	return result;
}


	} // namespace Renderers
} // namespace CoreFx