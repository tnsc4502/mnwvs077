#include "SkillEntry.h"
#include "SkillLevelData.h"

SkillEntry::SkillEntry()
{
	m_nMaxLevel = 0;
}

SkillEntry::~SkillEntry()
{
	for (auto& x : m_aLevelData)
		delete x;
}

void SkillEntry::SetLevelData(SkillLevelData * pLevelData, int nSLV)
{
	m_aLevelData[nSLV] = pLevelData;
	m_nMaxLevel = m_nMaxLevel > nSLV ? m_nMaxLevel : nSLV;
}

void SkillEntry::AddLevelData(SkillLevelData * pLevelData)
{
	//m_aLevelData.push_back(pLevelData);
}

const SkillLevelData * SkillEntry::GetLevelData(int nLevel) const
{
	if (nLevel < 0)
		return nullptr;
	if (nLevel > m_nMaxLevel)
		return m_aLevelData[m_nMaxLevel];

	return m_aLevelData[nLevel];
}

const SkillLevelData** SkillEntry::GetAllLevelData() const
{
	return (const SkillLevelData**)m_aLevelData;
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
