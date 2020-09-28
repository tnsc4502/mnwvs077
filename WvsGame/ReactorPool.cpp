#include "ReactorPool.h"
#include "Field.h"
#include "User.h"
#include "ReactorTemplate.h"
#include "Reactor.h"
#include "LifePool.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsGame\ReactorPacketTypes.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include <mutex>

std::atomic<int> ReactorPool::ReactorGen::stGenID = 1000;

ReactorPool::ReactorPool()
{
}


ReactorPool::~ReactorPool()
{
}

void ReactorPool::Init(Field * pField, void * pImg)
{
	m_pField = pField;
	auto& reactorImg = *((WzIterator*)pImg);
	for (auto& reactor : reactorImg)
	{
		ReactorGen newGen;
		newGen.nTemplateID = atoi(((std::string)reactor["id"]).c_str());
		newGen.nX = (int)reactor["x"];
		newGen.nY = (int)reactor["y"];
		newGen.bFlip = (int)reactor["f"] == 0 ? false : true;
		newGen.tRegenInterval = (int)reactor["reactorTime"] * 1000;
		newGen.sName = (std::string)reactor["name"];

		m_aReactorGen.push_back(std::move(newGen));
	}
	TryCreateReactor(true);
}

void ReactorPool::TryCreateReactor(bool bReset)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxReactorPoolMutex);
	std::vector<ReactorGen*> aGen;

	auto tCur = GameDateTime::GetTime();
	if (!bReset && (int)(tCur - m_tLastCreateReactorTime) < UPDATE_INTERVAL)
		return;

	for (auto& gen : m_aReactorGen)
	{
		if (!bReset && gen.tRegenInterval <= 0)
			continue;
		if (!bReset && (gen.nReactorCount || tCur < gen.tRegenAfter))
			continue;
		aGen.push_back(&gen);
	}

	while (aGen.size() > 0)
	{
		int nIdx = 0;
		if (!(aGen[nIdx]->tRegenInterval))
			nIdx = ((int)Rand32::GetInstance()->Random()) % aGen.size();
		auto pGen = aGen[nIdx];
		aGen.erase(aGen.begin() + nIdx);
		CreateReactor(pGen);
	}
	m_tLastCreateReactorTime = tCur;
}

void ReactorPool::CreateReactor(ReactorGen * pPrg)
{
	auto pTemplate = ReactorTemplate::GetReactorTemplate(pPrg->nTemplateID);
	if (pTemplate == nullptr)
		return;

	auto pReactor = MakeUnique<Reactor>(pTemplate, m_pField);
	pReactor->m_nTemplateID = pPrg->nTemplateID;
	pReactor->m_ptPos.x = pPrg->nX;
	pReactor->m_ptPos.y = pPrg->nY;
	pReactor->m_bFlip = pPrg->bFlip;
	pReactor->m_nHitCount = 0;
	pReactor->m_nState = 0;
	pReactor->m_nOldState = 0;
	pReactor->m_tStateEnd = GameDateTime::GetTime();
	pReactor->m_sReactorName = pPrg->sName;
	pReactor->m_nFieldObjectID = ++ReactorGen::stGenID;
	pReactor->m_pReactorGen = pPrg;
	++pPrg->nReactorCount;

	OutPacket oPacket;
	pReactor->MakeEnterFieldPacket(&oPacket);
	m_mReactorName[pReactor->m_sReactorName] = pReactor->m_nFieldObjectID;
	m_mReactor.insert({ pReactor->m_nFieldObjectID, std::move(pReactor) });
	m_pField->BroadcastPacket(&oPacket);
}

void ReactorPool::OnEnter(User * pUser)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxReactorPoolMutex);
	for (auto& pReactor : m_mReactor)
	{
		OutPacket oPacket;
		pReactor.second->MakeEnterFieldPacket(&oPacket);
		pUser->SendPacket(&oPacket);
	}
}

void ReactorPool::OnPacket(User *pUser, int nType, InPacket * iPacket)
{
	switch (nType)
	{
		case ReactorRecvPacketType::Reactor_OnHitReactor:
			OnHit(pUser, iPacket);
			break;
		case ReactorRecvPacketType::Reactor_OnClickReactor:
			break;
	}
}

void ReactorPool::OnHit(User *pUser, InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxReactorPoolMutex);
	int nID = iPacket->Decode4();
	auto pFindIter = m_mReactor.find(nID);
	if (pFindIter == m_mReactor.end())
		return;
	pFindIter->second->OnHit(pUser, iPacket);
}

int ReactorPool::GetState(const std::string & sName)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxReactorPoolMutex);
	auto findNameIter = m_mReactorName.find(sName);
	if (findNameIter == m_mReactorName.end())
		return -1;
	auto findIter = m_mReactor.find(findNameIter->second);
	if (findIter == m_mReactor.end())
		return -1;
	auto &pReactor = findIter->second;
	return pReactor->m_nState;
}

void ReactorPool::SetState(const std::string &sName, int nState)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxReactorPoolMutex);
	auto findNameIter = m_mReactorName.find(sName);
	if (findNameIter == m_mReactorName.end())
		return;
	auto findIter = m_mReactor.find(findNameIter->second);
	if (findIter == m_mReactor.end())
		return;
	auto &pReactor = findIter->second;
	pReactor->m_nState = nState;
	pReactor->FindAvailableAction();
	auto pInfo = pReactor->m_pTemplate->GetStateInfo(nState);
	pReactor->m_bRemove = (!pInfo || pInfo->m_aEventInfo.size() == 0);

	OutPacket oPacket;
	pReactor->MakeStateChangePacket(&oPacket, 0, -1);
	m_pField->BroadcastPacket(&oPacket);
}

void ReactorPool::RemoveAllReactor()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxReactorPoolMutex);
	while(m_mReactor.size() > 0)
		RemoveReactor(m_mReactor.begin()->second, true);
}

void ReactorPool::RemoveReactor(Reactor *pReactor, bool bForce)
{
	if (bForce) 
	{
		pReactor->CancelAllEvent();
		pReactor->m_bDestroyAfterEvent = false; 
	}

	pReactor->SetRemoved();
	OutPacket oPacket;
	pReactor->MakeLeaveFieldPacket(&oPacket);
	m_pField->BroadcastPacket(&oPacket);
	if (!pReactor->m_bDestroyAfterEvent) 
	{
		m_pField->OnReactorDestroyed(pReactor);
		m_mReactorName.erase(((ReactorGen*)pReactor->m_pReactorGen)->sName);
		m_mReactor.erase(pReactor->m_nFieldObjectID);
	}
}

void ReactorPool::RegisterNpc(Npc * pNpc)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxReactorPoolMutex);
	m_lNpc.push_back(pNpc);
}

void ReactorPool::RemoveNpc()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxReactorPoolMutex);
	for (auto& pNpc : m_lNpc)
		m_pField->GetLifePool()->RemoveNpc(pNpc);
}

std::recursive_mutex& ReactorPool::GetLock()
{
	return m_mtxReactorPoolMutex;
}

void ReactorPool::Update(unsigned int tCur)
{
	TryCreateReactor(false);
	std::lock_guard<std::recursive_mutex> lock(m_mtxReactorPoolMutex);

	for(auto iter = m_mReactor.begin(); iter != m_mReactor.end(); )
	{
		if (iter->second->m_bRemove && 
			!iter->second->m_bDestroyAfterEvent && 
			tCur > iter->second->m_tStateEnd)
			RemoveReactor((iter++)->second);
		else
			(iter++)->second->DoActionByUpdateEvent();
	}
}

void ReactorPool::Reset(bool bShuffle)
{
	RemoveNpc();
	RemoveAllReactor();
	TryCreateReactor(true);

	if (bShuffle)
	{
		int nRnd = 0, nIter = 0, nX = 0, nY = 0;
		for (auto& prReactor1 : m_mReactor)
		{
			nIter = 0;
			nRnd = (int)(Rand32::GetInstance()->Random() % m_mReactor.size());
			for (auto& prReactor2 : m_mReactor)
				if (nIter++ == nRnd)
				{
					nX = prReactor1.second->GetPosX();
					nY = prReactor1.second->GetPosY();
					prReactor1.second->SetPosX(prReactor2.second->GetPosX());
					prReactor1.second->SetPosY(prReactor2.second->GetPosY());
					prReactor2.second->SetPosX(nX);
					prReactor2.second->SetPosY(nY);
					break;
				}
		}
	}
}
