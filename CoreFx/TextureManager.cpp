#include "stdafx.h"
#include "CoreFx.h"
#include "tiffio.h"

namespace CoreFx
{

	void OnLibTIFFWarning(const char* title, const char* msgFmt, va_list args)
	{
		PRINT_WARNING("[Lib tiff] : %s.", title);
		PRINT_WARNING_VA(msgFmt, args);
	}

	void OnLibTIFFError(const char* title, const char* msgFmt, va_list args)
	{
		PRINT_ERROR("[Lib tiff] : %s", title);
		PRINT_ERROR_VA(msgFmt, args);
	}

TextureManager::TextureManager()
	: mDefault2D(nullptr)
	, mDefaultCubeMap(nullptr)
	, mDefaultTexGroup(nullptr)
{
	TIFFSetWarningHandler(OnLibTIFFWarning);
	TIFFSetErrorHandler(OnLibTIFFError);
}


TextureManager::~TextureManager()
{
	Release();
}

void TextureManager::Release()
{
	ReleaseAllTexture2D();
	ReleaseAllCubeMapTexture();
	ReleaseAllTextureGroup();

	if (mDefault2D != nullptr)
	{
		glDeleteTextures(1, &mDefault2D->mId);
		SAFE_DELETE(mDefault2D);
	}
}

void TextureManager::ReleaseAllTexture2D()
{
	ReleaseTextureMap(m2DTexMap);
}

void TextureManager::ReleaseAllCubeMapTexture()
{
	ReleaseTextureMap(mCubeMapTexMap);
}

void TextureManager::ReleaseAllTextureGroup()
{
	if (mTexGroupMap.empty())
		return;

	size_t max = mTexGroupMap.size();
	std::vector<GLuint> ids(max);
	size_t count = 0;
	for (TexGroupMap::const_iterator it = mTexGroupMap.begin(); it != mTexGroupMap.end(); ++it)
	{
		//if (it->second->GetResourceId() != mDefaultTexGroup->GetResourceId())
		{
			ids[count++] = it->second->GetResourceId();
		}

		delete it->second;
	}

	mTexGroupMap.clear();

	glDeleteTextures((GLsizei)count, ids.data());
}

struct RGBA
{
	GLubyte r, g, b, a;

	RGBA(GLubyte _r, GLubyte _g, GLubyte _b, GLubyte _a)
		: r(_r), g(_g), b(_b), a(_a)
	{}
};

void TextureManager::Initialize()
{
	const std::string filename("medias/uvtemplate.tif");

	GLuint id = 0;
	GLenum target;

	if (!LoadTiffTex2D(id, target, filename, true))
	{
		GLsizei w = 128, h = 128;
		GLubyte* pData = (GLubyte*)malloc(w * h * 4);

		RGBA* ptr = (RGBA*)pData;
		for (auto i = 0; i < h; ++i)
		{
			float k = (float)i / (float)h;
			GLubyte c = (GLubyte)(k * 255);
			GLubyte a = c < 4 ? 4 : c;
			for (auto j = 0; j < w; ++j)
			{
				*ptr++ = RGBA(c, c, 255, a);
			}
		}

		target = GL_TEXTURE_2D;

		glGenTextures(1, &id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, id);
		//set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//allocate texture 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		free(pData);
	}

	mDefault2D = new Texture2D(id, target);

	PRINT_MESSAGE("[TextureManager] Texture mDefault2D : %i.\n", id);

	id = 0;
	target = GL_TEXTURE_CUBE_MAP;
	const std::string prefixFilename("medias/CubeMaps/uvCubeMap");
	std::string filenames[6];
	filenames[0] = prefixFilename + "_+X.tif";
	filenames[1] = prefixFilename + "_-X.tif";
	filenames[2] = prefixFilename + "_+Y.tif";
	filenames[3] = prefixFilename + "_-Y.tif";
	filenames[4] = prefixFilename + "_+Z.tif";
	filenames[5] = prefixFilename + "_-Z.tif";

	if (!LoadTiffTexCubeMap(id, target, filenames, true))
	{
		target = GL_TEXTURE_CUBE_MAP;
		glGenTextures(1, &id);
		glBindTexture(target, id);

		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		const uint32_t colors[] = 
		{
			0xFFFF6A00,
			0xFFFFB27F,
			0xFF0094FF,
			0xFF7F3F5B,
			0xFF00FF90,
			0xFFB200FF
		};

		uint32_t buffer[4];

		for (int i = 0; i < 6; ++i)
		{
			memset(buffer, colors[i], sizeof(buffer));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		}

		glBindTexture(target, 0);
	}
	mDefaultCubeMap = new CubeMapTexture(id, target);

}

bool TextureManager::LoadTiffTex2D(GLuint & id, GLenum & target, std::string const &tiffFilename, bool generateMipMap)
{
	id = 0;
	target = GL_TEXTURE_2D;
	bool loaded = LoadTiffImage(tiffFilename, [&id, target, generateMipMap](uint32_t w, uint32_t h, const uint32_t * raster)
	{
		glGenTextures(1, &id);
		glBindTexture(target, id);

		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if (generateMipMap)
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		else
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexImage2D(target, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, raster);

		if (generateMipMap)
			glGenerateMipmap(target);

		glBindTexture(target, 0);
	});

	return loaded;
}

Texture2D const * TextureManager::LoadTexture2D(std::string const &tiffFilename, bool generateMipMap)
{
	Texture2D * returnTexture = nullptr;

	std::string s;
	s.resize(tiffFilename.size());
	std::transform(tiffFilename.begin(), tiffFilename.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });

	Tex2DIdMap::const_iterator it = m2DTexMap.find(s);

	if (it != m2DTexMap.end())
	{
		returnTexture = it->second;
	}
	else
	{
		GLuint id = 0;
		GLenum target = GL_TEXTURE_2D;

		if (LoadTiffTex2D(id, target, tiffFilename, generateMipMap))
		{
			returnTexture = new Texture2D(id, target);
		}
		else
		{
			returnTexture = new Texture2D(mDefault2D->GetResourceId(), mDefault2D->GetTarget());
		}
		m2DTexMap[s] = returnTexture;
	}

	return returnTexture;
}

bool TextureManager::LoadTiffImage(std::string const &tiffFilename, std::function<void(uint32_t w, uint32_t h, const uint32_t * raster)> func, uint32_t * desiredWidth, uint32_t * desiredHeight, bool invertY)
{
	bool loaded = false;

	TIFF* tif = TIFFOpen(tiffFilename.c_str(), "r");
	if (tif)
	{
		uint32_t w, h;
		size_t npixels;
		uint32_t * raster;
		if (desiredWidth != nullptr)
			w = *desiredWidth;
		else
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
		if (desiredHeight != nullptr)
			h = *desiredHeight;
		else
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
		npixels = w * h;
		raster = (uint32*)_TIFFmalloc(npixels * sizeof(uint32));
		if (raster != NULL)
		{
			if (TIFFReadRGBAImageOriented(tif, w, h, raster, invertY ? ORIENTATION_TOPLEFT : ORIENTATION_BOTLEFT, 0))
			{
				if(func != nullptr)
					func(w, h, raster);
				loaded = true;
			}
			else
			{
				PRINT_ERROR("Loading texture '%s' has failed : cannot read the image data.\n", tiffFilename.c_str())
			}
			_TIFFfree(raster);
		}
		else
		{
			PRINT_ERROR("Loading texture '%s' has failed : cannot allocate memory.\n", tiffFilename.c_str())
		}
		TIFFClose(tif);
	}
	else
	{
		PRINT_ERROR("Loading texture '%s' has failed : cannot open the file.\n", tiffFilename.c_str())
	}

	return loaded;
}

bool TextureManager::LoadTiffTexCubeMap(GLuint & id, GLenum & target, std::string tiffFilenames[6], bool generateMipMap)
{
	target = GL_TEXTURE_CUBE_MAP;
	glGenTextures(1, &id);
	glBindTexture(target, id);

	for (int i = 0; i < 6; ++i)
	{
		uint32_t desiredW = 0, desiredH = 0;
		GLenum trg = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		bool loaded = LoadTiffImage(tiffFilenames[i], [trg, &desiredW, &desiredH](uint32_t w, uint32_t h, const uint32_t * raster)
		{
			glTexImage2D(trg, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, raster);
			if (desiredW == 0)
				desiredW = w;
			if (desiredH == 0)
				desiredH = h;
		}, desiredW != 0 ? &desiredW : nullptr, desiredH != 0 ? &desiredH : nullptr);
		if(!loaded)
		{
			glDeleteTextures(1, &id);
			id = 0;
			return false;
		}
	}
	
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if (generateMipMap)
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	else
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (generateMipMap)
		glGenerateMipmap(target);

	glBindTexture(target, 0);

	return true;
}

CubeMapTexture const * TextureManager::LoadTextureCubeMap(std::string const & prefixFilename, bool generateMipMap)
{
	CubeMapTexture * returnTexture = nullptr;
	std::string s;
	s.resize(prefixFilename.size());
	std::transform(prefixFilename.begin(), prefixFilename.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });

	CubeMapTexIdMap::const_iterator it = mCubeMapTexMap.find(s);

	if (it != mCubeMapTexMap.end())
	{
		returnTexture = it->second;
	}
	else
	{
		GLuint id = 0;
		GLenum target = GL_TEXTURE_CUBE_MAP;

		std::string filenames[6];
		filenames[0] = prefixFilename + "_+X.tif";
		filenames[1] = prefixFilename + "_-X.tif";
		filenames[2] = prefixFilename + "_+Y.tif";
		filenames[3] = prefixFilename + "_-Y.tif";
		filenames[4] = prefixFilename + "_+Z.tif";
		filenames[5] = prefixFilename + "_-Z.tif";

		if (LoadTiffTexCubeMap(id, target, filenames, generateMipMap))
		{
			returnTexture = new CubeMapTexture(id, target);
		}
		else
		{
			returnTexture = new CubeMapTexture(mDefaultCubeMap->GetResourceId(), mDefaultCubeMap->GetTarget());
		}
		mCubeMapTexMap[s] = returnTexture;
	}
	
	return returnTexture;
}

void TextureManager::ReleaseTexture2D(Texture2D const *& texture)
{
	texture = nullptr;
}

void TextureManager::ReleaseCubeMapTexture(CubeMapTexture const *& texture)
{
	texture = nullptr;
}

void TextureManager::ReleaseTextureGroup(TextureGroup const *& texture)
{
	texture = nullptr;
}

TextureGroup const * TextureManager::LoadTextureGroup(TextureGroupId groupId, std::vector<std::string> tex2DFilenameList, bool generateMipMap)
{
	TexGroupMap::const_iterator it = mTexGroupMap.find(groupId);
	if (it != mTexGroupMap.end())
		return it->second;

	if (tex2DFilenameList.empty())
	{
		PRINT_ERROR("Cannot load texture group : tex2DFilenameList is empty!\n");
		return mDefaultTexGroup;
	}

	uint32_t width = TextureInfo::GetWidthFromGroupId(groupId);
	uint32_t height = TextureInfo::GetHeightFromGroupId(groupId);
	size_t npixels = width * height;

	GLsizei mipMapCount = generateMipMap ? GetNumberOfMipMapLevels(width, height) : 1;

	GLint layerCount = (GLint)tex2DFilenameList.size();

	GLuint id = 0;
	GLenum target = GL_TEXTURE_2D_ARRAY;

	glGenTextures(1, &id); GL_CHECK_ERRORS;
	glBindTexture(target, id); GL_CHECK_ERRORS;
	glTexStorage3D(target, mipMapCount, GL_RGBA8, width, height, layerCount); GL_CHECK_ERRORS;

	PRINT_MESSAGE("[LoadTextureGroup] Texture group '%I64x' (Cat=%i) : %i.\n", groupId, (int)TextureInfo::GetCategoryFromGroupId(groupId), id);

	for (int index = 0; index < layerCount; ++index)
	{
		bool loaded = LoadTiffImage(tex2DFilenameList[index], [target, index](uint32_t w, uint32_t h, const uint32_t * raster)
		{
			glTexSubImage3D(target, 0, 0, 0, index, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, raster); GL_CHECK_ERRORS;
		}, &width, &height);
		if (!loaded)
		{
			uint32_t * ptr = new uint32_t[npixels];
			memset(ptr, 0xFFFF00DC, npixels * sizeof(uint32_t));
			glTexSubImage3D(target, 0, 0, 0, index, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, ptr); GL_CHECK_ERRORS;
			delete[] ptr;
		}
	}

	TextureGroup * texGroup = new TextureGroup(id, groupId, target, layerCount);

	TextureWrap wrapS = TextureInfo::GetWrapSFromGroupId(texGroup->GetGroupId());
	TextureWrap wrapT = TextureInfo::GetWrapTFromGroupId(texGroup->GetGroupId());

	if(generateMipMap)
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	else
		glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(target, GL_TEXTURE_WRAP_S, TextureInfo::FromTextureWrap(wrapS));
	glTexParameteri(target, GL_TEXTURE_WRAP_T, TextureInfo::FromTextureWrap(wrapT));
	glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, mipMapCount);
	
	glGenerateMipmap(target); GL_CHECK_ERRORS;

	glBindTexture(target, 0); GL_CHECK_ERRORS;

	mTexGroupMap[groupId] = texGroup;

	return texGroup;
}



GLsizei TextureManager::GetNumberOfMipMapLevels(uint32_t imgWidth, uint32_t imgHeight)
{
	return (GLsizei)(1 + floorf(log((float)max(imgWidth, imgHeight)) / log(2.f)));
}

bool TextureManager::GetTiffImageSize(std::string const &tiffFilename, uint32_t & imgWidth, uint32_t & imgHeight)
{
	bool loaded = false;

	TIFF* tif = TIFFOpen(tiffFilename.c_str(), "r");
	if (tif)
	{
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imgWidth);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imgHeight);

		TIFFClose(tif);

		loaded = true;
	}

	return loaded;
}

} // namespace CoreFx