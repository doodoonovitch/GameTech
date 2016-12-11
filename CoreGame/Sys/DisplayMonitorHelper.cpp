#include "stdafx.h"
#include "CoreGame.h"


namespace CoreGame
{
	namespace Sys
	{

	static void DumpDevice(const DISPLAY_DEVICE& dd, int nSpaceCount)
	{
		wprintf(L"%*sDevice Name: %s\n", nSpaceCount, L"", dd.DeviceName);
		wprintf(L"%*sDevice String: %s\n", nSpaceCount, L"", dd.DeviceString);
		wprintf(L"%*sState Flags: %x\n", nSpaceCount, L"", dd.StateFlags);
		wprintf(L"%*sDeviceID: %s\n", nSpaceCount, L"", dd.DeviceID);
		wprintf(L"%*sDeviceKey: ...%s\n\n", nSpaceCount, L"", dd.DeviceKey + 42);
	}

DisplayMonitorHelper::DisplayMonitorHelper()
	: mPrimaryMonitorIndex(0)
{

	DISPLAY_DEVICE dd;

	dd.cb = sizeof(DISPLAY_DEVICE);

	DWORD deviceNum = 0;
	while (EnumDisplayDevices(NULL, deviceNum, &dd, 0))
	{
		//DumpDevice(dd, 0);
		DISPLAY_DEVICE newdd = { 0 };
		newdd.cb = sizeof(DISPLAY_DEVICE);
		DWORD monitorNum = 0;
		while (EnumDisplayDevices(dd.DeviceName, monitorNum, &newdd, 0))
		{
			//DumpDevice(newdd, 4);

			mMonitorList.push_back(DisplayMonitor());
			DisplayMonitor & dm = mMonitorList.back();
			dm.mName = dd.DeviceName;
			dm.mLabel = dd.DeviceString;
			dm.mStateFlags = dd.StateFlags;
			dm.mAdapterIndex = mAdapterList.size();

			monitorNum++;
		}

		if (monitorNum > 0)
		{
			mAdapterList.push_back(DisplayAdapter(deviceNum));
			DisplayAdapter & da = mAdapterList.back();
			da.mName = dd.DeviceName;
			da.mLabel = dd.DeviceString;
			da.mStateFlags = dd.StateFlags;
		}

		//puts("");
		deviceNum++;
	}

	const POINT ptZero = { 0, 0 };
	EnumDisplayMonitors(NULL, NULL, MonitorEnumCallback, reinterpret_cast<LPARAM>(this));

	for (DisplayMonitorList::iterator it = mMonitorList.begin(); it != mMonitorList.end(); ++it)
	{
		DisplayMonitor & dm = *it;
		DEVMODE devMode;
		devMode.dmSize = sizeof(DEVMODE);
		if (EnumDisplaySettings(dm.mName.c_str(), ENUM_CURRENT_SETTINGS, &devMode))
		{
			dm.mX = devMode.dmPosition.x;
			dm.mY = devMode.dmPosition.y;
			dm.mWidth = (LONG)devMode.dmPelsWidth;
			dm.mHeight = (LONG)devMode.dmPelsHeight;
			dm.mBitsPerPel = devMode.dmBitsPerPel;
			dm.mDisplayFlags = devMode.dmDisplayFlags;
			dm.mDisplayFrequency = devMode.dmDisplayFrequency;

		}
	}
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

	DisplayAdapterList::iterator it = std::find_if(mAdapterList.begin(), mAdapterList.end(), [&info](const DisplayAdapter & da)->bool { return da.mName == info.mName; });
	if (it != mAdapterList.end())
	{
		const DisplayAdapter & da = *it;
		DisplayMonitorList::iterator it2 = std::find_if(mMonitorList.begin(), mMonitorList.end(), [&da](const DisplayMonitor & dm)->bool { return dm.mAdapterIndex == da.mIndex; });

		if (it2 != mMonitorList.end())
		{
			DisplayMonitor & dm = *it2;
			dm.mHMonitor = hMonitor;
			dm.mVirtualScreen = mi.rcWork;
		}
	}
}




	} // namespace Sys
} // namespace CoreGame
