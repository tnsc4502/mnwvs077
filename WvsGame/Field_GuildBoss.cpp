#include "Field_GuildBoss.h"
#include "FieldPacketTypes.hpp"
#include "LifePool.h"
#include "Mob.h"

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Wz\WzResMan.hpp"

Field_GuildBoss::Field_GuildBoss(void * pData, int nFieldID)
	: Field(pData, nFieldID)
{
	auto& mapWz = *((WzIterator*)pData);
	auto& healer = mapWz["healer"];

	m_nYMin = healer["yMin"];
	m_nYMax = healer["yMax"];
	m_nFall = healer["fall"];
	m_nRise = healer["rise"];
	m_nHealMin = healer["healMin"];
	m_nHealMax = healer["healMax"];
	ResetState();
}

Field_GuildBoss::~Field_GuildBoss()
{
}

void Field_GuildBoss::OnPacket(User * pUser, InPacket * iPacket)
{

	int nType = iPacket->Decode2();
	if (nType == FieldRecvPacketType::Field_GuildBoss_OnPullyHit)
		OnPullyHit();
	else
	{
		iPacket->RestorePacket();
		Field::OnPacket(pUser, iPacket);
	}
}

void Field_GuildBoss::ResetState()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldLock);
	m_nState = 1;
	m_nY = m_nYMin;
}

void Field_GuildBoss::OnPullyHit()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldLock);
	if (m_nState == 0 && m_nY >= m_nYMin && m_nY <= m_nYMax)
	{
		m_nY = std::max(m_nYMin, m_nY - m_nRise);
		SendHealerMove();
	}
}

void Field_GuildBoss::OnEnter(User * pUser)
{
	std::lock_guard<std::recursive_mutex> userGuard(m_mtxFieldLock);
	Field::OnEnter(pUser);

	if (m_nState)
		m_nState = 0;

	SendHealerMove();
}

void Field_GuildBoss::ChangePullyState(int nState)
{
	m_nState = nState;
	SendPullyStateChange(nState);
}

void Field_GuildBoss::SendPullyStateChange(int nState)
{
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketType::Field_GuildBoss_OnPullyStateChange);
	oPacket.Encode1(nState);

	BroadcastPacket(&oPacket);
}

void Field_GuildBoss::SendHealerMove()
{
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketType::Field_GuildBoss_OnHealerMove);
	oPacket.Encode2(m_nY);
	BroadcastPacket(&oPacket);
}

void Field_GuildBoss::Update()
{
	auto aMob = m_pLifePool->GetMobByTemplateID(9300028);
	if (m_nState == 0)
	{
		if (aMob.size())
		{
			for (auto& pMob : aMob)
			{
				if (m_nY >= m_nYMax)
					pMob->Heal(m_nHealMin, m_nHealMax);
				else
				{
					m_nY = std::min(m_nYMax, m_nY + m_nFall);
					SendHealerMove();
				}
			}
		}
		else
			ChangePullyState(1);
	}
	else
		ChangePullyState(0);

	Field::Update();
}
