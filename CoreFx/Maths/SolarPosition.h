#pragma once
#ifndef COREFX_MATHS_SOLARPOSITION_H
#define COREFX_MATHS_SOLARPOSITION_H


namespace CoreFx
{
	namespace Maths
	{
		// =======================================================================
		// =======================================================================


class SolarPosition
{
public:
	SolarPosition();
	~SolarPosition();

	void SetLocation(float latitudeInDegrees, float longitudeInDegrees);
	void SetTime(const SYSTEMTIME& time);
	void SetTimeZone(int32_t timeZone, bool daySaving);

	const SYSTEMTIME& GetTime() const { return mTime; }
	int32_t GetTimeZone() const { return mTimeZone; }
	bool GetDaySavigin() const { return mDaySaving; }

	float GetLatitude() const { return mLatitude; }
	float GetLongitude() const { return mLongitude; }

	void Update();

	const glm::vec3 & GetPosition() const { return mPosition; }

public:

	inline static void FromHorizontal(glm::vec3 & location, float elevation, float azimut);
	inline static double ComputeJulianDate(const SYSTEMTIME& tm);
	inline static double ComputeGMST(double julianDay);
	inline static double ComputeLMST(double gmst, double timeZone);
	inline static uint32_t ComputeDayOfYear(const SYSTEMTIME& tm);
	//inline static double ComputeDeclinaison(uint32_t dayOfYear);
	//inline static double ComputeHourAngle(double lmst, double rightAscension);

private:

	void Computation();

private:

	float mLatitude;
	float mLongitude;
	SYSTEMTIME mTime;
	int32_t mTimeZone;

	glm::vec3 mPosition;

	bool mDaySaving;
};



// =======================================================================
// =======================================================================



inline void SolarPosition::FromHorizontal(glm::vec3 & location, float elevation, float azimut)
{
	//float phi = MathHelper::HalfPi - elevation;
	float cosE = std::cos(elevation);
	float sinE = std::sin(elevation);
	float sinA = std::sin(azimut);
	float cosA = std::cos(azimut);

	location.y = sinE;
	location.x = cosE * sinA;
	location.z = -cosE * cosA;
}

inline double SolarPosition::ComputeJulianDate(const SYSTEMTIME& tm)
{
	double y, m, d;
	if (m <= 2)
	{
		y = tm.wYear - 1;
		m = tm.wMonth + 12;
	}
	else
	{
		y = tm.wYear;
		m = tm.wMonth;
	}

	d = tm.wDay + (tm.wHour + (tm.wMinute + tm.wSecond / 60) / 60) / 24;

	double jd = 1720996.5 + ((int32_t)(y * 0.01)) + ((int32_t)(y * 0, 0025)) + ((int32_t)(365.25 * y)) + ((int32_t)(30.6001 * (m + 1))) + d - 2451545;

	return jd;
}

inline double SolarPosition::ComputeGMST(double julianDay)
{
	return 18.69737375 + 24.06570982 * julianDay;
}

inline double SolarPosition::ComputeLMST(double gmst, double timeZone)
{
	return gmst + timeZone;
}

inline uint32_t SolarPosition::ComputeDayOfYear(const SYSTEMTIME& tm)
{
	uint32_t n1 = (uint32_t)(275 * tm.wMonth / 9);
	uint32_t n2 = (uint32_t)((tm.wMonth + 9) / 12);
	uint32_t n3 = (1 + (uint32_t)((tm.wYear - 4 * (uint32_t)(tm.wYear / 4) + 2) / 3));
	uint32_t n = n1 - (n2 * n3) + tm.wDay - 30;

	return n;
}

//inline double SolarPosition::ComputeDeclinaison(uint32_t dayOfYear)
//{
//	static double va = 360 / 365.25;
//
//	//double dec = std::asin(0.3978 * std::sin(va * (dayOfYear - (81 - 2 * std::sin(va * (j - 2))))));
//	double dec = std::asin(0.398 * std::sin(0.985 * dayOfYear));
//
//	return dec;
//}
//
//inline double SolarPosition::ComputeHourAngle(double lmst, double rightAscension)
//{
//	return lmst + rightAscension;
//}



		// =======================================================================
		// =======================================================================
	} // namespace Maths
} // namespace CoreFx
#endif // COREFX_MATHS_SOLARPOSITION_H