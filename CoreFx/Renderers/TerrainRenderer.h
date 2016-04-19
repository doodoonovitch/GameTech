#pragma once

#ifndef COREFX_RENDERERS_TERRAINRENDERER_H
#define COREFX_RENDERERS_TERRAINRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class TerrainRenderer : public RendererHelper<Renderables::Grid, 1>
{
public:

	struct MapDesc
	{
		MapDesc(const char * filename, GLint heightMapTextureWidth, bool invertY)
			: mFilename(filename)
			, mHeightMapTextureWidth(heightMapTextureWidth)
			, mInvertY(invertY)
		{}

		std::string mFilename;
		GLint mHeightMapTextureWidth;
		bool mInvertY = true;

		//glm::vec3 mPosition;
		//glm::quat mRotation;
	};

	typedef std::vector<MapDesc> MapDescList;

	struct MaterialDesc : Renderer::MaterialDesc
	{
		MaterialDesc(GLfloat heightMin, GLfloat heightMax, 
			const glm::vec3& diffuse, TextureIndex diffuseTextureIndex, 
			const glm::vec3& specular, int8_t specularPower, TextureIndex specularTextureIndex, 
			const glm::vec3& emissive, TextureIndex emissiveTextureIndex, 
			TextureIndex normalTextureIndex)
			: Renderer::MaterialDesc(diffuse, diffuseTextureIndex, specular, specularPower, specularTextureIndex, emissive, emissiveTextureIndex, normalTextureIndex)
			, mHeightMin(heightMin)
			, mHeightMax(heightMax)
		{}

		MaterialDesc(const MaterialDesc & src)
			: Renderer::MaterialDesc(src)
			, mHeightMin(src.mHeightMin)
			, mHeightMax(src.mHeightMax)
		{
		}

		GLfloat mHeightMin;
		GLfloat mHeightMax;
	};

	typedef std::vector<MaterialDesc> MaterialDescList;

	struct Desc : public Renderer::Desc
	{
		Desc(GLint heightMapWidth, GLint heightMapDepth, const glm::vec3 & scale, GLfloat slowSlopeMax = 0.35f, GLfloat hiSlopeMin = 0.30f)
			: Renderer::Desc()
			, mHeightMapWidth(heightMapWidth)
			, mHeightMapDepth(heightMapDepth)
			, mScale(scale)
			, mLowSlopeMax(slowSlopeMax)
			, mHiSlopeMin(hiSlopeMin)
		{ }

		GLint mHeightMapWidth;
		GLint mHeightMapDepth;
		glm::vec3 mScale;
		GLfloat mLowSlopeMax;
		GLfloat mHiSlopeMin;

		MapDescList mTerrains;
		MaterialDescList mLowSlopeMaterials;
		MaterialDescList mHiSlopeMaterials;
	};

public:
	TerrainRenderer(const Desc & desc);
	virtual ~TerrainRenderer();

	virtual void Render() override;
	virtual void DebugRender() override;


private:

	void BuildMaterailShader(std::string & generatedSource, const Desc & desc);
	void LoadShaders(const Desc & desc);
	void LoadHeightMap(const MapDescList & terrainDescList);
	void UpdateMaterialTextureIndex(const Desc & desc);

private:

	enum EUniformIndex
	{
		u_PatchCount,
		u_MapSize,
		u_Scale,
		u_TexScale,
		u_HeightMap,
		//u_DiffuseMap,

		__uniforms_count__
	};

	enum EUniformIndex2
	{
		u_NormalMagnitude,
		u_VertexNormalColor,
		//perInstanceDataSampler,

		__uniforms2_count__
	};

	const int FIRST_TEXTURE_SAMPLER_INDEX = 1;


	glm::ivec2 mMapSize;
	glm::ivec2 mPatchCount;
	glm::vec3 mScale;
	GLfloat mLowSlopeMax;
	GLfloat mHiSlopeMin;

	GLuint mHeightMapTextureId;
	Shader mDrawNormalShader;
	//const TextureGroup * mDiffuseTextures;

};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_TERRAINRENDERER_H