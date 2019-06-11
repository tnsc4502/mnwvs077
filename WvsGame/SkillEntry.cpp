#include "SkillEntry.h"
#include "SkillLevelData.h"

SkillEntry::SkillEntry()
{
}


SkillEntry::~SkillEntry()
{
	for (auto& x : m_aLevelData)
		delete x;
}

void SkillEntry::AddLevelData(SkillLevelData * pLevelData)
{
	m_aLevelData.push_back(pLevelData);
}

const SkillLevelData * SkillEntry::GetLevelData(int nLevel) const
{
	if (nLevel < 0)
		return nullptr;
	if (nLevel >= (int)m_aLevelData.size())
		return m_aLevelData.back();

	return m_aLevelData[nLevel];
}

const std::vector<SkillLevelData*>& SkillEntry::GetAllLevelData() const
{
	return m_aLevelData;
}

void SkillEntry::SetMasterLevel(int nLevel)
{
	m_nMasterLevel = nLevel;
}

void SkillEntry::SetMaxLevel(int nLevel)
{
	//m_aLevelData.resize(nLevel + 1);
	m_nMaxLevel = nLevel;
}

void SkillEntry::SetSkillID(int nID)
{
	m_nSkillID = nID;
}

void SkillEntry::SetAttackElemAttr(int nAttr)
{
	m_nAttackElemAttr = nAttr;
}

void SkillEntry::SetInvisible(bool bInvisible)
{
	m_bInvisible = bInvisible;
}

int SkillEntry::GetMasterLevel() const
{
	return m_nMasterLevel;
}

int SkillEntry::GetMaxLevel() const
{
	return m_nMaxLevel;
}

int SkillEntry::GetSkillID() const
{
	return m_nSkillID;
}

int SkillEntry::GetAttackElemAttr() const
{
	return m_nAttackElemAttr;
}

bool SkillEntry::IsInvisible() const
{
	return m_bInvisible;
}
