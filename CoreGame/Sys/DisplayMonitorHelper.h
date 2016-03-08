#pragma once
#ifndef COREGAME_DISPLAYMONITORHELPER_H
#define COREGAME_DISPLAYMONITORHELPER_H

namespace CoreGame
{
	namespace Sys
	{



struct DisplayMonitorInfo
{
	HMONITOR mHMonitor;
	RECT mVirtualScreen;

	DisplayMonitorInfo()
		: mHMonitor(0)
	{}

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

	const DisplayMonitorInfo & GetPrimaryMonitorInfo() const
	{
		return mDispInfoList[mPrimaryMonitorIndex];
	}

private:

	void AddMonitorInfo(HMONITOR hMonitor, HDC hdcMonitor, const RECT & rcMonitor)
	{
		mDispInfoList.push_back(DisplayMonitorInfo());
		DisplayMonitorInfo & info = mDispInfoList.back();
		info.mHMonitor = hMonitor;
		info.mVirtualScreen = rcMonitor;
		if (mPrimaryHMonitor == hMonitor)
		{
			mPrimaryMonitorIndex = (int)(mDispInfoList.size() - 1);
		}
	}

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

	DisplayMonitorInfoList mDispInfoList;
	HMONITOR mPrimaryHMonitor;
	int mPrimaryMonitorIndex;
};




	} // namespace Sys
} // namespace CoreGame
#endif // COREGAME_DISPLAYMONITORHELPER_H