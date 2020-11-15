#pragma once
#include <vector>

struct SkillLevelData;

class SkillEntry
{
	static const int MAX_SKILL_ENTRY_COUNT = 40;
	SkillLevelData* m_aLevelData[MAX_SKILL_ENTRY_COUNT];

	int m_nMasterLevel, m_nMaxLevel, m_nSkillID, m_nAttackElemAttr = 0;
	bool m_bInvisible = false;

	std::vector<std::pair<int, int>> m_aReqSkill; //<ReqSkillID, ReqLevel>

public:
	SkillEntry();
	~SkillEntry();

	void SetLevelData(SkillLevelData* pLevelData, int nSLV);
	void AddLevelData(SkillLevelData* pLevelData);
	void AddRequiredSkill(const std::pair<int, int>& prReqSkill);
	const SkillLevelData* GetLevelData(int nLevel) const;
	const SkillLevelData** GetAllLevelData() const;
	const std::vector<std::pair<int, int>>& GetRequiredSkill() const;

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

