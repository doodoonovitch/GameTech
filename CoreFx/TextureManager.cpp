#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{


TextureManager::TextureManager()
	: mDefault2D(nullptr)
{
}


TextureManager::~TextureManager()
{
	Release();
}

void TextureManager::Release()
{
	ReleaseAllTexture2D();

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

struct RGBA
{
	GLubyte r, g, b, a;

	RGBA(GLubyte _r, GLubyte _g, GLubyte _b, GLubyte _a)
		: r(_r), g(_g), b(_b), a(_a)
	{}
};

void TextureManager::Initialize()
{
	const std::string filename("medias/uvtemplate.dds");
	GLuint id = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, 0, SOIL_FLAG_INVERT_Y);

	if (id == 0)
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

		GLuint id = SOIL_load_OGL_texture(s.c_str(), SOIL_LOAD_AUTO, 0, SOIL_FLAG_INVERT_Y);
		if (id != 0)
		{
			tex->SetId(id);
		}

		return tex;
	}
}

void TextureManager::ReleaseTexture2D(Texture2D const *& texture)
{
	texture = nullptr;
}

} // namespace CoreFx