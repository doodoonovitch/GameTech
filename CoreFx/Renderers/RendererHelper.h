#pragma once

#ifndef COREFX_RENDERERS_RENDERERHELPER_H
#define COREFX_RENDERERS_RENDERERHELPER_H


namespace CoreFx
{
	namespace Renderers
	{



template<typename T_Object, int T_vbo_count>
class RendererHelper : public Renderer
{
public:

	static constexpr int mVboCount = T_vbo_count;

protected:

	RendererHelper(GLuint propertyCount)
		: Renderer(propertyCount)
		, mVaoID(0)
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
class SceneObjectRenderer : public RendererHelper<T_Object, T_vbo_count>
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

	virtual void BeginFrame() override
	{
		
	}

	virtual void EndFrame() override
	{
		ClearObjectState();
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

	SceneObjectRenderer(GLuint propertyCount, size_t capacity, size_t pageSize)
		: RendererHelper<T_Object, T_vbo_count>(propertyCount)
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