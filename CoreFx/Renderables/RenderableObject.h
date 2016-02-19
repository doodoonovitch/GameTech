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

	static constexpr bool mHasFrame = T_hasFrame;
	static constexpr int mVboCount = T_vbo_count;

	RenderableObject(void) 
		: SceneObject(mHasFrame)
		, mVaoID(0)
	{
		std::memset(mVboIDs, 0, sizeof(mVboIDs));
	}

	virtual ~RenderableObject(void)
	{
		//Destroy shader
		mShader.DeleteShaderProgram();

		// Release buffers (vao & vbo)
		ReleaseBuffers();
	}

protected:

	void ReleaseBuffers()
	{
		//Destroy vao and vbo
		glDeleteBuffers(mVboCount, mVboIDs);
		glDeleteVertexArrays(1, &mVaoID);
		std::memset(mVboIDs, 0, sizeof(mVboIDs));
		mVaoID = 0;
	}

protected:
	GLuint mVaoID;
	GLuint mVboIDs[mVboCount];
	Shader mShader;
};




	} // namespace Renderables
} // namespace CoreFx

#endif // COREFX_RENDERABLEOBJECT_H