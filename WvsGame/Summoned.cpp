#include "Summoned.h"
#include "User.h"
#include "MovePath.h"
#include "Field.h"
#include "LifePool.h"
#include "AttackInfo.h"
#include "SkillInfo.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterLevel.h"
#include "..\WvsGame\SummonedPacketTypes.hpp"

Summoned::Summoned()
{
}

Summoned::~Summoned()
{
}

int Summoned::GetOwnerID() const
{
	return m_pOwner->GetUserID();
}

int Summoned::GetSkillID() const
{
	return m_nSkillID;
}

int Summoned::GetSkillLevel() const
{
	return m_nSLV;
}

unsigned int Summoned::GetTimeEnd() const
{
	return m_tEnd;
}

void Summoned::Init(User * pUser, int nSkillID, int nSLV)
{
	m_pOwner = pUser;
	m_nSkillID = nSkillID;
	m_pSkillEntry = SkillInfo::GetInstance()->GetSkillByID(m_nSkillID);

	m_nSLV = nSLV;
	m_pField = pUser->GetField();

	m_nMoveAbility = GetMoveAbility(m_nSkillID);
	m_nAssitType = GetAssitType(m_nSkillID);
}

void Summoned::OnPacket(InPacket * iPacket, int nType)
{
	switch (nType)
	{
		case SummonedRecvPacketType::Summoned_OnMoveRequest:
			OnMove(iPacket);
			break;
		case SummonedRecvPacketType::Summoned_OnAttackRequest:
			OnAttack(iPacket);
			break;
		case SummonedRecvPacketType::Summoned_OnHitRequest:
			break;
		case SummonedRecvPacketType::Summoned_OnDoingHealRequest:
			break;
		case SummonedRecvPacketType::Summoned_OnRemoveRequest:
			break;
	}
}

void Summoned::OnMove(InPacket * iPacket)
{
	//iPacket->Offset(12);
	MovePath movePath;
	movePath.Decode(iPacket);
	ValidateMovePath(&movePath);

	OutPacket oPacket;
	oPacket.Encode2((short)SummonedSendPacketType::Summoned_OnMove);
	oPacket.Encode4(m_pOwner->GetUserID());
	oPacket.Encode4(GetFieldObjectID());
	movePath.Encode(&oPacket);

	m_pField->SplitSendPacket(&oPacket, m_pOwner);
}

void Summoned::MakeEnterFieldPacket(OutPacket * oPacket)
{
	oPacket->Encode2((short)SummonedSendPacketType::Summoned_OnCreated);
	oPacket->Encode4(m_pOwner->GetUserID());
	oPacket->Encode4(GetFieldObjectID());
	oPacket->Encode4(m_nSkillID);
	//oPacket->Encode1(m_pOwner->GetCharacterData()->mLevel->nLevel);
	oPacket->Encode1((unsigned char)(m_nSLV - 1));
	oPacket->Encode2(GetPosX());
	oPacket->Encode2(GetPosY());
	oPacket->Encode1(GetMoveAction());
	oPacket->Encode2(GetFh());
	oPacket->Encode1(m_nMoveAbility);
	oPacket->Encode1(m_nAssitType);
	oPacket->Encode1(m_nEnterType);
	oPacket->Encode4(m_nMobID);
	oPacket->Encode1(m_bFlyMob);
	oPacket->Encode1(m_bBeforeFirstAttack);
	oPacket->Encode4(m_nLookID);
	oPacket->Encode4(m_nBulletID);

	oPacket->Encode1(m_bMirrored);
	if (m_bMirrored)
		m_pOwner->GetCharacterData()->EncodeAvatarLook(oPacket);

	oPacket->Encode1(m_bJaguarActive);
	oPacket->Encode4(m_tEnd);
	oPacket->Encode1(m_bAttackActive);
	oPacket->Encode4(0);
	oPacket->Encode1(1);
	oPacket->Encode4(0);
}

void Summoned::MakeLeaveFieldPacket(OutPacket * oPacket)
{
	oPacket->Encode2((short)SummonedSendPacketType::Summoned_OnRemoved);
	oPacket->Encode4(m_pOwner->GetUserID());
	oPacket->Encode4(GetFieldObjectID());
	oPacket->Encode1(0x0A);
}

int Summoned::GetMoveAbility(int nSkillID)
{
	if (nSkillID == 1321007 ||  
		nSkillID == 2121005 || 
		nSkillID == 2221005 || 
		nSkillID == 2321003 ||
		nSkillID == 3121006 ||
		nSkillID == 3221005
		)
		return 1;
	else if (nSkillID == 3111002 || nSkillID == 3211002)
		return 0;
	return 3;
}

int Summoned::GetAssitType(int nSkillID)
{
	if (nSkillID == 1321007)
		return 2; 
	else if (nSkillID == 3111002 || nSkillID == 3211002)
		return 0;
	return 1;
}

void Summoned::OnAttack(InPacket *iPacket)
{
	AttackInfo info;
	int tLastAttackClient = iPacket->Decode4();
	info.m_bDamageInfoFlag = iPacket->Decode1();
	info.m_bLeft = (info.m_bDamageInfoFlag >> 7) & 1;
	info.m_nAction = info.m_bDamageInfoFlag & 0x7F;
	int nMobCount = iPacket->Decode1(), nSkillMobCount = m_pSkillEntry->GetLevelData(m_nSLV)->m_nMobCount;
	auto tCur = GameDateTime::GetTime();


	if (tCur - m_tLastAttackTime < 2000 || nMobCount > nSkillMobCount && nSkillMobCount)
		return;
	else if (nSkillMobCount == 0)
		nMobCount = 1;

	User::TryParsingDamageData(&info, iPacket, nMobCount, 1);
	info.m_nSkillID = m_nSkillID;
	info.m_nSLV = m_nSLV;
	info.m_nAttackType = m_nSkillID / 1000000 != 2 ? 0 : 2;

	m_pField->GetLifePool()->OnSummonedAttack(
		m_pOwner,
		this,
		m_pSkillEntry,
		&info
	);
	m_tLastAttackTime = tCur;
}