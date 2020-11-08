#include "GameDateTime.h"
#include <Windows.h>
#include <chrono>

GameDateTime::GameDateTime()
{
}


GameDateTime::~GameDateTime()
{
}

long long int GameDateTime::GetCurrentDate()
{
	return GetDateExpireFromPeriodAsMsec(0);
}

long long int GameDateTime::GetDateExpireFromPeriod(int nPeriod)
{
	return GetDateExpireFromPeriodAsMsec(nPeriod * 86400 * 1000);
}

long long int GameDateTime::GetDateExpireFromPeriodAsMsec(int nSec)
{
	namespace sc = std::chrono;
	auto time = sc::system_clock::now();
	auto since_epoch = time.time_since_epoch();
	auto millis = sc::duration_cast<sc::milliseconds>(since_epoch);
	return (millis.count() + nSec * 1000) * 10000 + 116444736000000000L;
}

int unsigned GameDateTime::GetTime()
{
	return timeGetTime();
}
