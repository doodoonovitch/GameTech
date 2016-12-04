#pragma once

#ifndef COREFX_RENDERERS_RENDERERHELPER_H
#define COREFX_RENDERERS_RENDERERHELPER_H


namespace CoreFx
{
	namespace Renderers
	{



template<int T_vbo_count>
class RendererHelperBase : public Renderer
{
public:

	static constexpr int mVboCount = T_vbo_count;

protected:

	RendererHelperBase(ERenderPass renderPass = Deferred_Pass)
		: Renderer(renderPass)
		, mVaoID(0)
	{
		std::memset(mVboIDs, 0, sizeof(mVboIDs));
	}

	virtual ~RendererHelperBase(void)
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
		std::memset(mVboIDs, 0, sizeof(mVboIDs));

		glDeleteVertexArrays(1, &mVaoID);
		mVaoID = 0;
	}

	GLuint GetVaoID() const
	{
		return mVaoID;
	}

	const GLuint* GetVboIDs() const
	{
		return mVboIDs;
	}

	virtual void Update() override
	{
	}

protected:

	GLuint mVaoID;
	GLuint mVboIDs[mVboCount];	
};

template<int T_vbo_count, typename T_MainShader = Shader, typename T_WireFrameShader = Shader>
class RendererHelper : public RendererHelperBase<T_vbo_count>
{
protected:

	RendererHelper(const char * shaderTitle = nullptr, const char * wireFramShaderTitle = nullptr, ERenderPass renderPass = Deferred_Pass)
		: RendererHelperBase<T_vbo_count>(renderPass)
		, mShader(shaderTitle)
		, mWireFrameShader(wireFramShaderTitle)
	{
		std::memset(mVboIDs, 0, sizeof(mVboIDs));
	}

	virtual ~RendererHelper(void)
	{
		//Destroy shader
		mShader.DeleteShaderProgram();
		mWireFrameShader.DeleteShaderProgram();
	}

protected:

	typename T_MainShader mShader;
	typename T_WireFrameShader mWireFrameShader;
};



template<typename T_Object, int T_vbo_count, typename T_MainShader = Shader, typename T_WireFrameShader = Shader>
class SceneObjectRenderer : public RendererHelper<T_vbo_count, T_MainShader, T_WireFrameShader>
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

	SceneObjectRenderer(size_t capacity, size_t pageSize, const char * shaderTitle = nullptr, const char * wireFramShaderTitle = nullptr, ERenderPass renderPass = Deferred_Pass)
		: RendererHelper<T_vbo_count>(shaderTitle, wireFramShaderTitle, renderPass)
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
		mObjs.DetachAll();
	}

	typedef InstanceContainer<T_Object> Container;

protected:

	Container mObjs;
};



	} // namespace Renderers
} // namespace CoreFx

#endif // COREFX_RENDERERS_RENDERERHELPER_H