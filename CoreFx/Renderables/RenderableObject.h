#pragma once

#ifndef COREFX_RENDERABLEOBJECT_H
#define COREFX_RENDERABLEOBJECT_H


namespace CoreFx
{
	namespace Renderables
	{



template<int T_vbo_count, bool T_hasFrame = true>
class RenderableObject : public SceneObject
{
public:

	static constexpr bool m_HasFrame = T_hasFrame;
	static constexpr int m_vboCount = T_vbo_count;

	RenderableObject(void) 
		: SceneObject(m_HasFrame)
		, m_vaoID(0)
	{
		std::memset(m_vboIDs, 0, sizeof(m_vboIDs));
	}

	virtual ~RenderableObject(void)
	{
		//Destroy shader
		m_shader.DeleteShaderProgram();

		// Release buffers (vao & vbo)
		ReleaseBuffers();
	}

protected:

	void ReleaseBuffers()
	{
		//Destroy vao and vbo
		glDeleteBuffers(m_vboCount, m_vboIDs);
		glDeleteVertexArrays(1, &m_vaoID);
		std::memset(m_vboIDs, 0, sizeof(m_vboIDs));
		m_vaoID = 0;
	}

protected:
	GLuint m_vaoID;
	GLuint m_vboIDs[m_vboCount];
	Shader m_shader;
};




	} // namespace Renderables
} // namespace CoreFx

#endif // COREFX_RENDERABLEOBJECT_H