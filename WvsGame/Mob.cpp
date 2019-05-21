#include "Mob.h"
#include "MobTemplate.h"
#include "..\Database\GW_MobReward.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "Reward.h"
#include "QWUQuestRecord.h"
#include "DropPool.h"
#include "User.h"
#include "ItemInfo.h"
#include "QWUser.h"
#include "Field.h"
#include "MobStat.h"

#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\MobPacketFlags.hpp"

Mob::Mob()
{
	m_pStat = nullptr;
}

Mob::~Mob()
{
	if (m_pStat)
		FreeObj(m_pStat);
}

void Mob::MakeEnterFieldPacket(OutPacket *oPacket)
{
	oPacket->Encode2(MobSendPacketFlag::Mob_OnMakeEnterFieldPacket); //MobPool::SpawnMonster
	EncodeInitData(oPacket);
}

void Mob::MakeLeaveFieldPacket(OutPacket * oPacket)
{
	oPacket->Encode2(MobSendPacketFlag::Mob_OnMakeLeaveFieldPacket); //MobPool::SpawnMonster
	oPacket->Encode4(GetFieldObjectID());
	oPacket->Encode1(1);
}

void Mob::EncodeInitData(OutPacket *oPacket, bool bIsControl)
{
	//printf("Encode Init Data oid = %d template ID = %d Is Control?%d\n", GetFieldObjectID(), GetTemplateID(), (int)bIsControl);
	oPacket->Encode4(GetFieldObjectID());
	oPacket->Encode1(bIsControl ? 2 : 1); //Control Mode
	oPacket->Encode4(GetTemplateID());

	oPacket->Encode4(0); // MobStat

	//MobStat::EncodeTemporary
	oPacket->Encode2(GetPosX());
	oPacket->Encode2(GetPosY());
	oPacket->Encode1(GetMoveAction()); // m_bMoveAction

	oPacket->Encode2(GetFh()); // Current FH m_nFootholdSN
	oPacket->Encode2(GetFh()); // Origin FH m_nHomeFoothold
	
	int nSpawnType = 
		m_pMobTemplate->m_nSummonType <= 1 || m_pMobTemplate->m_nSummonType == 24 ? -2 : m_pMobTemplate->m_nSummonType;

	if (!bIsControl) {
		oPacket->Encode1(nSpawnType /*m_pMobTemplate->m_nSummonType*/);
		if (nSpawnType == -3 || nSpawnType >= 0/*m_pMobTemplate->m_nSummonType == -3 || m_pMobTemplate->m_nSummonType >= 0*/)
			oPacket->Encode4(0); //dwSummonOption  -->  Linked ObjectID
	}
	else
		oPacket->Encode1(-1);

	oPacket->Encode1(-1); //Carnival Team
	if (m_nTemplateID / 10000 == 961)
		oPacket->EncodeStr("");
	oPacket->Encode4(0);
}

void Mob::SendChangeControllerPacket(User* pUser, int nLevel)
{
	if (nLevel)
	{
		OutPacket oPacket;
		oPacket.Encode2(MobSendPacketFlag::Mob_OnMobChangeController);
		oPacket.Encode1(nLevel);
		EncodeInitData(&oPacket, true);
		pUser->SendPacket(&oPacket);
	}
	else
		SendReleaseControllPacket(pUser, GetFieldObjectID());
}

void Mob::SendReleaseControllPacket(User* pUser, int dwMobID)
{
	OutPacket oPacket;
	oPacket.Encode2(MobSendPacketFlag::Mob_OnMobChangeController);
	oPacket.Encode1(0);
	oPacket.Encode4(dwMobID);
	//EncodeInitData(&oPacket);
	pUser->SendPacket(&oPacket);
}

void Mob::SetMobTemplate(MobTemplate *pTemplate)
{
	m_pMobTemplate = pTemplate;
}

const MobTemplate* Mob::GetMobTemplate() const
{
	return m_pMobTemplate;
}

void Mob::SetController(Controller * pController)
{
	m_pController = pController;
}

Controller* Mob::GetController()
{
	return m_pController;
}

void Mob::SetMovePosition(int x, int y, char bMoveAction, short nSN)
{
	//v6 = m_stat.nDoom_ == 0;
	SetPosX(x);
	SetPosY(y);
	SetMoveAction(bMoveAction);

	SetFh(nSN);
}

bool Mob::IsLucidSpecialMob(int dwTemplateID)
{
	if ((dwTemplateID == 8880150 || dwTemplateID == 8880151 || (dwTemplateID >= 8880160 && dwTemplateID < 8880200)) 
		&& (dwTemplateID != 8880182) 
		&& (dwTemplateID != 8880184) 
		&& (dwTemplateID != 8880171) 
		&& (dwTemplateID != 8880161)
		)
		return true;
	return false;
}

void Mob::OnMobHit(User * pUser, long long int nDamage, int nAttackType)
{
	m_mAttackRecord[pUser->GetUserID()] += nDamage;
	this->SetHP(this->GetHP() - nDamage);
	if (GetHP() > 0)
	{
		OutPacket oPacket;
		oPacket.Encode2(MobSendPacketFlag::Mob_OnHPIndicator);
		oPacket.Encode4(GetFieldObjectID());
		oPacket.Encode1((char)((GetHP() / (double)GetMobTemplate()->m_lnMaxHP) * 100));
		pUser->SendPacket(&oPacket);
	}
}

void Mob::OnMobDead(int nHitX, int nHitY, int nMesoUp, int nMesoUpByItem)
{
	int nOwnType, nOwnPartyID, nLastDamageCharacterID;
	int nOwnerID = DistributeExp(nOwnType, nOwnPartyID, nLastDamageCharacterID);

	GiveReward(
		nOwnerID,
		nOwnType,
		nOwnPartyID,
		nHitX,
		nHitY,
		0,
		nMesoUp,
		nMesoUpByItem
	);
}

int Mob::DistributeExp(int & refOwnType, int & refOwnParyID, int & refLastDamageCharacterID)
{
	long long int nHighestDamageRecord = 0;
	int nHighestDamageUser = 0;
	long long int nTotalHP = GetMobTemplate()->m_lnMaxHP;

	refOwnType = 0;
	refOwnParyID = 0;

	for (auto& dmg : m_mAttackRecord)
	{
		if (nHighestDamageUser == 0 || nHighestDamageRecord > dmg.second)
		{
			nHighestDamageUser = dmg.first;
			nHighestDamageRecord = dmg.second;
		}
		auto pUser = User::FindUser(dmg.first);
		if (pUser != nullptr)
		{
			auto nDamaged = dmg.second;
			if (nDamaged >= nTotalHP)
				nDamaged = nTotalHP;
			int nIncEXP = (int)floor((double)GetMobTemplate()->m_nEXP * ((double)dmg.second / (double)nTotalHP));
			auto nStatChanged = QWUser::IncEXP(pUser, nIncEXP, false);
			pUser->SendCharacterStat(false, nStatChanged);
		}
	}
	return nHighestDamageUser;
}

void Mob::GiveReward(unsigned int dwOwnerID, unsigned int dwOwnPartyID, int nOwnType, int nX, int nY, int tDelay, int nMesoUp, int nMesoUpByItem)
{
	auto& aReward = m_pMobTemplate->GetMobReward();

	Reward* pDrop = nullptr;
	std::vector<Reward*> apDrop;
	User* pOwner = User::FindUser(dwOwnerID);

	auto aRewardDrop = Reward::Create(
		&aReward, false, 1.0, 1.0, 1.0, 1.0, nullptr
	);

	if (aRewardDrop.size() == 0)
		return;

	int nXOffset = ((int)aRewardDrop.size() - 1) * (GetMobTemplate()->m_bIsExplosiveDrop ? -20 : -10);
	nXOffset += GetPosX();
	for (auto& pReward : aRewardDrop)
	{
		GetField()->GetDropPool()->Create(
			pReward,
			dwOwnerID,
			dwOwnPartyID,
			nOwnType,
			dwOwnerID,
			GetPosX(),
			GetPosY(),
			nXOffset,
			0,
			0,
			0,
			0,
			0);
		nXOffset += GetMobTemplate()->m_bIsExplosiveDrop ? 40 : 20;
	}
}

void Mob::SetHP(long long int liHP)
{
	m_liHP = liHP;
}

void Mob::SetMP(long long int liMP)
{
	m_liMP = liMP;
}

void Mob::SetMobStat(MobStat * pStat)
{
	m_pStat = pStat;
}

MobStat* Mob::GetMobStat()
{
	return m_pStat;
}

void* Mob::GetMobGen() const
{
	return m_pMobGen;
}

void Mob::SetMobGen(void * pGen)
{
	m_pMobGen = pGen;
}

long long int Mob::GetHP() const
{
	return m_liHP;
}

long long int Mob::GetMP() const
{
	return m_liMP;
}

std::pair<int, int> Mob::GetDropPos()
{
	std::pair<int, int> ret;

	int nPosDiff = 0, nDropPosX = GetPosX(), nDropPosY = GetPosY();

	nPosDiff = ((Rand32::GetInstance()->Random())) % 12;
	nDropPosX = (nDropPosX + ((nPosDiff % 2 == 0) ? (25 * (nPosDiff + 1) / 2) : -(nPosDiff * (nPosDiff / 2))));
	ret.first = nDropPosX;
	ret.second = nDropPosY;
	return ret;
}
