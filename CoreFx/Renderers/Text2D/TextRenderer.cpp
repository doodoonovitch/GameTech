#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



TextRenderer::TextRenderer()
	: RendererHelper<1>("TextRenderer", "TextWireFrameRenderer", Renderer::HUD_Pass)
	, mIsShaderBufferUpdated(false)
{
	auto ptr = NewTextGroup();
	mDefaultTextGroup = ptr.lock();
	mActiveTextGroup = mDefaultTextGroup;
}


TextRenderer::~TextRenderer()
{
	mDefaultTextGroup.reset();
	mActiveTextGroup.reset();

	mTextGroupList.clear();
	mTextPageList.clear();
}

void TextRenderer::Render()
{
	UpdateShaderStorageBuffer();

	mShader.Use();
		glBindVertexArray(mVaoID);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, u_GlyphInfoBuffer, mShaderGlyphInfoBuffer.GetBufferId());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, u_PerInstanceDataBuffer, mShaderBuffer.GetBufferId());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(mTexture->GetTarget(), mTexture->GetResourceId());

			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, mCharCount);

		glBindVertexArray(0);
	mShader.UnUse();
}

void TextRenderer::RenderWireFrame()
{
	UpdateShaderStorageBuffer();

	mWireFrameShader.Use();
	glBindVertexArray(mVaoID);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, u_GlyphInfoBuffer, mShaderGlyphInfoBuffer.GetBufferId());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, u_PerInstanceDataBuffer, mShaderBuffer.GetBufferId());

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, mCharCount);

	glBindVertexArray(0);
	mWireFrameShader.UnUse();
}

void TextRenderer::ResizeBuffer(GLsizei characterCount)
{
	mCharCountBufferCapacity = characterCount;
	mShaderBuffer.Resize(GL_STATIC_DRAW, mCharCountBufferCapacity * sizeof(TextPage::TextLine::CharacterShaderData), nullptr);
	InvalidateShaderBuffer();
}

void TextRenderer::InvalidateShaderBuffer()
{
	mIsShaderBufferUpdated = false;
}

void TextRenderer::UpdateShaderStorageBuffer()
{
	if (mIsShaderBufferUpdated)
		return;
	
	mIsShaderBufferUpdated = true;

	GLenum target = GL_SHADER_STORAGE_BUFFER;

	glBindBuffer(target, mShaderBuffer.GetBufferId());
	TextPage::TextLine::CharacterShaderData * bufferBase = (TextPage::TextLine::CharacterShaderData *)glMapBuffer(target, GL_WRITE_ONLY);

	GLsizei index = 0;

	TextGroup::TextPageList & pageList = mActiveTextGroup->mTextPageList;

	for (TextGroup::TextPageList::iterator pageIt = pageList.begin(); pageIt != pageList.end() && index < mCharCountBufferCapacity; ++pageIt)
	{
		std::shared_ptr<TextPage> page = (*pageIt).lock();
		if (page == nullptr || !page->GetIsVisible())
		{
			continue;
		}

		for (TextPage::TextLineList::iterator lineIt = page->mTextLineList.begin(); lineIt != page->mTextLineList.end() && index < mCharCountBufferCapacity; ++lineIt)
		{
			TextPage::TextLine & textLine = *lineIt;

			textLine.mShaderBufferIndex = index;
			GLsizei index2 = index + (GLsizei)textLine.mText.size();
			GLsizei charToWrite = index2 <= mCharCountBufferCapacity ? (index2 - index) : (mCharCountBufferCapacity - index);

			memcpy(&bufferBase[index], textLine.mDataBuffer.data(), charToWrite * sizeof(TextPage::TextLine::CharacterShaderData));

			index = index2;
		}
	}

	mCharCount = index;

	glUnmapBuffer(target);
	glBindBuffer(target, 0);
}

void TextRenderer::InitializeShader()
{
	const char * attributeNames[__attributes_count__] =
	{
		"u_FontTextureSampler"
	};

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/text.vs.glsl");
	//mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/text.gs.glsl");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/text.forward.fs.glsl");
	mShader.CreateAndLinkProgram();
	mShader.Use();
		mShader.AddAttributes(attributeNames, __attributes_count__);

		glUniform1i(mShader.GetUniform(u_FontTextureSampler), 0); GL_CHECK_ERRORS;

		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	//setup shader
	mWireFrameShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/text.vs.glsl");
	//mWireFrameShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/text.gs.glsl");
	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/text.wireframe.fs.glsl");
	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();
	//mWireFrameShader.AddAttributes(attributeNames, __attributes_count__);
	mWireFrameShader.SetupFrameDataBlockBinding();
	mWireFrameShader.UnUse();

	GL_CHECK_ERRORS;

	const VAOBufferData quadVertices[] =
	{
		// Positions        
		{{0.0f, 1.0f},		{0, 0}},
		{{0.0f, 0.0f},		{0, 1}},
		{{1.0f, 1.0f},		{1, 0}},
		{{1.0f, 0.0f},		{1, 1}}
	};


	//setup vao and vbo stuff
	CreateBuffers();

	glBindVertexArray(mVaoID);

	glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
	glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, sizeof(VAOBufferData), (GLvoid*)0);
	GL_CHECK_ERRORS;

	glEnableVertexAttribArray(Shader::UV_ATTRIBUTE);
	glVertexAttribPointer(Shader::UV_ATTRIBUTE, 2, GL_INT, GL_FALSE, sizeof(VAOBufferData), (GLvoid*)offsetof(VAOBufferData, mTexCoordIndex));
	GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	mShaderGlyphInfoBuffer.CreateResource(GL_STATIC_DRAW, mGlyphInfoBuffer.size(), sizeof(ShaderGlyphInfo), mGlyphInfoBuffer.data(), sizeof(GlyphInfo));

	mShaderBuffer.CreateResource(GL_STATIC_DRAW, mCharCountBufferCapacity * sizeof(TextPage::TextLine::CharacterShaderData), nullptr);

	mIsInitialized = true;
}

void TextRenderer::Initialize(const Desc & desc)
{
	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize TextRenderer.....");

	mCharCountBufferCapacity = desc.mCharacterCountBufferCapacity;

	mLayerCount = 0;
	glm::uvec2 topLeft(0);

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

	//Engine::GetInstance()->DisplayTexture2DArray(mTexture, 0);

	PRINT_MESSAGE(" \t Layer count = %i", mLayerCount);

	InitializeShader();

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

	GLushort * lineHeight = nullptr;
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
	fi.mIndex = index;
	fi.mFileName = fontName;
	fi.mFamilyName = ftFace->family_name;
	fi.mStyleName = ftFace->style_name;
	fi.mStartLayerIndex = layerIndex;
	fi.mDesiredCharWidth = charWidth == 0 ? charHeight : charWidth;
	fi.mDesiredCharHeight = charHeight == 0 ? charWidth : charHeight;
	fi.mDefaultWidth = fi.mDesiredCharWidth * 64;
	fi.mBufferEntryIndex = (GLuint)mGlyphInfoBuffer.size();
	fi.mUndefinedCharIndex = (GLuint)mGlyphInfoBuffer.size();
	fi.mLineHeight = 0;

	lineHeight = &fi.mLineHeight;

	{
		GlyphInfo undefinedCharGlyphInfo;
		const glm::uvec2 zero(0);
		undefinedCharGlyphInfo.Set(zero, zero, mTextureSize, 0, GlyphInfo::Status::Undefined);
		mGlyphInfoBuffer.push_back(undefinedCharGlyphInfo);
	}

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

	{
		const GlyphMetrics * defaultChar = nullptr;
		std::wstring chars(L" HOKM#");
		for (std::wstring::const_iterator it = chars.begin(); it != chars.end() && defaultChar == nullptr; ++it)
		{
			defaultChar = fi.GetGlyph(*it);
		}

		GlyphInfo & undefinedCharGlyphInfo = mGlyphInfoBuffer[fi.mUndefinedCharIndex];
		glm::uvec2 c2;
		if (defaultChar != nullptr)
		{
			c2.x = GlyphMetrics::toPixel(defaultChar->mSize.x);
			c2.y = GlyphMetrics::toPixel(defaultChar->mSize.y);
		}
		else
		{
			c2.x = GlyphMetrics::toPixel(fi.mDefaultWidth);
			c2.y = GlyphMetrics::toPixel(fi.mLineHeight);
		}
		undefinedCharGlyphInfo.SetCorners(undefinedCharGlyphInfo.mLTCorner, c2, mTextureSize);
	}
	

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

bool TextRenderer::AddCharacterMetrics(FT_Face ftFace, FT_ULong ftCharCode, FT_UInt ftGlyphIndex, FontInfo & fi, GLushort & lineHeightMax, GLuint & currLineBitmapHeight, glm::uvec2 & topLeft, GLushort & layerIndex)
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

		if (ftCharCode == L' ')
		{
			if (ftFace->glyph->metrics.vertAdvance > lineHeightMax)
				lineHeightMax = (GLushort)ftFace->glyph->metrics.vertAdvance;

			GlyphMetrics gm;
			gm.mAdvance = glm::u16vec2(ftFace->glyph->metrics.horiAdvance, ftFace->glyph->metrics.vertAdvance);
			gm.mSize = gm.mAdvance;
			gm.mBearing = glm::ivec2(0, gm.mSize.y);
			gm.mGlyphInfoBufferIndex = (GLuint)mGlyphInfoBuffer.size();
#ifdef _DEBUG
			gm.mChar = (wchar_t)ftCharCode;
#endif // _DEBUG
			fi.mMapping[ftCharCode] = gm;

			GlyphInfo gi;
			const glm::uvec2 zero(0);
			gi.Set(zero, glm::uvec2(GlyphMetrics::toPixel(GlyphMetrics::floor(gm.mAdvance.x)), GlyphMetrics::toPixel(GlyphMetrics::floor(gm.mAdvance.y))), mTextureSize, 0, GlyphInfo::Status::WhiteSpace);
			mGlyphInfoBuffer.push_back(gi);
		}
		return true;
	}

	{
		if (ftFace->glyph->metrics.vertAdvance > lineHeightMax)
			lineHeightMax = (GLushort)ftFace->glyph->metrics.vertAdvance;

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

		bool ok = false;
		while (!ok)
		{
			if (topLeft.x >= mTextureSize.x || (topLeft.x + bitmapSize.x) > mTextureSize.x)
			{
				topLeft.x = 0;
				topLeft.y += (currLineBitmapHeight + 1);
			}

			if (bitmapSize.y > currLineBitmapHeight)
				currLineBitmapHeight = bitmapSize.y;

			if (topLeft.y >= mTextureSize.y || (topLeft.y + currLineBitmapHeight) > mTextureSize.y)
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
				topLeft.y += (currLineBitmapHeight + 1);
				currLineBitmapHeight = bitmapSize.y;
				continue;
			}

			if (bottomRight.y >= mTextureSize.y)
			{
				topLeft.x = 0;
				topLeft.y = 0;
				++layerIndex;
				currLineBitmapHeight = bitmapSize.y;
				continue;
			}

			ok = true;

			gi.Set(topLeft, bottomRight, mTextureSize, layerIndex, GlyphInfo::Status::Character);
			mGlyphInfoBuffer.push_back(gi);
			topLeft.x = bottomRight.x + 2;
		}

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

	mTexture = Engine::GetInstance()->GetTextureManager()->CreateTexture2DArray(1, GL_R8, mTextureSize.x, mTextureSize.y, mLayerCount, GL_NEAREST, GL_NEAREST, GL_CLAMP, GL_CLAMP);

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

			if (gi.GetLayerIndex() > layerIndex)
			{
				glTexSubImage3D(mTexture->GetTarget(), 0, 0, 0, layerIndex, mTextureSize.x, mTextureSize.y, 1, GL_RED, GL_UNSIGNED_BYTE, buffer); GL_CHECK_ERRORS;

				layerIndex = gi.GetLayerIndex();
				memset(buffer, 0, bufferSize);
			}

			if ((GlyphInfo::Status)gi.GetStatus() == GlyphInfo::Status::Character)
			{
				error = FT_Load_Char(ftFace, ftCharCode, FT_LOAD_RENDER);
				if (error != 0)
				{
					PRINT_ERROR("Could not load glyph for the character code=%li! (error=%li)", ftCharCode, error);
				}
				else
				{
					assert(gi.mRBCorner.x - gi.mLTCorner.x + 1 == ftFace->glyph->bitmap.width);
					assert(gi.mRBCorner.y - gi.mLTCorner.y + 1 == ftFace->glyph->bitmap.rows);

					std::uint8_t * dst = buffer + ((mTextureSize.y - gi.mLTCorner.y - 1) * mTextureSize.x) + gi.mLTCorner.x;
					std::uint8_t * src = ftFace->glyph->bitmap.buffer;
					for (auto y = gi.mLTCorner.y; y <= gi.mRBCorner.y; ++y)
					{
						GLuint n = (GLuint)gi.mRBCorner.x - (GLuint)gi.mLTCorner.x + 1;
							
						assert(ftFace->glyph->bitmap.width == n);
						assert(gi.mLTCorner.x + ftFace->glyph->bitmap.width <= mTextureSize.x);

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


void TextRenderer::BuildPageList()
{
	for (TextPageList::iterator it = mTextPageList.begin(); it != mTextPageList.end(); ++it)
	{
		std::shared_ptr<TextPage> page = *it;
		page->Build();
	}
}

void TextRenderer::TextPageDelete(TextPage * page)
{
	delete page;
}

TextPageWeakPtr TextRenderer::NewPage(bool isVisible, GLsizei textGroupIndex)
{
	TextGroupWeakPtr textGroupPtr = (textGroupIndex >= 0 && textGroupIndex < (GLsizei)mTextGroupList.size()) ? mTextGroupList[textGroupIndex] : mDefaultTextGroup;
	return NewPage(isVisible, textGroupPtr);
}

TextPageWeakPtr TextRenderer::NewPage(bool isVisible, TextGroupWeakPtr attachToThisTextGroup)
{
	if (!mIsInitialized)
	{
		PRINT_ERROR("[TextRenderer::NewPage] The text renderer is not initialized!");
		return TextPageWeakPtr();
	}

	std::shared_ptr<TextPage> page(new TextPage(isVisible, this), TextRenderer::TextPageDelete);
	mTextPageList.push_back(page);

	TextPageWeakPtr pagePtr = page;
	auto grpPtr = attachToThisTextGroup.lock();
	if (grpPtr == nullptr)
	{
		grpPtr = mDefaultTextGroup;
	}
	grpPtr->AttachPage(pagePtr, false);

	//if (isVisible && grpPtr == mActiveTextGroup)
	//{
	//	InvalidateShaderBuffer();
	//}

	return pagePtr;
}

void TextRenderer::DeletePage(TextPageWeakPtr page)
{
	auto ptr = page.lock();
	if (ptr == nullptr)
		return;

	bool isActivePage = ptr->GetIsVisible() && mActiveTextGroup->IsPageAttached(ptr);

	std::remove(mTextPageList.begin(), mTextPageList.end(), ptr);

	if (isActivePage)
	{
		InvalidateShaderBuffer();
	}
}

void TextRenderer::TextGroupDelete(TextGroup * textGroup)
{
	delete textGroup;
}

TextGroupWeakPtr TextRenderer::NewTextGroup()
{
	std::shared_ptr<TextGroup> ptr(new TextGroup(this), TextRenderer::TextGroupDelete);

	mTextGroupList.push_back(ptr);

	return ptr;
}

void TextRenderer::DeleteTextGroup(TextGroupWeakPtr textGroup)
{
	auto ptr = textGroup.lock();

	if (ptr == mDefaultTextGroup)
	{
		PRINT_ERROR("[TextRenderer::DeleteTextGroup] The default text group cannot be deleted!");
		return;
	}

	if (ptr == mActiveTextGroup)
	{
		SetActiveTextGroup(mDefaultTextGroup);
	}

	std::remove(mTextGroupList.begin(), mTextGroupList.end(), ptr);
}

TextGroupWeakPtr TextRenderer::GetTextGroup(GLsizei groupIndex) const
{
	if (((size_t)groupIndex) < mTextGroupList.size())
	{
		return mTextGroupList[groupIndex];
	}
	else
	{
		return TextGroupWeakPtr();
	}
}

void TextRenderer::SetActiveTextGroup(std::shared_ptr<TextGroup> groupPtr)
{
	if (groupPtr != nullptr)
	{
		if (mActiveTextGroup != groupPtr)
		{
			mActiveTextGroup = groupPtr;
			InvalidateShaderBuffer();
		}
	}
	else
	{
		if(mActiveTextGroup != mDefaultTextGroup)
		{
			mActiveTextGroup = mDefaultTextGroup;
			InvalidateShaderBuffer();
		}
	}
}

void TextRenderer::SetActiveTextGroup(TextGroupWeakPtr groupPtr)
{
	SetActiveTextGroup(groupPtr.lock());
}

void TextRenderer::SetActiveTextGroup(GLsizei index)
{
	if (index < 0 || index >= (GLsizei)mTextGroupList.size())
	{
		PRINT_ERROR("[TextRenderer::SetActiveTextGroup] Text group index=%i is not defined!", index);
		return;
	}

	SetActiveTextGroup(mTextGroupList[index]);
}

bool TextRenderer::IsActiveTextGroup(TextGroupWeakPtr groupPtr) const
{
	return mActiveTextGroup == groupPtr.lock();
}

bool TextRenderer::IsActiveTextGroup(GLsizei index) const
{
	if (index < 0 || index >= (GLsizei)mTextGroupList.size())
	{
		return false;
	}

	return mActiveTextGroup == mTextGroupList[index];
}

bool TextRenderer::IsAnActivePage(TextPageWeakPtr page) const
{
	return mActiveTextGroup->IsPageAttached(page);
}

bool TextRenderer::IsAnActivePage(const TextPage * page) const
{
	return mActiveTextGroup->IsPageAttached(page);
}

	} // namespace Renderers
} // namespace CoreFx