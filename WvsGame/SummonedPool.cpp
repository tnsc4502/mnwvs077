#include "SummonedPool.h"
#include "Summoned.h"
#include "Field.h"
#include "User.h"
#include "WvsPhysicalSpace2D.h"
#include "StaticFoothold.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

SummonedPool::SummonedPool(Field *pField)
{
	m_pField = pField;
	m_nSummonedIdCounter = 1000;
}

SummonedPool::~SummonedPool()
{
}

std::mutex & SummonedPool::GetSummonedPoolLock()
{
	std::lock_guard<std::mutex> poolLock(m_mtxSummonedLock);
	return m_mtxSummonedLock;
}

void SummonedPool::OnEnter(User * pUser)
{
	std::lock_guard<std::mutex> poolLock(m_mtxSummonedLock);
	for (auto& pSummoned : m_sSummoned)
	{
		OutPacket oPacket;
		pSummoned->MakeEnterFieldPacket(&oPacket);
		pUser->SendPacket(&oPacket);
	}
}

Summoned * SummonedPool::GetSummoned(int nFieldObjID)
{
	std::lock_guard<std::mutex> poolLock(m_mtxSummonedLock);
	for (auto& pSummoned : m_sSummoned)
		if (pSummoned->GetFieldObjectID() == nFieldObjID)
			return pSummoned;
	return nullptr;
}

bool SummonedPool::CreateSummoned(User* pUser, Summoned *pSummoned, const FieldPoint & pt)
{
	if (!pSummoned->GetMoveAbility(pSummoned->m_nSkillID))
		return false; 

	std::lock_guard<std::mutex> poolLock(m_mtxSummonedLock);
	pSummoned->SetFieldObjectID(m_nSummonedIdCounter++);
	pSummoned->m_ptPos = pt;
	pSummoned->Init(pUser, pSummoned->m_nSkillID, pSummoned->m_nSLV);
	OutPacket oPacket;
	pSummoned->MakeEnterFieldPacket(&oPacket);
	m_pField->BroadcastPacket(&oPacket);
	m_sSummoned.insert(pSummoned);

	return true;
}

Summoned *SummonedPool::CreateSummoned(User* pUser, int nSkillID, int nSLV, const FieldPoint & pt, unsigned int tEnd, bool bMigrate)
{
	if (bMigrate && !Summoned::GetMoveAbility(nSkillID))
		return nullptr;
		
	int nX = pUser->GetPosX();
	int nY = pUser->GetPosY() - 5;
	auto pFH = m_pField->GetSpace2D()->GetFootholdUnderneath(nX, nY, &(nY));
	if (pFH)
	{
		Summoned *pRet = AllocObj(Summoned);
		pRet->SetFh(pFH->GetSN());
		pRet->m_ptPos.x = nX;
		pRet->m_ptPos.y = nY;
		pRet->Init(pUser, nSkillID, nSLV);
		pRet->m_tEnd = tEnd;
		pRet->SetFieldObjectID(m_nSummonedIdCounter++);
		OutPacket oPacket;
		pRet->MakeEnterFieldPacket(&oPacket);
		m_pField->BroadcastPacket(&oPacket);

		m_sSummoned.insert(pRet);
		return pRet;
	}

	return nullptr;
}

void SummonedPool::RemoveSummoned(int nCharacterID, int nSkillID, int nLeaveType)
{
	std::lock_guard<std::mutex> poolLock(m_mtxSummonedLock);
	for(auto& pSummoned : m_sSummoned)
	{
		//auto &pSummoned = m_sSummoned[i];
		if ((nSkillID == -1 || pSummoned->GetSkillID() == nSkillID) && 
			pSummoned->GetOwnerID() == nCharacterID)
		{
			m_sSummoned.erase(pSummoned);
			OutPacket oPacket;
			pSummoned->MakeLeaveFieldPacket(&oPacket);
			m_pField->SplitSendPacket(&oPacket, nullptr);
			if(nLeaveType != Summoned::eLeave_TransferField)
				FreeObj(pSummoned);
			return;
		}
	}
}

void SummonedPool::Update(unsigned int tCur)
{
	std::lock_guard<std::mutex> poolLock(m_mtxSummonedLock);
	for (auto iter = m_sSummoned.begin(); iter != m_sSummoned.end();)
	{
		if (tCur > (*iter)->m_tEnd)
		{
			(*iter)->m_pOwner->RemoveSummoned((*iter));
			OutPacket oPacket;
			(*iter)->MakeLeaveFieldPacket(&oPacket);
			m_pField->SplitSendPacket(&oPacket, nullptr);
			FreeObj((*iter));
			iter = m_sSummoned.erase(iter);
		}
		else
			++iter;
	}
}
