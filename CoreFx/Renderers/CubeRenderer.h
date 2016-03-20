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

	CubeRenderer(std::vector<std::string> const & texture, std::uint8_t materialCount, size_t capacity, size_t pageSize = 10);
	virtual ~CubeRenderer();

	virtual void Render() override;

	Renderables::Cube * CreateCube(std::uint8_t materialIndex);
	void DeleteCube(Renderables::Cube *& cube);

	void SetMaterial(std::uint8_t materialIndex, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, 
		std::int8_t ambientTextureIndex, std::int8_t diffuseTextureIndex, std::int8_t specularTextureIndex, std::int8_t normalTextureIndex);

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
		__declspec(align(4)) GLbitfield mTextureIndexes; // diffuse, specular, normal texture index
		__declspec(align(4)) GLfloat mDiffuse[3];
		__declspec(align(4)) GLbitfield _mUnused1;
		__declspec(align(4)) GLfloat mSpecular[3];
		__declspec(align(4)) float mShininess;
	};

private:
	
	TextureGroup const * mTexture;

	GLuint mMaterialCount;

	TextureBuffer mModelMatrixBuffer;
	TextureBuffer mMaterialIndexBuffer;
	TextureBuffer mMaterialDataBuffer;
	bool mIsMaterialIndexBufferSet;
	bool mIsMaterialDataBufferSet;

	friend class Engine;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_CUBERENDERER_H