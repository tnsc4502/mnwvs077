#pragma once
#include <vector>
#include "MobSkillLevelData.h"

class MobSkillEntry
{
	friend class SkillInfo;

	std::vector<MobSkillLevelData*> m_apLevelData;
	int m_nSkillID = 0;

public:
	MobSkillEntry();
	~MobSkillEntry();

	int GetSkillID() const;
	int GetMaxLevel() const;
	const MobSkillLevelData* GetLevelData(int nSLV);
};

