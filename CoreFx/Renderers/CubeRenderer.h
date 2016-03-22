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

	CubeRenderer(std::vector<std::string> const & texture, std::uint8_t materialCount, size_t capacity, size_t pageSize = 10);
	virtual ~CubeRenderer();

	virtual void Render() override;

	virtual void UpdateMaterialTextureIndex() override;

	Renderables::Cube * CreateCube(std::uint8_t materialIndex);
	void DeleteCube(Renderables::Cube *& cube);

	void SetMaterial(std::uint16_t materialIndex, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, 
		TextureIndex ambientTextureIndex, TextureIndex diffuseTextureIndex, TextureIndex specularTextureIndex, TextureIndex normalTextureIndex);

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
		__declspec(align(4)) GLbitfield mTextureIndexes;			// diffuse, specular, normal, ambient texture index
		__declspec(align(4)) GLfloat mDiffuse[3];
		__declspec(align(4)) GLbitfield mTextureSamplerIndexes;		// diffuse, specular, normal, ambient texture index
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

private:
	
	TextureGroup const * mTexture;

	GLuint mMaterialCount;

	TextureBuffer mModelMatrixBuffer;
	TextureBuffer mMaterialIndexBuffer;
	TextureBuffer mMaterialDataBuffer;

	MaterialTextureIndexesList mMaterialTextureIndexesList;

	bool mIsMaterialIndexBufferSet;
	bool mIsMaterialDataBufferSet;

	friend class Engine;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_CUBERENDERER_H