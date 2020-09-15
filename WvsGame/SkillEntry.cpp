#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "..\WvsLib\Memory\ZMemory.h"

SkillEntry::SkillEntry()
{
	m_nMaxLevel = 0;
}

SkillEntry::~SkillEntry()
{
	for (auto& pLevelData : m_aLevelData)
		FreeObj(pLevelData);
}

void SkillEntry::SetLevelData(SkillLevelData * pLevelData, int nSLV)
{
	if (nSLV >= MAX_SKILL_ENTRY_COUNT)
		return;
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

int SkillEntry::AdjustDamageDecRate(int nSLV, int nOrder, int * aDamage, bool bFinalAfterSlashBlast) const
{	
	static const double dRate_SlashBlast[] = { 0.666667, 0.222222, 0.074074, 0.024691, 0.008230, 0.002743, 0.000914, 0.000305, 0.000102, 0.000033, 0.000011, 0.000004, 0.000001, 0.000000, 0.000000 };
	static const double dRate_IronArrowCrossbow[] = { 1.000000, 0.900000, 0.810000, 0.729000, 0.656100, 0.590490, 0.531441, 0.478296, 0.430467, 0.387410, 0.348678, 0.313810, 0.282429, 0.254186, 0.228767 };
	static const double dRate_ChainLightning[] = { 1.000000, 0.500000, 0.250000, 0.125000, 0.062500, 0.031250, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000 };
	static const double dRate_PiercingArrow[] = { 1.000000, 1.200000, 1.440000, 1.728000, 2.073600, 2.488320, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000 };
	static const int MAX_DMG_COUNT = sizeof(dRate_SlashBlast) / sizeof(double);

	if (nOrder >= MAX_DMG_COUNT || nSLV >= MAX_SKILL_ENTRY_COUNT)
		return 0;

	double dDecRate = 0;
	if (bFinalAfterSlashBlast)
		dDecRate = dRate_SlashBlast[nOrder];
	else if (m_nSkillID == 2221006)
		dDecRate = dRate_ChainLightning[nOrder];
	else if (m_nSkillID == 3101005)
		if (nOrder)
			dDecRate = (double)GetLevelData(nSLV)->m_nX * 0.01;
		else
			dDecRate = 0.5;
	else if (m_nSkillID == 3201005)
		dDecRate = dRate_IronArrowCrossbow[nOrder];
	else if (m_nSkillID == 3221001)
		dDecRate = dRate_PiercingArrow[nOrder];
	else
		return 0;

	for (int i = 0; i < MAX_DMG_COUNT; ++i)
		aDamage[i] = (int)((double)aDamage[i] * dDecRate);

	return 0;
}

bool SkillEntry::IsInvisible() const
{
	return m_bInvisible;
}
