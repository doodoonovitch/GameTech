#pragma once
#ifndef COREFX_RENDERERS_CUBERENDERER_H
#define COREFX_RENDERERS_CUBERENDERER_H


namespace CoreFx
{
	namespace Renderers
	{



class CubeRenderer : public SceneObjectRenderer<Renderables::Cube, 2>
{
public:

	struct Desc : public Renderer::Desc
	{
		Desc()
			: Renderer::Desc()
		{}

		MaterialDescList mMaterials;
	};

public:

	enum
	{
		Property_Per_Material = 3
	};


public:

	CubeRenderer(const Desc & desc, size_t capacity, size_t pageSize = 10);
	virtual ~CubeRenderer();

	virtual void Render() override;
	virtual void DebugRender() override;

	virtual void UpdateMaterialTextureIndex() override;

	Renderables::Cube * CreateCube(std::uint8_t materialIndex);
	void DeleteCube(Renderables::Cube *& cube);

	void SetMaterial(std::uint16_t materialIndex, const glm::vec3& diffuse, TextureIndex diffuseTextureIndex, const glm::vec3& specular, int8_t specularPower, TextureIndex specularTextureIndex, const glm::vec3& emissive, TextureIndex emissiveTextureIndex, TextureIndex normalTextureIndex);

	void SetMaterial(std::uint16_t materialIndex, const MaterialDesc & mat);

	void SetMaterials(const MaterialDescList & materials);

private:

	enum EUniformIndex
	{
		u_MaterialBaseIndex,
		u_perInstanceDataSampler,
		u_materialIndexSampler,
		u_materialDataSampler,

		__uniforms_count__
	};

	enum VBOIndex
	{
		VBO_Vertex = 0,
		VBO_Index = 1,
	};

	struct PerInstanceData
	{
		//glm::mat4 mModel;
		glm::quat mQreal;
		glm::quat mQdual;
	};

	struct MaterialData
	{
		__declspec(align(4)) GLfloat mDiffuse[3];
		__declspec(align(4)) GLbitfield mDiffuseSpecularIndexes;	// diffuse, specular sampler and texture index
		__declspec(align(4)) GLfloat mSpecular[3];
		__declspec(align(4)) float mShininess;
		__declspec(align(4)) GLfloat mEmissive[3];
		__declspec(align(4)) float mEmissiveNormalIndex;
	};

	struct MaterialTextureIndexes
	{
		//TextureIndex mAmbient = CubeRenderer::NoTexture;
		TextureIndex mDiffuse = CubeRenderer::NoTexture;
		TextureIndex mSpecular = CubeRenderer::NoTexture;
		TextureIndex mEmissive = CubeRenderer::NoTexture;
		TextureIndex mNormal = CubeRenderer::NoTexture;
		
	};

	typedef std::vector<MaterialTextureIndexes> MaterialTextureIndexesList;

	const int mIndexCount = 36;

	const int FIRST_TEXTURE_SAMPLER_INDEX = 3;

private:

	static void ComputeTangent(GLfloat vertices[], int vertexCount, GLushort indices[], int indexCount);

	void InitializeShader();

private:
	
	GLuint mMaterialCount;

	TextureBuffer mModelMatrixBuffer;
	TextureBuffer mMaterialIndexBuffer;

	MaterialTextureIndexesList mMaterialTextureIndexesList;

	bool mIsMaterialIndexBufferSet;
	bool mIsMaterialDataBufferSet;

	friend class Engine;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_CUBERENDERER_H