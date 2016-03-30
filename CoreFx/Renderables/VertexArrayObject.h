#pragma once

#ifndef COREFX_RENDERABLEOBJECT_H
#define COREFX_RENDERABLEOBJECT_H


namespace CoreFx
{
	namespace Renderables
	{



template<int T_vbo_count>
class VertexArrayObject
{
public:

	static constexpr int mVboCount = T_vbo_count;

	VertexArrayObject()
		: mVaoID(0)
	{
		std::memset(mVboIDs, 0, sizeof(mVboIDs));
	}

	virtual ~VertexArrayObject(void)
	{
		// Release buffers (vao & vbo)
		ReleaseBuffers();
	}

	void CreateBuffers()
	{
		glGenVertexArrays(1, &mVaoID);
		glGenBuffers(mVboCount, mVboIDs);
	}

	void ReleaseBuffers()
	{
		//Destroy vao and vbo
		glDeleteBuffers(mVboCount, mVboIDs);
		glDeleteVertexArrays(1, &mVaoID);
		std::memset(mVboIDs, 0, sizeof(mVboIDs));
		mVaoID = 0;
	}

	inline GLuint GetVao() const					{ return mVaoID; }
	inline GLuint GetVbo(int index) const			{ assert(index < mVboCount); return mVboIDs[index]; }


protected:
	GLuint mVaoID;
	GLuint mVboIDs[mVboCount];
};




	} // namespace Renderables
} // namespace CoreFx

#endif // COREFX_RENDERABLEOBJECT_H