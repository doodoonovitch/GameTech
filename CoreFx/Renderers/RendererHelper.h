#pragma once

#ifndef COREFX_RENDERERS_RENDERERHELPER_H
#define COREFX_RENDERERS_RENDERERHELPER_H


namespace CoreFx
{
	namespace Renderers
	{



template<int T_vbo_count>
class RendererHelper : public Renderer
{
public:

	static constexpr int mVboCount = T_vbo_count;

protected:

	RendererHelper(GLuint propertyCount, const char * shaderTitle = nullptr)
		: Renderer(propertyCount)
		, mVaoID(0)
		, mShader(shaderTitle)
	{
		std::memset(mVboIDs, 0, sizeof(mVboIDs));
	}

	virtual ~RendererHelper(void)
	{
		//Destroy shader
		mShader.DeleteShaderProgram();

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
		std::memset(mVboIDs, 0, sizeof(mVboIDs));

		glDeleteVertexArrays(1, &mVaoID);
		mVaoID = 0;
	}

protected:

	GLuint mVaoID;
	GLuint mVboIDs[mVboCount];	
	Shader mShader;
};



template<typename T_Object, int T_vbo_count>
class SceneObjectRenderer : public RendererHelper<T_vbo_count>
{
public:

	void ClearObjectState()
	{
		mObjs.ForEach([](T_Object* obj)
		{
			if (obj->GetFrame() != nullptr)
				obj->GetFrame()->SetIsModified(false);
		});
	}

	size_t GetCapacity() const
	{
		return mObjs.GetCapacity();
	}

	size_t GetCount() const
	{
		return mObjs.GetCount();
	}

protected:

	SceneObjectRenderer(GLuint propertyCount, size_t capacity, size_t pageSize, const char * shaderTitle = nullptr)
		: RendererHelper<T_vbo_count>(propertyCount, shaderTitle)
		, mObjs(capacity, pageSize)
	{

	}

	virtual ~SceneObjectRenderer(void)
	{
		ReleaseObjects();
	}

	void ReleaseObjects()
	{
		mObjs.ForEach([](T_Object* obj)
		{
			delete obj;
		});
		mObjs.DirtyDetachAll();
	}

	typedef InstanceContainer<T_Object> Container;

protected:

	Container mObjs;
};



	} // namespace Renderers
} // namespace CoreFx

#endif // COREFX_RENDERERS_RENDERERHELPER_H