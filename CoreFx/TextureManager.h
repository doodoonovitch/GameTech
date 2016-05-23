#pragma once
#ifndef COREFX_TEXTUREMANAGER_H
#define COREFX_TEXTUREMANAGER_H


namespace CoreFx
{
	
	class Engine;



class TextureManager
{
public:

	Texture2D const * LoadTexture2D(std::string const &ktxFilename);
	CubeMapTexture const * LoadTextureCubeMap(std::string const &ktxFilename);

	TextureGroup const * LoadTextureGroup(TextureGroupId groupId, std::vector<std::string> filenames);

	TextureGroup const * LoadTexture2DArrayAsTextureGroup(uint16_t rendererId, TextureCategory category, TextureWrap wrapS, TextureWrap wrapT, std::string const & filename);


	void ReleaseTexture2D(Texture2D const *& texture);
	void ReleaseCubeMapTexture(CubeMapTexture const *& texture);

	void ReleaseTextureGroup(TextureGroup const *& texture);

	Texture2D const * GetDefaultTexture2D()	{ return mDefault2D;	}

public:

	typedef struct KTX_header_t
	{
		uint8_t  identifier[12];
		uint32_t endianness;
		uint32_t glType;
		uint32_t glTypeSize;
		uint32_t glFormat;
		uint32_t glInternalFormat;
		uint32_t glBaseInternalFormat;
		uint32_t pixelWidth;
		uint32_t pixelHeight;
		uint32_t pixelDepth;
		uint32_t numberOfArrayElements;
		uint32_t numberOfFaces;
		uint32_t numberOfMipmapLevels;
		uint32_t bytesOfKeyValueData;
	} KTX_header;



	static bool KTX_ReadHeader(FILE* f, KTX_header & header);
	static bool KTX_ReadHeader(const char * filename, KTX_header & header);

private:

	TextureManager();
	~TextureManager();

	void Initialize();
	void Release();

	void ReleaseAllTexture2D();
	void ReleaseAllCubeMapTexture();
	void ReleaseAllTextureGroup();

	TextureManager(TextureManager const &) = delete;
	TextureManager & operator=(TextureManager const &) = delete;

	template<typename TTexture> 
	void LoadKtxTexture(std::map<std::string, TTexture*> texMap, GLuint defaultTexture, std::string const & ktxFilename, TTexture *& returnTexture, bool & alreadyDefinedOrDefault)
	{
		std::string s;
		s.resize(ktxFilename.size());
		std::transform(ktxFilename.begin(), ktxFilename.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });

		std::map<std::string, TTexture*>::const_iterator it = texMap.find(s);

		if (it != texMap.end())
		{
			alreadyDefinedOrDefault = true;
			returnTexture = it->second;
		}
		else
		{
			returnTexture = new TTexture(defaultTexture);
			texMap[s] = returnTexture;

			GLuint id;
			GLenum target;
			KTX_dimensions dimensions;
			GLboolean isMipmapped;
			GLenum glerr;

			KTX_error_code err = ktxLoadTextureN(ktxFilename.c_str(), &id, &target, &dimensions, &isMipmapped, &glerr, nullptr, nullptr);

			if (id != 0)
			{
				returnTexture->SetId(id);
				PRINT_MESSAGE("[LoadTexture] Texture '%s' : %i.\n", ktxFilename.c_str(), id);
				alreadyDefinedOrDefault = false;
			}
			else
			{
				PRINT_MESSAGE("[LoadTexture] Cannot load the texture '%s' : %s.", ktxFilename.c_str(), ktxErrorString(err));
				alreadyDefinedOrDefault = true;
			}
		}
	}

	template<typename TTexture>
	void ReleaseTextureMap(std::map<std::string, TTexture*> & texMap)
	{
		if (texMap.empty())
			return;

		size_t max = texMap.size();
		std::vector<GLuint> ids(max);
		size_t count = 0;
		for (std::map<std::string, TTexture*>::const_iterator it = texMap.begin(); it != texMap.end(); ++it)
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


private:

	Texture2D * mDefault2D;
	TextureGroup * mDefaultTexGroup;

	KTX_dimensions mDefault2DDimensions;

	typedef std::map<std::string, Texture2D*> Tex2DIdMap;
	typedef std::map<std::string, CubeMapTexture*> CubeMapTexIdMap;
	typedef std::map<TextureGroupId, TextureGroup*> TexGroupMap;

	Tex2DIdMap m2DTexMap;
	CubeMapTexIdMap mCubeMapTexMap;
	TexGroupMap mTexGroupMap;


	friend class Engine;
};

} // namespace CoreFx
#endif // COREFX_TEXTUREMANAGER_H