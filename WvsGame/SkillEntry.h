#pragma once
#include <vector>

struct SkillLevelData;

class SkillEntry
{
	static const int MAX_SKILL_ENTRY_COUNT = 40;
	SkillLevelData* m_aLevelData[MAX_SKILL_ENTRY_COUNT];

	int m_nMasterLevel, m_nMaxLevel, m_nSkillID, m_nAttackElemAttr = 0;
	bool m_bInvisible = false;

public:
	SkillEntry();
	~SkillEntry();

	void SetLevelData(SkillLevelData* pLevelData, int nSLV);
	void AddLevelData(SkillLevelData* pLevelData);
	const SkillLevelData* GetLevelData(int nLevel) const;
	const SkillLevelData** GetAllLevelData() const;

	void SetMasterLevel(int nLevel);
	void SetMaxLevel(int nLevel);
	void SetSkillID(int nID);
	void SetAttackElemAttr(int nAttr);
	void SetInvisible(bool bInvisible);

	int GetMasterLevel() const;
	int GetMaxLevel() const;
	int GetSkillID() const;
	int GetAttackElemAttr() const;
	int AdjustDamageDecRate(int nSLV, int nOrder, int *aDamage, bool bFinalAfterSlashBlast) const;
	bool IsInvisible() const;
};

