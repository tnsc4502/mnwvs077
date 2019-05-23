#pragma once
#include <string>
#include <vector>

struct MobSummonItem
{
	std::string sItemName;
	int nItemID,
		nType;

	// pair < MobID, Prob >
	std::vector<std::pair<int, int>> lMob;
};

