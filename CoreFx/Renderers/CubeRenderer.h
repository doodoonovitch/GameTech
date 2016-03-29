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

	enum
	{
		Property_Per_Material = 3
	};

	typedef std::uint8_t TextureIndex;

	static constexpr TextureIndex NoTexture = (TextureIndex)-1;

	struct MaterialDesc
	{
		MaterialDesc(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess,
			TextureIndex ambientTextureIndex, TextureIndex diffuseTextureIndex, TextureIndex specularTextureIndex, TextureIndex normalTextureIndex)
			: mAmbient(ambient)
			, mDiffuse(diffuse)
			, mSpecular(specular)
			, mShininess(shininess)
			, mAmbientTextureIndex(ambientTextureIndex)
			, mDiffuseTextureIndex(diffuseTextureIndex)
			, mSpecularTextureIndex(specularTextureIndex)
			, mNormalTextureIndex(normalTextureIndex)
		{}

		glm::vec3 mAmbient;
		glm::vec3 mDiffuse;
		glm::vec3 mSpecular;
		float mShininess;
		TextureIndex mAmbientTextureIndex;
		TextureIndex mDiffuseTextureIndex;
		TextureIndex mSpecularTextureIndex;
		TextureIndex mNormalTextureIndex;
	};

public:

	CubeRenderer(std::uint16_t materialCount, size_t capacity, size_t pageSize = 10);
	virtual ~CubeRenderer();

	virtual void Render() override;
	virtual void DebugRender() override;

	virtual void UpdateMaterialTextureIndex() override;

	Renderables::Cube * CreateCube(std::uint8_t materialIndex);
	void DeleteCube(Renderables::Cube *& cube);

	void SetMaterial(std::uint16_t materialIndex, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, 
		TextureIndex ambientTextureIndex, TextureIndex diffuseTextureIndex, TextureIndex specularTextureIndex, TextureIndex normalTextureIndex);

	void SetMaterial(std::uint16_t materialIndex, const MaterialDesc & mat);

private:

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
		__declspec(align(4)) GLfloat mAmbient[3];
		__declspec(align(4)) GLbitfield mAmbientNormalIndexes;		// ambient, normal sampler and texture index
		__declspec(align(4)) GLfloat mDiffuse[3];
		__declspec(align(4)) GLbitfield mDiffuseSpecularIndexes;	// diffuse, specular sampler and texture index
		__declspec(align(4)) GLfloat mSpecular[3];
		__declspec(align(4)) float mShininess;
	};

	struct MaterialTextureIndexes
	{
		TextureIndex mAmbient = CubeRenderer::NoTexture;
		TextureIndex mDiffuse = CubeRenderer::NoTexture;
		TextureIndex mSpecular = CubeRenderer::NoTexture;
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