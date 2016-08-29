#pragma once
#ifndef COREFX_TEXTUREMANAGER_H
#define COREFX_TEXTUREMANAGER_H


namespace CoreFx
{
	
	class Engine;



class TextureManager
{
public:

	Texture2D const * LoadTexture2D(std::string const &tiffFilename, GLenum wrapS = GL_REPEAT, GLenum wrapT = GL_REPEAT, bool generateMipMap = true);
	CubeMapTexture const * LoadTextureCubeMap(std::string const & prefixFilename, bool generateMipMap = true);

	struct LoadTextureGroupDesc
	{
		LoadTextureGroupDesc(const std::string & filename, bool invertY)
			: mFilename(filename)
			, mInvertY(invertY)
		{ }

		std::string mFilename;
		bool mInvertY;
	};

	TextureGroup const * LoadTextureGroup(TextureGroupId groupId, const std::vector<LoadTextureGroupDesc> & list, bool generateMipMap = true);

	Texture2DArray const * CreateTexture2DArray(GLsizei mipMapCount, GLenum format, GLsizei width, GLsizei height, GLsizei depth);
	

	void ReleaseTexture2D(Texture2D const *& texture);
	void ReleaseCubeMapTexture(CubeMapTexture const *& texture);

	void ReleaseTextureGroup(TextureGroup const *& texture);

	Texture2D const * GetDefaultTexture2D()	{ return mDefault2D; }

public:
	
	static bool LoadTiffImage(std::string const &tiffFilename, std::function<void(uint32_t w, uint32_t h, const uint32_t * raster)> func, uint32_t * desiredWidth = nullptr, uint32_t * desiredHeight = nullptr, bool invertY = true);

	static bool GetTiffImageSize(std::string const &tiffFilename, uint32_t & imgWidth, uint32_t & imgHeight);

	static GLsizei GetNumberOfMipMapLevels(uint32_t imgWidth, uint32_t imgHeight);

private:

	TextureManager();
	~TextureManager();

	void Initialize();
	void Release();

	void ReleaseAllTexture2D();
	void ReleaseAllCubeMapTexture();
	void ReleaseAllTextureGroup();
	void ReleaseAllTexture2DArray();

	TextureManager(TextureManager const &) = delete;
	TextureManager & operator=(TextureManager const &) = delete;

	template<typename TKey, typename TTexture>
	void ReleaseTextureMap(std::map<TKey, TTexture*> & texMap)
	{
		if (texMap.empty())
			return;

		size_t max = texMap.size();
		std::vector<GLuint> ids(max);
		size_t count = 0;
		for (std::map<TKey, TTexture*>::const_iterator it = texMap.begin(); it != texMap.end(); ++it)
		{
			if (it->second->GetResourceId() != mDefault2D->GetResourceId())
			{
				ids[count++] = it->second->GetResourceId();
			}

			delete it->second;
		}

		texMap.clear();

		glDeleteTextures((GLsizei)count, ids.data());
	}

	bool LoadTiffTex2D(GLuint & id, GLenum & target, std::string const &tiffFilename, GLenum wrapS = GL_REPEAT, GLenum wrapT = GL_REPEAT, bool generateMipMap = true);
	bool LoadTiffTexCubeMap(GLuint & id, GLenum & target, std::string tiffFilenames[6], bool generateMipMap = true);

private:

	Texture2D * mDefault2D;
	CubeMapTexture * mDefaultCubeMap;
	TextureGroup * mDefaultTexGroup;

	typedef std::map<std::string, Texture2D*> Tex2DIdMap;
	typedef std::map<std::string, CubeMapTexture*> CubeMapTexIdMap;
	typedef std::map<TextureGroupId, TextureGroup*> TexGroupMap;
	typedef std::map<Texture2DArray*, Texture2DArray*> Tex2DArrayMap;

	Tex2DIdMap m2DTexMap;
	CubeMapTexIdMap mCubeMapTexMap;
	TexGroupMap mTexGroupMap;
	Tex2DArrayMap mTex2DArrayMap;

	friend class Engine;
};

} // namespace CoreFx
#endif // COREFX_TEXTUREMANAGER_H