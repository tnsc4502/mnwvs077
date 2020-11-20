#pragma once
class GameDateTime
{
public:
	GameDateTime();
	~GameDateTime();

	const static long long int TIME_PERMANENT = 150841440000000000L;
	const static long long int TIME_UNLIMITED = -1;
	const static long long int TIMEZONE_OFFSET = 12 * 60 * 60;
	//const static long long int TIME_UNLIMITED = 94354848000000000L;

	static long long int GetCurrentDate();
	static long long int GetDateExpireFromPeriod(int nPeriod);
	static long long int GetDateExpireFromPeriodAsMsec(long long int nSec);
	static unsigned int GetTime();
};

