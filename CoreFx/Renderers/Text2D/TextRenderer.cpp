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

	GLint lineHeight = 0;
	glm::uvec2 topLeft(0);

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
	fi.mDesiredCharWidth = charWidth == 0 ? charHeight : charWidth;
	fi.mDesiredCharHeight = charHeight == 0 ? charWidth : charHeight;
	fi.mBufferEntryIndex = (GLuint)mGlyphInfoBuffer.size();
	fi.mUndefinedCharIndex = (GLuint)mGlyphInfoBuffer.size();


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
			PRINT_WARNING("Could not render the glyph bitmap for the character code=%li (0x%x)!", ftCharCode, ftCharCode);

			if (ftCharCode == ' ')
			{
				if (ftFace->glyph->metrics.vertAdvance > lineHeight)
					lineHeight = ftFace->glyph->metrics.vertAdvance;

				GlyphMetrics gm;
				gm.mAdvance = glm::u16vec2(ftFace->glyph->metrics.horiAdvance, ftFace->glyph->metrics.vertAdvance);
				gm.mSize = gm.mAdvance;
				gm.mBearing = glm::ivec2(0);
				fi.mMapping[ftCharCode] = gm;

				GlyphInfo gi;
				gi.Set(0, 0, (GLushort)GlyphMetrics::toPixel(GlyphMetrics::floor(gm.mAdvance.x)), (GLushort)GlyphMetrics::toPixel(GlyphMetrics::floor(gm.mAdvance.y)), 0, GlyphInfo::Status::WhiteSpace);
				mGlyphInfoBuffer.push_back(gi);
			}
			goto NextCharacter;
		}
		
		{
			if (ftFace->glyph->metrics.vertAdvance > lineHeight)
				lineHeight = ftFace->glyph->metrics.vertAdvance;

			GlyphMetrics gm;
			gm.mAdvance = glm::u16vec2(ftFace->glyph->metrics.horiAdvance, ftFace->glyph->metrics.vertAdvance);
			gm.mSize = glm::ivec2(ftFace->glyph->metrics.width, ftFace->glyph->metrics.height);
			gm.mBearing = glm::i16vec2(ftFace->glyph->metrics.horiBearingX, ftFace->glyph->metrics.horiBearingY);
			fi.mMapping[ftCharCode] = gm;

			GlyphInfo gi;

			glm::uvec2 bitmapSize(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows);
			
			glm::uvec2 bottomRight = topLeft + bitmapSize - glm::uvec2(1);
			if (bottomRight.x >= mTextureSize.x)
			{
				topLeft.x = 0;
				topLeft.y = bottomRight.y;
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