#pragma once
#ifndef COREFX_TEXTUREMANAGER_H
#define COREFX_TEXTUREMANAGER_H


namespace CoreFx
{
	
	
class TextureManager
{
public:

	Texture2D const * LoadTexture2D(std::string const &filename);

	void ReleaseTexture2D(Texture2D const *& texture);

private:

	TextureManager();
	~TextureManager();

	void Initialize();
	void Release();

	void ReleaseAllTexture2D();

	TextureManager(TextureManager const &) = delete;
	TextureManager & operator=(TextureManager const &) = delete;

private:

	Texture2D * m_default2D;

	typedef std::map<std::string, Texture2D*> Tex2DIdMap;
	Tex2DIdMap m_2DTexMap;

	friend class Engine;
};

} // namespace CoreFx
#endif // COREFX_TEXTUREMANAGER_H