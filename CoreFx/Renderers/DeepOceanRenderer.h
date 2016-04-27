#pragma once

#ifndef COREFX_RENDERERS_DEEPOCEANRENDERER_H
#define COREFX_RENDERERS_DEEPOCEANRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class DeepOceanRenderer : public RendererHelper<Renderables::Grid, 1>
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

	struct Desc : public Renderer::Desc
	{
		Desc(GLint mapWidth, GLint mapDepth, const glm::vec2 & scale)
			: Renderer::Desc()
			, mMapWidth(mapWidth)
			, mMapDepth(mapDepth)
			, mScale(scale.x, 1.0f, scale.y)
		{ }

		GLint mMapWidth;
		GLint mMapDepth;
		glm::vec3 mScale;
		MapDescList mMaps;

	};

public:
	DeepOceanRenderer(const Desc & desc);
	virtual ~DeepOceanRenderer();

	virtual void Render() override;
	virtual void DebugRender() override;


private:

	void LoadShaders(const Desc & desc);

private:

	enum EUniformIndex
	{
		u_PatchCount,
		u_MapSize,
		u_Scale,
		u_PerMapDataSampler,

		__uniforms_count__
	};

	enum EUniformIndex2
	{
		u_NormalMagnitude,
		u_VertexNormalColor,

		__uniforms2_count__
	};


	struct PerMapData
	{
		Maths::DualQuaternion mModelDQ;
	};


	const int FIRST_TEXTURE_SAMPLER_INDEX = 2;


	glm::ivec2 mMapSize;
	glm::ivec2 mPatchCount;
	glm::vec3 mScale;
	GLint mMapCount;

	Shader mDrawNormalShader;
	TextureBuffer mModelMatrixBuffer;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_DEEPOCEANRENDERER_H