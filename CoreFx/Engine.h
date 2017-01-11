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

	enum class EGBuffer
	{
		NormalBuffer,
		UI32Buffer1,
		DepthBuffer,
		//PositionBuffer,

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
		u_InvViewMatrix,
		u_ViewProjMatrix,
		u_ViewDQ,
		u_ViewPosition,
		u_AmbientLight,
		u_WireFrameDrawColor,
		u_VertexNormalColor,
		//u_BufferViewportSize,
		u_NearFarFovYAspect,
		//u_LeftRightTopBottom,
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
		ShowMetallicBuffer		= 3,
		ShowRoughnessBuffer		= 4,
		ShowPositionBuffer		= 5,
		ShowSSAOBuffer			= 6,
		ShowEmissiveBuffer		= 7,
		ShowDepthBuffer			= 8,

		__count__
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

	GLint GetViewPortX() const { return mViewportOrigin.x; }
	GLint GetViewPortY() const { return mViewportOrigin.y; }
	GLsizei GetViewPortWidth() const { return (GLsizei)mViewportSize.x; }
	GLsizei GetViewPortHeight() const { return (GLsizei)mViewportSize.y; }
	const glm::ivec2 & GetViewPortOrigin() const { return mViewportOrigin; }
	const glm::ivec2 & GetViewPortSize() const { return mViewportSize; }

	
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

	CubeMapTexture const * GetSkyCubeMap() const;

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

public:

	float GetSSAORadius() const
	{
		return mSSAORadius;
	}

	void SetSSAORadius(GLfloat value)
	{
		mSSAORadius = value;
	}

	GLuint GetSSAOKernelSize() const
	{
		return mSSAOKernelSize;
	}

	void SetSSAOKernelSize(GLuint value)
	{
		mSSAOKernelSize = value;
	}


	bool IsSSAOEnable() const
	{
		return mIsSSAOEnabled != 0;
	}

	void EnableSSAO(bool enable)
	{
		mIsSSAOEnabled = enable;
	}


	bool IsEnvMapGenEnabled() const
	{
		return mEnvmapGen.mEnabled;
	}

	void EnableEnvMapGen(bool enable)
	{
		mEnvmapGen.mEnabled = enable;
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

public:

	void ComputeViewTransform(GLuint shaderStorageSourceId, GLuint shaderStorageTargetId, GLuint itemCount, GLuint sourceStride, GLuint targetStride);

	const glm::mat4 & GetOrthoProjMatrix() const { return mOrthoProjMatrix; }

private:

	typedef InstanceContainer<Frame> FrameContainer;
	typedef InstanceContainer<ComputeShader> ComputeShaderContainer;
	typedef InstanceContainer<Renderer> RendererContainer;
	typedef InstanceContainer<Lights::Light> LightContainer;
	typedef std::vector<FrameContainer *> FrameContainerContainer;

private:

	void InternalInitialize(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight);
	void InternalRelease();

	void InternalInitializeViewportParams(GLint viewportX, GLint viewportY, GLsizei viewportWidth, GLsizei viewportHeight, GLsizei gBufferWidth, GLsizei gBufferHeight);

	void InternalCreateFrameDataBuffer(GLuint program);

	void InternalGenerateBuffersAndFBOs();
	void InternalReleaseBuffersAndFBOs();

	static GLenum CreateGBuffers(GLuint * gbuffers, GLuint fbo, GLsizei width, GLsizei height);
	static GLenum CreateHDRBuffers(GLuint * buffers, GLuint fbo, GLuint depthBufferId, GLsizei width, GLsizei height);

	void InternalCreateGBuffers();
	void InternalReleaseGBuffers();

	void InternalCreateHdrBuffers();
	void InternalReleaseHdrBuffers();

	void InternalCreateSSAOBuffers();
	void InternalReleaseSSAOBuffers();

	//void InternalCreateMaterialBuffer();
	//static void InternalCreateMaterialBuffer(RendererContainer * renderers, GLsizeiptr & offset, GLint & baseIndex);
	//void InternalCreateTextures();

	void InternalInitializeQuadVAO();
	void InternalInitializeDeferredPassShader();
	void InternalInitializeToneMappingShader();
	void InternalInitializeCopyShader();
	void InternalInitializeViewTex2DArrayShader();
	void InternalInitializeShowDeferredBuffersShader();
	void InternalInitializeSSAOShader();
	void InternalInitializeSSAOBlurShader();
	void InternalInitializePreComputeMatrixShader();

	void InternalUpdateDrawGBufferNormalsPatchCount();
	void InternalUpdateFrameDataUniformBuffer(const Camera & camera);


	void InternalComputePass();
	void InternalRenderObjects();
	void InternalEnvMapRenderObjects(int face);
	void InternalDisplayTexture();
	void InternalRenderDeferredBuffers();

	void InternalInitializeFrameConstainers();
	void InternalReleaseFrameConstainers();

	void InternalInitializeCubemapGen();
	void InternalReleaseCubemapGen();


	Engine();
	~Engine();

	Engine(Engine const &) = delete;
	Engine & operator=(Engine const &) = delete;


private:

	enum EDeferredShaderUniformIndex
	{
		u_DepthSampler,
		u_NBufferSampler,
		u_GBuffer1Sampler,
		u_SSAOSampler,
		u_lightDescSampler,
		u_lightDataSampler,
		u_EnvMap1Sampler,
		u_EnvMap2Sampler,
		u_HasEnvMap,
		u_IsEnvMapHDR,
		u_IsSSAOEnabled,

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
		u_DepthSampler,
		u_NBufferSampler,
		u_GBuffer1Sampler,
		u_BufferToShow,
		u_SSAOSampler,

		__uniforms_count__
	};

	enum class ESSAOShaderUniformIndex
	{
		u_DepthSampler,
		u_NBufferSampler,
		u_GBuffer1Sampler,
		u_NoiseSampler,
		u_KernelSampler,
		u_KernelSize,
		u_Radius,
		u_NoiseScale,

		__uniforms_count__
	};

	enum class ESSAOBlurShaderUniformIndex
	{
		u_ImageIn,
		u_TexOffset,

		__uniforms_count__
	};

	enum class EPreComputeMatrixShaderUniformIndex
	{
		u_SourceStride,
		u_TargetStride,

		__uniforms_count__
	};

	enum class EPreComputeMatrixShaderBindingIndex
	{
		u_TargetBuffer,
		u_SourceBuffer,
	};


	static Engine* sInstance;
	static bool sIsUsedExtensionSupported[__UsedExtensions_count__ + 1];

	TextureManager * mTextureManager;
	RendererContainer * mRenderers;
	RendererContainer * mForwardRenderers;
	RendererContainer * mHUDRenderers;
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
	Shader mSSAOShader;
	Shader mSSAOBlurShader;
	Shader mPreCompMatrixShader;

	enum EFBOIndex
	{
		Deferred_FBO,
		HDR_FBO,
		Forward_FBO,
		SSAO_FBO,

		__fbo_count__
	};
	GLuint mFBOs[__fbo_count__];
	//GLuint mDepthRBO;

	GLuint mGBuffers[(int)EGBuffer::__gBuffer_count__];

	GLuint mForwardBuffer;

	GLuint mHdrBuffer;

	enum ESSAOBufferIndex
	{
		SSAOBuffer_Main,
		SSAOBuffer_Temp,

		SSAOBuffer_Kernel,
		SSAOBuffer_Noise,

		__ssaobuffer_count__
	};
	GLuint mSSAOBuffers[__ssaobuffer_count__];
	GLuint mSSAOKernelSize = 64;
	GLfloat mSSAORadius = 1.0f;
	glm::vec2 mNoiseScale;

	GLsizei mGBufferWidth;
	GLsizei mGBufferHeight;
	glm::ivec2 mViewportOrigin;
	glm::ivec2 mViewportSize;
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
		"u_InvViewMatrix",
		"u_ViewProjMatrix",
		"u_ViewDQ.Qr",
		"u_ViewPosition",
		"u_AmbientLight",
		"u_WireFrameDrawColor",
		"u_VertexNormalColor",
		//"u_BufferViewportSize",
		"u_NearFarFovYAspect",
		//"u_LeftRightTopBottom",
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
	//std::vector<GLfloat> mMaterials;

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

	const CubeMapTexture * mPrecompEnvMapTexture = nullptr;
	const CubeMapTexture * mEnvMapTexture1 = nullptr;
	const CubeMapTexture * mEnvMapTexture2 = nullptr;
	GLboolean mHasEnvMap1 = false;
	GLboolean mHasEnvMap2 = false;
	GLboolean mIsEnvMapHDR1 = false;
	GLboolean mIsEnvMapHDR2 = false;

	GLboolean mIsSSAOEnabled = true;
	
	struct EnvmapGenData
	{
		enum class EFBOIndex
		{
			Deferred_FBO_EnvmapGen,
			HDR_FBO_EnvmapGen,

			__fbo_count_envmapgen__
		};


		//glm::mat4 mProjMatrix;
		//glm::mat4 mViewMatrix[6];
		//glm::mat4 mInvViewMatrix[6];

		GLfloat mNearZ = 1.f;
		GLfloat mFarZ = 1000.f;

		Camera * mCamera;

		CubeMapTexture const * mTexture = nullptr;

		glm::ivec2 mTextureSize = glm::ivec2(128);

		GLuint mFBOs[(int)EFBOIndex::__fbo_count_envmapgen__];
		GLuint mGBuffers[(int)EGBuffer::__gBuffer_count__];

		bool mEnabled = false;

		EnvmapGenData()
		{
			memset(mGBuffers, 0, sizeof(mGBuffers));
			memset(mFBOs, 0, sizeof(mFBOs));
		}
	};

	EnvmapGenData mEnvmapGen;
};


// =======================================================================
// =======================================================================
} // namespace CoreFx
#endif // COREFX_ENGINE_H