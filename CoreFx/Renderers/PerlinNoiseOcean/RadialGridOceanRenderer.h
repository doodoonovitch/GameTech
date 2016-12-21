#pragma once

#ifndef COREFX_RENDERERS_RADIALGRIDOCEAN_RENDERER_H
#define COREFX_RENDERERS_RADIALGRIDOCEAN_RENDERER_H


namespace CoreFx
{
	namespace Renderers
	{
		namespace PerlinNoiseOcean
		{
		// =======================================================================
		// =======================================================================


class RadialGridOceanRenderer : public RendererHelper<2>
{
public:
	enum class ENormalMode
	{
		PerVertexNormal,
		PerPixelNormal
	};

	struct Desc : public CoreFx::Renderer::Desc
	{
		Desc()
			: CoreFx::Renderer::Desc()
		{ }

		Desc & SetGrid(GLushort slides = 36, GLushort rings = 100, GLfloat maxDetail = 0.1f, GLfloat gridSize = 40000.f)
		{
			mGridSlides = slides;
			mGridRings = rings;
			mMaxDetail = maxDetail;
			mGridSize = gridSize;
			return *this;
		}

		//Desc & SetHeightMapSize(const GLsizei size)
		//{
		//	mHeightMapTextureSize = size;
		//	return *this;
		//}

		Desc & SetCustomSkyCubeMap(const std::string & filename)
		{
			mCustomSkyCubeMapFilename = filename;
			return *this;
		}

		Desc & SetNormalMode(ENormalMode mode)
		{
			mNormalMode = mode;
			return *this;
		}

		Desc & SetWaveAmplitude(GLfloat value)
		{
			mWaveAmplitude = value;
			return *this;
		}

		Desc & SetTexScale(GLfloat value)
		{
			mTexScale = value;
			return *this;
		}

	protected:

		GLushort mGridSlides = 36;
		GLushort mGridRings = 100;
		GLfloat mMaxDetail = 0.1f;
		GLfloat mGridSize = 40000.f;
		GLfloat mWaveAmplitude = 2.f;
		GLfloat mTexScale = 2.f;
		//GLsizei mHeightMapTextureSize = 256;
		std::string mCustomSkyCubeMapFilename;
		ENormalMode mNormalMode = ENormalMode::PerPixelNormal;

		friend class RadialGridOceanRenderer;
	};

public:

	RadialGridOceanRenderer(const Desc & desc);
	virtual ~RadialGridOceanRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

	GLfloat GetWaveAmplitude() const { return mScale.y; }
	void SetWaveAmplitude(GLfloat value) 
	{ 
		if(value > 0)
			mScale.y = value; 
	}

	GLfloat GetTexScale() const { return mTexScale; }
	void SetTexScale(GLfloat value) 
	{ 
		if (mTexScale + value > 0)
		{
			mTexScale = value;
			mScale.x = mTexScale / mHeightMapTextureSize;
		}
	}

private:

	void LoadShaders(const Desc & desc);
	void LoadMainShader(const Desc & desc);
	void LoadWireFrameShader(const Desc & desc);

	bool LoadRaw(const char * filename, void * data, size_t sizeBytes);
	bool InitializeHeightMaps();

	void UpdateFrame();

private:

	enum EUniformIndex
	{
		u_Frame0,
		//u_Frame1,
		u_Scale,

		u_HeightMapsSampler,
		u_NormalMapsSampler,
		u_SkyboxCubeMapSampler,
		u_TextureSampler,

		__uniforms_count__
	};

	enum VertexBufferIndex
	{
		VertexArrayBufferIndex,
		IndexArrayBufferIndex,
	};

	const int FIRST_TEXTURE_SAMPLER_INDEX = 2;

	GLsizei mVertexCount;
	GLsizei mIndexCount;

	Texture2DArray const * mHeightMaps = nullptr;
	Texture2DArray const * mNormalMaps = nullptr;
	CubeMapTexture const * mCubeMapTexture;
	Texture2D const * mOceanColorTexture;

	GLsizei mHeightMapTextureSize;

	GLint mFrame0;
	//GLint mFrame1;
	GLfloat mTexScale;
	glm::vec2 mScale;
};


		} // namespace PerlinNoiseOcean
	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_RADIALGRIDOCEAN_RENDERER_H