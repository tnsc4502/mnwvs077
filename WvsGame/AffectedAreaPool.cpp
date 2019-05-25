#include "AffectedAreaPool.h"
#include "AffectedArea.h"
#include "Field.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"

AffectedAreaPool::AffectedAreaPool(Field *pField)
{
	m_pField = pField;
	m_nAffectedAreaIDCounter = 1;
}

AffectedAreaPool::~AffectedAreaPool()
{
}

const std::vector<AffectedArea*>& AffectedAreaPool::GetAffectedAreas() const
{
	return m_apAffectedArea;
}

void AffectedAreaPool::InsertAffectedArea(bool bMobSkill, int nOwnerID, int nSkillID, int nSLV, int tStart, int tEnd, const FieldPoint & pt, const FieldRect & rc, bool bSmoke)
{
	std::lock_guard<std::recursive_mutex> lock(m_pField->GetFieldLock());
	AffectedArea* pArea = AllocObj(AffectedArea);
	pArea->SetFieldObjectID(++m_nAffectedAreaIDCounter);
	pArea->m_bMobSkill = bMobSkill;
	pArea->m_nOwnerID = nOwnerID;
	pArea->m_nSkillID = nSkillID;
	pArea->m_nSLV = nSLV;
	pArea->m_tStart = tStart;
	pArea->m_tEnd = tEnd;
	pArea->m_rcAffectedArea = rc;
	pArea->SetPosX(pt.x);
	pArea->SetPosY(pt.y);
	m_apAffectedArea.push_back(pArea);
	OutPacket oPacket;
	pArea->MakeEnterFieldPacket(&oPacket);
	m_pField->BroadcastPacket(&oPacket);
}

AffectedArea* AffectedAreaPool::GetAffectedAreaByPoint(const FieldPoint & pt)
{
	int tCur = GameDateTime::GetTime();
	std::lock_guard<std::recursive_mutex> lock(m_pField->GetFieldLock());
	for (auto& pAffectedArea : m_apAffectedArea)
	{
		if ((tCur - pAffectedArea->m_tStart > 0) &&
			!pAffectedArea->m_bMobSkill &&
			pAffectedArea->m_rcAffectedArea.PtInRect(pt))
			return pAffectedArea;
	}
	return nullptr;
}

void AffectedAreaPool::Update(int tCur)
{
	std::lock_guard<std::recursive_mutex> lock(m_pField->GetFieldLock());
	for (int i = 0; i < (int)m_apAffectedArea.size(); )
	{
		if (tCur - m_apAffectedArea[i]->m_tEnd > 0)
		{
			OutPacket oPacket;
			m_apAffectedArea[i]->MakeLeaveFieldPacket(&oPacket);
			m_pField->BroadcastPacket(&oPacket);
			FreeObj(m_apAffectedArea[i]);
			m_apAffectedArea.erase(m_apAffectedArea.begin() + i);
			continue;
		}
		else
			++i;
	}
}
