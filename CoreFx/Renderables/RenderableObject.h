#pragma once

#ifndef COREFX_RENDERABLEOBJECT_H
#define COREFX_RENDERABLEOBJECT_H


namespace CoreFx
{
	namespace Renderables
	{



template<int T_vbo_count>
class RenderableObject
{
public:

	static constexpr int mVboCount = T_vbo_count;

	RenderableObject(void) 
		: mVaoID(0)
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

	inline GLuint GetVao() const					{ return mVaoID; }
	inline GLuint GetVbo(int index) const			{ assert(index < mVboCount); return mVboIDs[index]; }
	inline const Shader & GetShader() const			{ return mShader; }
	inline Shader & GetShader()						{ return mShader; }


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