#include "stdafx.h"
#include "CoreGame.h"


namespace CoreGame
{
	namespace Sys
	{


DisplayMonitorHelper::DisplayMonitorHelper()
	: mPrimaryMonitorIndex(0)
{

	const POINT ptZero = { 0, 0 };
	EnumDisplayMonitors(NULL, NULL, MonitorEnumCallback, reinterpret_cast<LPARAM>(this));
}


DisplayMonitorHelper::~DisplayMonitorHelper()
{
}

void DisplayMonitorHelper::AddMonitorInfo(HMONITOR hMonitor, HDC hdcMonitor, const RECT & rcMonitor)
{
	mDispInfoList.push_back(DisplayMonitorInfo());
	DisplayMonitorInfo & info = mDispInfoList.back();
	info.mHMonitor = hMonitor;

	MONITORINFOEX mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);
	info.mVirtualScreen = mi.rcWork;
	info.mName = mi.szDevice;

	if (mi.dwFlags & MONITORINFOF_PRIMARY)
	{
		mPrimaryMonitorIndex = (int)(mDispInfoList.size() - 1);
	}
}




	} // namespace Sys
} // namespace CoreGame
