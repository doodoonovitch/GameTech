#pragma once

#ifndef COREFX_RENDERERS_MODELRENDERER_H
#define COREFX_RENDERERS_MODELRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{




class ModelRenderer : public SceneObjectRenderer<Renderables::Model, 4>
{
public:

	enum
	{
		Property_Per_Material = 3
	};


public:

	ModelRenderer(const Renderer::VertexDataVector & vertexList, const Renderer::IndexVector & indexList, const Renderer::MaterialDescList & materialDescList, const Renderer::TextureDescList & textureDescList, const Renderer::DrawElementsIndirectCommandList & meshDrawInstanceList, size_t capacity = 64, size_t pageSize = 10);
	virtual ~ModelRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

	// ---------------------------------------------

	static ModelRenderer * CreateFromFile(const std::string & modelFilePath, const std::string & textureBasePath, size_t capacity = 64, size_t pageSize = 10);

	// ---------------------------------------------

	void SetMaterial(std::uint16_t materialIndex, const glm::vec3& diffuse, TextureIndex diffuseTextureIndex, const glm::vec3& specular, GLfloat roughness, TextureIndex specularTextureIndex, const glm::vec3& emissive, TextureIndex emissiveTextureIndex, TextureIndex normalTextureIndex);
	void SetMaterial(std::uint16_t materialIndex, const Renderer::MaterialDesc & mat);
	void SetMaterials(const Renderer::MaterialDescList & materials);

	// ---------------------------------------------

	Renderables::Model * CreateModelInstance(std::uint8_t materialGroupIndex);
	void DeleteModelInstance(Renderables::Model * modelInstance);

private:
	
	enum class EMainShaderUniformIndex
	{
		//u_MaterialBaseIndex,
		u_perInstanceDataSampler,
		//u_materialIndexSampler,
		u_materialDataSampler,

		__uniforms_count__
	};

	enum class EWireFrameShaderUniformIndex
	{
		//u_MaterialBaseIndex,
		u_perInstanceDataSampler,
		//u_materialIndexSampler,

		__uniforms_count__
	};

	enum VBOIndex
	{
		VBO_Vertex,
		VBO_Index,
		VBO_Indirect,
		VBO_MeshId,
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
		__declspec(align(4)) GLfloat mRoughness;
		__declspec(align(4)) GLfloat mEmissive[3];
		__declspec(align(4)) GLfloat mEmissiveNormalIndex;
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

	const int FIRST_TEXTURE_SAMPLER_INDEX = 2;

private:

	void InitializeShaders();
	void InitializeMainShader();
	void InitializeWireFrameShader();
	void UpdateMaterialTextureIndex();

	void UpdateShaderData();

private:

	GLuint mMaterialCount;
	GLsizei mDrawCmdCount;
	GLsizei mIndexCount;

	TextureBuffer mModelMatrixBuffer;
	//TextureBuffer mMaterialIndexBuffer;

	MaterialTextureIndexesList mMaterialTextureIndexesList;

	Renderer::DrawElementsIndirectCommandList mMeshDrawInstanceList;

	bool mIsShaderBufferSet;

	friend class Engine;

};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_MODELRENDERER_H
