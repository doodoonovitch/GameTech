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
	, mPerlinNoisePermutation(nullptr)
	, mPerlinNoisePermutation2D(nullptr)
	, mPerlinNoiseGradient(nullptr)
	, mPerlinNoiseGradient4D(nullptr)
	, mPerlinNoisePermutationGradient4D(nullptr)
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
	ReleaseAllTexture2DArray();

	ReleaseTexture(mDefault2D);
	ReleaseTexture(mDefaultCubeMap);
	ReleaseTexture(mDefaultTexGroup);
	ReleaseTexture(mPerlinNoisePermutation);
	ReleaseTexture(mPerlinNoisePermutation2D);
	ReleaseTexture(mPerlinNoiseGradient);
	ReleaseTexture(mPerlinNoisePermutationGradient);
}

void TextureManager::ReleaseAllTexture2D()
{
	ReleaseTextureMap(m2DTexMap);
}

void TextureManager::ReleaseAllCubeMapTexture()
{
	ReleaseTextureMap(mCubeMapTexMap);
}

void TextureManager::ReleaseAllTexture2DArray()
{
	ReleaseTextureMap(mTex2DArrayMap);
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
	InitializePerlinNoise();

	const std::string filename("medias/uvtemplate.tif");

	GLuint id = 0;
	GLenum target;
	GLsizei width, height;

	if (!LoadTiffTex2D(id, target, width, height, filename, GL_REPEAT, GL_REPEAT, true))
	{
		width = 128; height = 128;
		GLubyte* pData = (GLubyte*)malloc(width * height * 4);

		RGBA* ptr = (RGBA*)pData;
		for (auto i = 0; i < height; ++i)
		{
			float k = (float)i / (float)height;
			GLubyte c = (GLubyte)(k * 255);
			GLubyte a = c < 4 ? 4 : c;
			for (auto j = 0; j < width; ++j)
			{
				*ptr++ = RGBA(c, c, 255, a);
			}
		}

		target = GL_TEXTURE_2D;

		glGenTextures(1, &id);
		glBindTexture(target, id);
		CreateTexStorage2D(target, width, height, pData, true, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
		glBindTexture(target, 0);
		free(pData);
	}

	mDefault2D = new Texture2D(width, height, id, target);

	PRINT_MESSAGE("[TextureManager] Texture mDefault2D : %i.", id);

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

	if (!LoadTiffTexCubeMap(id, target, width, height, filenames, true))
	{
		width = 2; height = 2;
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
			CreateTexStorage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, width, height, buffer, false);
		}

		glBindTexture(target, 0);
	}
	mDefaultCubeMap = new CubeMapTexture(width, height, id, target);

}

void TextureManager::CreateTexStorage2D(GLenum target, uint32_t w, uint32_t h, const void * raster, bool generateMipMap, GLint internalFormat, GLenum rasterDataFormat, GLenum rasterDataType)
{
	//glTexImage2D(target, 0, internalFormat, w, h, 0, rasterDataFormat, rasterDataType, raster);
	GLsizei numLevels = generateMipMap ? (GLsizei)(1 + floor(log2(max(w, h)))) : 1;
	glTexStorage2D(target, numLevels, internalFormat, w, h);
	GL_CHECK_ERRORS;
	if (raster != nullptr)
	{
		glTexSubImage2D(target, 0, 0, 0, w, h, rasterDataFormat, rasterDataType, raster);
		GL_CHECK_ERRORS;
	}

	if (generateMipMap)
	{
		glGenerateMipmap(target);
		GL_CHECK_ERRORS;
	}
}

void TextureManager::CreateTexStorage2D(GLenum target, uint32_t w, uint32_t h, const void * raster, bool generateMipMap, GLenum texMinFilter, GLenum texMagFilter, GLenum wrapS, GLenum wrapT, GLint internalFormat, GLenum rasterDataFormat, GLenum rasterDataType)
{
	CreateTexStorage2D(target, w, h, raster, generateMipMap, internalFormat, rasterDataFormat, rasterDataType);

	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);

	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, texMinFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, texMagFilter);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	GL_CHECK_ERRORS;
}

bool TextureManager::LoadTiffTex2D(GLuint & id, GLenum & target, GLsizei & width, GLsizei & height, std::string const &tiffFilename, GLenum wrapS, GLenum wrapT, bool generateMipMap)
{
	id = 0;
	target = GL_TEXTURE_2D;
	bool loaded = LoadTiffImage(width, height, tiffFilename, [&id, target, wrapS, wrapT, generateMipMap](uint32_t w, uint32_t h, const uint32_t * raster)
	{
		glGenTextures(1, &id);
		glBindTexture(target, id);
		CreateTexStorage2D(target, w, h, raster, generateMipMap, generateMipMap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR, GL_LINEAR, wrapS, wrapT);
		glBindTexture(target, 0);
	});

	return loaded;
}

Texture2D const * TextureManager::LoadTexture2D(std::string const &tiffFilename, GLenum wrapS, GLenum wrapT, bool generateMipMap)
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
		GLsizei width, height;
		if (LoadTiffTex2D(id, target, width, height, tiffFilename, wrapS, wrapT, generateMipMap))
		{
			returnTexture = new Texture2D(width, height, id, target);
		}
		else
		{
			returnTexture = new Texture2D(mDefault2D->GetWidth(), mDefault2D->GetHeight(), mDefault2D->GetResourceId(), mDefault2D->GetTarget());
		}
		m2DTexMap[s] = returnTexture;
	}

	return returnTexture;
}

bool TextureManager::LoadTiffImage(GLsizei & width, GLsizei & height, std::string const &tiffFilename, std::function<void(uint32_t w, uint32_t h, const uint32_t * raster)> func, uint32_t * desiredWidth, uint32_t * desiredHeight, bool invertY)
{
	bool loaded = false;

	TIFF* tif = TIFFOpen(tiffFilename.c_str(), "r");
	if (tif)
	{
		size_t npixels;
		uint32_t * raster;
		if (desiredWidth != nullptr)
			width = *desiredWidth;
		else
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
		if (desiredHeight != nullptr)
			height = *desiredHeight;
		else
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
		npixels = width * height;
		raster = (uint32*)_TIFFmalloc(npixels * sizeof(uint32));
		if (raster != NULL)
		{
			if (TIFFReadRGBAImageOriented(tif, width, height, raster, invertY ? ORIENTATION_TOPLEFT : ORIENTATION_BOTLEFT, 0))
			{
				if(func != nullptr)
					func(width, height, raster);
				loaded = true;
			}
			else
			{
				PRINT_ERROR("Loading texture '%s' has failed : cannot read the image data.", tiffFilename.c_str())
			}
			_TIFFfree(raster);
		}
		else
		{
			PRINT_ERROR("Loading texture '%s' has failed : cannot allocate memory.", tiffFilename.c_str())
		}
		TIFFClose(tif);
	}
	else
	{
		PRINT_ERROR("Loading texture '%s' has failed : cannot open the file.", tiffFilename.c_str())
	}

	return loaded;
}

bool TextureManager::LoadTiffTexCubeMap(GLuint & id, GLenum & target, GLsizei & width, GLsizei & height, std::string tiffFilenames[6], bool generateMipMap)
{
	target = GL_TEXTURE_CUBE_MAP;
	glGenTextures(1, &id);
	glBindTexture(target, id);

	for (int i = 0; i < 6; ++i)
	{
		uint32_t desiredW = 0, desiredH = 0;
		GLenum trg = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		bool loaded = LoadTiffImage(width, height, tiffFilenames[i], [trg, &desiredW, &desiredH](uint32_t w, uint32_t h, const uint32_t * raster)
		{
			//CreateTexStorage2D(trg, w, h, raster, true);
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
	{
		glGenerateMipmap(target);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

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

		GLsizei width, height;

		if (LoadTiffTexCubeMap(id, target, width, height, filenames, generateMipMap))
		{
			returnTexture = new CubeMapTexture(width, height, id, target);
		}
		else
		{
			returnTexture = new CubeMapTexture(mDefaultCubeMap->GetWidth(), mDefaultCubeMap->GetHeight(), mDefaultCubeMap->GetResourceId(), mDefaultCubeMap->GetTarget());
		}
		mCubeMapTexMap[s] = returnTexture;
	}
	
	return returnTexture;
}

CubeMapTexture const * TextureManager::CreateTextureCubeMap(std::string const & textureBaseName, GLsizei textureSize, GLint internalFormat, bool generateMipMap)
{
	CubeMapTexture * returnTexture = nullptr;

	GLuint id = 0;
	GLenum target = GL_TEXTURE_CUBE_MAP;

	GLsizei numLevels = generateMipMap ? (GLsizei)(1 + floor(log2(textureSize))) : 1;

	glGenTextures(1, &id);
	glBindTexture(target, id);

	glTexStorage2D(target, numLevels, internalFormat, textureSize, textureSize);
	GL_CHECK_ERRORS;

	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if (generateMipMap)
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(target, 0);

	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
	returnTexture = new CubeMapTexture(textureSize, textureSize, id, target);
	const size_t bufferSize = 60;
	char tmp[bufferSize + 1];
	int ret = sprintf_s(tmp, bufferSize, "_%p", returnTexture);
	std::string name;
	if (ret != -1)
	{
		tmp[ret] = 0;
		name = textureBaseName + tmp;
	}
	else
	{
		name = textureBaseName + "_some_errors_has_occured";
	}

	mCubeMapTexMap[name] = returnTexture;

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

Texture2DArray const * TextureManager::CreateTexture2DArray(GLsizei mipMapCount, GLenum format, GLsizei width, GLsizei height, GLsizei depth)
{
	GLuint id = 0;
	GLenum target = GL_TEXTURE_2D_ARRAY;

	glGenTextures(1, &id); GL_CHECK_ERRORS;
	glBindTexture(target, id); GL_CHECK_ERRORS;
	glTexStorage3D(target, mipMapCount, format, width, height, depth); GL_CHECK_ERRORS;

	glBindTexture(target, 0); GL_CHECK_ERRORS;

	Texture2DArray * texture = new Texture2DArray(width, height, depth, id, target);
	mTex2DArrayMap[texture] = texture;

	return texture;
}

TextureGroup const * TextureManager::LoadTextureGroup(TextureGroupId groupId, const std::vector<LoadTextureGroupDesc> & list, bool generateMipMap)
{
	TexGroupMap::const_iterator it = mTexGroupMap.find(groupId);
	if (it != mTexGroupMap.end())
		return it->second;

	if (list.empty())
	{
		PRINT_ERROR("Cannot load texture group : tex2DFilenameList is empty!");
		return mDefaultTexGroup;
	}

	uint32_t width = TextureInfo::GetWidthFromGroupId(groupId);
	uint32_t height = TextureInfo::GetHeightFromGroupId(groupId);
	size_t npixels = width * height;

	GLsizei mipMapCount = generateMipMap ? GetNumberOfMipMapLevels(width, height) : 1;

	GLint layerCount = (GLint)list.size();

	GLuint id = 0;
	GLenum target = GL_TEXTURE_2D_ARRAY;

	glGenTextures(1, &id); GL_CHECK_ERRORS;
	glBindTexture(target, id); GL_CHECK_ERRORS;
	glTexStorage3D(target, mipMapCount, GL_RGBA8, width, height, layerCount); GL_CHECK_ERRORS;

	PRINT_MESSAGE("[LoadTextureGroup] Texture group '%I64x' (Cat=%i) : %i.", groupId, (int)TextureInfo::GetCategoryFromGroupId(groupId), id);

	for (int index = 0; index < layerCount; ++index)
	{
		const LoadTextureGroupDesc & texDesc = list[index];
		GLsizei w, h;
		bool loaded = LoadTiffImage(w, h, texDesc.mFilename, [target, index, &texDesc, groupId](uint32_t w, uint32_t h, const uint32_t * raster)
		{
			glTexSubImage3D(target, 0, 0, 0, index, w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE, raster); GL_CHECK_ERRORS;
			PRINT_MESSAGE("[LoadTextureGroup] Texture group '%I64x' (Cat=%i) : image '%s' loaded.", groupId, (int)TextureInfo::GetCategoryFromGroupId(groupId), texDesc.mFilename.c_str());
		}, &width, &height, texDesc.mInvertY);
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

void TextureManager::InitializePerlinNoise()
{
	static uint8_t permutation[] = 
	{ 
		151,	160,	137,	91,		90,		15,		131,	13,
		201,	95,		96,		53,		194,	233,	7,		225,
		140,	36,		103,	30,		69,		142,	8,		99,
		37,		240,	21,		10,		23,		190,	6,		148,
		247,	120,	234,	75,		0,		26,		197,	62,
		94,		252,	219,	203,	117,	35,		11,		32,
		57,		177,	33,		88,		237,	149,	56,		87,
		174,	20,		125,	136,	171,	168,	68,		175,
		74,		165,	71,		134,	139,	48,		27,		166,
		77,		146,	158,	231,	83,		111,	229,	122,
		60,		211,	133,	230,	220,	105,	92,		41,
		55,		46,		245,	40,		244,	102,	143,	54,
		65,		25,		63,		161,	1,		216,	80,		73,
		209,	76,		132,	187,	208,	89,		18,		169,
		200,	196,	135,	130,	116,	188,	159,	86,
		164,	100,	109,	198,	173,	186,	3,		64,
		52,		217,	226,	250,	124,	123,	5,		202,
		38,		147,	118,	126,	255,	82,		85,		212,
		207,	206,	59,		227,	47,		16,		58,		17,
		182,	189,	28,		42,		223,	183,	170,	213,
		119,	248,	152,	2,		44,		154,	163,	70,
		221,	153,	101,	155,	167,	43,		172,	9,
		129,	22,		39,		253,	19,		98,		108,	110,
		79,		113,	224,	232,	178,	185,	112,	104,
		218,	246,	97,		228,	251,	34,		242,	193,
		238,	210,	144,	12,		191,	179,	162,	241,
		81,		51,		145,	235,	249,	14,		239,	107,
		49,		192,	214,	31,		181,	199,	106,	157,
		184,	84,		204,	176,	115,	121,	50,		45,
		127,	4,		150,	254,	138,	236,	205,	93,
		222,	114,	67,		29,		24,		72,		243,	141,
		128,	195,	78,		66,		215,	61,		156,	180
	};

	{
		GLuint id = 0;
		GLenum target = GL_TEXTURE_1D;
		glGenTextures(1, &id);
		glBindTexture(target, id);

		//allocate texture 
		glTexImage1D(target, 0, GL_RED, 256, 0, GL_RED, GL_UNSIGNED_BYTE, permutation);
		GL_CHECK_ERRORS;

		//set texture parameters
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		GL_CHECK_ERRORS;

		glBindTexture(target, 0);

		mPerlinNoisePermutation = new Texture1D(256, id, target);
	}

	{
		glm::uvec4 * permutation2D = new glm::uvec4[256 * 256];
		for (int v = 0; v < 256; ++v)
		{
			for (int u = 0; u < 256; ++u)
			{
				glm::uvec2 p(u, v);
				int A = permutation[p.x] + p.y;
				int AA = permutation[A & 255];
				int AB = permutation[(A + 1) & 255];
				int B = permutation[(p.x + 1) & 255] + p.y;
				int BA = permutation[B & 255];
				int BB = permutation[(B + 1) & 255];
				permutation2D[v * 256 + u] = glm::uvec4(AA, AB, BA, BB) / 255.0;
			}
		}

		GLuint id = 0;
		GLenum target = GL_TEXTURE_2D;
		glGenTextures(1, &id);
		glBindTexture(target, id);
		//set texture parameters
		//glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//allocate texture 
		CreateTexStorage2D(target, 256, 256, permutation2D, false, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
		//glTexImage2D(target, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, permutation2D);
		GL_CHECK_ERRORS;

		glBindTexture(target, 0);

		delete[] permutation2D;

		mPerlinNoisePermutation2D = new Texture2D(256, 256, id, target);
	}

	static float gradients[] =
	{
		1,	 1,	 0,
		-1,	 1,	 0,
		1,	-1,	 0,
		-1,	-1,	 0,
		1,	 0,	 1,
		-1,	 0,	 1,
		1,	 0,	-1,
		-1,	 0,	-1,
		0,	 1,	 1,
		0,	-1,	 1,
		0,	 1,	-1,
		0,	-1,	-1,
		1,	 1,	 0,
		0,	-1,	 1,
		-1,	 1,	 0,
		0,	-1,	-1
	};
	{
		GLuint id = 0;
		GLenum target = GL_TEXTURE_1D;
		glGenTextures(1, &id);
		glBindTexture(target, id);

		//allocate texture 
		glTexImage1D(target, 0, GL_RGB, 16, 0, GL_RGB, GL_FLOAT, gradients);
		GL_CHECK_ERRORS;

		//set texture parameters
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		GL_CHECK_ERRORS;

		glBindTexture(target, 0);

		mPerlinNoiseGradient = new Texture1D(16, id, target);
	}

	{
		float permutationGradients[256 * 3];
		for (int i = 0; i < 256; ++i)
		{
			memcpy(&permutationGradients[i * 3], &gradients[(permutation[i] & 15) * 3], sizeof(float) * 3);
		}

		GLuint id = 0;
		GLenum target = GL_TEXTURE_1D;
		glGenTextures(1, &id);
		glBindTexture(target, id);

		//allocate texture 
		glTexImage1D(target, 0, GL_RGB, 256, 0, GL_RGB, GL_FLOAT, permutationGradients);
		GL_CHECK_ERRORS;

		//set texture parameters
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		GL_CHECK_ERRORS;

		glBindTexture(target, 0);

		mPerlinNoisePermutationGradient = new Texture1D(256, id, target);
	}

	// gradients for 4D noise
	static float gradients4D[] = 
	{
		0, -1, -1, -1,
		0, -1, -1, 1,
		0, -1, 1, -1,
		0, -1, 1, 1,
		0, 1, -1, -1,
		0, 1, -1, 1,
		0, 1, 1, -1,
		0, 1, 1, 1,
		-1, -1, 0, -1,
		-1, 1, 0, -1,
		1, -1, 0, -1,
		1, 1, 0, -1,
		-1, -1, 0, 1,
		-1, 1, 0, 1,
		1, -1, 0, 1,
		1, 1, 0, 1,

		-1, 0, -1, -1,
		1, 0, -1, -1,
		-1, 0, -1, 1,
		1, 0, -1, 1,
		-1, 0, 1, -1,
		1, 0, 1, -1,
		-1, 0, 1, 1,
		1, 0, 1, 1,
		0, -1, -1, 0,
		0, -1, -1, 0,
		0, -1, 1, 0,
		0, -1, 1, 0,
		0, 1, -1, 0,
		0, 1, -1, 0,
		0, 1, 1, 0,
		0, 1, 1, 0,
	};

	{
		GLuint id = 0;
		GLenum target = GL_TEXTURE_1D;
		glGenTextures(1, &id);
		glBindTexture(target, id);
		//allocate texture 
		glTexImage1D(target, 0, GL_RGBA, 32, 0, GL_RGBA, GL_FLOAT, gradients4D);
		GL_CHECK_ERRORS;

		//set texture parameters
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		GL_CHECK_ERRORS;

		glBindTexture(target, 0);

		mPerlinNoiseGradient4D = new Texture1D(32, id, target);
	}

	{
		float permutationGradients4D[256 * 4];
		for (int i = 0; i < 256; ++i)
		{
			memcpy(&permutationGradients4D[i * 4], &gradients4D[(permutation[i] & 31) * 4], sizeof(float) * 4);
		}

		GLuint id = 0;
		GLenum target = GL_TEXTURE_1D;
		glGenTextures(1, &id);
		glBindTexture(target, id);

		//allocate texture 
		glTexImage1D(target, 0, GL_RGBA, 256, 0, GL_RGBA, GL_FLOAT, permutationGradients4D);
		GL_CHECK_ERRORS;

		//set texture parameters
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		GL_CHECK_ERRORS;

		glBindTexture(target, 0);

		mPerlinNoisePermutationGradient4D = new Texture1D(256, id, target);
	}

}


} // namespace CoreFx