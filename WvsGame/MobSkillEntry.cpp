#include "MobSkillEntry.h"

MobSkillEntry::MobSkillEntry()
{
}


MobSkillEntry::~MobSkillEntry()
{
}

int MobSkillEntry::GetSkillID() const
{
	return m_nSkillID;
}

int MobSkillEntry::GetMaxLevel() const
{
	return (int)m_apLevelData.size() - 1;
}

const MobSkillLevelData * MobSkillEntry::GetLevelData(int nSLV)
{
	if (nSLV < 1 || nSLV >= (int)m_apLevelData.size())
		return nullptr;
	return m_apLevelData[nSLV];
}
