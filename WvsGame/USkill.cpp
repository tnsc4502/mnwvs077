#include "USkill.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_SkillRecord.h"

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
#include "AdminSkills.h"
#include "Field.h"

#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Random\Rand32.h"


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
		!ConsumeHPAndMPBySkill(pUser, pSkillEntry->GetLevelData(pSkillRecord->nSLV)))
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

bool USkill::ConsumeHPAndMPBySkill(User *pUser, const SkillLevelData *pLevelData)
{
	int nMpCon = pLevelData ? pLevelData->m_nMpCon : 0;
	int nHpCon = pLevelData ? pLevelData->m_nHpCon : 0;
	long long int liFlag = 0;

	if (nMpCon && pUser->GetSecondaryStat()->nInfinity_ == 0)
	{
		if (QWUser::GetMP(pUser) < nMpCon)
			return false;
		liFlag |= QWUser::IncMP(pUser, -nMpCon, false);
	}

	if (nHpCon)
	{
		if (QWUser::GetHP(pUser) < nHpCon)
			return false;
		liFlag |= QWUser::IncHP(pUser, -nHpCon, false);
	}
	pUser->SendCharacterStat(false, liFlag);
	return true;
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
	else
		DoActiveSkill_SelfStatChange(pUser, pEntry, nSLV, iPacket, 0, bResetBySkill, bForceSetTime, nForceSetTime);
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
		&pSkill,
		0,
		0,
		0,
		0);
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
		case 2001002: // magic guard
			REGISTER_TS(MagicGuard, pSkillLVLData->m_nX);
			break;
		case 2301003: // invincible
			REGISTER_TS(Invincible, pSkillLVLData->m_nX);
			break;
		case 4001003: // darksight
			REGISTER_TS(DarkSight, std::max(1, pSkillLVLData->m_nX));
			break;
		case 4211003: // pickpocket
			REGISTER_TS(PickPocket, pSkillLVLData->m_nX);
			break;
		case 4211005: // mesoguard
			REGISTER_TS(MesoGuard, pSkillLVLData->m_nX);
			break;
		case 4111001: // mesoup
			REGISTER_TS(MesoUp, pSkillLVLData->m_nX);
			break;
		case 4111002: // shadowpartner
			REGISTER_TS(ShadowPartner, pSkillLVLData->m_nX);
			break;
		case 3101004: // soul arrow
		case 3201004:
		case 2311002: // mystic door - hacked buff icon
			REGISTER_TS(SoulArrow, pSkillLVLData->m_nX);
			break;
		case 1211003:
		case 1211004:
		case 1211005:
		case 1211006: // wk charges
		case 1211007:
		case 1211008:
		case 1221003:
		case 1221004:
			REGISTER_TS(WeaponCharge, pSkillLVLData->m_nX);
			break;
		case 1101004:
		case 1101005: // booster
		case 1201004:
		case 1201005:
		case 1301004:
		case 1301005:
		case 2111005: // spell booster, do these work the same?
		case 2211005:
		case 3101002:
		case 3201002:
		case 4101003:
		case 4201002:
		case 5101006:
		case 5201003:
			//WvsLogger::LogFormat("Weapon Charge, NX = %d\n", pSkillLVLData->m_nX);
			REGISTER_TS(Booster, pSkillLVLData->m_nX);
			break;
			//case 5121009:
			//	statups.add(new Pair<MapleBuffStat, Integer>(MapleBuffStat.SPEED_INFUSION, ret.x));
			//	break; 		
		case 5121009:
		case 5221010:
			//statups.add(new Pair<MapleBuffStat, Integer>(MapleBuffStat.SPEED_INFUSION, Integer.valueOf(-4)));
			break;
		case 1101006: // rage
			REGISTER_TS(PDD, pSkillLVLData->m_nPdd);
			break;
		case 1121010: // enrage
			REGISTER_TS(PAD, pSkillLVLData->m_nPad);
			break;
		case 1301006: // iron will
			REGISTER_TS(MDD, pSkillLVLData->m_nMdd);
		case 1001003: // iron body
			REGISTER_TS(PDD, pSkillLVLData->m_nPdd);
			break;
		case 2001003: // magic armor
			REGISTER_TS(PDD, pSkillLVLData->m_nPdd);
			break;
		case 2101001: // meditation
		case 2201001: // meditation
			REGISTER_TS(MAD, pSkillLVLData->m_nMad);
			break;
		case 4101004: // haste
		case 4201003: // haste
		case 9101001: // gm haste
			REGISTER_TS(Speed, pSkillLVLData->m_nSpeed);
			REGISTER_TS(Jump, pSkillLVLData->m_nJump);
			break;
		case 2301004: // bless
			REGISTER_TS(PDD, pSkillLVLData->m_nPdd);
			REGISTER_TS(MDD, pSkillLVLData->m_nMdd);
		case 3001003: // focus
			REGISTER_TS(ACC, pSkillLVLData->m_nAcc);
			REGISTER_TS(EVA, pSkillLVLData->m_nEva);
			break;
		case 9101003: // gm bless
			REGISTER_TS(MAD, pSkillLVLData->m_nMad);
		case 3121008: // concentrate

			REGISTER_TS(Concentration, pSkillLVLData->m_nX);
			//statups.add(new Pair<MapleBuffStat, Integer>(MapleBuffStat.WATK, Integer.valueOf(ret.watk)));
			break;
		case 5001005: // Dash
			//statups.add(new Pair<MapleBuffStat, Integer>(MapleBuffStat.DASH, Integer.valueOf(1)));
			break;
		case 1101007: // pguard
		case 1201007:
			REGISTER_TS(PowerGuard, pSkillLVLData->m_nX);
			break;
		case 1301007:
		case 9101008:
			REGISTER_TS(MaxHP, pSkillLVLData->m_nX);
			REGISTER_TS(MaxMP, pSkillLVLData->m_nY);
			break;
		case 1001: // recovery
			//statups.add(new Pair<MapleBuffStat, Integer>(MapleBuffStat.RECOVERY, Integer.valueOf(x)));
			break;
		case 1111002: // combo
			REGISTER_TS(ComboCounter, 1);
			//statups.add(new Pair<MapleBuffStat, Integer>(MapleBuffStat.COMBO, Integer.valueOf(1)));
			break;
		case 1004: // monster riding 
			REGISTER_TS(RideVehicle, 1);
			//statups.add(new Pair<MapleBuffStat, Integer>(MapleBuffStat.MONSTER_RIDING, Integer.valueOf(1)));
			break;
		case 5221006: // 4th Job - Pirate riding 
			//statups.add(new Pair<MapleBuffStat, Integer>(MapleBuffStat.MONSTER_RIDING, 1932000));
			break;
		case 1311006: //dragon roar
			//ret.hpR = -x / 100.0;
			break;
		case 1311008: // dragon blood
			REGISTER_TS(DragonBlood, pSkillLVLData->m_nX);
			break;
		case 1321007: // Beholder
		case 2221005: // ifrit
		case 2311006: // summon dragon
		case 2321003: // bahamut
		case 3121006: // phoenix
		case 5211001: // Pirate octopus summon
		case 5211002: // Pirate bird summon
		case 5220002: // wrath of the octopi
			REGISTER_TS(Summon, pSkillLVLData->m_nX);
			break;
		case 2311003: // hs
		case 9101002: // GM hs
			REGISTER_TS(HolySymbol, pSkillLVLData->m_nX);
			break;
		case 4121006: // spirit claw
			REGISTER_TS(Attract, pSkillLVLData->m_nX);
			break;
		case 2121004:
		case 2221004:
		case 2321004: // Infinity
			REGISTER_TS(Infinity, pSkillLVLData->m_nX);
			break;
		case 1121002:
		case 1221002:
		case 1321002: // Stance
			REGISTER_TS(Stance, pSkillLVLData->m_nX);
			break;
		case 2121002: // mana reflection
		case 2221002:
		case 2321002:
			REGISTER_TS(ManaReflection, pSkillLVLData->m_nX);
			break;
		case 2321005: // holy shield
			REGISTER_TS(HolyShield, pSkillLVLData->m_nX);
			break;
		case 3121007: // Hamstring
			REGISTER_TS(HamString, pSkillLVLData->m_nX);
			break;
		case 3221006: // Blind
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
			case 1121000:
			case 1221000:
			case 1321000:
			case 2121000:
			case 2221000:
			case 2321000:
			case 3121000:
			case 3221000:
			case 4121000:
			case 4221000:
			case 5121000:
			case 5221000:
				REGISTER_TS(BasicStatUp, pSkillLVLData->m_nX);
				break;
			case 3121002: 
			case 3221002: 
				REGISTER_TS(SharpEyes, ((pSkillLVLData->m_nX << 8) | pSkillLVLData->m_nY));
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
	pUser->SendCharacterStat(true, 0);
}

void USkill::DoActiveSkill_SmokeShell(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket)
{
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
		auto nSLV = SkillInfo::GetInstance()->GetSkillLevel(pUser->GetCharacterData(), nReason, &pSkill, 0, 0, 0, 0);
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
