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
	mPrimaryHMonitor = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);

	EnumDisplayMonitors(NULL, NULL, MonitorEnumCallback, reinterpret_cast<LPARAM>(this));
}


DisplayMonitorHelper::~DisplayMonitorHelper()
{
}





	} // namespace Sys
} // namespace CoreGame
