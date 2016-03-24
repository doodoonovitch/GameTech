#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{



TextureManager::TextureManager()
	: mDefault2D(nullptr)
	, mDefaultTexGroup(nullptr)
{
}


TextureManager::~TextureManager()
{
	Release();
}

void TextureManager::Release()
{
	ReleaseAllTexture2D();
	ReleaseAllTextureGroup();

	if (mDefault2D != nullptr)
	{
		glDeleteTextures(1, &mDefault2D->mId);
		SAFE_DELETE(mDefault2D);
	}
}

void TextureManager::ReleaseAllTexture2D()
{
	if (m2DTexMap.empty())
		return;

	size_t max = m2DTexMap.size();
	std::vector<GLuint> ids(max);
	size_t count = 0;
	for (Tex2DIdMap::const_iterator it = m2DTexMap.begin(); it != m2DTexMap.end(); ++it)
	{
		if (it->second->GetId() != mDefault2D->GetId())
		{
			ids[count++] = it->second->GetId();
		}

		delete it->second;
	}

	m2DTexMap.clear();

	glDeleteTextures((GLsizei)count, ids.data());
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
		if (it->second->GetId() != mDefault2D->GetId())
		{
			ids[count++] = it->second->GetId();
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
	const std::string filename("medias/uvtemplate.ktx");

	GLuint id;
	GLenum target;
	GLboolean isMipmapped;
	GLenum glerr;

	KTX_error_code err = ktxLoadTextureN(filename.c_str(), &id, &target, &mDefault2DDimensions, &isMipmapped, &glerr, nullptr, nullptr);

	if (err != KTX_SUCCESS)
	{
		printf("Cannot load the texture '%s' : %s.", filename.c_str(), ktxErrorString(err));

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

	mDefault2D = new Texture2D(id);

}

Texture2D const * TextureManager::LoadTexture2D(std::string const &filename)
{
	std::string s;
	s.resize(filename.size());
	std::transform(filename.begin(), filename.end(), s.begin(), std::tolower);

	Tex2DIdMap::const_iterator it = m2DTexMap.find(s);

	if (it != m2DTexMap.end())
	{
		return it->second;
	}
	else
	{
		Texture2D* tex = new Texture2D(mDefault2D->GetId());
		m2DTexMap[s] = tex;

		GLuint id;
		GLenum target;
		KTX_dimensions dimensions;
		GLboolean isMipmapped;
		GLenum glerr;

		KTX_error_code err = ktxLoadTextureN(filename.c_str(), &id, &target, &dimensions, &isMipmapped, &glerr, nullptr, nullptr);

		if (id != 0)
		{
			tex->SetId(id);
		}
		else
		{
			printf("Cannot load the texture '%s' : %s.", filename.c_str(), ktxErrorString(err));
		}

		return tex;
	}
}

void TextureManager::ReleaseTexture2D(Texture2D const *& texture)
{
	texture = nullptr;
}

void TextureManager::ReleaseTextureGroup(TextureGroup const *& texture)
{
	texture = nullptr;
}

TextureGroup const * TextureManager::LoadTextureGroup(TextureGroupId groupId, std::vector<std::string> tex2DFilenameList)
{
	TexGroupMap::const_iterator it = mTexGroupMap.find(groupId);
	if (it != mTexGroupMap.end())
		return it->second;

	if (tex2DFilenameList.empty())
	{
		printf("Error: Cannot load texture group : tex2DFilenameList is empty!\n");
		return mDefaultTexGroup;
	}

	KTX_error_code err;
	GLenum target;
	KTX_dimensions dimensions;
	GLboolean isMipmapped;
	GLenum glerr;


	KTX_header header;
	if (!KTX_ReadHeader(tex2DFilenameList[0].c_str(), header))
	{
		printf("Error: Cannot load texture group : reading header has failed!\n");
		return mDefaultTexGroup;
	}

	GLint layerCount = (GLint)tex2DFilenameList.size();

	GLuint id = 0;

	if (layerCount == 1)
	{
		err = ktxLoadTextureN(tex2DFilenameList.front().c_str(), &id, &target, &dimensions, &isMipmapped, &glerr, nullptr, nullptr);
		if (id != 0)
		{
			TextureGroup * texGroup = new TextureGroup(id, groupId, (GLint)header.numberOfArrayElements);
			mTexGroupMap[groupId] = texGroup;
			return texGroup;
		}
	}

	glGenTextures(1, &id); GL_CHECK_ERRORS;
	glBindTexture(GL_TEXTURE_2D_ARRAY, id); GL_CHECK_ERRORS;
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, header.numberOfMipmapLevels, header.glInternalFormat, header.pixelWidth, header.pixelHeight, layerCount); GL_CHECK_ERRORS;
	

	GLuint fbo = 0;
	glGenFramebuffers(1, &fbo); GL_CHECK_ERRORS;
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo); GL_CHECK_ERRORS;

	for (int index = 0; index < layerCount; ++index)
	{
		GLuint srcId = 0;
		const std::string & filename = tex2DFilenameList[index];
		err = ktxLoadTextureN(filename.c_str(), &srcId, &target, &dimensions, &isMipmapped, &glerr, nullptr, nullptr);
		if (srcId == 0)
		{
			srcId = mDefault2D->GetId();
		}

		glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, srcId, 0); GL_CHECK_ERRORS;
		glReadBuffer(GL_COLOR_ATTACHMENT0); GL_CHECK_ERRORS;

		uint32_t wCopied = 0, hCopied = 0;
		uint32_t x = 0, y = 0;
		while (hCopied < header.pixelHeight)
		{
			uint32_t h = min(header.pixelHeight, (uint32_t)dimensions.height);
			while (wCopied < header.pixelWidth)
			{
				uint32_t w = min(header.pixelWidth, (uint32_t)dimensions.width);

				glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, (GLint)x, (GLint)y, (GLint)index, (GLint)x, (GLint)y, (GLsizei)w, (GLsizei)h); GL_CHECK_ERRORS;
				wCopied += w;
				x = wCopied;
			}
			hCopied += h;
			y = hCopied;
		}
		glReadBuffer(GL_NONE); GL_CHECK_ERRORS;

		if (srcId != mDefault2D->GetId())
		{
			glDeleteTextures(1, &srcId);
		}
	}

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY); GL_CHECK_ERRORS;

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0); GL_CHECK_ERRORS;

	glDeleteFramebuffers(1, &fbo); GL_CHECK_ERRORS;

	TextureGroup * texGroup = new TextureGroup(id, groupId, layerCount);

	mTexGroupMap[groupId] = texGroup;

	return texGroup;
}

bool TextureManager::KTX_ReadHeader(FILE* f, KTX_header & header)
{
	static const char ktxIdentifierRef[12] = { '«', 'K', 'T', 'X', ' ', '1', '1', '»', '\r', '\n', '\x1A', '\n' };
	static const uint32_t ktxEndianRef = 0x04030201;
	static const uint32_t ktxEndianRefReverse = 0x01020304;

	if (fread(&header, 1, sizeof(KTX_header), f) != sizeof(KTX_header))
	{
		printf("Error: Cannot read KTX file header !\n");
		return false;
	}

	if (memcmp(&header.identifier, ktxIdentifierRef, sizeof(ktxIdentifierRef)) != 0)
	{
		printf("Error: Cannot read KTX file header : file identifier not found !\n");
		return false;
	}

	if (header.endianness == ktxEndianRefReverse)
	{

	}

	if (header.endianness != ktxEndianRef)
	{
		printf("Error: Cannot read KTX file header : wrong endianness!\n");
		return false;
	}

	return true;
}

bool TextureManager::KTX_ReadHeader(const char * filename, KTX_header & header)
{
	FILE * f;
	bool ret = false;

	if (fopen_s(&f, filename, "rb") == 0)
	{
		ret = KTX_ReadHeader(f, header);
		fclose(f);
	}
	else
	{
		printf("Error: Cannot open file '%s'!\n", filename);
	}

	return ret;
}


} // namespace CoreFx