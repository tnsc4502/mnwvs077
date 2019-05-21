#include "Reactor.h"
#include "Reward.h"
#include "User.h"
#include "Field.h"
#include "DropPool.h"
#include "ReactorTemplate.h"
#include "ReactorPool.h"
#include "MobTemplate.h"
#include "Mob.h"
#include "LifePool.h"
#include "ReactorEventManager.h"
#include "NPC.h"
#include "WvsPhysicalSpace2D.h"
#include "User.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\ReactorPacketFlags.hpp"
#include "..\WvsLib\Random\Rand32.h"

int Reactor::GetHitTypePriorityLevel(int nOption, int nType)
{
	int v2; // ecx@1
	unsigned int v3; // eax@1
	int result; // eax@3
	int v5; // ecx@7

	v2 = nType;
	v3 = nOption & 1;
	if (nOption & 2)
	{
		if (!nType)
			return 1;
	}
	else
	{
		if (!nType)
			return 2;
		if (nType == 1)
			return v3 != 0 ? -1 : 1;
		v2 = nType - 2;
		if (nType == 2)
			return v3 != 0 ? 1 : -1;
	}
	v5 = v2 - 1;
	if (v5)
	{
		if (v5 == 1)
			result = (v3 != 0) - 1;
		else
			result = -1;
	}
	else
		result = -(v3 != 0);
	return result;
}

Reactor::Reactor(ReactorTemplate* pTemplate, Field* pField)
	: m_pTemplate(pTemplate),
	  m_pField(pField)
{
}

Reactor::~Reactor()
{
}

void Reactor::MakeEnterFieldPacket(OutPacket * oPacket)
{
	oPacket->Encode2((short)ReactorSendPacketFlag::Reactor_OnReactorEnterField);
	oPacket->Encode4(m_nFieldObjectID);
	oPacket->Encode4(m_nTemplateID);
	oPacket->Encode1(m_nState);
	oPacket->Encode2((short)m_ptPos.x);
	oPacket->Encode2((short)m_ptPos.y);
	oPacket->Encode1(m_bFlip);
	oPacket->EncodeStr(m_sReactorName);
}

void Reactor::MakeLeaveFieldPacket(OutPacket * oPacket)
{
	oPacket->Encode2((short)ReactorSendPacketFlag::Reactor_OnReactorLeaveField);
	oPacket->Encode4(m_nFieldObjectID);
	oPacket->Encode1(m_nState);
	oPacket->Encode2((short)m_ptPos.x);
	oPacket->Encode2((short)m_ptPos.y);
}

int Reactor::GetHitDelay(int nEventIdx)
{
	auto pInfo = m_pTemplate->GetStateInfo(m_nState);
	auto pEventInfo = m_pTemplate->GetEventInfo(m_nState, nEventIdx);
	int nRet = 0;
	if (pInfo && pEventInfo)
	{
		int v4 = 0;
		if (nEventIdx >= 0
			&& (v4 = pEventInfo->m_tHitDelay, v4 < 0)
			&& (v4 = pInfo->m_tHitDelay, pInfo->m_tHitDelay < 0)
			&& (v4 = m_pTemplate->m_tHitDelay, v4 < 0))
			nRet = 0;
		else
			nRet = v4;
	}
	return nRet;
}

void Reactor::OnHit(User * pUser, InPacket * iPacket)
{
	int nOption = iPacket->Decode4();
	int tActionDelay = iPacket->Decode2();

	auto aInfo = m_pTemplate->m_aStateInfo;
	if ((m_nState >= aInfo.size()) || aInfo[m_nState].m_aEventInfo.size() == 0)
		return;

	auto& info = aInfo[m_nState];
	int nLevel = -1, nEventIdx = 0, nIterIdx = 0;
	for (auto& eventInfo : info.m_aEventInfo)
	{
		int nRet = GetHitTypePriorityLevel(nOption, eventInfo.m_nType);
		if (nRet != -1 && (nRet < nLevel || nLevel == -1))
		{
			nLevel = nRet;
			nEventIdx = nIterIdx;
		}
		if (nRet == 0)
			break;
		++nIterIdx;
	}
	if (nLevel != -1)
	{
		if (!m_nHitCount && !m_nOwnerID)
		{
			m_nOwnerID = pUser->GetUserID();
			m_nOwnPartyID = pUser->GetPartyID() == -1 ? 0 : pUser->GetPartyID();
			//if (m_nOwnPartyID > 0)
			//	m_nOwnType = 1;
		}
		if (m_pTemplate->m_nReqHitCount > 0)
			++m_nHitCount;
		if (m_nHitCount >= m_pTemplate->m_nReqHitCount)
		{
			SetState(nEventIdx, tActionDelay);
			m_nHitCount = 0;
			m_nLastHitCharacterID = pUser->GetUserID();
		}
		m_tLastHit = GameDateTime::GetTime();
	}
}

void Reactor::SetState(int nEventIdx, int tActionDelay)
{
	auto tCur = GameDateTime::GetTime();
	m_tStateEnd = tCur + GetHitDelay(nEventIdx) + tActionDelay;

	auto pInfo = m_pTemplate->GetStateInfo(m_nState);
	auto pEventInfo = m_pTemplate->GetEventInfo(m_nState, nEventIdx);
	if (pInfo == nullptr || pEventInfo == nullptr)
		m_nState = m_pTemplate->m_aStateInfo[m_nState].m_aEventInfo[nEventIdx].m_nStateToBe;
	else
		m_nState = (m_nState + 1) % m_pTemplate->m_aStateInfo.size();
	pInfo = m_pTemplate->GetStateInfo(m_nState);
	m_tTimeout = pInfo->m_tTimeout;
	m_bRemove = (pInfo->m_aEventInfo.size() == 0) && (m_pField->GetFieldSet() == nullptr || m_pTemplate->m_bRemoveInFieldSet);
	FindAvailableAction();
	if (m_bRemove && !m_bDestroyAfterEvent)
		m_pField->GetReactorPool()->RemoveReactor(this);
	else
	{
		OutPacket oPacket;
		MakeStateChangePacket(&oPacket, tActionDelay, nEventIdx);
		m_pField->BroadcastPacket(&oPacket);
	}
}

void Reactor::MakeStateChangePacket(OutPacket * oPacket, int tActionDelay, int nProperEventIdx)
{
	oPacket->Encode2((short)ReactorSendPacketFlag::Reactor_OnReactorChangeState);
	oPacket->Encode4(m_nFieldObjectID);
	oPacket->Encode1(m_nState);
	oPacket->Encode2(m_ptPos.x);
	oPacket->Encode2(m_ptPos.y);
	oPacket->Encode2(tActionDelay);
	oPacket->Encode1(nProperEventIdx);
	oPacket->Encode1((m_tStateEnd - GameDateTime::GetTime() + 99) / 100);
	oPacket->Encode4(m_nOwnerID);
}

void Reactor::FindAvailableAction()
{
	int tCur = GameDateTime::GetTime();
	int nDropIdx = 0;
	for (auto& info : m_pTemplate->m_aActionInfo)
	{
		if (info.nType != -1 && info.nState == m_nState)
		{
			if (info.nType == 2)
			{
				++nDropIdx;
				DoAction(&info, m_tStateEnd - tCur, nDropIdx);
			}
			else if (info.nType == 11)
			{

			}
			else
			{
				if (m_bRemove)
					m_bDestroyAfterEvent = true;
				int nEventSN = ReactorEventManager::GetInstance()->RegisterEvent();
				m_mEvent.insert({ nEventSN, &info });
				ReactorEventManager::GetInstance()->SetEvent(this, m_tStateEnd, nEventSN);
			}
		}
	}
}

void Reactor::DoAction(void *pInfo_, int tDelay, int nDropIdx)
{
	int x1 = GetPosX(), y1 = GetPosY();
	auto pInfo = (ReactorTemplate::ActionInfo*)pInfo_;
	if (pInfo->nType == 0)
	{
		User *pUser = nullptr;
		if (pInfo->anArgs.size() == 0 || pInfo->asArgs.size() == 0)
			return;
		int nRnd = (int)(Rand32::GetInstance()->Random() % pInfo->asArgs.size());
		if (pInfo->anArgs[1 + nRnd] == 999999999)
			return;

		//Transfer last hit user.
		if (pInfo->anArgs[0])
		{
			pUser = User::FindUser(m_nLastHitCharacterID);
			if (pUser)
			{
				//if(pInfo->sMessage != "")
				//	pUser->SendChatMessage(9, pInfo->sMessage);
				pUser->TryTransferField(pInfo->anArgs[1 + nRnd], pInfo->asArgs[nRnd]);
			}
		}
		else //Transfer all.
		{
			std::lock_guard<std::recursive_mutex> lock(m_pField->GetFieldLock());
			auto& mUsers = m_pField->GetUsers();
			for (auto prUser : mUsers)
			{
				//if (pInfo->sMessage != "")
				//	prUser.second->SendChatMessage(9, pInfo->sMessage);
				prUser.second->TryTransferField(pInfo->anArgs[1 + nRnd], pInfo->asArgs[nRnd]);
			}
		}
	}
	else if(pInfo->nType == 1) //Summon Mobs
	{
		if (m_pField)
		{
			std::lock_guard<std::recursive_mutex> lock(m_pField->GetFieldLock());
			int y = GetPosY(), nMobType = 0;
			m_pField->GetSpace2D()->GetFootholdUnderneath(x1, y1 - 20, &y);
			if (pInfo->anArgs.size() < 3)
				return;
			if (pInfo->anArgs.size() >= 5)
			{
				x1 = pInfo->anArgs[4];
				y1 = pInfo->anArgs[5];
			}
			if (pInfo->anArgs.size() >= 4)
				nMobType = pInfo->anArgs[3];
			for (int i = 0; i < pInfo->anArgs[2]; ++i)
				m_pField->GetLifePool()->CreateMob(pInfo->anArgs[0], x1, y, GetFh(), 0, pInfo->anArgs[1], 0, 1, nMobType, nullptr);
		}
	}
	else if (pInfo->nType == 2) //Drop rewards
	{
		if (m_pTemplate->m_aRewardInfo)
		{
			auto aRewardDrop = Reward::Create(m_pTemplate->m_aRewardInfo, false, 1, 1, 1, 1, nullptr);
			if (aRewardDrop.size() > 0)
			{
				int nDropBaseX = 20 * nDropIdx,
					nDropOffsetX = 20 * (int)(aRewardDrop.size() / 2);

				nDropIdx = 0;
				for (auto pDrop : aRewardDrop)
				{
					m_pField->GetDropPool()->Create(
						pDrop,
						m_nOwnerID,
						m_nOwnPartyID,
						m_nOwnType,
						m_nTemplateID,
						x1,
						y1,
						x1 + nDropBaseX + nDropIdx - nDropOffsetX,
						y1,
						tDelay,
						0,
						0,
						1
					);
					nDropIdx += 20;
					tDelay += 200;
				}
			}
		}
	}
	else if (pInfo->nType == 6)
	{

	}
	else if (pInfo->nType == 7)
	{

	}
	else if (pInfo->nType == 10)
	{

	}
}

void Reactor::SetRemoved()
{
	auto pGen = ((ReactorPool::ReactorGen*) m_pReactorGen);
	if (pGen)
	{
		if (pGen->tRegenInterval && (!--pGen->nReactorCount))
		{
			int tCur = GameDateTime::GetTime();
			int tBase = 6 * pGen->tRegenInterval / 10;
			pGen->tRegenAfter = tCur + (7 * pGen->tRegenAfter / 10) + (tBase > 0 ? Rand32::GetInstance()->Random() % tBase : 0);
		}
	}
}

void Reactor::CancelAllEvent()
{
	for (auto prEvent : m_mEvent)
		ReactorEventManager::GetInstance()->Cancel(prEvent.first);
}

void Reactor::OnTime(int nEventSN)
{
	auto p = m_mEvent.find(nEventSN);
	if (p == m_mEvent.end())
		return;

	auto pAction = (ReactorTemplate::ActionInfo*)p->second;
	DoAction(p->second, 0, 0);
	m_mEvent.erase(nEventSN);
	if (m_bDestroyAfterEvent && m_mEvent.size() == 0)
		m_pField->GetReactorPool()->RemoveReactor(this);
}
