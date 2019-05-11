#include "USkill.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_SkillRecord.h"

#include "User.h"
#include "TemporaryStat.h"
#include "SecondaryStat.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "SkillInfo.h"
#include "QWUSkillRecord.h"

#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"

#include "..\WvsLib\Logger\WvsLogger.h"


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
int nDuration = pSkillLVLData->m_nTime * 1000;\
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
pSS->n##name = bResetBySkill ? 0 : value;\
pSS->r##name = bResetBySkill ? 0 : nSkillID;\
pSS->t##name = bResetBySkill ? 0 : nDuration;\
pSS->nLv##name = bResetBySkill ? 0 : nSLV;\
if(!bResetBySkill)\
{\
	pRef->first = bForcedSetTime ? nForcedSetTime : GameDateTime::GetTime();\
	pRef->second.push_back(&pSS->n##name);\
	pRef->second.push_back(&pSS->r##name);\
	pRef->second.push_back(&pSS->t##name);\
	pRef->second.push_back(&pSS->nLv##name);\
}\

#define REGISTER_INDIE_TS(name, value)\
tsFlag |= GET_TS_FLAG(##name);\
pRefIndie = &pSS->m_mSetByIndieTS[TemporaryStat::TS_##name][nSkillID]; pRefIndie->second.clear();\
pSS->n##name += bResetBySkill ? -value : value;\
if(!bResetBySkill)\
{\
	pRefIndie->first = bForcedSetTime ? nForcedSetTime : GameDateTime::GetTime();\
	pRefIndie->second.push_back(value);\
	pRefIndie->second.push_back(nSkillID);\
	pRefIndie->second.push_back(nDuration);\
	pRefIndie->second.push_back(nSLV);\
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
	if (pSkillEntry == nullptr || pSkillRecord == nullptr)
	{
		SendFailPacket(pUser);
		return;
	}
	nSLV = nSLV > pSkillRecord->nSLV ? pSkillRecord->nSLV : nSLV;
	if (!pUser->GetField() || nSLV <= 0) 
	{
		SendFailPacket(pUser);
		return;
	}
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
			iPacket,
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
		case 1321015:
		{
			//REGISTER_TS(IgnoreTargetDEF, pSkillLVLData->m_nIgnoreMobpdpR);
			//REGISTER_TS(BDR, pSkillLVLData->m_nBdR);
			break;
		}
		case 2001002:
		{
			REGISTER_TS(MagicGuard, pSkillLVLData->m_nY);
			break;
		}
		case 2301003: // invincible
		{
			REGISTER_TS(Invincible, pSkillLVLData->m_nX);
			break;
		}
		case 35120000://Extreme proto
		case 35001002://mech proto

			break;
		case 9101004:
		case 9001004: // hide
		{
			REGISTER_TS(DarkSight, pSkillLVLData->m_nX);
			break;
		}
		case 4001003: // Dark Sight
		{
			REGISTER_TS(DarkSight, pSkillLVLData->m_nX); // d
			break;
		}
		case 4211003: // pickpocket
		{
			REGISTER_TS(PickPocket, pSkillLVLData->m_nX);
			break;
		}
		case 4211005: // mesoguard
		case 4201011:
		{
			REGISTER_TS(MesoGuard, pSkillLVLData->m_nX);
			break;
		}
		case 4111001: // mesoup
		{
			REGISTER_TS(MesoUp, pSkillLVLData->m_nX);
			break;
		}
		case 4211008:
		{
			REGISTER_TS(ShadowPartner, nSLV);
			break;
		}
		case 3101004: // soul arrow
		case 3201004:
		case 2311002: // mystic door - hacked buff icon
		{
			REGISTER_TS(SoulArrow, pSkillLVLData->m_nX);
			break;
		}
		case 2321010:
		case 2221009:
		case 2121009:
		{
			//REGISTER_TS(BUFF_MASTERY, pSkillLVLData->m_nX);
			break;
		}
		case 1211006: // wk charges
		case 1211004:
		case 1211008:
		case 1221004:
			// case 51111003: // Mihile's Radiant Charge
		{
			REGISTER_TS(WeaponCharge, pSkillLVLData->m_nX);
			//REGISTER_TS(DamR, pSkillLVLData->m_nZ);
			break;
			//                    case 51111004:
			//                        REGISTER_TS(ABNORMAL_STATUS_R, pSkillLVLData->m_nY);
			//                        REGISTER_TS(ELEMENTAL_STATUS_R, pSkillLVLData->m_nZ);
			//                        REGISTER_TS(DEFENCE_BOOST_R, pSkillLVLData->m_nX);
			//                        break;
			//                    case 51121006:
			//                        REGISTER_TS(DAMAGE_BUFF, pSkillLVLData->m_nX);
			//                        REGISTER_TS(HowlingCritical, pSkillLVLData->m_ncriticaldamageMin.x);
			//                        REGISTER_TS(HowlingCritical, pSkillLVLData->m_ncriticaldamageMax.x);
			//                        break;
		}
		case 2211008:
		{
			//REGISTER_TS(ElementalReset, pSkillLVLData->m_nX);
			break;
		}
		case 3111000:
		case 3121008:
		{
			REGISTER_TS(Concentration, pSkillLVLData->m_nX);
			break;
		}
		case 5110001: // Energy Charge
		{
			REGISTER_TS(EnergyCharged, 0);
			break;
		}
		case 1101004:
		case 1201004:
		case 1301004:
		case 2111005:
		case 2211005:
		case 2311006:
		case 3101002:
		case 3201002:
		case 4101003:
		case 4201002:
		case 5101006:
		case 5201003:
		case 5301002:
		{
			REGISTER_TS(Booster, pSkillLVLData->m_nX * 2);
		}
		break;
		case 5111007:
		case 5211007:
		case 5120012:
		case 5220014:
		{
			//REGISTER_TS(Dice, 0);
			break;
		}
		case 5120011:
		case 5220012:
		{
			//REGISTER_TS(DAMAGE_RATE, (int)ret.info.get(MapleStatInfo.damR); //i think
			break;
		}
		case 5121009:
		{
			REGISTER_TS(Speed, pSkillLVLData->m_nX);
			break;
		}
		case 4321000: //tornado spin uses same buffstats
		{
			REGISTER_TS(Dash_Speed, 100 + pSkillLVLData->m_nX);
			REGISTER_TS(Dash_Jump, pSkillLVLData->m_nY); //always 0 but its there
			break;
		}
		case 5001005: // Dash
		{
			REGISTER_TS(Dash_Speed, pSkillLVLData->m_nX);
			REGISTER_TS(Dash_Jump, pSkillLVLData->m_nY);
			break;
		}
		case 1101007: // pguard
		case 1201007:
		{
			REGISTER_TS(PowerGuard, pSkillLVLData->m_nX);
			break;
		}
		case 1301007: // hyper body
		case 9001008:
		case 9101008:
		{
			REGISTER_TS(MaxHP, pSkillLVLData->m_nX);
			REGISTER_TS(MaxMP, pSkillLVLData->m_nX);
			break;
		}
		case 1111002: // combo
		case 11111001: // combo
		case 1101013:
		{
			REGISTER_TS(ComboCounter, 1);
			//  System.out.println("Combo just buff stat");
			break;
		}
		case 5211006: // Homing Beacon
		case 5220011: // Bullseye
		{
			//REGISTER_TS(StopForceAtomInfo, pSkillLVLData->m_nX);
			break;
		}
		case 1311015: // Cross Surge
		{
			//REGISTER_TS(CROSS_SURGE, pSkillLVLData->m_nX);
			break;
		}
		case 21111009: //combo recharge
		case 1311006: //dragon roar
		case 1311005: //NOT A BUFF - Sacrifice
			//ret.hpR = -ret.info.get(MapleStatInfo.x) / 100.0;
			break;
		case 1211010: //NOT A BUFF - HP Recover
			//ret.hpR = ret.info.get(MapleStatInfo.x) / 100.0;
			break;
		case 4331003:
		{
			//REGISTER_TS(OWL_SPIRIT, pSkillLVLData->m_nY);
			break;
		}
		case 1311008: // dragon blood
					  // REGISTER_TS(DRAGONBLOOD, pSkillLVLData->m_nX);
			break;
		case 5321005:
		case 1121000: // maple warrior, all classes
		case 5721000:
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
						// case 51121005: //Mihile's Maple Warrior
			//REGISTER_INDIE_TS(IndiePAD, pSkillLVLData->m_nX);
			//REGISTER_TS(BasicStatUp, pSkillLVLData->m_nX);
			break;
		case 15111006: //spark
			//REGISTER_TS(SPARK, pSkillLVLData->m_nX);
			break;
		case 3121002: // sharp eyes bow master
		case 3221002: // sharp eyes marksmen
			//REGISTER_TS(SharpEyes, (pSkillLVLData->m_nX << 8) + (pSkillLVLData->m_nCriticaldamageMax));
			break;
		case 2000007:
			//REGISTER_TS(WeaknessMdamage, pSkillLVLData->m_nX);
			break;
		case 1211009:
		case 1111007:
		case 1311007: //magic crash
			//ret.monsterStatus.put(MonsterStatus.MOB_STAT_MagicCrash, 1);
			break;
		case 1220013:
		{
			//REGISTER_TS(BlessingArmor, pSkillLVLData->m_nX + 1);
			break;
		}
		case 1211011: 
		{
			//REGISTER_TS(CombatOrders, pSkillLVLData->m_nX);
			break;
		}
		case 2311009: //holy magic
			//REGISTER_TS(HolyMagicShell, pSkillLVLData->m_nX);
			
			//ret.hpR = ret.info.get(MapleStatInfo.z) / 100.0;
			break;
		case 1121010: //enrage
			//REGISTER_TS(Enrage, pSkillLVLData->m_nX * 100 + pSkillLVLData->m_nMobCount);
			break;
		case 3120012:
		case 3220012:
		case 3111002: // puppet ranger
		case 3211002: // puppet sniper
		case 5211001: // Pirate octopus summon
		case 5220002: // wrath of the octopi
		case 5321003:
		case 5211014:
			//REGISTER_TS(PUPPET, 1);
			break;
		case 3120006:
		case 3220005:
			//REGISTER_TS(TerR, pSkillLVLData->m_nTerR);
			//REGISTER_TS(SpiritLink, 1);
			break;
		case 1321007: // Beholder
		case 1301013: // Evil Eye
		case 1311013: // Evil Eye of Domination
			//REGISTER_TS(Beholder, nSLV);
			break;
		case 2311003: // hs
		case 9001002: // GM hs
		case 9101002:
			//REGISTER_TS(HolySymbol, pSkillLVLData->m_nX);
			break;
		case 4111009: // Shadow Stars
		case 5201008:
			//REGISTER_TS(NoBulletConsume, 0);
			break;
		case 2121004:
		case 2221004:
		case 2321004: // Infinity
			//ret.hpR = ret.info.get(MapleStatInfo.y) / 100.0;
			//ret.mpR = ret.info.get(MapleStatInfo.y) / 100.0;
			REGISTER_TS(Infinity, pSkillLVLData->m_nX);
			REGISTER_TS(Stance, pSkillLVLData->m_nProp);
			break;
		case 1121002:
		case 1221002:
		case 1321002:
		case 5321010:
			REGISTER_TS(Stance, pSkillLVLData->m_nProp);
			break;
		case 2121002: // mana reflection
		case 2221002:
		case 2321002:
			REGISTER_TS(ManaReflection, 1);
			break;
		case 2321005: // holy shield, TODO Jump
					  //                            REGISTER_TS(AdvancedBless, GameConstants.GMS ? (int) ret.level : ret.info.get(MapleStatInfo.x);
			//REGISTER_TS(AdvancedBless, pSkillLVLData->m_nX);
			//REGISTER_TS(IncMaxHP, pSkillLVLData->m_nY);//fix names
			//REGISTER_TS(IncMaxMP, pSkillLVLData->m_nZ);
			break;
		case 3121007: // Hamstring
			//REGISTER_TS(IllusionStep, pSkillLVLData->m_nX);
			//ret.monsterStatus.put(MonsterStatus.SPEED, pSkillLVLData->m_nX);
			break;
		case 3221006: // Blind
			REGISTER_TS(Blind, pSkillLVLData->m_nX);
			//ret.monsterStatus.put(MonsterStatus.ACC, pSkillLVLData->m_nX);
			break;
		case 2301004:
		case 9001003:
			//REGISTER_TS(Bless, nSLV);
			break;
		case 5121015:
			//REGISTER_TS(DamR, pSkillLVLData->m_nX);
			break;
		case 5211009:
			
			break;
		case 4111002:
			REGISTER_TS(ShadowPartner, pSkillLVLData->m_nX);
			break;
		case 4311009:
			REGISTER_TS(Booster, pSkillLVLData->m_nX);
			break;
		case 5221053://epic Aventure corsair
			//REGISTER_TS(IndieDamR, pSkillLVLData->m_nIndieDamR);
			//REGISTER_TS(IndieMaxDamageOver, pSkillLVLData->m_nIndieMaxDamageOver);
			break;
		case 4121054: // bleed dart
			REGISTER_TS(Speed, 1);
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

void USkill::DoActiveSkill_PartyStatChange(User* pUser, const SkillEntry * pSkill, int nSLV, InPacket * iPacket)
{
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
	}
}
