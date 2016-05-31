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

	namespace Renderers
	{
		class SkyboxRenderer;
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
		gBuffer_NormalBuffer,
		//gBuffer_DepthBuffer,

		__gBuffer_count__
	};

	enum
	{
		MAX_LIGHT_COUNT = 200,
		FIRST_TEXTURE_SAMPLER_INDEX = 10,
		MAX_TEXTURE_SAMPLER = 32
	};

	enum FrameDataUniforms
	{
		u_ProjMatrix,
		u_InvProjMatrix,
		u_ViewMatrix,
		u_ViewDQ,
		u_ViewPosition,
		u_AmbientLight,
		u_VertexNormalColor,
		u_BufferViewportSize,
		u_DepthRangeFovYAspect,
		u_TimeDeltaTime,
		u_NormalMagnitude,
		u_PointLightCount,
		u_SpotLightCount,
		u_DirectionalLightCount,

		__uniforms_count__
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

	GLfloat GetDeltaTime() const { return mTimeDeltaTime[1]; }
	GLfloat GetTime() const { return mTimeDeltaTime[0]; }

	void SetDeltaTime(GLfloat value) { mTimeDeltaTime[1] = value; }
	void SetTime(GLfloat value) { mTimeDeltaTime[0] = value; }


	inline TextureManager * GetTextureManager() const 
	{
		assert(mInitialized);
		return mTextureManager;
	}

	void CreateDynamicResources();

	void UpdateObjects();
	void RenderObjects();

	void SetViewport(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight);

	Camera* GetCamera() const
	{
		return mCamera;
	}

	void SetCamera(Camera * camera)
	{
		mCamera = camera;
	}

	void SetExposure(float value)
	{
		mExposure = value;
	}

	float GetExposure() const
	{
		return mExposure;
	}

	void SetGamma(float value)
	{
		mGamma = value;
		mInvGamma = 1.0f / mGamma;
	}

	float GetGamma() const
	{
		return mGamma;
	}

	bool IsDrawVertexNormalEnabled() const
	{
		return mIsDrawVertexNormalEnabled;
	}

	void EnableDrawVertexNormal(bool isEnabled)
	{
		mIsDrawVertexNormalEnabled = isEnabled;
	}

	bool IsDrawGBufferNormalEnabled() const
	{
		return mIsDrawGBufferNormalEnabled;
	}

	void EnableDrawGBufferNormal(bool isEnabled)
	{
		mIsDrawGBufferNormalEnabled = isEnabled;
	}

	// Renderers
public:

	bool AttachRenderer(Renderer* renderer);

	bool DetachRenderer(Renderer* renderer);

	bool AttachSkyboxRenderer(Renderers::SkyboxRenderer * skybox);
	bool DetachSkyboxRenderer(Renderers::SkyboxRenderer * skybox);

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

	Lights::PointLight * CreatePointLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, GLfloat constantAttenuation = 1.f, GLfloat linearAttenuation = 0.f, GLfloat quadraticAttenuation = 0.f);
	Lights::SpotLight * CreateSpotLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, const glm::vec3 & direction, float innerConeAngle, float outerConeAngle, GLfloat constantAttenuation = 1.f, GLfloat linearAttenuation = 0.f, GLfloat quadraticAttenuation = 0.f);

	Lights::DirectionalLight * CreateDirectionalLight(const glm::vec3 & direction, glm::vec3 const & color, GLfloat intensity);
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


	bool GetWireFrame() const
	{
		return mWireFrame;
	}
	void SetWireFrame(bool value)
	{
		mWireFrame = value;
	}

	void SetDrawGBufferNormalGrid(GLint drawEveryXPixels, GLint drawEveryYPixels);
	const glm::ivec2 & GetDrawGBufferNormalGrid() const 
	{
		return mDrawGBufferNormalGridSpan;
	}

private:

	typedef InstanceContainer<Renderer> RendererContainer;
	typedef InstanceContainer<Lights::Light> LightContainer;

private:

	void InternalInitialize(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight);
	void InternalRelease();

	void InternalCreateFrameDataBuffer(GLuint program);

	void InternalCreateGBuffers();
	void InternalReleaseGBuffers();

	void InternalCreateHdrBuffers();
	void InternalReleaseHdrBuffers();

	void InternalCreateMaterialBuffer();
	static void InternalCreateMaterialBuffer(RendererContainer * renderers, GLsizeiptr & offset, GLint & baseIndex);
	//void InternalCreateTextures();

	void InternalInitializeQuadVAO();
	void InternalInitializeDeferredPassShader();
	void InternalInitializeToneMappingShader();
	void InternalInitializeWireFramePassShader();

	void InternalUpdateDrawGBufferNormalsPatchCount();

	Engine();
	~Engine();

	Engine(Engine const &) = delete;
	Engine & operator=(Engine const &) = delete;


private:

	enum EDeferredShaderUniformIndex
	{
		u_gBufferPosition,
		u_gBufferData,
		u_gBufferNormal,
		//u_materialDataSampler,
		u_lightDescSampler,
		u_lightDataSampler,

		__deferred_uniforms_count__
	};

	enum EToneMappingShaderUniformIndex
	{
		u_Exposure,
		u_InvGamma,
		u_HdrBuffer,

		__tonemapping_uniforms_count__
	};

	enum EWireFrameShaderUniformIndex
	{
		u_WireFrameDrawColor,

		__wireframe_uniforms_count__
	};

	static Engine* sInstance;

	TextureManager * mTextureManager;
	RendererContainer * mRenderers;
	RendererContainer * mForwardRenderers;
	LightContainer * mLights[Lights::Light::__light_type_count__];

	Camera * mCamera;
	Renderers::SkyboxRenderer * mSkybox;

	GLfloat mTimeDeltaTime[2]; // Time and Delta time

	Renderables::VertexArrayObject<1> * mQuad;
	Shader mDeferredShader;
	Shader mToneMappingShader;
	Shader mWireFrameShader;

	GLuint mDeferredFBO;
	GLuint mDepthRBO;
	GLuint mGBuffers[__gBuffer_count__];

	GLuint mForwardFBO;

	GLuint mHdrFBO;
	GLuint mHdrBuffer;

	GLsizei mGBufferWidth;
	GLsizei mGBufferHeight;
	GLint mViewportX, mViewportY;
	GLsizei mViewportWidth, mViewportHeight;

	float mExposure;
	float mGamma;
	float mInvGamma;

	glm::vec4 mAmbientLight;

	GLuint mBufferIds[__BufferId_Count__];

	glm::vec4 mWireFrameColor;
	glm::vec4 mDrawVertexNormalColor;
	glm::vec4 mDrawPointLightColor;
	glm::vec4 mDrawDirectionalLightColor;
	GLfloat mDrawVertexNormalMagnitude;
	GLfloat mDrawLightMagnitude;
	GLint mFirstLightIndexToDraw;
	GLint mLightToDrawCount;
	Renderers::DrawNormalShader mDrawVertexNormalShader;

	GLsizei mFrameDataSize;

	GLuint mFrameDataUniformIndices[__uniforms_count__];
	GLint mFrameDataUniformSizes[__uniforms_count__];
	GLint mFrameDataUniformOffsets[__uniforms_count__];
	const char * mFrameDataUniformNames[__uniforms_count__] =
	{
		"u_ProjMatrix",
		"u_InvProjMatrix",
		"u_ViewMatrix",
		"u_ViewDQ.Qr",
		"u_ViewPosition",
		"u_AmbientLight",
		"u_VertexNormalColor",
		"u_BufferViewportSize",
		"u_DepthRangeFovYAspect",
		"u_TimeDeltaTime",
		"u_NormalMagnitude",
		"u_PointLightCount",
		"u_SpotLightCount",
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

	//TextureMapping mLightPassTextureMapping;

	glm::ivec2 mDrawGBufferNormalGridSpan;
	glm::ivec2 mDrawGBufferNormalPatchCount;
	Renderers::DrawGBufferNormalShader mDrawGBufferNormalShader;

	bool mInitialized;
	bool mIsDrawVertexNormalEnabled;
	bool mIsDrawGBufferNormalEnabled;
	bool mWireFrame;

};


// =======================================================================
// =======================================================================
} // namespace CoreFx
#endif // COREFX_ENGINE_H