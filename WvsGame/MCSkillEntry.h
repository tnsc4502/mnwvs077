#pragma once
#include <string>

struct MCSkillEntry
{
	std::string sDesc, sName;
	int nLevel = 0,
		nMobSkillID = 0,
		nSpendCP = 0,
		nTarget = 0;
};