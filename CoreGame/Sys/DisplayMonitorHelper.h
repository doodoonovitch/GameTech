#pragma once
#ifndef COREGAME_DISPLAYMONITORHELPER_H
#define COREGAME_DISPLAYMONITORHELPER_H

namespace CoreGame
{
	namespace Sys
	{


struct DisplayAdapter
{
	size_t mIndex;
	std::wstring mName;
	std::wstring mLabel;
	DWORD mStateFlags;

	DisplayAdapter(size_t index) : mIndex(index), mStateFlags(0) {}
};
typedef std::vector<DisplayAdapter> DisplayAdapterList;

struct DisplayMonitor
{
	size_t mAdapterIndex;
	std::wstring mName;
	std::wstring mLabel;
	DWORD mStateFlags;
	HMONITOR mHMonitor;
	RECT mVirtualScreen;

	LONG mX;
	LONG mY;
	LONG mWidth;
	LONG mHeight;

	DWORD mBitsPerPel;
	DWORD mDisplayFlags;
	DWORD mDisplayFrequency;
	
	DisplayMonitor() : mAdapterIndex(0), mHMonitor(0)	{}
};
typedef std::vector<DisplayMonitor> DisplayMonitorList;



struct DisplayMonitorInfo
{
	HMONITOR mHMonitor;
	RECT mVirtualScreen;
	std::wstring mName;
	std::wstring mLabel;

	DisplayMonitorInfo() : mHMonitor(0)	{}

};
typedef std::vector<DisplayMonitorInfo> DisplayMonitorInfoList;

class DisplayMonitorHelper
{
public:


public:
	DisplayMonitorHelper();
	~DisplayMonitorHelper();

	const DisplayMonitorInfoList & GetDisplayMonitorList() const
	{
		return mDispInfoList;
	}

	int GetPrimaryMonitorIndex() const
	{
		return mPrimaryMonitorIndex;
	}

	const DisplayAdapterList & GetAdapterList() const
	{
		return mAdapterList;
	}
	
	const DisplayMonitorList & GetMonitorList() const
	{
		return mMonitorList;
	}


private:

	void AddMonitorInfo(HMONITOR hMonitor, HDC hdcMonitor, const RECT & rcMonitor);

	static BOOL CALLBACK MonitorEnumCallback(
		_In_ HMONITOR hMonitor,
		_In_ HDC      hdcMonitor,
		_In_ LPRECT   lprcMonitor,
		_In_ LPARAM   dwData
		)
	{
		DisplayMonitorHelper * helper = reinterpret_cast<DisplayMonitorHelper *>(dwData);
		helper->AddMonitorInfo(hMonitor, hdcMonitor, *lprcMonitor);
		return TRUE;
	}

private:

	DisplayAdapterList mAdapterList;
	DisplayMonitorList mMonitorList;
	DisplayMonitorInfoList mDispInfoList;	
	int mPrimaryMonitorIndex;
};




	} // namespace Sys
} // namespace CoreGame
#endif // COREGAME_DISPLAYMONITORHELPER_H