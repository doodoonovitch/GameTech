#ifndef COREFX_ENGINE_H
#define COREFX_ENGINE_H

#pragma once

namespace CoreFx
{
	// =======================================================================
	// =======================================================================

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
private:

	enum BufferIdIndex
	{
		FrameData_BufferId,

		__BufferId_Count__
	};

	enum GBufferType
	{
		gBuffer_PositionBuffer,
		gBuffer_DataBuffer,
		//gBuffer_DepthBuffer,

		__gBuffer_count__
	};

	enum
	{
		MAX_LIGHT_COUNT = 16,
		FIRST_TEXTURE_SAMPLER_INDEX = 10,
		MAX_TEXTURE_SAMPLER = 32
	};

	enum FrameDataUniforms
	{
		u_ProjMatrix,
		u_ViewDQ,
		u_AmbientLight,
		u_PointLightCount,
		u_DirectionalLightCount,

		__uniforms_count__
	};

	enum class LightType
	{
		PointLight,
		DirectionalLight,

		__count__
	};

public:

	enum BindingIndex
	{
		FrameDataBuffer_BindingIndex = 0
	};

public:

	static void Initialize(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight);
	static void Release();

	static Engine* GetInstance();

	inline TextureManager * GetTextureManager() const 
	{
		assert(mInitialized);
		return mTextureManager;
	}

	void CreateDynamicResources();

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

	bool AttachRenderer(Renderer* renderer);

	bool DetachRenderer(Renderer* renderer);


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

	Lights::PointLight * CreatePointLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat ambient, GLfloat diffuse, GLfloat specular, GLfloat constantAttenuation = 1.f, GLfloat linearAttenuation = 0.f, GLfloat quadraticAttenuation = 0.f);
	Lights::DirectionalLight * CreateDirectionalLight(const glm::vec3 & direction, glm::vec3 const & color, GLfloat ambient, GLfloat diffuse, GLfloat specular);
	void DeleteLight(Lights::Light * & light);

	const TextureBuffer & GetLightDescBuffer() const
	{
		return mLightDescBuffer;
	}

	const TextureBuffer & GetLightDataBuffer() const
	{
		return mLightDataBuffer;
	}

	const TextureBuffer & GetMaterialDataBuffer() const
	{
		return mMaterialBuffer;
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

private:

	void InternalInitialize(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight);
	void InternalRelease();

	void InternalCreateFrameDataBuffer();

	void InternalCreateGBuffers();

	void InternalCreateMaterialBuffer();
	void InternalCreateTextures();

	void InitializeDeferredPassQuadShader();

	Engine();
	~Engine();

	Engine(Engine const &) = delete;
	Engine & operator=(Engine const &) = delete;


private:

	static Engine* sInstance;

	TextureManager * mTextureManager;
	RendererContainer * mRenderers;
	LightContainer * mLights[Lights::Light::__light_type_count__];

	Camera * mCamera;

	Renderables::RenderableObject<1> * mDeferredLightPass;

	GLuint mGBuffer;
	GLuint mDepthBuffer;
	GLuint mGBufferTex[__gBuffer_count__];
	GLsizei mGBufferWidth;
	GLsizei mGBufferHeight;

	GLint mViewportX, mViewportY;
	GLsizei mViewportWidth, mViewportHeight;

	glm::vec4 mAmbientLight;

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

	GLsizei mFrameDataSize;

	GLuint mFrameDataUniformIndices[__uniforms_count__];
	GLint mFrameDataUniformSizes[__uniforms_count__];
	GLint mFrameDataUniformOffsets[__uniforms_count__];
	const char * mFrameDataUniformNames[__uniforms_count__] =
	{
		"u_ProjMatrix",
		"u_ViewDQ.Qr",
		"u_AmbientLight",
		"u_PointLightCount",
		"u_DirectionalLightCount",
	};

	// Light description buffer : 
	//	- index 0 : light count (same value with u_LightCount)
	//  - index 1 - n : light description with the following incoding
	//		bits 16 - 31 (16 bits) : index in the light data texture buffer
	//		bits 0 - 3 (4 bits) : light type
	//		bits 4 - 15 : reserved 
	TextureBuffer mLightDescBuffer;
	TextureBuffer mLightDataBuffer;

	TextureBuffer mMaterialBuffer;
	std::vector<GLfloat> mMaterials;

	//TextureGroupList mTextureGroupList;
	TextureMapping mLightPassTextureMapping;
};


// =======================================================================
// =======================================================================
} // namespace CoreFx
#endif // COREFX_ENGINE_H