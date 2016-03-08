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
	std::wstring mName;

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

	int GetPrimaryMonitorIndex() const
	{
		return mPrimaryMonitorIndex;
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

	DisplayMonitorInfoList mDispInfoList;
	int mPrimaryMonitorIndex;
};




	} // namespace Sys
} // namespace CoreGame
#endif // COREGAME_DISPLAYMONITORHELPER_H