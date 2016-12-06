#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Maths
	{
		// =======================================================================
		// =======================================================================



SolarPosition::SolarPosition()
	: mDaySaving(false)
{
}


SolarPosition::~SolarPosition()
{
}

void SolarPosition::SetLocation(float latitudeInDegrees, float longitudeInDegrees)
{
	mLatitude = glm::radians<float>(latitudeInDegrees);
	mLongitude = glm::radians<float>(longitudeInDegrees);
}

void SolarPosition::SetTime(const SYSTEMTIME& tm)
{
	mTime = tm;
}

void SolarPosition::SetTimeZone(int32_t timeZone, bool daySaving)
{
	mTimeZone = timeZone;
	mDaySaving = daySaving;
}

void SolarPosition::Update()
{
	Computation();
}

void SolarPosition::Computation()
{
	static const float declinaisonMax = glm::radians<float>(23.45f);
	static const float timeZoneLenght = glm::radians<float>(15.f);
	static const float halfTimeZoneLength = glm::radians<float>(7.5f);

	uint32_t dayOfYear = ComputeDayOfYear(mTime);
	float va = glm::two_pi<float>() / 365.f;
	float x = va * (dayOfYear - 1); // radian
	float x2 = 2.f * x; // radian

	float eot = 0.258f * std::cos(x) - 7.416f * std::sin(x) - 3.648f * std::cos(x2) - 9.228f * std::sin(x2); // minutes

	float lc = (mLongitude - ((mTimeZone * timeZoneLenght) - halfTimeZoneLength)) / timeZoneLenght; // hours
	float lct = mTime.wHour + (mTime.wMinute + mTime.wSecond / 60.f) / 60.f; // local clock time in hours
	float ts = lct + eot / 60.f - lc - (mDaySaving ? 1.f : 0.f); // solar time in hours

	float h = timeZoneLenght * (ts - 12); // hour andle in radian

	float d = declinaisonMax * std::sin(va * (dayOfYear + 284)); // declinaison in radian

	float cosD = std::cos(d);
	float cosH = std::cos(h);
	float sinH = std::sin(h);
	float sinD = std::sin(d);

	float sm1 = cosD * cosH;
	float se1 = cosD * sinH;
	float sp1 = sinD;

	float cosP = std::cos(mLatitude);
	float sinP = std::sin(mLatitude);

	float sz = sm1 * cosP + sp1 * sinP;
	float se = se1;
	float sn = sp1 * cosP - sm1 * sinP;

	mPosition.x = se;
	mPosition.y = sz;
	mPosition.z = sn;
}


		// =======================================================================
		// =======================================================================
	} // namespace Maths
} // namespace CoreFx
