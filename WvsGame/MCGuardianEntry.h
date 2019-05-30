#pragma once
#include <string>

struct MCGuardianEntry
{
	std::string sDesc, sName;
	int nLevel = 0,
		nMobSkillID = 0,
		nSpendCP = 0,
		nType = 0;
};
