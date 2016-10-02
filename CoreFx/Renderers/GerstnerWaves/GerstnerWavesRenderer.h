#pragma once

#ifndef COREFX_RENDERERS_GERSTNERWAVES_RENDERER_H
#define COREFX_RENDERERS_GERSTNERWAVES_RENDERER_H


namespace CoreFx
{
	namespace Renderers
	{
		namespace GerstnerWaves
		{
		// =======================================================================
		// =======================================================================


class Renderer : public RendererHelper<1>
{
public:

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
		Desc(bool precomputeNormals, GLint mapWidth, GLint mapDepth, const glm::vec2 & scale, const std::string & skyboxCubeMapTextureFilename)
			: CoreFx::Renderer::Desc()
			, mMapWidth(mapWidth)
			, mMapDepth(mapDepth)
			, mScale(scale.x, 1.0f, scale.y)
			, mSkyboxCubeMapTextureFilename(skyboxCubeMapTextureFilename)
			, mPrecomputeNormals(precomputeNormals)
		{ }

		GLint mMapWidth;
		GLint mMapDepth;
		glm::vec3 mScale;
		MapDescList mMaps;
		WavePropertyList mWaveProps;
		std::string mSkyboxCubeMapTextureFilename;
		bool mPrecomputeNormals;
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

	static void LoadTessellationShader(Shader & shader, GLenum whichShader, const std::string & filename, const Desc & desc);

private:

	enum EUniformIndex
	{
		u_PatchCount,
		u_MapSize,
		u_Scale,
		u_HeightMapSampler,
		u_PerMapDataSampler,
		u_SkyboxCubeMapSampler,
		u_textureSampler,

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

	GLuint mHeightMapTextureId;

	glm::ivec2 mTextureSize;
	glm::ivec2 mMapSize;
	glm::ivec2 mPatchCount;
	glm::vec3 mScale;
	GLint mMapCount;

	Shader mDrawNormalShader;
	TextureBuffer mModelMatrixBuffer;
};


		} // namespace GerstnerWaves
	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_GERSTNERWAVES_RENDERER_H