#pragma once
#ifndef COREGAME_SIMPLECAMERA_H
#define COREGAME_SIMPLECAMERA_H



namespace CoreGame
{


class SimpleCamera : public GameEngine
{
public:

	SimpleCamera(GameProgram & gameProgram);
	~SimpleCamera();


	void OnInit();
	void OnShutdown();

	void OnUpdate();
	void OnRender();

	void OnWindowResize(int nw, int nh);

	void OnMouseButton(MouseState mouseState, int x, int y);
	void OnMouseMove(int x, int y);
	void OnKeyDown(WPARAM key, bool wasPressed, int repeatCount, bool altPressed);
	void OnKeyUp(WPARAM key, bool wasPressed, int repeatCount, bool altPressed);

	void OnWindowActivate() {}
	void OnWindowUnactivate() {}

	void UpdateSunPosition();

private:

	enum class EFont
	{
		Normal,
		NormalBold,
		NormalItalic,
		Title,
		Header,

		__fonts_count__
	};

	enum class ETextGroup
	{
		Default,
		ShowDeferredBuffers,
		HelpInfo,
	};

	struct CityPositionInfo
	{
		float mLatitude;
		float mLongitude;
		int mYear;
		int mMonth;
		int mDay;
		int mSunrise;
		int mSunset;
		int mTimeZone;
		bool mDaySaving;
		std::wstring mName;
	};

	enum class ECity
	{
		Athens,
		BikiniAtoll,
		Kyoto,
		Longyearbyen,
		Nuuk,
		Oslo,
		Paris,
		Rome,
		Sydney,
		Singapore,
		Stanley,

		__count__
	};

private:

	void filterMouseMoves(float dx, float dy);

	void SetupViewportAndProjection();

	void InitializeTextRenderer();
	void InitializeTextPages();

	void UpdateShowDeferredBuffersText();

	void SetCity(ECity cityIndex);
	void UpdateSunPosTextPage();
	void UpdateSunPosTextPageTime();

private:

	static CityPositionInfo mCityList[(int)ECity::__count__];

	static const int VK_W = 0x57;
	static const int VK_S = 0x53;
	static const int VK_A = 0x41;
	static const int VK_D = 0x44;
	static const int VK_Q = 0x51;
	static const int VK_Z = 0x5a;

	static const int MOUSE_HISTORY_BUFFER_SIZE = 10;

	const float MOUSE_FILTER_WEIGHT = 0.75f;
	const float DEFAULT_MOVE_SPEED = 20.f;

	float mMoveSpeed = DEFAULT_MOVE_SPEED;

	GLsizei mWindowWidth = 1280;
	GLsizei mWindowHeight = 960;

	//camera transformation variables
	int mState = 0, mOldX = 0, mOldY = 0;
	float mRotX = 0.f, mRotY = 135.f, mDist = 0.f;

	CoreFx::Camera* mCamera = nullptr;
	CoreFx::Renderers::SkydomeRenderer * mSkydome = nullptr;
	CoreFx::Lights::DirectionalLight * mSunLight = nullptr;
	CoreFx::Renderers::TextRenderer * mTextRenderer = nullptr;

	CoreFx::Renderers::TextPageWeakPtr mHelpInfoPage;
	CoreFx::Renderers::TextPageWeakPtr mShowDeferredBuffersPage;
	CoreFx::Renderers::TextPageWeakPtr mFrameInfoPage;
	CoreFx::Renderers::TextPageWeakPtr mSunPosPage;
	CoreFx::Renderers::TextPageWeakPtr mTestPage;

	CoreFx::Renderers::TextGroupWeakPtr mDefaultTextGroup;
	CoreFx::Renderers::TextGroupWeakPtr mHelpInfoTextGroup;
	CoreFx::Renderers::TextGroupWeakPtr mShowDeferredBuffersTextGroup;

	CoreFx::Renderers::TextGroupWeakPtr mLastActiveTextGroup;

	size_t mTimeTextLineIndex;

	ECity mCity = ECity::Paris;
	int mDayTimeIncMinutes = 10;
	int mCurrentDayTime = 8 * 60;

	glm::vec2 m_mouseHistory[MOUSE_HISTORY_BUFFER_SIZE];

	float mMouseX = 0, mMouseY = 0; //filtered mouse values
	bool mUseFiltering = true;
	bool mFreezeTimer = false;

	GLint mShowDeferredBufferState = 0;
	bool mIsInitialized = false;

	CoreFx::Maths::SolarPosition mSolarPosition;

};



} // namespace CoreGame
#endif // COREGAME_SIMPLECAMERA_H
