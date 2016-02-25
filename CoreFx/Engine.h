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

	Lights::PointLight * CreatePointLight(const glm::vec4 & color, const glm::vec3 & position);
	Lights::DirectionalLight * CreateDirectionalLight(const glm::vec4 & color, const glm::vec3 & direction);
	void DeleteLight(Lights::Light * & light);

private:

	typedef InstanceContainer<Renderer> RendererContainer;
	typedef InstanceContainer<Lights::Light> LightContainer;

	enum BufferIdIndex
	{
		FrameData_BufferId,

		__BufferId_Count__
	};

	enum
	{
		MAX_LIGHT_COUNT = 16,
		MAX_LIGHT_DATA_COUNT = 64
	};

	struct FrameData
	{
		FrameData()
			: vAmbientLight(0.f, 0.f, 0.f, 0.f)
		{

		}

		__declspec(align(16)) glm::mat4 mView;
		__declspec(align(16)) glm::mat4 mProj;
		__declspec(align(16)) glm::vec4 vAmbientLight;
		__declspec(align(16)) GLuint uLightDesc[MAX_LIGHT_COUNT];
		__declspec(align(16)) glm::vec4 vLightData[MAX_LIGHT_DATA_COUNT];
		__declspec(align(4)) GLuint uLightCount;
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


	bool mInitialized;
	bool mIsDrawVertexNormalEnabled;

	static Engine* sInstance;
};


// =======================================================================
// =======================================================================
} // namespace CoreFx
#endif // COREFX_ENGINE_H