#pragma once
#ifndef COREFX_TEXTUREMANAGER_H
#define COREFX_TEXTUREMANAGER_H


namespace CoreFx
{
	
	class Engine;



class TextureManager
{
public:

	Texture2D const * LoadTexture2D(std::string const &filename);

	TextureGroup const * LoadTextureGroup(TextureGroupId groupId, std::vector<std::string> filenames);

	void ReleaseTexture2D(Texture2D const *& texture);

	void ReleaseTextureGroup(TextureGroup const *& texture);

private:

	TextureManager();
	~TextureManager();

	void Initialize();
	void Release();

	void ReleaseAllTexture2D();
	void ReleaseAllTextureGroup();

	TextureManager(TextureManager const &) = delete;
	TextureManager & operator=(TextureManager const &) = delete;

private:

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

	Texture2D * mDefault2D;
	TextureGroup * mDefaultTexGroup;

	KTX_dimensions mDefault2DDimensions;

	typedef std::map<std::string, Texture2D*> Tex2DIdMap;
	typedef std::map<TextureGroupId, TextureGroup*> TexGroupMap;

	Tex2DIdMap m2DTexMap;
	TexGroupMap mTexGroupMap;


	friend class Engine;
};

} // namespace CoreFx
#endif // COREFX_TEXTUREMANAGER_H