#pragma once

#ifndef COREFX_RENDERERS_MODELRENDERER_H
#define COREFX_RENDERERS_MODELRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{




class ModelRenderer : public SceneObjectRenderer<Renderables::Model, 3>
{
public:

	enum
	{
		Property_Per_Material = 3
	};


public:

	ModelRenderer(size_t capacity = 64, size_t pageSize = 10);
	virtual ~ModelRenderer();

	void SetModel(const Renderer::VertexDataVector & vertexList, const Renderer::IndexVector & indexList, const Renderer::MaterialDescList & materialDescList, const Renderer::TextureDescList & textureDescList, const Renderer::DrawElementsIndirectCommandList & meshDrawInstanceList, const Geometry::ModelData::ModelMappingList & modelMapping);

	virtual void Update() override;
	virtual void Render() override;
	virtual void RenderWireFrame() override;

	// ---------------------------------------------

	static ModelRenderer * CreateFromFile(Engine * engine, const std::string & modelFilePath, const std::string & textureBasePath, const Geometry::ModelData::LoadOptions & options, size_t capacity = 64, size_t pageSize = 10);
	static ModelRenderer * CreateFromModel(Engine * engine, const Geometry::ModelData & model, size_t capacity = 64, size_t pageSize = 10);

	// ---------------------------------------------

	void SetMaterial(std::uint16_t materialIndex, const glm::vec3& diffuse, TextureIndex diffuseTextureIndex, const glm::vec3& specular, TextureIndex specularTextureIndex, GLfloat roughness, TextureIndex roughnessTextureIndex, const glm::vec3& emissive, TextureIndex emissiveTextureIndex, TextureIndex normalTextureIndex);
	void SetMaterial(std::uint16_t materialIndex, const Renderer::MaterialDesc & mat);
	void SetMaterials(const Renderer::MaterialDescList & materials);

	// ---------------------------------------------

	Renderables::Model * CreateModelInstance(GLuint modelIndex);
	void DeleteModelInstance(Renderables::Model * modelInstance);

private:
	
	enum class EMainShaderUniformIndex
	{
		u_MaterialBaseIndex,
		u_MaterialDataSampler,

		__uniforms_count__
	};

	enum VBOIndex
	{
		VBO_Vertex,
		VBO_Index,
		VBO_Indirect,
	};

	enum EBinding
	{
		u_PerInstanceDataBuffer,
		u_PerInstanceDataIndexBuffer,
	};

#pragma pack(push, 1)
	struct PerInstanceData
	{
		//glm::mat4 mModel;
		glm::quat mQreal;
		glm::quat mQdual;
		glm::vec3 mScale;
		GLfloat __padding__;
	};
#pragma pack(pop)

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
		TextureIndex mRoughness = CubeRenderer::NoTexture;
		TextureIndex mEmissive = CubeRenderer::NoTexture;
		TextureIndex mNormal = CubeRenderer::NoTexture;

	};

	typedef std::vector<MaterialTextureIndexes> MaterialTextureIndexesList;

	struct ModelInstanceMapping : public Geometry::ModelData::ModelMapping
	{
		ModelInstanceMapping(GLuint drawElementsIndirectCommandIndex = 0, GLuint drawElementsIndirectCommandCount = 0, bool hasBones = false, GLuint instanceCount = 0)
			: Geometry::ModelData::ModelMapping(drawElementsIndirectCommandIndex, drawElementsIndirectCommandCount, hasBones)
			, mInstanceCount(instanceCount)
		{ }

		ModelInstanceMapping(const ModelInstanceMapping & src)
			: Geometry::ModelData::ModelMapping(src)
			, mInstanceCount(src.mInstanceCount)
		{
		}

		ModelInstanceMapping(const Geometry::ModelData::ModelMapping & src)
			: Geometry::ModelData::ModelMapping(src)
			, mInstanceCount(0)
		{
		}

		GLuint mInstanceCount;
	};

	typedef std::vector<ModelInstanceMapping> ModelInstanceMappingList;

	const int FIRST_TEXTURE_SAMPLER_INDEX = 3;

private:

	void InitializeShaders();
	void InitializeMainShader();
	void InitializeWireFrameShader();
	void UpdateMaterialTextureIndex();

	void UpdateShaderData();

	void UpdateObjMaxtrixIndexProp();
	void UpdateDrawCommandListInstanceCount(const ModelInstanceMapping & mapping);

private:

	GLuint mMaterialCount;
	GLsizei mDrawCmdCount;

	MaterialTextureIndexesList mMaterialTextureIndexesList;

	Renderer::DrawElementsIndirectCommandList mDrawCommandList;
	ModelInstanceMappingList mModelInstanceMappingList;

	bool mIsModelSet;
	bool mIsShaderBufferSet;
	bool mUpdateObjMatrixIndexProp;

	friend class Engine;

};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_MODELRENDERER_H
