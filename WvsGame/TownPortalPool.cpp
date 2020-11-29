#include "TownPortalPool.h"
#include "TownPortal.h"
#include "Field.h"
#include "User.h"
#include "PortalMap.h"
#include "Portal.h"
#include "PartyMan.h"
#include "..\WvsLib\Net\OutPacket.h"

TownPortalPool::TownPortalPool()
{
}


TownPortalPool::~TownPortalPool()
{
}

void TownPortalPool::OnEnter(User * pUser)
{
	std::lock_guard<std::mutex> lock(m_mtxLock);
	for (auto& prTownPortal : m_mTownPortal)
	{
		OutPacket oPacket;
		prTownPortal.second->MakeEnterFieldPacket(&oPacket);
		pUser->SendPacket(&oPacket);
	}
}

bool TownPortalPool::CreateTownPortal(int nCharacterID, int nX, int nY, unsigned int tEnd)
{
	RemoveTownPortal(nCharacterID);
	TownPortal *pPortal = AllocObj(TownPortal);
	pPortal->Init(nCharacterID, nX, nY, tEnd);

	std::lock_guard<std::mutex> lock(m_mtxLock);
	OutPacket oPacket;
	pPortal->MakeEnterFieldPacket(&oPacket, 0);
	m_pField->BroadcastPacket(&oPacket);
	m_mTownPortal.insert({ nCharacterID, pPortal });
	return true;
}

void TownPortalPool::RemoveTownPortal(int nCharacterID)
{
	std::lock_guard<std::mutex> lock(m_mtxLock);
	auto findIter = m_mTownPortal.find(nCharacterID);
	if (findIter != m_mTownPortal.end())
	{
		OutPacket oPacket;
		findIter->second->MakeLeaveFieldPacket(&oPacket);
		m_mTownPortal.erase(findIter);

		m_pField->BroadcastPacket(&oPacket);
	}
}

void TownPortalPool::Update(unsigned int tCur)
{
	std::lock_guard<std::mutex> lock(m_mtxLock);
	auto prPortal = m_mTownPortal.begin();
	while(prPortal != m_mTownPortal.end())
	{
		if (prPortal->second->m_tEnd < tCur)
		{
			OutPacket oPacket;
			prPortal->second->MakeLeaveFieldPacket(&oPacket, 0);
			m_pField->BroadcastPacket(&oPacket);
			//auto pUser = User::FindUser(prPortal->second->m_nCharacterID);
			//if (pUser)
			PartyMan::GetInstance()->NotifyTownPortalChanged(prPortal->second->m_nCharacterID, 999999999, 999999999, -1, -1);
			prPortal = m_mTownPortal.erase(prPortal);
		}
		else
			++prPortal;
	}
}

void TownPortalPool::SetField(Field * pField)
{
	m_pField = pField;
}

void TownPortalPool::AddTownPortalPos(FieldPoint pos)
{
	m_aTownPortal.push_back(pos);
}

FieldPoint TownPortalPool::GetTownPortalPos(int nIdx)
{
	if (nIdx >= 0 && nIdx < m_aTownPortal.size())
		return m_aTownPortal[nIdx];
	else
	{
		auto pPortal = m_pField->GetPortalMap()->GetRandStartPoint();
		if (pPortal)
			return{ pPortal->GetX(), pPortal->GetY() };
		else
			return{ 0, 0 };
	}
}

const TownPortal * TownPortalPool::GetTownPortal(int nCharacterID)
{
	std::lock_guard<std::mutex> lock(m_mtxLock);
	auto findIter = m_mTownPortal.find(nCharacterID);
	return findIter == m_mTownPortal.end() ? nullptr : (TownPortal *)findIter->second;
}
