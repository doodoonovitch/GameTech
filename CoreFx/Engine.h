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
		class SkydomeRenderer;
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
		//gBuffer_PositionBuffer,
		gBuffer_NormalBuffer,
		gBuffer_AlbedoAndStatus,
		gBuffer_SpecularRoughness,
		gBuffer_Emissive,
		gBuffer_DepthBuffer,

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
		u_OrthoProjMatrix,
		u_ViewMatrix,
		u_ViewDQ,
		u_ViewPosition,
		u_AmbientLight,
		u_WireFrameDrawColor,
		u_VertexNormalColor,
		u_BufferViewportSize,
		u_NearFarFovYAspect,
		u_LeftRightTopBottom,
		u_TimeDeltaTime,
		u_NormalMagnitude,
		u_Exposure,
		u_InvGamma,
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

	enum UsedExtensions
	{
		ARB_shader_draw_parameters,

		__UsedExtensions_count__
	};

	enum class EDeferredDebug
	{
		ShowNormalBuffer		= 1,
		ShowAlbedoBuffer		= 2,
		ShowSpecularBuffer		= 3,
		ShowRoughnessBuffer		= 4,
		ShowPositionBuffer		= 5,
	};

public:

	static void Initialize(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight);
	static void Release();

	static Engine* GetInstance();

	void EnableDeferredDebug(EDeferredDebug showBuffer)
	{
		mDeferredDebugState = (GLint)showBuffer;
	}
	void DisableDeferredDebug()
	{
		mDeferredDebugState = 0;
	}

	bool IsUsedExtensionSupported(UsedExtensions extension) const
	{
		return sIsUsedExtensionSupported[extension];
	}

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

	GLint GetViewPortX() const { return mViewportX; }
	GLint GetViewPortY() const { return mViewportY; }
	GLsizei GetViewPortWidth() const { return mViewportWidth; }
	GLsizei GetViewPortHeight() const { return mViewportHeight; }
	
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

	bool AttachComputeShader(ComputeShader* cs);
	bool DetachComputeShader(ComputeShader* cs);

	bool AttachRenderer(Renderer* renderer);
	bool DetachRenderer(Renderer* renderer);

	bool AttachSkyboxRenderer(Renderers::SkyboxRenderer * skybox);
	bool DetachSkyboxRenderer(Renderers::SkyboxRenderer * skybox);

	bool AttachSkydomeRenderer(Renderers::SkydomeRenderer * skydome);
	bool DetachSkydomeRenderer(Renderers::SkydomeRenderer * skydome);

	// Lights
public:

	Frame * CreateObjectLocation(SceneObjectType sceneObjectType, size_t defaultContainerCapacity = 100, size_t defaultContainerPageSize = 100);
	void DeleteObjectLocation(Frame *& frame);

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

	Lights::PointLight * CreatePointLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, GLfloat raiuds);
	Lights::SpotLight * CreateSpotLight(const glm::vec3 & position, glm::vec3 const & color, GLfloat intensity, GLfloat radius, const glm::vec3 & direction, float innerConeAngle, float outerConeAngle);

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

	// ---------------------------------------------------------------------------


	bool GetWireFrame() const
	{
		return mWireFrame;
	}
	void SetWireFrame(bool value)
	{
		mWireFrame = value;
	}

	const glm::vec4 & GetWireFrameColor() const
	{
		return mWireFrameColor;
	}
	void SetWireFrameColor(const glm::vec4 & value)
	{
		mWireFrameColor = value;
	}

	void SetDrawGBufferNormalGrid(GLint drawEveryXPixels, GLint drawEveryYPixels);
	const glm::ivec2 & GetDrawGBufferNormalGrid() const 
	{
		return mDrawGBufferNormalGridSpan;
	}

	bool IsDrawLightPositionEnabled() const { return mIsDrawLightPositionEnabled; }
	void SetIsDrawLightPositionEnabled(bool value)
	{
		mIsDrawLightPositionEnabled = value;
	}

public:

	void DisplayTexture2DArray(const Texture * texture, GLint layerIndex);
	void UndisplayTexture2DArray();

private:

	typedef InstanceContainer<Frame> FrameContainer;
	typedef InstanceContainer<ComputeShader> ComputeShaderContainer;
	typedef InstanceContainer<Renderer> RendererContainer;
	typedef InstanceContainer<Lights::Light> LightContainer;
	typedef std::vector<FrameContainer *> FrameContainerContainer;

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
	void InternalInitializeCopyShader();
	void InternalInitializeViewTex2DArrayShader();
	void InternalInitializeShowDeferredBuffersShader();

	void InternalUpdateDrawGBufferNormalsPatchCount();

	void InternalComputePass();
	void InternalRenderObjects();
	void InternalDisplayTexture();
	void InternalRenderDeferredBuffers();

	void InternalInitializeFrameConstainers();
	void InternalReleaseFrameConstainers();

	Engine();
	~Engine();

	Engine(Engine const &) = delete;
	Engine & operator=(Engine const &) = delete;


private:

	enum EDeferredShaderUniformIndex
	{
		//u_gBufferPosition,
		u_gDepthMap,
		u_gBufferNormal,
		u_gBufferAlbedoAndStatus,
		u_gBufferSpecularRoughness,
		u_gBufferEmissive,
		//u_materialDataSampler,
		u_lightDescSampler,
		u_lightDataSampler,

		__deferred_uniforms_count__
	};

	enum EToneMappingShaderUniformIndex
	{
		u_HdrBuffer,

		__tonemapping_uniforms_count__
	};

	enum ECopyShaderUniformIndex
	{
		u_SourceBuffer,

		__copyshader_uniforms_count__
	};

	enum ECopyText2DArrayShaderUniformIndex
	{
		u_TextureSampler,
		u_LayerIndex,

		__copytex2Darrayshader_uniforms_count__
	};

	enum class EShowDeferredShaderUniformIndex 
	{
		//u_gBufferPosition,
		u_gDepthMap,
		u_gBufferNormal,
		u_gBufferAlbedoAndStatus,
		u_gBufferSpecularRoughness,
		u_gBufferEmissive,
		u_lightDescSampler,
		u_lightDataSampler,
		u_BufferToShow,

		__uniforms_count__
	};

	static Engine* sInstance;
	static bool sIsUsedExtensionSupported[__UsedExtensions_count__ + 1];

	TextureManager * mTextureManager;
	RendererContainer * mRenderers;
	RendererContainer * mForwardRenderers;
	ComputeShaderContainer * mComputes;
	LightContainer * mLights[Lights::Light::__light_type_count__];
	GLint mLightDescIndexOffsets[Lights::Light::__light_type_count__];
	FrameContainerContainer * mFrameContainers;

	Camera * mCamera;
	Renderers::SkyboxRenderer  * mSkybox;
	Renderers::SkydomeRenderer * mSkydome;

	GLfloat mTimeDeltaTime[2]; // Time and Delta time

	Renderables::VertexArrayObject<1> * mQuad;
	Shader mDeferredShader;
	Shader mToneMappingShader;
	Shader mCopyShader;
	Shader mViewTex2DArrayShader;
	Shader mShowDeferredBuffersShader;

	GLuint mDeferredFBO;
	//GLuint mDepthRBO;
	GLuint mGBuffers[__gBuffer_count__];

	GLuint mForwardFBO;
	GLuint mForwardBuffer;

	GLuint mHdrFBO;
	GLuint mHdrBuffer;

	GLsizei mGBufferWidth;
	GLsizei mGBufferHeight;
	GLint mViewportX, mViewportY;
	GLsizei mViewportWidth, mViewportHeight;
	glm::mat4 mOrthoProjMatrix;

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

	GLsizei mFrameDataSize;

	GLuint mFrameDataUniformIndices[__uniforms_count__];
	GLint mFrameDataUniformSizes[__uniforms_count__];
	GLint mFrameDataUniformOffsets[__uniforms_count__];
	const char * mFrameDataUniformNames[__uniforms_count__] =
	{
		"u_ProjMatrix",
		"u_InvProjMatrix",
		"u_OrthoProjMatrix",
		"u_ViewMatrix",
		"u_ViewDQ.Qr",
		"u_ViewPosition",
		"u_AmbientLight",
		"u_WireFrameDrawColor",
		"u_VertexNormalColor",
		"u_BufferViewportSize",
		"u_NearFarFovYAspect",
		"u_LeftRightTopBottom",
		"u_TimeDeltaTime",
		"u_NormalMagnitude",
		"u_Exposure",
		"u_InvGamma",
		"u_PointLightCount",
		"u_SpotLightCount",
		"u_DirectionalLightCount",
	};
	friend class Renderers::PointLightPositionRenderer;
	friend class Renderers::SpotLightPositionRenderer;

	// Light description buffer : 
	//	- index 0 : light count (same value with u_LightCount)
	//  - index 1 - n : light description with the following incoding
	//		bits 16 - 31 (16 bits) : index in the light data texture buffer
	//		bits 0 - 3 (4 bits) : light type
	//		bits 4 - 15 : reserved 
	TextureBuffer mLightDescBuffer;
	TextureBuffer mLightDataBuffer;


	Renderers::PointLightPositionRenderer * mPointLightPositionRenderer;
	Renderers::SpotLightPositionRenderer * mSpotLightPositionRenderer;

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
	bool mIsDrawLightPositionEnabled;

	const Texture * mDisplayTexture;
	GLint mDisplayTextureLayerIndex;

	GLint mDeferredDebugState = 0;

};


// =======================================================================
// =======================================================================
} // namespace CoreFx
#endif // COREFX_ENGINE_H