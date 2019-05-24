#pragma once
#include "FieldRect.h"
#include <vector>
struct MobSkillLevelData
{
	std::vector<int> anTemplateID;
	FieldRect rcAffectedArea;

	int tTime = 0,
		tInterval = 0,
		nProp = 0,
		nX = 0,
		nHP = 0,
		nLimit = 0,
		nSummonEffect = 0,
		nMPCon = 0;
};

