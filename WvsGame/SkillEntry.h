#pragma once
#include <vector>

struct SkillLevelData;

class SkillEntry
{
	std::vector<SkillLevelData*> m_aLevelData;

	int m_nMasterLevel, m_nMaxLevel, m_nSkillID, m_nAttackElemAttr = 0;
	bool m_bInvisible = false;

public:
	SkillEntry();
	~SkillEntry();

	void AddLevelData(SkillLevelData* pLevelData);
	const SkillLevelData* GetLevelData(int nLevel) const;
	const std::vector<SkillLevelData*>& GetAllLevelData() const;

	void SetMasterLevel(int nLevel);
	void SetMaxLevel(int nLevel);
	void SetSkillID(int nID);
	void SetAttackElemAttr(int nAttr);
	void SetInvisible(bool bInvisible);

	int GetMasterLevel() const;
	int GetMaxLevel() const;
	int GetSkillID() const;
	int GetAttackElemAttr() const;
	bool IsInvisible() const;
};

