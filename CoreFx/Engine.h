#ifndef COREFX_ENGINE_H
#define COREFX_ENGINE_H

#pragma once

namespace CoreFx
{
	// =======================================================================
	// =======================================================================

	class TextureManager;
	class SceneObject;
	class Camera;



	namespace Lights
	{
		class Light;
		class PointLight;
		class DirectionalLight;
	}

class Engine
{
public:

	enum BindingIndex
	{
		FrameDataBuffer_BindingIndex = 0
	};

public:

	static void Initialize();
	static void Release();

	static Engine* GetInstance();

	inline TextureManager * GetTextureManager() const 
	{
		assert(mInitialized);
		return mTextureManager;
	}

	void UpdateObjects();
	void RenderObjects();

	Camera* GetCamera() const
	{
		return mCamera;
	}

	void SetCamera(Camera * camera)
	{
		mCamera = camera;
	}

	bool IsDrawVertexNormalEnabled() const
	{
		return mIsDrawVertexNormalEnabled;
	}

	void EnableDrawVertexNormal(bool isEnabled)
	{
		mIsDrawVertexNormalEnabled = isEnabled;
	}

	// Renderers
public:

	bool AttachRenderer(Renderer* renderer)
	{
		return mRenderers->Attach(renderer);
	}

	bool DetachRenderer(Renderer* renderer)
	{
		return mRenderers->Detach(renderer);
	}


	// Lights
public:

	const glm::vec4 & GetAmbientLight() const
	{
		return mAmbientLight;
	}

	void SetAmbientLight(const glm::vec4 & value)
	{
		mAmbientLight = value;
	}

	Lights::PointLight * CreatePointLight(const glm::vec4 & color, const glm::vec3 & position, GLfloat constantAttenuation = 1.f, GLfloat linearAttenuation = 0.f, GLfloat quadraticAttenuation = 0.f);
	Lights::DirectionalLight * CreateDirectionalLight(const glm::vec4 & color, const glm::vec3 & direction);
	void DeleteLight(Lights::Light * & light);

	const TextureBuffer & GetLightDescBuffer() const
	{
		return mLightDescBuffer;
	}

	const TextureBuffer & GetLightDataBuffer() const
	{
		return mLightDataBuffer;
	}

	// ---------------------------------------------------------------------------
	// Debug Tools
	//
	// Draw normals and light direction 
public:

	const Renderers::DrawNormalShader & GetDrawVertexNormal()
	{
		return mDrawVertexNormalShader;
	}
	
	GLfloat GetDrawVertexNormalMagnitude() const { return mDrawVertexNormalMagnitude; }

	void SetDrawVertexNormalMagnitude(GLfloat value)
	{
		mDrawVertexNormalMagnitude = value;
	}

	const glm::vec4 & GetDrawVertexNormalColor() const { return mDrawVertexNormalColor; }

	void SetDrawVertexNormalColor(const glm::vec4 & value)
	{
		mDrawVertexNormalColor = value;
	}

	GLfloat GetDrawLightMagnitude() const { return mDrawLightMagnitude; }

	void SetDrawLightMagnitude(GLfloat value)
	{
		mDrawLightMagnitude = value;
	}

	const glm::vec4 & GetDrawPointLightColor() const { return mDrawPointLightColor; }

	void SetDrawPointLightColor(const glm::vec4 & value)
	{
		mDrawPointLightColor = value;
	}

	const glm::vec4 & GetDrawDirectionalLightColor() const { return mDrawDirectionalLightColor; }

	void SetDrawDirectionalLightlColor(const glm::vec4 & value)
	{
		mDrawDirectionalLightColor = value;
	}

	GLint GetFirstLightIndexToDraw() const	{ return mFirstLightIndexToDraw; }

	void SetFirstLightIndexToDraw(GLint value)
	{
		mFirstLightIndexToDraw = value;
	}

	GLint GetLightToDrawCount() const { return mLightToDrawCount; }

	void SetLightToDrawCount(GLint value)
	{
		mLightToDrawCount = value;
	}
	
	// ---------------------------------------------------------------------------


private:

	typedef InstanceContainer<Renderer> RendererContainer;
	typedef InstanceContainer<Lights::Light> LightContainer;

	enum BufferIdIndex
	{
		FrameData_BufferId,

		__BufferId_Count__
	};


private:

	void InternalInitialize();
	void InternalRelease();

	void InternalCreateFrameDataBuffer();


	Engine();
	~Engine();

	Engine(Engine const &) = delete;
	Engine & operator=(Engine const &) = delete;


private:
	
	TextureManager * mTextureManager;
	RendererContainer * mRenderers;
	LightContainer * mLights;

	Camera* mCamera;

	glm::vec4 mAmbientLight;

	GLuint mLightDataIndex;

	GLuint mBufferIds[__BufferId_Count__];

	glm::vec4 mDrawVertexNormalColor;
	glm::vec4 mDrawPointLightColor;
	glm::vec4 mDrawDirectionalLightColor;
	GLfloat mDrawVertexNormalMagnitude;
	GLfloat mDrawLightMagnitude;
	GLint mFirstLightIndexToDraw;
	GLint mLightToDrawCount;
	Renderers::DrawNormalShader mDrawVertexNormalShader;

	bool mInitialized;
	bool mIsDrawVertexNormalEnabled;

	enum
	{
		MAX_LIGHT_COUNT = 16,
	};

	enum FrameDataUniforms
	{
		u_ProjMatrix,
		u_ViewDQ,
		u_AmbientLight,
		u_LightCount,

		__uniforms_count__
	};

	GLsizei mFrameDataSize;

	GLuint mFrameDataUniformIndices[__uniforms_count__];
	GLint mFrameDataUniformSizes[__uniforms_count__];
	GLint mFrameDataUniformOffsets[__uniforms_count__];
	const char * mFrameDataUniformNames[__uniforms_count__] =
	{
		"u_ProjMatrix",
		"u_ViewDQ.Qr",
		"u_AmbientLight",
		"u_LightCount",
	};

	// Light description buffer : 
	//	- index 0 : light count (same value with u_LightCount)
	//  - index 1 - n : light description with the following incoding
	//		bits 16 - 31 (16 bits) : index in the light data texture buffer
	//		bits 0 - 3 (4 bits) : light type
	//		bits 4 - 15 : reserved 
	TextureBuffer mLightDescBuffer;
	TextureBuffer mLightDataBuffer;


	static Engine* sInstance;
};


// =======================================================================
// =======================================================================
} // namespace CoreFx
#endif // COREFX_ENGINE_H