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

	GLfloat GetDrawVertexNormalMagnitude() const { return mDrawVertexNormalMagnitude; }

	void SetDrawVertexNormalMagnitude(GLfloat value)
	{
		mDrawVertexNormalMagnitude = value;
	}

	const glm::vec4 & GetDrawVertexNormalColor() const { return mDrawVertexNormalColor; }

	void SetDrawVertexNormalColor(const glm::vec4 & value)
	{
		mDrawVertexNormalColor = value;
	}

private:

	void InitializeDrawVertexNormalShader();

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
	glm::vec4 mDrawVertexNormalColor;
	GLfloat mDrawVertexNormalMagnitude;
	GLuint mModelMatrixBuffer;
	Shader mDrawVertexNormalShader;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_CUBERENDERER_H