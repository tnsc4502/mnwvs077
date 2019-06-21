#pragma once
#include <vector>
#include <string>
#include <map>
#include "TemporaryStat.h"

class User;

struct StateChangeItem
{
	std::string sItemName;
	int nItemID;
	std::map<std::string, int> spec;

	TemporaryStat::TS_Flag Apply(User* pUser, unsigned int tCur, bool bApplyBetterOnly, bool bResetByItem = false, bool bForcedSetTime = false, unsigned int nForcedSetTime = 0);
};

