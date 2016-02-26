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
	CubeRenderer(std::string const & texture, size_t capacity = 64, size_t pageSize = 10);
	virtual ~CubeRenderer();

	virtual void Render() override;

	Renderables::Cube * CreateCube(GLuint textureIndex);
	void DeleteCube(Renderables::Cube *& cube);


private:

	enum VBOIndex
	{
		VBO_Vertex = 0,
		VBO_Index = 1,
	};

	struct PerInstanceData
	{
		glm::mat4 mModel;
	};

private:
	
	Texture2D const * mTexture;

	GLuint mModelMatrixBuffer;

	DrawNormalShader mDrawVertexNormalShader;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_CUBERENDERER_H