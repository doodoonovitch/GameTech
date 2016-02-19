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

	RendererHelper()
		: Renderer()
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

protected:

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

	SceneObjectRenderer(size_t capacity, size_t pageSize)
		: RendererHelper<T_Object, T_vbo_count>()
		, mObjs(capacity, pageSize)
	{

	}

	virtual ~SceneObjectRenderer(void)
	{
		ReleaseObjects();
	}

	void UpdateObjectMatrix()
	{
		mObjs.ForEach([](T_Object* obj)
		{
			if (obj->GetFrame() != nullptr)
				obj->GetFrame()->BuildMatrix();
		});
	}

	void ClearObjectState()
	{
		mObjs.ForEach([](T_Object* obj)
		{
			if (obj->GetFrame() != nullptr)
				obj->GetFrame()->ClearHasNewValue();
		});
	}

	virtual void BeginFrame() override
	{
		UpdateObjectMatrix();
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