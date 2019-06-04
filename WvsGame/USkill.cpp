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

#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Random\Rand32.h"


/*
This MACRO defines some variables that are required by REGISTER_TS
Please define this in the beginning section of every DoActiveSkill_XXX
*/
#define REGISTER_USE_SKILL_SECTION \
nSLV = (nSLV > pSkill->GetMaxLevel() ? pSkill->GetMaxLevel() : nSLV);\
auto pSkillLVLData = pSkill->GetLevelData(nSLV);\
auto tsFlag = TemporaryStat::TS_Flag::GetDefault();\
std::pair<long long int, std::vector<int*>>* pRef = nullptr;\
std::pair<long long int, std::vector<int>>* pRefIndie = nullptr;\
auto pSS = pUser->GetSecondaryStat();\
int nSkillID = pSkill->GetSkillID();\
int tDelay = 0;\
int nDuration = pSkillLVLData->m_nTime;\
	if (nDuration == 0)\
		nDuration = INT_MAX;


/*
此MACRO作為註冊TemporaryStat(TS)用。
每個TS一定都要先呼叫此MACRO後，再補上自己需要的屬性。
此限制帶來的好處是User::Update檢測每個TS的時候，可以查看時間是否已經逾時，也追蹤Reason技能(n##name)。
同時也會將n, r, t都放到m_mSetByTS中，並以TS_FLAG作為key。
當bResetBySkill為true時，表示以清除指定技能帶來的TS，會將n, r, t都設定為0，並且將m_mSetByTS對應的值清空。
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
	pRef->first = bForcedSetTime ? nForcedSetTime : GameDateTime::GetTime();\
	pRef->second.push_back(&pSS->n##name##_);\
	pRef->second.push_back(&pSS->r##name##_);\
	pRef->second.push_back(&pSS->t##name##_);\
	pRef->second.push_back(&pSS->nLv##name##_);\
}\

void USkill::ValidateSecondaryStat(User * pUser)
{
	auto pSS = pUser->GetSecondaryStat();
	auto iter = pSS->m_mSetByTS.begin(),
		 iterEnd = pSS->m_mSetByTS.end();

	while (iter != iterEnd)
		if (iter->second.second.size() == 0)
		{
			pSS->m_mSetByTS.erase(iter);
			iter = pSS->m_mSetByTS.begin();
		}
		else
			++iter;
}

void USkill::OnSkillUseRequest(User * pUser, InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> userGuard(pUser->GetLock());
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
	nSLV = nSLV > pSkillRecord->nSLV ? pSkillRecord->nSLV : nSLV;
	if (!pUser->GetField() || 
		nSLV <= 0 ||
		nSLV > pSkillEntry->GetMaxLevel() ||
		pUser->GetCharacterData()->mStat->nMP < pSkillEntry->GetLevelData(nSLV)->m_nMpCon)
	{
		SendFailPacket(pUser);
		return;
	}

	if(pUser->GetSecondaryStat()->nInfinity_ == 0)
		pUser->SendCharacterStat(
			false,
			QWUser::IncMP(pUser, -pSkillEntry->GetLevelData(nSLV)->m_nMpCon, false)
		);

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

void USkill::OnSkillUseRequest(User * pUser, InPacket *iPacket, const SkillEntry * pEntry, int nSLV, bool bResetBySkill, bool bForceSetTime, int nForceSetTime)
{
	int nSkillID = pEntry->GetSkillID();
	if (SkillInfo::IsSummonSkill(nSkillID))
		DoActiveSkill_Summon(pUser, pEntry, nSLV, iPacket, bResetBySkill, bForceSetTime, nForceSetTime);
	else if (SkillInfo::IsPartyStatChangeSkill(nSkillID))
		DoActiveSkill_PartyStatChange(pUser, pEntry, nSLV, iPacket, bResetBySkill, bForceSetTime, nForceSetTime);
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
		bDecSP,
		bCheckMasterLevel,
		aChange))
	{
		pUser->ValidateStat();
		pUser->SendCharacterStat(false, BasicStat::BS_SP);
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

void USkill::DoActiveSkill_SelfStatChange(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket, int nOptionValue, bool bResetBySkill, bool bForcedSetTime, int nForcedSetTime)
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
		case 9101004: // hide
			//ret.duration = 2100000000;
			//ret.overTime = true;
		case 4001003: // darksight
			REGISTER_TS(DarkSight, pSkillLVLData->m_nX);
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
		case 1121000: // maple warrior, all classes
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
		case 3121002: // sharp eyes bow master
		case 3221002: // sharp eyes marksmen
					  // hack much (TODO is the order correct?)
			REGISTER_TS(SharpEyes, ((pSkillLVLData->m_nX << 8) | pSkillLVLData->m_nY));
			//statups.add(new Pair<MapleBuffStat, Integer>(MapleBuffStat.SHARP_EYES, Integer.valueOf(ret.x << 8 | ret.y)));
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
			//statups.add(new Pair<MapleBuffStat, Integer>(MapleBuffStat.STANCE, Integer.valueOf(iprop)));
			break;
		case 1005: // Echo of Hero
			//statups.add(new Pair<MapleBuffStat, Integer>(MapleBuffStat.ECHO_OF_HERO, Integer.valueOf(ret.x)));
			break;
		case 2121002: // mana reflection
		case 2221002:
		case 2321002:
			REGISTER_TS(ManaReflection, pSkillLVLData->m_nX);
			break;
		case 2321005: // holy shield
			REGISTER_TS(HolyShield, pSkillLVLData->m_nX);
			break;
		case 3111002: // puppet ranger
		case 3211002: // puppet sniper

			REGISTER_TS(PickPocket, 1);
			//statups.add(new Pair<MapleBuffStat, Integer>(MapleBuffStat.PUPPET, Integer.valueOf(1)));
			break;

			// ----------------------------- MONSTER STATUS PUT! ----------------------------- //
		case 4001002: // disorder
			//monsterStatus.put(MonsterStatus.WATK, Integer.valueOf(ret.x));
			//monsterStatus.put(MonsterStatus.WDEF, Integer.valueOf(ret.y));
			break;
		case 1201006: // threaten
			//monsterStatus.put(MonsterStatus.WATK, Integer.valueOf(ret.x));
			//monsterStatus.put(MonsterStatus.WDEF, Integer.valueOf(ret.y));
			break;
		case 1111005: // coma: sword
		case 1111006: // coma: axe
		case 1111008: // shout
		case 1211002: // charged blow
		case 3101005: // arrow bomb
		case 4211002: // assaulter
		case 4221007: // boomerang step
		case 5101002: // Backspin Blow
		case 5101003: // Double Uppercut
		case 5121004: // pirate 8 hit punches
		case 5121005: // pirate pull mob skill? O.o
		case 5121007: // pirate 6 hit shyt...
		case 5201004: // pirate blank shot
			//monsterStatus.put(MonsterStatus.STUN, Integer.valueOf(1));
			break;
			//case 5201004: // pirate blank shot
		case 4121003:
		case 4221003:
			//monsterStatus.put(MonsterStatus.SHOWDOWN, Integer.valueOf(1));
			break;
		case 2201004: // cold beam
		case 2211002: // ice strike
		case 2211006: // il elemental compo
		case 2221007: // Blizzard
		case 3211003: // blizzard
		case 5211005:
			//monsterStatus.put(MonsterStatus.FREEZE, Integer.valueOf(1));
			//ret.duration *= 2; // freezing skills are a little strange
			break;
		case 2121006://Paralyze
		case 2101003: // fp slow
		case 2201003: // il slow
			//monsterStatus.put(MonsterStatus.SPEED, Integer.valueOf(ret.x));
			break;
		case 2101005: // poison breath
		case 2111006: // fp elemental compo
			//monsterStatus.put(MonsterStatus.POISON, Integer.valueOf(1));
			break;
		case 2311005:
			//monsterStatus.put(MonsterStatus.DOOM, Integer.valueOf(1));
			break;
		case 3111005: // golden hawk
		case 3211005: // golden eagle
			REGISTER_TS(Summon, pSkillLVLData->m_nX);
			//statups.add(new Pair<MapleBuffStat, Integer>(MapleBuffStat.SUMMON, Integer.valueOf(1)));
			//monsterStatus.put(MonsterStatus.STUN, Integer.valueOf(1));
			break;
		case 2121005: // elquines
		case 3221005: // frostprey
			REGISTER_TS(Summon, pSkillLVLData->m_nX);
			//monsterStatus.put(MonsterStatus.FREEZE, Integer.valueOf(1));
			break;
		case 2111004: // fp seal
		case 2211004: // il seal
			//monsterStatus.put(MonsterStatus.SEAL, 1);
			break;
		case 4111003: // shadow web
			//monsterStatus.put(MonsterStatus.SHADOW_WEB, 1);
			break;
		case 3121007: // Hamstring
			REGISTER_TS(HamString, pSkillLVLData->m_nX);
			//monsterStatus.put(MonsterStatus.SPEED, x);
			break;
		case 3221006: // Blind
			REGISTER_TS(Blind, pSkillLVLData->m_nX);
			//monsterStatus.put(MonsterStatus.ACC, x);
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

void USkill::DoActiveSkill_PartyStatChange(User* pUser, const SkillEntry *pSkill, int nSLV, InPacket *iPacket, bool bResetBySkill, bool bForcedSetTime, int nForcedSetTime)
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
			!(pMember = User::FindUser(nID)) ||
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
			!(pMember = User::FindUser(nID))
			|| (bResetBySkill && pMember != pUser))
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
		else
		{
			pMember->SendCharacterStat(pMember == pUser, liFlag);
			pMember->SendTemporaryStatSet(tsFlag, 0);
			pMember->SendTemporaryStatSet(tsFlag, tDelay);
			if (pMember != pUser)
				pMember->SendUseSkillEffectByParty(nSkillID, nSLV);
		}
	}
}

void USkill::DoActiveSkill_MobStatChange(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket, int bSendResult)
{
}

void USkill::DoActiveSkill_Summon(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket, bool bResetBySkill, bool bForcedSetTime, int nForcedSetTime)
{
	nSLV = (nSLV > pSkill->GetMaxLevel() ? pSkill->GetMaxLevel() : nSLV);
	REGISTER_USE_SKILL_SECTION;
	REGISTER_TS(ComboCounter, 1);
	//REGISTER_TS(EVAR, 10);
	//REGISTER_TS(FireBomb, 10);
	pUser->RemoveSummoned(nSkillID, 0, nSkillID);
	if (bResetBySkill)
	{
		pUser->SendTemporaryStatReset(tsFlag);
		ValidateSecondaryStat(pUser);
	}
	else
	{
		pUser->CreateSummoned(pSkill, nSLV, { pUser->GetPosX(), pUser->GetPosY() }, false);
		pUser->SendTemporaryStatReset(tsFlag);
		pUser->SendTemporaryStatSet(tsFlag, 0);
	}
	pUser->SendCharacterStat(true, 0);
}

void USkill::DoActiveSkill_SmokeShell(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket)
{
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
