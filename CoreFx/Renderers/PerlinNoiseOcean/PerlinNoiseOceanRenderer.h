#pragma once

#ifndef COREFX_RENDERERS_PERLINNOISEOCEAN_RENDERER_H
#define COREFX_RENDERERS_PERLINNOISEOCEAN_RENDERER_H


namespace CoreFx
{
	namespace Renderers
	{
		namespace PerlinNoiseOcean
		{
		// =======================================================================
		// =======================================================================


class Renderer : public RendererHelper<1>
{
public:
	enum class ENormalMode
	{
		PerVertexNormal,
		PerPixelNormal
	};

	struct MapDesc
	{
		MapDesc(const glm::vec3 & origin, const glm::quat & rotation)
		{
			mModelDQ.SetRotationTranslation(rotation, origin);
		}

		Maths::DualQuaternion mModelDQ;
	};

	typedef std::vector<MapDesc> MapDescList;

	struct Desc : public CoreFx::Renderer::Desc
	{
		Desc(GLint mapWidth, GLint mapDepth, const std::string & skyboxCubeMapTextureFilename, ENormalMode normalMode = ENormalMode::PerPixelNormal)
			: CoreFx::Renderer::Desc()
			, mMapWidth(mapWidth)
			, mMapDepth(mapDepth)
			, mSkyboxCubeMapTextureFilename(skyboxCubeMapTextureFilename)
			, mNormalMode(normalMode)
		{ }

		GLint mMapWidth;
		GLint mMapDepth;
		glm::ivec2 mHeightMapTextureSize = glm::ivec2(512);
		MapDescList mMaps;
		WavePropertyList mWaveProps;
		std::string mSkyboxCubeMapTextureFilename;
		ENormalMode mNormalMode;
	};

public:
	Renderer(const Desc & desc);
	virtual ~Renderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;


private:

	void LoadHeightMapComputeShader(const Desc & desc);
	void LoadShaders(const Desc & desc);
	void LoadMainShader(const Desc & desc);
	void LoadWireFrameShader(const Desc & desc);

private:

	enum EUniformIndex
	{
		u_PatchCount,
		u_MapSize,
		u_HeightMapTextureSize,
		u_HeightMapSampler,
		u_PerMapDataSampler,
		u_SkyboxCubeMapSampler,
		u_textureSampler,
		u_NormalMapSampler,

		__uniforms_count__
	};

	struct PerMapData
	{
		Maths::DualQuaternion mModelDQ;
	};

	enum VertexBufferIndex
	{
		VertexArrayBufferIndex,
	};

	const int FIRST_TEXTURE_SAMPLER_INDEX = 2;

	HeightMapCS * mHeightMapCS;
	CubeMapTexture const * mCubeMapTexture;
	Texture2D const * mOceanColorTexture;
	GLuint mNormalTextureId;

	glm::ivec2 mHeightMapTextureSize;
	glm::ivec2 mMapSize;
	glm::ivec2 mPatchCount;
	GLint mMapCount;

	Shader mDrawNormalShader;
	TextureBuffer mModelMatrixBuffer;
};


		} // namespace PerlinNoiseOcean
	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_PERLINNOISEOCEANRENDERER_H