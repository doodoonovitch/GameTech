#pragma once

#ifndef COREFX_RENDERABLES_CUBE_H
#define COREFX_RENDERABLES_CUBE_H


namespace CoreFx
{
	namespace Renderables
	{


class Cube : public RenderableObject<2>
{
public:
	Cube(std::string const & texture);
	virtual ~Cube();

	virtual void Render(glm::mat4 const & VP) override;

private:

	enum VBOIndex
	{
		VBO_Vertex = 0,
		VBO_Index = 1
	};

	struct Vertex
	{
		Vertex() {}
		Vertex(glm::vec3 const & _pos, glm::vec2 const & _uv)
			: pos(_pos)
			, uv(_uv)
		{}

		glm::vec3 pos;
		glm::vec2 uv;
	};

	//texture ID
	//GLuint m_textureID;
	Texture2D const * m_texture;
};



	} // namespace Renderables
} // namespace CoreFx
#endif // COREFX_RENDERABLES_CUBE_H