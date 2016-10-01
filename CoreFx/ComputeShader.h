#pragma once
#ifndef COREFX_COMPUTESHADER_H
#define COREFX_COMPUTESHADER_H


namespace CoreFx
{

	class Engine;



class ComputeShader : public ObjectInstance
{
	friend class Engine;

public:


	struct Desc
	{
	};


public:

	ComputeShader();
	virtual ~ComputeShader();

	virtual void Compute() = 0;

	bool GetIsInitialized() const { return mIsInitialized; }


protected:

	bool mIsInitialized;
};


template<int T_buffer_count, typename T_Shader = Shader>
class ComputeShaderHelper : public ComputeShader
{
public:

	static constexpr int mBufferCount = T_buffer_count;

protected:

	ComputeShaderHelper(const char * shaderTitle = nullptr)
		: ComputeShader()
		, mShader(shaderTitle)
	{
		std::memset(mBufferIds, 0, sizeof(mBufferIds));
	}

	virtual ~ComputeShaderHelper(void)
	{
		ReleaseBuffers();

		mShader.DeleteShaderProgram();
	}

	virtual void Compute() override
	{
	}

	void CreateBuffers()
	{
		glGenBuffers(mBufferCount, mBufferIds);
	}

	void ReleaseBuffers()
	{
		//Destroy vao and vbo
		glDeleteBuffers(mBufferCount, mBufferIds);
		std::memset(mBufferIds, 0, sizeof(mBufferIds));
	}

	const GLuint* GetBufferIds() const
	{
		return mBufferIds;
	}

protected:

	GLuint mBufferIds[mBufferCount];
	typename T_Shader mShader;

};




} // namespace CoreFx
#endif // COREFX_COMPUTESHADER_H