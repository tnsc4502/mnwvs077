#include "Mob.h"
#include "MobTemplate.h"
#include "..\Database\GW_MobReward.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "Reward.h"
#include "QWUQuestRecord.h"
#include "DropPool.h"
#include "User.h"
#include "SecondaryStat.h"
#include "ItemInfo.h"
#include "QWUser.h"
#include "Field.h"
#include "MobStat.h"
#include "PartyMan.h"
#include "SkillInfo.h"
#include "MobSkillEntry.h"
#include "FieldRect.h"
#include "User.h"
#include "LifePool.h"
#include "WvsPhysicalSpace2D.h"
#include "StaticFoothold.h"

#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\MobPacketFlags.hpp"

#undef min
#undef max

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

	oPacket->Encode4(m_pStat->nFlagSet); // MobStat
	if (m_pStat->nFlagSet)
		m_pStat->EncodeTemporary(m_pStat->nFlagSet, oPacket, GameDateTime::GetTime());

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

bool Mob::OnMobMove(bool bNextAttackPossible, int nAction, int nData, unsigned char *nSkillCommand, unsigned char *nSLV, bool *bShootAttack)
{
	auto& aMobSkill = m_pMobTemplate->m_aMobSkill;
	if (bNextAttackPossible && (int)aMobSkill.size() > 0)
	{
		//int nTrialCount = aMobSkill.size();
		int nRnd = (int)(Rand32::GetInstance()->Random() % aMobSkill.size());
		*nSkillCommand = aMobSkill[nRnd].first;
		*nSLV = aMobSkill[nRnd].second;
		
	}
	if (!DoSkill(*nSkillCommand, *nSLV, nData >> 16))
	{
		*nSkillCommand = 0;
		*nSLV = 0;
	}
	/*if (nAction >= 0)
	{
		if (nAction < 12 || nAction > 20)
		{
			if (nAction >= 21 && nAction <= 25)
				DoSkill(*nSkillCommand, *nSLV, nData >> 16);
		}
	}*/
	return true;
}

//#include "..\WvsLib\Logger\WvsLogger.h"

bool Mob::DoSkill(int nSkillID, int nSLV, int nOption)
{
	auto pEntry = SkillInfo::GetInstance()->GetMobSkill(nSkillID);
	if (pEntry)
	{
		auto pLevelData = pEntry->GetLevelData(nSLV);
		if (pLevelData)
		{
			//WvsLogger::LogFormat("Current : %d, Last %d, Interval : %d Diff=  %d\n", GameDateTime::GetTime(), tLastSkillUseTime, pLevelData->tInterval * 1000, GameDateTime::GetTime() - tLastSkillUseTime);
			if ((GameDateTime::GetTime() - tLastSkillUseTime < (pLevelData->tInterval * 1000)) ||
				m_liHP < pLevelData->nMPCon)
				return false;

			m_liMP -= pLevelData->nMPCon;

			switch (nSkillID)
			{
			case 130:
			case 131:
				DoSkill_AffectArea(nSkillID, nSLV, pLevelData, nOption);
				break;
			case 100:
			case 101:
			case 102:
			case 103:
			case 104:
			case 140:
			case 141:
			case 142:
			case 150:
			case 151:
			case 152:
			case 153:
			case 154:
			case 155:
			case 156:
			case 157:
				DoSkill_StateChange(nSkillID, nSLV, pLevelData, nOption);
				break;
			case 120:
			case 121:
			case 122:
			case 123:
			case 124:
			case 125:
			case 126:
			case 127:
			case 128:
			case 129:
				DoSkill_UserStatChange(200, nSkillID, nSLV, pLevelData, nOption);
				break;
			case 110:
			case 111:
			case 112:
			case 113:
				DoSkill_PartizanStatChange(nSkillID, nSLV, pLevelData, nOption);
				break;
			case 114:
				DoSkill_PartizanOneTimeStatChange(nSkillID, nSLV, pLevelData, nOption);
				break;
			case 200:
				DoSkill_Summon(pLevelData, nOption);
				break;
			}
		}
		tLastSkillUseTime = GameDateTime::GetTime();
	}
	return true;
}

void Mob::DoSkill_AffectArea(int nSkillID, int nSLV, const MobSkillLevelData * pLevel, int tDelay)
{
}

void Mob::DoSkill_StateChange(int nSkillID, int nSLV, const MobSkillLevelData * pLevel, int tDelay, bool bResetBySkill)
{
#define REGISTER_MOB_STAT(name, value) \
nFlagSet |= MobStat::MS_##name; \
m_pStat->nFlagSet &= ~(MobStat::MS_##name); \
m_pStat->n##name = bResetBySkill ? 0 : value; \
m_pStat->r##name = bResetBySkill ? 0 : nSkillID | (nSLV << 16); \
m_pStat->t##name = bResetBySkill ? 0 : GameDateTime::GetTime() + nDuration; \
if(!bResetBySkill) {\
	m_pStat->nFlagSet |= (MobStat::MS_##name); \
}\

	int nDuration = tDelay + pLevel->tTime * 1000;
	int nFlagSet = 0;
	switch (nSkillID)
	{
		case 140: 
		{
			REGISTER_MOB_STAT(PImmune, pLevel->nX);
			break;
		}
		case 110:
		{
			REGISTER_MOB_STAT(PowerUp, pLevel->nX);
			break;
		}
		case 111:
		{
			REGISTER_MOB_STAT(MagicUp, pLevel->nX);
			break;
		}
		case 112:
		{
			REGISTER_MOB_STAT(PGuardUp, pLevel->nX);
			break;
		}
		case 113:
		{
			REGISTER_MOB_STAT(MGuardUp, pLevel->nX);
			break;
		}
		case 100:
		case 101:
		case 102:
		case 103:
		case 104:
		case 156: 
		{
			REGISTER_MOB_STAT(Speed, pLevel->nX);
			break;
		}
		case 141: 
		{
			REGISTER_MOB_STAT(MImmune, pLevel->nX);
			break;
		}
		case 142: 
		{
			REGISTER_MOB_STAT(HardSkin, pLevel->nX);
			break;
		}
		case 150: 
		{
			REGISTER_MOB_STAT(PAD, pLevel->nX);
			break;
		}
		case 151: 
		{
			REGISTER_MOB_STAT(MAD, pLevel->nX);
			break;
		}
		case 152: 
		{
			REGISTER_MOB_STAT(PDD, pLevel->nX);
			break;
		}
		case 153: 
		{
			REGISTER_MOB_STAT(MDD, pLevel->nX);
			break;
		}
		case 154: 
		{
			REGISTER_MOB_STAT(ACC, pLevel->nX);
			break;
		}
		case 155: 
		{
			REGISTER_MOB_STAT(EVA, pLevel->nX);
			break;
		}
		case 157:
		{
			REGISTER_MOB_STAT(SealSkill, pLevel->nX);
			break;
		}
		default:
			break;
	}
	if (nFlagSet)
		SendMobTemporaryStatSet(nFlagSet, tDelay);
}

void Mob::DoSkill_UserStatChange(int nArg, int nSkillID, int nSLV, const MobSkillLevelData * pLevel, int tDelay)
{
	std::lock_guard<std::recursive_mutex> lock(m_pField->GetFieldLock());
	auto& mUser = m_pField->GetUsers();

	//Not sure
	FieldRect rect;
	rect.left = GetPosX() - std::max(nArg, -(pLevel->rcAffectedArea.left));
	rect.right = GetPosX() + std::max(nArg, (pLevel->rcAffectedArea.right));
	rect.top = GetPosY() - std::max(nArg, -(pLevel->rcAffectedArea.top));
	rect.bottom = GetPosY() + std::max(nArg, (pLevel->rcAffectedArea.bottom));

	//Can't find where this variable is set.
	int nTargetUserCount = 1; 
	bool bAffectAllWithinRect = (nTargetUserCount == 0);
	for (auto& prUser : mUser)
	{
		if (rect.PtInRect({ prUser.second->GetPosX(), prUser.second->GetPosY() }) && QWUser::GetHP(prUser.second))
		{
			std::lock_guard<std::recursive_mutex> uLock(prUser.second->GetLock());
			prUser.second->OnStatChangeByMobSkill(nSkillID, nSLV, pLevel, tDelay, m_nTemplateID);
			--nTargetUserCount;

			if (!bAffectAllWithinRect && nTargetUserCount == 0)
				break;
		}
	}
}

void Mob::DoSkill_PartizanStatChange(int nSkillID, int nSLV, const MobSkillLevelData * pLevel, int tDelay)
{
	FieldRect rect;
	rect.left = GetPosX() + pLevel->rcAffectedArea.left;
	rect.right = GetPosX() + pLevel->rcAffectedArea.right;
	rect.top = GetPosY() + pLevel->rcAffectedArea.top;
	rect.bottom = GetPosY() + pLevel->rcAffectedArea.bottom;

	auto apMob = m_pField->GetLifePool()->FindAffectedMobInRect(rect, nullptr);
	for (auto pMob : apMob)
		pMob->DoSkill_StateChange(nSkillID, nSLV, pLevel, tDelay);
}

void Mob::DoSkill_PartizanOneTimeStatChange(int nSkillID, int nSLV, const MobSkillLevelData * pLevel, int tDelay)
{

}

void Mob::DoSkill_Summon(const MobSkillLevelData *pLevel, int tDelay)
{
	if (pLevel->anTemplateID.size() == 0)
		return;
	FieldRect rect;
	rect.left = -150;
	rect.top = -100;
	rect.right = 100;
	rect.bottom = 150;

	if (pLevel->rcAffectedArea.bottom && pLevel->rcAffectedArea.left &&
		pLevel->rcAffectedArea.top && pLevel->rcAffectedArea.right)
		rect = pLevel->rcAffectedArea;

	//Offset Rect
	rect.left += GetPosX();
	rect.right += GetPosX();
	rect.bottom += GetPosY();
	rect.top += GetPosY();

	int x = 0, y = 0;
	auto aPt = m_pField->GetSpace2D()->GetFootholdRandom((int)pLevel->anTemplateID.size(), rect);
	for (int i = 0; i < (int)pLevel->anTemplateID.size(); ++i)
	{
		if (m_pField->GetLifePool()->GetMobCount() >= m_pField->GetLifePool()->GetMaxMobCapacity())
			break;

		x = aPt[i].x;
		y = aPt[i].y;
		auto pFh = m_pField->GetSpace2D()->GetFootholdUnderneath(x, y, &y);
		if (pFh)
		{
			m_pField->GetLifePool()->CreateMob(
				pLevel->anTemplateID[i],
				x,
				y,
				pFh->GetSN(),
				false,
				pLevel->nSummonEffect,
				tDelay,
				0,
				0,
				nullptr);
		}
	}
}

void Mob::SendMobTemporaryStatSet(int nSet, int tDelay)
{
	OutPacket oPacket;
	oPacket.Encode2(MobSendPacketFlag::Mob_OnStatSet);
	oPacket.Encode4(GetFieldObjectID());
	oPacket.Encode4(nSet);
	m_pStat->EncodeTemporary(nSet, &oPacket, GameDateTime::GetTime());
	oPacket.Encode2(tDelay);
	oPacket.Encode1(0);
	m_pField->BroadcastPacket(&oPacket);
}

void Mob::SendMobTemporaryStatReset(int nSet)
{
	OutPacket oPacket;
	oPacket.Encode2(MobSendPacketFlag::Mob_OnStatReset);
	oPacket.Encode4(GetFieldObjectID());
	oPacket.Encode4(nSet);
	oPacket.Encode1(0);
	m_pField->BroadcastPacket(&oPacket);
}

void Mob::OnMobHit(User * pUser, long long int nDamage, int nAttackType)
{
	//m_mAttackRecord[pUser->GetUserID()] += nDamage;
	auto& log = m_damageLog.mInfo[pUser->GetUserID()];
	log.nDamage += (int)nDamage;
	log.nCharacterID = pUser->GetUserID();
	m_damageLog.nLastHitCharacter = pUser->GetUserID();
	m_damageLog.liTotalDamage += nDamage;
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
	long long int nHighestDamageRecord = 0, nTotalHP = GetMobTemplate()->m_lnMaxHP;
	double dLastHitBonus = 0.0, dStatBonusRate = 1.0, dBaseExp = 0, dIncEXP = 0;
	int nHighestDamageUser = 0;

	refOwnType = 0;
	refOwnParyID = 0;

	std::vector<PartyDamage> aParty;
	int nIdx = -1, nLevel = 255;
	for (auto& dmg : m_damageLog.mInfo) 
	{
		refLastDamageCharacterID = dmg.second.nCharacterID;
		if (nHighestDamageUser == 0 || nHighestDamageRecord > dmg.second.nDamage)
		{
			nHighestDamageUser = dmg.second.nCharacterID;
			nHighestDamageRecord = dmg.second.nDamage;
		}
	}

	for (auto& info : m_damageLog.mInfo)
	{
		auto pUser = User::FindUser(info.second.nCharacterID);
		if (pUser != nullptr)
		{
			std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
			int nPartyID = PartyMan::GetInstance()->GetPartyIDByCharID(pUser->GetUserID());
			if (nPartyID > 0)
			{
				nIdx = -1;
				nLevel = QWUser::GetLevel(pUser);
				for(nIdx = 0; nIdx < (int)aParty.size(); ++nIdx)
					if (aParty[nIdx].nParty == nPartyID)
						break;
					
				if (nIdx == (int)aParty.size())
					aParty.push_back({});
	
				auto& ptInfo = aParty[nIdx];
				ptInfo.nParty = nPartyID;
				ptInfo.nDamage += info.second.nDamage;
				if (info.second.nDamage > ptInfo.nMaxDamage)
				{
					ptInfo.nMaxDamage = info.second.nDamage;
					ptInfo.nMaxDamageCharacter = pUser->GetUserID();
					ptInfo.nMaxDamageLevel = nLevel;
				}
				if (nLevel < ptInfo.nMinLevel)
					ptInfo.nMinLevel = nLevel;
			}
			else
			{
				dLastHitBonus = info.second.nCharacterID == m_damageLog.nLastHitCharacter ? 
					0.2 * (double)GetMobTemplate()->m_nEXP : 0.0;

				dBaseExp = ((double)GetMobTemplate()->m_nEXP * (double)info.second.nDamage);
				dIncEXP = (dBaseExp * 0.8 / (double)m_damageLog.liTotalDamage + dLastHitBonus);
				dIncEXP *= (pUser->GetSecondaryStat()->nHolySymbol * 0.2 + 100.0);
				dIncEXP *= 0.01;
				dIncEXP *= (dStatBonusRate * 1.0); //1.0 = User::GetIncEXPRate()
				dIncEXP *= m_pField->GetIncEXPRate();
				if (pUser->GetSecondaryStat()->nCurse)
					dIncEXP *= 0.5;
				dIncEXP = dIncEXP > 1.0 ? dIncEXP : 1.0;

				/*auto nDamaged = info.second.nDamage;
				if (nDamaged >= nTotalHP)
					nDamaged = nTotalHP;*/
				//int nIncEXP = (int)floor((double)GetMobTemplate()->m_nEXP * ((double)info.second.nDamage / (double)m_damageLog.liTotalDamage));
				auto liFlag = QWUser::IncEXP(pUser, (int)dIncEXP, false);
				if (liFlag)
				{
					pUser->SendIncEXPMessage(true, (int)dIncEXP, false, 0, 0, 0, 0, 0, 0, 0, 0);
					pUser->SendCharacterStat(false, liFlag);
				}
			}
		}
	}
	GiveExp(aParty);
	return nHighestDamageUser;
}

void Mob::GiveExp(const std::vector<PartyDamage>& aPartyDamage)
{
	double dPartyBonusRate = 0,
		dPartyEXPIncRate = 0,
		dLastDamagePartyBonus = 0,
		dBaseEXP = 0,
		dLastDamageCharBonus = 0,
		dEXPMain = 0,
		dIncEXP = 0;

	int anLevel[PartyMan::MAX_PARTY_MEMBER_COUNT] = { 0 },
		anPartyMember[PartyMan::MAX_PARTY_MEMBER_COUNT] = { 0 },
		nPartyMemberCount = 0,
		nPartyBonusCount = 0,
		nMinLevel = 0,
		nLevelSum = 0;

	User *pUser = nullptr, *apUser[PartyMan::MAX_PARTY_MEMBER_COUNT];
	bool bLastDamageParty = false;

	for (auto& partyInfo : aPartyDamage)
	{
		nPartyMemberCount = 0; 
		nPartyBonusCount = 0;
		bLastDamageParty = false;

		if (!PartyMan::GetInstance()->GetParty(partyInfo.nParty))
			continue;

		PartyMan::GetInstance()->GetSnapshot(partyInfo.nParty, anPartyMember);
		for (int i = 0; i < PartyMan::MAX_PARTY_MEMBER_COUNT; ++i)
		{
			if (anPartyMember[i] &&
				(pUser = User::FindUser(anPartyMember[i]), pUser) &&
				pUser->GetField() && pUser->GetField()->GetFieldID() == m_damageLog.nFieldID)
			{
				if (pUser->GetUserID() == m_damageLog.nLastHitCharacter)
					bLastDamageParty = true;

				apUser[nPartyMemberCount] = pUser;
				anLevel[nPartyMemberCount] = QWUser::GetLevel(pUser);
				++nPartyMemberCount;
			}
		}
		nMinLevel = std::min(partyInfo.nMinLevel, m_pMobTemplate->m_nLevel) - 5;
		nLevelSum = 0;
		for (int i = 0; i < nPartyMemberCount; ++i)
			if (anLevel[i] >= nMinLevel) 
			{
				++nPartyBonusCount;
				nLevelSum += anLevel[i];
			}

		dPartyBonusRate = 0.05;
		dPartyEXPIncRate = nPartyBonusCount > 1 ? ((double)nPartyBonusCount) * dPartyBonusRate + 1.0 : 1.0;
		dLastDamagePartyBonus = (bLastDamageParty ? 1.0 : 0.0) * m_pMobTemplate->m_nEXP * 0.2;
		dBaseEXP = std::max(1.0, ((double)partyInfo.nDamage * m_pMobTemplate->m_nEXP * 0.8 / m_damageLog.liTotalDamage + dLastDamagePartyBonus) * dPartyEXPIncRate);
		dLastDamageCharBonus = dBaseEXP * 0.2;
		dEXPMain = dBaseEXP - dLastDamageCharBonus;

		while (--nPartyMemberCount >= 0)
		{
			if (anLevel[nPartyBonusCount] < nMinLevel)
				continue;
			pUser = apUser[nPartyMemberCount];
			dIncEXP = anLevel[nPartyMemberCount] * dEXPMain / nLevelSum;
			std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
			if (pUser->GetUserID() == m_damageLog.nLastHitCharacter)
				dIncEXP += dLastDamageCharBonus;

			if(nPartyBonusCount == 1)
				dIncEXP *= ((double)pUser->GetSecondaryStat()->nHolySymbol * 0.2 + 100.0) * 0.01;
			else if (nPartyBonusCount > 1)
			{
				dIncEXP *= ((double)pUser->GetSecondaryStat()->nHolySymbol + 100.0) * 0.01;
				dIncEXP = std::min(
					dIncEXP,
					((double)pUser->GetSecondaryStat()->nHolySymbol * 0.2 + 100.0) * m_pMobTemplate->m_nEXP * 0.01);
			}

			dIncEXP *= 1.0 * 1.0; //User::GetIncEXPRate & Showdown

			/*Calculate Marriage Bonus EXP*/

			if (pUser->GetSecondaryStat()->nCurse)
				dIncEXP *= 0.5;

			int nPartyBonusPercentage = (int)(dPartyEXPIncRate * 100.0 - 100.0);
			if (nPartyBonusPercentage <= 0)
				nPartyBonusPercentage = 0;
			
			int nWeddingBonusEXP = 0;

			auto liFlag = QWUser::IncEXP(pUser, (int)(dIncEXP), false);
			if (liFlag)
			{
				pUser->SendIncEXPMessage(
					pUser->GetUserID() == m_damageLog.nLastHitCharacter, 
					(int)dIncEXP, 
					false, 
					0, 
					0, 
					nPartyBonusPercentage, 
					0, 
					0, 
					0, 
					0, 
					0
				);
				pUser->SendCharacterStat(false, liFlag);
			}
		}
	}
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

Mob::DamageLog& Mob::GetDamageLog()
{
	return m_damageLog;
}

void Mob::Update(int tCur)
{
	auto nFlag = m_pStat->ResetTemporary(tCur);
	if (nFlag)
		SendMobTemporaryStatReset(nFlag);
}
