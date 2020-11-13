#include "USkill.h"
#include "User.h"
#include "QWUser.h"
#include "TemporaryStat.h"
#include "SecondaryStat.h"
#include "SkillEntry.h"
#include "MobSkillEntry.h"
#include "SkillLevelData.h"
#include "SkillInfo.h"
#include "QWUSkillRecord.h"
#include "PartyMan.h"
#include "Summoned.h"
#include "Field.h"
#include "LifePool.h"
#include "AffectedAreaPool.h"

//Skill constants
#include "AdminSkills.h"
#include "MagicSkills.h"
#include "WarriorSkills.h"
#include "ThiefSkills.h"
#include "BowmanSkills.h"
#include "PirateSkills.h"
#include "BeginnersSkills.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_SkillRecord.h"

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Random\Rand32.h"

#include <algorithm>
/*
This MACRO defines some variables that are required by REGISTER_TS
Please define this in the beginning section of every DoActiveSkill_XXX
*/
#define REGISTER_USE_SKILL_SECTION \
auto tCur = GameDateTime::GetTime(); \
nSLV = (nSLV > pSkill->GetMaxLevel() ? pSkill->GetMaxLevel() : nSLV);\
auto pSkillLVLData = pSkill->GetLevelData(bResetBySkill ? 1 : nSLV);\
auto tsFlag = TemporaryStat::TS_Flag::GetDefault();\
auto &pSS = pUser->GetSecondaryStat();\
std::lock_guard<std::recursive_mutex> _lock(pSS->m_mtxLock);\
std::pair<long long int, std::vector<int*>>* pRef = nullptr;\
std::pair<long long int, std::vector<int>>* pRefIndie = nullptr;\
int nSkillID = pSkill->GetSkillID(), tDelay = 0;\
int nDuration = (nForcedSetTime > tCur) ? (nForcedSetTime - tCur) : (pSkillLVLData->m_nTime ? pSkillLVLData->m_nTime : (INT_MAX)) ;\

/*
This MARCO is for TemporaryStat (TS) registration.
Using this MARCO to apply TSs to users benefits "User::Update" that will enable it the ability to check remaining time and trace applied skill (Reason, n##name) easily.

At same time, n##name, r##name and t##name will all be put into m_mSetByTS (in SecondaryStat), 
and the access key would be a TS_FLAG.

When bResetBySkill is set, it would set n##name, r##name and t##name to ZERO,
and erase the KV pair in m_mSetByTS.
*/

#define REGISTER_TS(name, value)\
tsFlag |= GET_TS_FLAG(##name);\
pRef = &pSS->m_mSetByTS[TemporaryStat::TS_##name]; pRef->second.clear();\
pSS->n##name##_ = bResetBySkill ? 0 : value;\
pSS->r##name##_ = bResetBySkill ? 0 : nSkillID;\
pSS->t##name##_ = bResetBySkill ? 0 : nDuration;\
pSS->nLv##name##_ = bResetBySkill ? 0 : nSLV;\
pSS->m_tsFlagSet ^= TemporaryStat::TS_##name;\
if(!bResetBySkill)\
{\
	pSS->m_tsFlagSet |= TemporaryStat::TS_##name;\
	pRef->first = bForcedSetTime ? nForcedSetTime : (tCur + nDuration);\
	pRef->second.push_back(&pSS->n##name##_);\
	pRef->second.push_back(&pSS->r##name##_);\
	pRef->second.push_back(&pSS->t##name##_);\
	pRef->second.push_back(&pSS->nLv##name##_);\
} else { pSS->m_mSetByTS.erase(TemporaryStat::TS_##name); }

void USkill::ValidateSecondaryStat(User * pUser)
{
	auto &pSS = pUser->GetSecondaryStat();
	auto iter = pSS->m_mSetByTS.begin(),
		 iterEnd = pSS->m_mSetByTS.end();

	while (iter != iterEnd)
		if (iter->second.second.size() == 0)
			iter = pSS->m_mSetByTS.erase(iter);
		else
			++iter;
}

void USkill::OnSkillUseRequest(User * pUser, InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> userGuard(pUser->GetLock());
	auto tCur = GameDateTime::GetTime();
	int tRequestTime = iPacket->Decode4();
	int nSkillID = iPacket->Decode4();
	int nSpiritJavelinItemID = 0;
	int nSLV = iPacket->Decode1();

	if (nSkillID == ThiefSkills::NightsLord_ShadowStars)
		nSpiritJavelinItemID = iPacket->Decode4();

	auto pSkillEntry = SkillInfo::GetInstance()->GetSkillByID(nSkillID);
	auto pSkillRecord = pUser->GetCharacterData()->GetSkill(nSkillID);

	if (pSkillEntry == nullptr || 
		pSkillRecord == nullptr ||
		pUser->GetCharacterData()->mStat->nHP <= 0)
	{
		SendFailPacket(pUser);
		return;
	}
	nSLV = std::min(nSLV, pSkillRecord->nSLV);
	if (!pUser->GetField() || 
		nSLV <= 0 ||
		nSLV > pSkillEntry->GetMaxLevel() ||
		pUser->GetSkillCooltime(nSkillID) > tCur ||
		!SkillInfo::GetInstance()->AdjustConsumeForActiveSkill(pUser, nSkillID, nSLV, false, nSpiritJavelinItemID))
	{
		SendFailPacket(pUser);
		return;
	}

	pUser->SetSkillCooltime(nSkillID, pSkillEntry->GetLevelData(pSkillRecord->nSLV)->m_nCooltime);
	USkill::OnSkillUseRequest(
		pUser,
		iPacket,
		pSkillEntry,
		nSLV,
		false,
		false,
		0
	);
}

void USkill::OnSkillUseRequest(User * pUser, InPacket *iPacket, const SkillEntry * pEntry, int nSLV, bool bResetBySkill, bool bForceSetTime, unsigned int nForceSetTime)
{
	int nSkillID = pEntry->GetSkillID();
	if (SkillInfo::IsSummonSkill(nSkillID))
		DoActiveSkill_Summon(pUser, pEntry, nSLV, iPacket, bResetBySkill, bForceSetTime, nForceSetTime);
	else if (SkillInfo::IsPartyStatChangeSkill(nSkillID))
		DoActiveSkill_PartyStatChange(pUser, pEntry, nSLV, iPacket, bResetBySkill, bForceSetTime, nForceSetTime);
	else if (SkillInfo::IsAdminSkill(nSkillID))
		DoActiveSkill_AdminSkill(pUser, pEntry, nSLV, iPacket, bResetBySkill, bForceSetTime, nForceSetTime);
	else if (SkillInfo::IsMobStatChangeSkill(nSkillID))
		DoActiveSkill_MobStatChange(pUser, pEntry, nSLV, iPacket, true);
	else if (nSkillID == ThiefSkills::Shadower_Smokescreen)
		DoActiveSkill_SmokeShell(pUser, pEntry, nSLV, iPacket);
	else
		DoActiveSkill_SelfStatChange(pUser, pEntry, nSLV, iPacket, 0, bResetBySkill, bForceSetTime, nForceSetTime);

	pUser->SendCharacterStat(true, 0);
}

void USkill::OnSkillUpRequest(User * pUser, InPacket * iPacket)
{
	int tTime = iPacket->Decode4();
	int nSkillID = iPacket->Decode4();
	int nAmount = 1;
	/*if (iPacket->GetPacketSize() > 8)
		nAmount = iPacket->Decode1();
	if (nAmount <= 0)
		nAmount = 1;*/
	OnSkillUpRequest(pUser, nSkillID, nAmount, true, true);
}

void USkill::OnSkillUpRequest(User * pUser, int nSkillID, int nAmount, bool bDecSP, bool bCheckMasterLevel)
{
	std::lock_guard<std::recursive_mutex> userGuard(pUser->GetLock());
	std::vector<GW_SkillRecord*> aChange;
	if (QWUSkillRecord::SkillUp(
		pUser,
		nSkillID,
		nAmount,
		(nSkillID == 1000 ||
		 nSkillID == 1001 || 
		 nSkillID == 1002) ? false : bDecSP,
		bCheckMasterLevel,
		aChange))
	{
		long long int liFlag = BasicStat::BS_SP;
		if (nSkillID == 1000001)
			liFlag |= pUser->IncMaxHPAndMP(0x2000, false);
		else if (nSkillID == 2000001)
			liFlag |= pUser->IncMaxHPAndMP(0x8000, false);
		pUser->ValidateStat();
		pUser->SendCharacterStat(false, liFlag);
	}
	QWUSkillRecord::SendCharacterSkillRecord(pUser, aChange);
}

void USkill::OnSkillPrepareRequest(User * pUser, InPacket * iPacket)
{
}

void USkill::OnSkillCancelRequest(User * pUser, InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> userGuard(pUser->GetLock());
	int nSkillID = iPacket->Decode4();
	SkillEntry* pSkill = nullptr;
	int nSLV = SkillInfo::GetInstance()->GetSkillLevel(
		pUser->GetCharacterData(),
		nSkillID,
		&pSkill);
	if (pSkill)
	{
		USkill::OnSkillUseRequest(
			pUser,
			nullptr,
			pSkill,
			nSLV,
			true,
			false,
			0
		);
	}
}

void USkill::SendFailPacket(User* pUser)
{
}

void USkill::DoActiveSkill_SelfStatChange(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket, int nOptionValue, bool bResetBySkill, bool bForcedSetTime, unsigned int nForcedSetTime)
{
	nSLV = (nSLV > pSkill->GetMaxLevel() ? pSkill->GetMaxLevel() : nSLV);
	REGISTER_USE_SKILL_SECTION;
	if (iPacket)
		tDelay = iPacket->Decode2();
	if (!pSkillLVLData) 
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "[USkill::DoActiveSkill_SelfStatChange]異常的技能資訊，技能ID = %d，技能等級 = %d\n", pSkill->GetSkillID(), nSLV);
		return;
	}
	if (pSkillLVLData->m_nPad > 0)
	{
		REGISTER_TS(PAD, pSkillLVLData->m_nPad);
	}
	if (pSkillLVLData->m_nPdd > 0)
	{
		REGISTER_TS(PDD, pSkillLVLData->m_nPdd);
	}
	if (pSkillLVLData->m_nMdd > 0)
	{
		REGISTER_TS(MDD, pSkillLVLData->m_nMdd);
	}
	if (pSkillLVLData->m_nMad > 0)
	{
		REGISTER_TS(MAD, pSkillLVLData->m_nMad);
	}
	if (pSkillLVLData->m_nAcc > 0)
	{
		REGISTER_TS(ACC, pSkillLVLData->m_nAcc);
	}
	if (pSkillLVLData->m_nEva > 0)
	{
		REGISTER_TS(EVA, pSkillLVLData->m_nEva);
	}
	if (pSkillLVLData->m_nCraft > 0)
	{
		REGISTER_TS(Craft, pSkillLVLData->m_nCraft);
	}
	if (pSkillLVLData->m_nSpeed > 0)
	{
		REGISTER_TS(Speed, pSkillLVLData->m_nSpeed);
	}
	if (pSkillLVLData->m_nJump > 0)
	{
		REGISTER_TS(Jump, pSkillLVLData->m_nJump);
	}
	auto iter = pSS->m_mSetByTS.begin();
	switch (nSkillID)
	{
		case MagicSkills::Magic_MagicGuard:
			REGISTER_TS(MagicGuard, pSkillLVLData->m_nX);
			break;
		case MagicSkills::Magic_Holy_Invincible:
			REGISTER_TS(Invincible, pSkillLVLData->m_nX);
			break;
		case ThiefSkills::Thief_DarkSight:
			REGISTER_TS(DarkSight, std::max(1, pSkillLVLData->m_nX));
			pSS->mDarkSight_ = tCur;
			break;
		case ThiefSkills::Chief_Bandit_Pickpocket:
			REGISTER_TS(PickPocket, pSkillLVLData->m_nX);
			break;
		case ThiefSkills::Chief_Bandit_MesoGuard:
			REGISTER_TS(MesoGuard, pSkillLVLData->m_nX);
			break;
		case ThiefSkills::Hermit_ShadowPartner:
			REGISTER_TS(ShadowPartner, pSkillLVLData->m_nX);
			break;
		case BowmanSkills::Hunter_SoulArrowBow:
		case BowmanSkills::Crossbowman_SoulArrowCrossbow:
		case 2311002: // mystic door - hacked buff icon
			REGISTER_TS(SoulArrow, pSkillLVLData->m_nX);
			break;
		case WarriorSkills::WhiteKnight_FireChargeSword:
		case WarriorSkills::WhiteKnight_FlameChargeBW:
		case WarriorSkills::WhiteKnight_IceChargeSword:
		case WarriorSkills::WhiteKnight_BlizzardChargeBW:
		case WarriorSkills::WhiteKnight_ThunderChargeSword:
		case WarriorSkills::WhiteKnight_LightningChargeBW:
		case WarriorSkills::Paladin_HolyChargeSword:
		case WarriorSkills::Paladin_DivineChargeBW:
			REGISTER_TS(WeaponCharge, pSkillLVLData->m_nX);
			break;
		case WarriorSkills::Fighter_SwordBooster:
		case WarriorSkills::Fighter_AxeBooster:
		case WarriorSkills::Page_SwordBooster:
		case WarriorSkills::Page_BWBooster:
		case WarriorSkills::Spearman_SpearBooster:
		case WarriorSkills::Spearman_PoleArmBooster:
		case MagicSkills::Adv_Magic_FP_SpellBooster:
		case MagicSkills::Adv_Magic_IL_SpellBooster:
		case BowmanSkills::Hunter_BowBooster:
		case BowmanSkills::Crossbowman_CrossbowBooster:
		case ThiefSkills::Assassin_ClawBooster:
		case ThiefSkills::Bandit_DaggerBooster:
		case PirateSkills::Brawler_KnucklerBooster:
		case PirateSkills::Gunslinger_GunBooster:
			REGISTER_TS(Booster, pSkillLVLData->m_nX);
			break;	
		case WarriorSkills::Fighter_Rage:
			REGISTER_TS(PDD, pSkillLVLData->m_nPdd);
			break;
		case WarriorSkills::Hero_Enrage:
			REGISTER_TS(PAD, pSkillLVLData->m_nPad);
			break;
		case WarriorSkills::Spearman_IronWill:
			REGISTER_TS(MDD, pSkillLVLData->m_nMdd);
		case WarriorSkills::Warrior_IronBody:
			REGISTER_TS(PDD, pSkillLVLData->m_nPdd);
			break;
		case MagicSkills::Magic_MagicArmor:
			REGISTER_TS(PDD, pSkillLVLData->m_nPdd);
			break;
		case MagicSkills::Magic_Holy_Bless:
			REGISTER_TS(PDD, pSkillLVLData->m_nPdd);
			REGISTER_TS(MDD, pSkillLVLData->m_nMdd);
		case AdminSkills::Admin_Super_Bless:
			REGISTER_TS(MAD, pSkillLVLData->m_nMad);
		case BowmanSkills::Bowman_Focus:
			REGISTER_TS(ACC, pSkillLVLData->m_nAcc);
			REGISTER_TS(EVA, pSkillLVLData->m_nEva);
			break;
		case BowmanSkills::Bow_Master_Concentrate:
			REGISTER_TS(Concentration, pSkillLVLData->m_nX);
			REGISTER_TS(PAD, pSkillLVLData->m_nPad);
			break;
		case PirateSkills::Pirate_Dash:
			REGISTER_TS(Dash_Speed, 1);
			REGISTER_TS(Dash_Jump, 1);
			break;
		case WarriorSkills::Fighter_PowerGuard:
		case WarriorSkills::Page_PowerGuard:
			REGISTER_TS(PowerGuard, pSkillLVLData->m_nX);
			break;
		case WarriorSkills::Spearman_HyperBody:
		case AdminSkills::Admin_Super_HyperBody:
			REGISTER_TS(MaxHP, pSkillLVLData->m_nX);
			REGISTER_TS(MaxMP, pSkillLVLData->m_nY);
			break;
		case BeginnersSkills::Beginner_Recovery:
			REGISTER_TS(Regen, pSkillLVLData->m_nX);
			break;
		case WarriorSkills::Crusader_ComboAttack: 
		{
			REGISTER_TS(ComboCounter, 1);
			SkillEntry *pADVCombo = nullptr;
			int nSLVAdvancedCombo = SkillInfo::GetInstance()->GetSkillLevel(pUser->GetCharacterData(), WarriorSkills::Hero_AdvancedComboAttack, &pADVCombo);
			if (nSLVAdvancedCombo) 
			{
				auto pLevelADVCombo = pADVCombo->GetLevelData(nSLVAdvancedCombo);
				if (pLevelADVCombo)
					pSS->mComboCounter_ = pLevelADVCombo->m_nX | (pLevelADVCombo->m_nProp << 16);
				else
					pSS->mComboCounter_ = pSkillLVLData->m_nX;
			}
			break;
		}
		case BeginnersSkills::Beginner_MonsterRider:
			REGISTER_TS(RideVehicle, 1);
			break;
		case PirateSkills::Corsair_Battleship:
			REGISTER_TS(RideVehicle, 1932000);
			break;
		case WarriorSkills::DragonKnight_DragonRoar: //dragon roar
			//ret.hpR = -x / 100.0;
			break;
		case WarriorSkills::DragonKnight_DragonBlood:
			REGISTER_TS(DragonBlood, pSkillLVLData->m_nX);
			break;
		case ThiefSkills::NightsLord_ShadowStars: //
			REGISTER_TS(SpiritJavelin, pSkillLVLData->m_nX);
			break;
		case WarriorSkills::Hero_PowerStance:
		case WarriorSkills::Paladin_PowerStance:
		case WarriorSkills::DarkKnight_PowerStance:
			REGISTER_TS(Stance, pSkillLVLData->m_nX);
			break;
		case MagicSkills::Highest_Magic_FP_Infinity:
		case MagicSkills::Highest_Magic_Holy_Infinity:
		case MagicSkills::Highest_Magic_IL_Infinity:
			REGISTER_TS(Infinity, pSkillLVLData->m_nX);
			break;
		case MagicSkills::Highest_Magic_FP_ManaReflection:
		case MagicSkills::Highest_Magic_Holy_ManaReflection:
		case MagicSkills::Highest_Magic_IL_ManaReflection:
			REGISTER_TS(ManaReflection, nSLV);
			break;
		case BowmanSkills::Bow_Master_Hamstring:
			REGISTER_TS(HamString, pSkillLVLData->m_nX);
			break;
		case BowmanSkills::Marksman_Blind:
			REGISTER_TS(Blind, pSkillLVLData->m_nX);
			break;
		default:
			break;
	}
	pUser->ValidateStat();
	long long int liFlag = 0;
	liFlag |= BasicStat::BasicStatFlag::BS_MaxHP;
	liFlag |= BasicStat::BasicStatFlag::BS_MaxMP;
	if (bResetBySkill)
	{
		pUser->SendTemporaryStatReset(tsFlag);
		ValidateSecondaryStat(pUser);
	}
	else
	{
		pUser->SendUseSkillEffect(nSkillID, nSLV);
		pUser->SendTemporaryStatReset(tsFlag);
		pUser->SendTemporaryStatSet(tsFlag, tDelay);
	}
	pUser->SendCharacterStat(true, liFlag);
}

void USkill::DoActiveSkill_WeaponBooster(User* pUser, const SkillEntry * pSkill, int nSLV, int nWT1, int nWT2)
{
}

void USkill::DoActiveSkill_TownPortal(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket)
{
}

void USkill::DoActiveSkill_PartyStatChange(User* pUser, const SkillEntry *pSkill, int nSLV, InPacket *iPacket, bool bResetBySkill, bool bForcedSetTime, unsigned int nForcedSetTime)
{
	nSLV = (nSLV > pSkill->GetMaxLevel() ? pSkill->GetMaxLevel() : nSLV);
	REGISTER_USE_SKILL_SECTION;

	int anCharacterID[PartyMan::MAX_PARTY_MEMBER_COUNT] = { 0 }, nHPInc = 0;
	auto pParty = PartyMan::GetInstance()->GetPartyByCharID(pUser->GetUserID());
	if (!pParty)
		anCharacterID[0] = pUser->GetUserID();
	else
		PartyMan::GetInstance()->GetSnapshot(pParty->nPartyID, anCharacterID);

	User *pMember = nullptr;
	int nValidMemberCount = 0;

	for (auto& nID : anCharacterID)
	{
		if (nID == 0 ||
			!(pMember = (nID == pUser->GetUserID() ? pUser : User::FindUser(nID))) ||
			pMember->GetField() != pUser->GetField()) 
		{
			nID = 0;
			continue;
		}
		++nValidMemberCount;
	}

	if (pSkillLVLData->m_nHp)
	{
		double dINT = (double)QWUser::GetINT(pUser);
		int nOffset = (int)(dINT * 0.2), nInc = 0;
		if (nOffset)
			nInc = (int)(Rand32::GetInstance()->Random() % nOffset);
		nOffset = pUser->GetSecondaryStat()->nMAD + pUser->GetSecondaryStat()->nMAD_;
		nOffset = (int)(((double)nInc * 1.15 + (double)QWUser::GetLUK(pUser)) * (double)nOffset * 0.01);
		nHPInc = (int)(((double)nOffset * ((double)nValidMemberCount * 0.3 + 1.0)) * (double)pSkillLVLData->m_nHp * 0.01);
	}

	for (auto& nID : anCharacterID)
	{
		if (nID == 0 ||
			!(pMember = (nID == pUser->GetUserID() ? pUser : User::FindUser(nID))) ||
			(bResetBySkill && pMember != pUser))
			continue;

		TemporaryStat::TS_Flag tsSet;
		SecondaryStat* pSS = pMember->GetSecondaryStat();
		long long int liFlag = 0;

		if (pSkillLVLData->m_nPad)
		{
			REGISTER_TS(PAD, pSkillLVLData->m_nPad);
		}
		if (pSkillLVLData->m_nPdd)
		{
			REGISTER_TS(PAD, pSkillLVLData->m_nPdd);
		}
		if (pSkillLVLData->m_nMad)
		{
			REGISTER_TS(PAD, pSkillLVLData->m_nMad);
		}		
		if (pSkillLVLData->m_nMdd)
		{
			REGISTER_TS(PAD, pSkillLVLData->m_nMdd);
		}
		if (pSkillLVLData->m_nAcc)
		{
			REGISTER_TS(PAD, pSkillLVLData->m_nAcc);
		}
		if (pSkillLVLData->m_nEva)
		{
			REGISTER_TS(PAD, pSkillLVLData->m_nEva);
		}
		if (pSkillLVLData->m_nCraft)
		{
			REGISTER_TS(PAD, pSkillLVLData->m_nCraft);
		}		
		if (pSkillLVLData->m_nSpeed)
		{
			REGISTER_TS(PAD, pSkillLVLData->m_nSpeed);
		}
		if (pSkillLVLData->m_nJump)
		{
			REGISTER_TS(Jump, pSkillLVLData->m_nJump);
		}
		switch (nSkillID)
		{
			case MagicSkills::Magic_FP_Meditation:
			case MagicSkills::Magic_IL_Meditation:
				REGISTER_TS(MAD, pSkillLVLData->m_nMad);
				break;
			case MagicSkills::Highest_Magic_Holy_HolyShield:
				REGISTER_TS(HolyShield, pSkillLVLData->m_nX);
				break;
			case WarriorSkills::Hero_MapleWarrior:
			case WarriorSkills::Paladin_MapleWarrior:
			case WarriorSkills::DarkKnight_MapleWarrior:
			case MagicSkills::Highest_Magic_FP_MapleWarrior:
			case MagicSkills::Highest_Magic_IL_MapleWarrior:
			case MagicSkills::Highest_Magic_Holy_MapleWarrior:
			case BowmanSkills::Bow_Master_MapleWarrior:
			case BowmanSkills::Marksman_MapleWarrior:
			case ThiefSkills::NightsLord_MapleWarrior:
			case ThiefSkills::Shadower_MapleWarrior:
			case PirateSkills::Buccaneer_MapleWarrior:
			case PirateSkills::Corsair_MapleWarrior:
				REGISTER_TS(BasicStatUp, pSkillLVLData->m_nX);
				break;
			case BowmanSkills::Bow_Master_SharpEyes:
			case BowmanSkills::Marksman_SharpEyes:
				REGISTER_TS(SharpEyes, ((pSkillLVLData->m_nX << 8) | pSkillLVLData->m_nY));
				break;
			case MagicSkills::Adv_Magic_Holy_HolySymbol:
			case AdminSkills::Admin_Super_HolySymbol:
				REGISTER_TS(HolySymbol, pSkillLVLData->m_nX);
				break;
			case ThiefSkills::Hermit_MesoUp:
				REGISTER_TS(MesoUp, pSkillLVLData->m_nX);
				break;
			case ThiefSkills::Assassin_Haste:
			case ThiefSkills::Bandit_Haste:
			case AdminSkills::Admin_Super_HasteSuper:
				REGISTER_TS(Speed, pSkillLVLData->m_nSpeed);
				REGISTER_TS(Jump, pSkillLVLData->m_nJump);
				break;
			case PirateSkills::Buccaneer_SpeedInfusion:
				REGISTER_TS(PartyBooster, pSkillLVLData->m_nX);
				break;
		}

		if (pSkillLVLData->m_nHp)
		{
			liFlag |= QWUser::IncHP(pMember, nHPInc / nValidMemberCount, false);
			if(liFlag)
			{
			}
		}	
		if (bResetBySkill)
		{
			pMember->SendTemporaryStatReset(tsFlag);
			ValidateSecondaryStat(pMember);
		}
		else if (!bForcedSetTime)
		{
			pMember->SendCharacterStat(pMember == pUser, liFlag);
			pMember->SendTemporaryStatSet(tsFlag, tDelay);
			if (pMember != pUser)
				pMember->SendUseSkillEffectByParty(nSkillID, nSLV);
			else
				pMember->SendUseSkillEffect(nSkillID, nSLV);
		}
	}
}

void USkill::DoActiveSkill_MobStatChange(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket, int bSendResult)
{
	static const int MAX_TARGET_COUNT = 16;
	int nMobID[MAX_TARGET_COUNT] = { 0 };
	int nMobCount = std::min(MAX_TARGET_COUNT, (int)iPacket->Decode1()), tDelay = 0;

	for (int i = 0; i < nMobCount; ++i)
		nMobID[i] = iPacket->Decode4();

	tDelay = iPacket->Decode2();
	for (int i = 0; i < nMobCount; ++i)
	{
		pUser->GetField()->GetLifePool()->OnMobStatChangeSkill(
			pUser,
			nMobID[i],
			pSkill,
			nSLV,
			tDelay
		);
	}

	if (bSendResult)
		pUser->SendUseSkillEffect(pSkill->GetSkillID(), nSLV);
}

void USkill::DoActiveSkill_Summon(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket, bool bResetBySkill, bool bForcedSetTime, unsigned int nForcedSetTime)
{
	nSLV = (nSLV > pSkill->GetMaxLevel() ? pSkill->GetMaxLevel() : nSLV);
	REGISTER_USE_SKILL_SECTION;
	pUser->RemoveSummoned(nSkillID, 0, nSkillID);
	if (bResetBySkill)
	{
		pUser->SendTemporaryStatReset(tsFlag);
		ValidateSecondaryStat(pUser);
	}
	else
	{
		pUser->CreateSummoned(pSkill, nSLV, { pUser->GetPosX(), pUser->GetPosY() }, false);
		pUser->SendUseSkillEffect(nSkillID, nSLV);
	}
}

void USkill::DoActiveSkill_SmokeShell(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket)
{
	FieldPoint pt;
	pt.x = iPacket->Decode2();
	pt.y = iPacket->Decode2();

	auto pLevelData = pSkill->GetLevelData(nSLV);
	if (!pLevelData)
		return;

	FieldRect rc;
	rc.left = pLevelData->m_rc.left;
	rc.top = pLevelData->m_rc.top;
	rc.right = pLevelData->m_rc.right;
	rc.bottom = pLevelData->m_rc.bottom;
	rc.OffsetRect(pt.x, pt.y);

	unsigned int tTime = GameDateTime::GetTime() + pLevelData->m_nTime + 300;

	pUser->GetField()->GetAffectedAreaPool()->InsertAffectedArea(
		false,
		pUser->GetUserID(),
		pSkill->GetSkillID(),
		nSLV,
		tTime - pLevelData->m_nTime,
		tTime,
		pt,
		rc,
		true
	);

	pUser->SendUseSkillEffect(pSkill->GetSkillID(), nSLV);
}

void USkill::DoActiveSkill_AdminSkill(User * pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket, bool bResetBySkill, bool bForcedSetTime, unsigned int nForcedSetTime)
{
	nSLV = (nSLV > pSkill->GetMaxLevel() ? pSkill->GetMaxLevel() : nSLV);
	REGISTER_USE_SKILL_SECTION;
	switch (nSkillID)
	{
		case AdminSkills::Admin_HyperHidden: 
			nDuration = 1000 * 60 * 60 * 24 * 15;
			REGISTER_TS(DarkSight, std::max(1, pSkillLVLData->m_nX));
			break;
	}

	if (nSkillID == AdminSkills::Admin_HyperHidden)
	{
		OutPacket oEnterField, oLeaveField;
		pUser->MakeEnterFieldPacket(&oEnterField);
		pUser->MakeLeaveFieldPacket(&oLeaveField);
		if (!bResetBySkill)
			pUser->GetField()->BroadcastPacket(&oLeaveField);
		pUser->GetField()->RegisterFieldObj(pUser, &oEnterField);
	}

	if (bResetBySkill)
	{
		pUser->SendTemporaryStatReset(tsFlag);
		ValidateSecondaryStat(pUser);
	}
	else
	{
		pUser->SendUseSkillEffect(nSkillID, nSLV);
		pUser->SendTemporaryStatReset(tsFlag);
		pUser->SendTemporaryStatSet(tsFlag, tDelay);
	}
	pUser->SendCharacterStat(true, 0);
}

void USkill::ResetTemporaryByTime(User * pUser, const std::vector<int>& aResetReason)
{
	std::lock_guard<std::recursive_mutex> userGuard(pUser->GetLock());
	for (auto nReason : aResetReason)
	{
		SkillEntry* pSkill = nullptr;
		auto nSLV = SkillInfo::GetInstance()->GetSkillLevel(pUser->GetCharacterData(), nReason, &pSkill);
		if (pSkill)
		{
			USkill::OnSkillUseRequest(
				pUser,
				nullptr,
				pSkill,
				nSLV,
				true,
				false,
				0
			);
		}
		else if (SkillInfo::GetInstance()->GetMobSkill(nReason & 0xFF))
		{
			//The nSVL doesn't matter
			auto pLevel = SkillInfo::GetInstance()->GetMobSkill(nReason & 0xFF)->GetLevelData(1);
			pUser->OnStatChangeByMobSkill(nReason & 0xFF, 1, pLevel, 0, 0, true);
		}
	}
}
