#include "ContinentMan.h"
#include "Field.h"
#include "LifePool.h"
#include "ReactorPool.h"
#include "FieldMan.h"
#include "ItemInfo.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\FieldPacketFlags.hpp"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Random\Rand32.h"

ContinentMan::ContinentMan()
{
	m_pTimer = AsyncScheduler::CreateTask(std::bind((&ContinentMan::Update), this), 1000, true);
}

ContinentMan::~ContinentMan()
{
}

ContinentMan* ContinentMan::GetInstance()
{
	static ContinentMan* pInstance = new ContinentMan;
	return pInstance;
}

void ContinentMan::Init()
{
	auto& img = stWzResMan->GetItem("Continent.img");
	ContiMov contiObj;
	for (auto& conti : img)
	{
		contiObj.Load(&conti);
		contiObj.m_tBoarding = GameDateTime::GetCurrentDate() + 600 * 1000 * 1000;
		contiObj.m_tBoardingTime = contiObj.m_tBoarding + contiObj.m_tWait / 10;
		m_aContiMove.push_back(contiObj);
	}
	m_pTimer->Start();
}

void ContinentMan::Update()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	for (auto& conti : m_aContiMove)
	{
		int nState = conti.GetState();

		//Taking off
		if (nState == ContiMov::ContiState::e_Conti_OnTakingOff)
		{
			SendContiPacket(conti.m_nFieldIDStartShipMove, 8, ContiMov::ContiState::e_Conti_OnTakingOff);
			MoveField(conti.m_nFieldIDWait, conti.m_nFieldIDMove);
			if (conti.m_sReactorName != "")
				SetReactorState(conti.m_nFieldIDStartShipMove, conti.m_sReactorName, conti.m_nStateOnStart);
		}
		else if (nState == ContiMov::ContiState::e_Conti_OnMobSummoned)
		{
			conti.SummonMob();
			SendContiPacket(conti.m_nFieldIDMove, 10, ContiMov::ContiState::e_Conti_OnMobSummoned);
		}
		else if (nState == ContiMov::ContiState::e_Conti_OnMobDestroyed)
		{
			conti.DestroyMob();
			SendContiPacket(conti.m_nFieldIDMove, 10, ContiMov::ContiState::e_Conti_OnMobDestroyed);
		}
		else if (nState == ContiMov::ContiState::e_Conti_OnArriving)
		{
			SendContiPacket(conti.m_nFieldIDEndShipMove, 12, ContiMov::ContiState::e_Conti_OnArriving);
			MoveField(conti.m_nFieldIDMove, conti.m_nFieldIDEnd);
			MoveField(conti.m_nFieldIDCabin, conti.m_nFieldIDEnd);
			if (conti.m_sReactorName != "")
				SetReactorState(conti.m_nFieldIDEndShipMove, conti.m_sReactorName, conti.m_nStateOnEnd);
		}
	}
}

void ContinentMan::SendContiPacket(int nFieldID, int nTarget, int nFlag)
{
	auto pField = FieldMan::GetInstance()->GetField(nFieldID);
	if (pField)
	{
		OutPacket oPacket;
		oPacket.Encode2(FieldSendPacketFlag::Field_OnContiMove);
		oPacket.Encode1(nTarget);
		oPacket.Encode1(nFlag);
		pField->BroadcastPacket(&oPacket);
	}
}

void ContinentMan::MoveField(int nFieldFrom, int nFieldTo)
{
	if (nFieldFrom == 999999999)
		return;
	FieldMan::GetInstance()->GetField(nFieldFrom)->TransferAll(nFieldTo, "");
}

void ContinentMan::SetReactorState(int nFieldID, const std::string & sName, int nState)
{
	auto pField = FieldMan::GetInstance()->GetField(nFieldID);
	if (pField)
	{
		std::lock_guard<std::recursive_mutex> lock(pField->GetFieldLock());
		pField->GetReactorPool()->SetState(sName, nState);
	}
}

int ContinentMan::FindContiMove(int nFieldID)
{
	for (int i = 0; i < (int)m_aContiMove.size(); ++i)
		if (m_aContiMove[i].m_nFieldIDStartShipMove == nFieldID ||
			m_aContiMove[i].m_nFieldIDMove == nFieldID)
			return i;
	return -1;
}

void ContinentMan::OnAllSummonedMobRemoved(int nFieldID)
{
	for (int i = 0; i < (int)m_aContiMove.size(); ++i)
		if (m_aContiMove[i].m_nFieldIDMove == nFieldID)
			SendContiPacket(nFieldID, 10, ContiMov::ContiState::e_Conti_OnMobDestroyed);
}

int ContinentMan::GetInfo(int nFieldID, int nFlag)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	int nIdx = FindContiMove(nFieldID);
	if (nIdx < 0 || nFlag > 1)
		return 0;

	if (!nFlag)
		return m_aContiMove[nIdx].m_bEventDoing == 1 ? 1 : 0;

	return m_aContiMove[nIdx].m_nState;
}

long long int ContinentMan::GetBoardingTime(int nFieldID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	int nIdx = FindContiMove(nFieldID);
	if (nIdx < 0)
		return 0;

	return m_aContiMove[nIdx].m_tBoardingTime;
}

void ContinentMan::ContiMov::ResetEvent()
{
	m_bEvent = (m_nMobItemID) && ((int)(Rand32::GetInstance()->Random() % 100) <= 30);
	m_bEventDoing = false;
	int tMobGen = 0, tRequired = (int)(m_tRequired / (long long int)(600 * 1000 * 1000));
	if (tRequired == 5)
		tMobGen = (int)(Rand32::GetInstance()->Random() % 5);
	else if (tRequired > 5)
		tMobGen = (int)(Rand32::GetInstance()->Random() % (tRequired - 5));

	m_tMobGen = m_tBoarding + 600 * 1000 * 1000 * tMobGen;
}

void ContinentMan::ContiMov::Load(void *pImg)
{
#define GetItem(node, d) node == empty ? d : node;

	auto& node = *((WzIterator*)pImg);
	auto empty = node.end();
	int nDefaultFieldID = 999999999;

	auto& field = node["field"];
	m_nFieldIDStartShipMove = GetItem(field["startShipMoveFieldID"], nDefaultFieldID);
	m_nFieldIDWait = GetItem(field["waitFieldID"], nDefaultFieldID);
	m_nFieldIDMove = GetItem(field["moveFieldID"], nDefaultFieldID);
	m_nFieldIDEnd = GetItem(field["endFieldID"], nDefaultFieldID);
	m_nFieldIDEndShipMove = GetItem(field["endShipMoveFieldID"], nDefaultFieldID);
	m_nFieldIDCabin = GetItem(field["cabinFieldID"], nDefaultFieldID);

	m_tTerm =    (long long int)((int)node["scheduler"]["tTerm"]) * 600 * 1000 * 1000;
	m_tWait =         (long long int)((int)node["time"]["tWait"]) * 600 * 1000 * 1000;
	m_tRequired = (long long int)((int)node["time"]["tRequired"]) * 600 * 1000 * 1000;
	m_tEventEnd = (long long int)((int)node["time"]["tEventEnd"]) * 600 * 1000 * 1000;

	m_nMobItemID = node["genMob"]["genMobItemID"];
	m_nMobPosition_x = node["genMob"]["genMobPosition_x"];
	m_nMobPosition_y = node["genMob"]["genMobPosition_y"];

	m_sReactorName = node["reactor"]["name"];
	m_nStateOnStart = node["reactor"]["stateOnStart"];
	m_nStateOnEnd = node["reactor"]["stateOnEnd"];
}

int ContinentMan::ContiMov::GetState()
{
	long long tCur = GameDateTime::GetCurrentDate();
	if (m_nState == e_Conti_OnWaitingClock)
	{
		if (tCur > m_tBoarding) 
		{
			m_nState = e_Conti_OnBoarding;
			m_tBoardingTime = tCur + m_tWait / 10;
		}
		return m_nState;
	}

	if (m_nState == e_Conti_OnBoarding && tCur > m_tBoarding + m_tWait)
	{
		m_nState = e_Conti_OnMoving;
		return e_Conti_OnTakingOff;
	}

	if (m_nState == e_Conti_OnMoving)
	{
		if (m_bEvent)
		{
			if (m_bEventDoing)
			{
				if (tCur > m_tBoarding + (m_tWait + m_tEventEnd))
				{
					m_bEventDoing = false;
					return e_Conti_OnMobDestroyed;
				}
			}
			else if(tCur > m_tMobGen && tCur < m_tBoarding + (m_tWait + m_tEventEnd))
			{
				m_bEventDoing = true;
				return e_Conti_OnMobSummoned;
			}
		}
		if (tCur > m_tBoarding + (m_tWait + m_tRequired))
		{
			ResetEvent();
			m_tBoarding = tCur;
			m_nState = e_Conti_OnWaitingClock;
			return e_Conti_OnArriving;
		}
	}

	return e_Conti_OnWaitingClock;
}

void ContinentMan::ContiMov::SummonMob()
{
	auto pField = FieldMan::GetInstance()->GetField(m_nFieldIDMove);
	if (pField)
	{
		std::lock_guard<std::recursive_mutex> lock(pField->GetFieldLock());
		auto pSummonItem = ItemInfo::GetInstance()->GetMobSummonItem(m_nMobItemID);
		if(pSummonItem)
			pField->GetLifePool()->OnMobSummonItemUseRequest(
				m_nMobPosition_x,
				m_nMobPosition_y,
				pSummonItem,
				false
			);
	}
}

void ContinentMan::ContiMov::DestroyMob()
{
	auto pField = FieldMan::GetInstance()->GetField(m_nFieldIDMove);
	if (pField)
	{
		std::lock_guard<std::recursive_mutex> lock(pField->GetFieldLock());
		pField->GetLifePool()->RemoveAllMob(false);
	}
}
