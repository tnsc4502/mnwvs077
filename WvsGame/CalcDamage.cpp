#include "CalcDamage.h"
#include <algorithm>
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "SkillInfo.h"
#include "User.h"
#include "SecondaryStat.h"
#include "BasicStat.h"
#include "MobStat.h"
#include "Mob.h"
#include "ItemInfo.h"
#include "QWUser.h"
#include "MobTemplate.h"
#include "..\Database\GA_Character.hpp"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"

static int ms_annStandardPDD[5][255];

#undef max
#undef min

#define NEXT_RAND nRnd = aRandom[nRndIdx++ % 7]
#define CURRENT_RAND (long double)(nRnd % 10000000) * 0.000000100000010000001

CalcDamage::CalcDamage(User* pUser)
{
	m_pUser = pUser;
	m_pRndGen = AllocObj(Rand32);
}

CalcDamage::~CalcDamage()
{
	FreeObj(m_pRndGen);
}

void CalcDamage::SetSeed(unsigned int uS1, unsigned int uS2, unsigned int uS3)
{
	m_RndGenForCharacter.Seed(uS1, uS2, uS3);
	m_RndForCheckDamageMiss.Seed(uS1, uS2, uS3);
	m_RndGenForMob.Seed(uS1, uS2, uS3);
}

SynchronizedRand32 & CalcDamage::GetRndGenForMob()
{
	return m_RndGenForMob;
}

SynchronizedRand32 & CalcDamage::GetRndGenForForCheckDamageMiss()
{
	return m_RndForCheckDamageMiss;
}

void CalcDamage::LoadStandardPDD()
{
	static WzIterator img = stWzResMan->GetItem("./StandardPDD.img");
	static auto empty = img.end();
	int nJobCategory = 0, nLastValue = 0;
	for (auto& node : img)
	{
		nLastValue = 0;
		nJobCategory = atoi(node.GetName().c_str());
		if (nJobCategory < 0 || nJobCategory > 5)
			continue;
		for (int i = 0; i < 256; ++i)
		{
			auto& v = node[std::to_string(i)];
			if (v != empty)
				nLastValue = v;
			ms_annStandardPDD[nJobCategory][i] = nLastValue;
		}
	}
}

int CalcDamage::GetStandardPDD(int nJobCategory, int nLevel)
{
	if (nJobCategory < 0 || nJobCategory > 5 || nLevel < 0 || nLevel > 255)
		return 0;
	return ms_annStandardPDD[nJobCategory][nLevel];
}

double CalcDamage::GetDamageAdjustedByElemAttr(long double damage, int nAttr, long double dAdjust)
{
	long double result = 0;
	if (!nAttr)
		result = damage;
	if (nAttr == 1)
		return 0.0;
	if (nAttr == 2)
		return (1.0 - dAdjust * 0.5) * damage;
	if (nAttr == 3)
	{
		result = (dAdjust * 0.5 + 1.0) * damage;
		if (damage >= result)
			result = damage;
		if (result >= 99999.0)
			result = 99999.0;
	}
	else
		result = damage;
	return result;
}

double CalcDamage::GetDamageAdjustedByElemAttr(double damage, const SkillEntry *pSkill, int *aDamagedElemAttr, int nSLV)
{
	double result = 0, dAdjust = 0, damagea = 0;
	if (pSkill && nSLV)
	{
		if (pSkill->GetSkillID() == 2111006)
			result = GetDamageAdjustedByElemAttr(damage * 0.5, aDamagedElemAttr[4], 1.0)
				+ GetDamageAdjustedByElemAttr(damage * 0.5, aDamagedElemAttr[2], 1.0);
		else if (pSkill->GetSkillID() == 2111006)
			result = GetDamageAdjustedByElemAttr(damage * 0.5, aDamagedElemAttr[3], 1.0)
			+ GetDamageAdjustedByElemAttr(damage * 0.5, aDamagedElemAttr[1], 1.0);
		else
		{
			if (pSkill->GetSkillID() == 3111003 || pSkill->GetSkillID() == 3211003)
				dAdjust = (double)pSkill->GetLevelData(nSLV)->m_nX  * 0.01;
			else
				dAdjust = 1.0;
			result = GetDamageAdjustedByElemAttr(damage, aDamagedElemAttr[pSkill->GetAttackElemAttr()], dAdjust);
		}
	}
	else
		return damage;
	return result;
}

double CalcDamage::GetDamageAdjustedByChargedElemAttr(double damage, int *aDamagedElemAttr, User *pUser)
{
	auto &ss = pUser->GetSecondaryStat();
	int nSkillID = ss->rWeaponCharge_;

	if (!ss->nWeaponCharge_)
		return damage;
	if (nSkillID > 1211008)
	{
		if (nSkillID < 1221003)
			return damage;

		if (nSkillID > 1221004)
			return damage;
	}

	if (nSkillID < 1211007)
	{
		if (nSkillID < 1211003)
			return damage;

		if (nSkillID > 1211006)
			return damage;
	}

	SkillEntry* pEntry = nullptr;
	int nSLV = SkillInfo::GetInstance()->GetSkillLevel(
		pUser->GetCharacterData(),
		ss->rWeaponCharge_,
		&pEntry
	);
	if (nSLV)
	{
		double dAdjust = (double)pEntry->GetLevelData(nSLV)->m_nZ * 0.01;
		int nElemAttr = 0;
		if (nSkillID == 1221003 || nSkillID == 1221004)
			nElemAttr = 5;
		else if (nSkillID >= 1211007)
			nElemAttr = 3;
		else if (nSkillID <= 1211004)
			nElemAttr = 2;
		else
			nElemAttr = 1;
		return GetDamageAdjustedByElemAttr(
			(double)pEntry->GetLevelData(nSLV)->m_nDamage * 0.01 * damage,
			aDamagedElemAttr[nElemAttr],
			dAdjust
		);
	}
	return damage;
}

int CalcDamage::GetComboDamageParam(User * pUser, int nSkillID, int nComboCounter)
{
	int result = 0;
	SkillEntry *pEntry1 = nullptr, *pEntry2 = nullptr;
	const SkillLevelData *pLevel = nullptr;
	int nSLV1 = SkillInfo::GetInstance()->GetSkillLevel(pUser->GetCharacterData(), 1111002, &pEntry1);
	int nSLV2 = SkillInfo::GetInstance()->GetSkillLevel(pUser->GetCharacterData(), 1120003, &pEntry2);

	if (!nSLV1)
		return 0;

	if (nSLV1 != 30 || nSLV2 <= 0)
		pLevel = pEntry1->GetLevelData(nSLV1);
	else
		pLevel = pEntry2->GetLevelData(nSLV2);

	if (nSkillID != 1111006 && nSkillID != 1111005 && nSkillID != 1111004 && nSkillID != 1111003)
	{
		if (nComboCounter >= 0)
		{
			switch (nComboCounter)
			{
			case 1:
				result = pLevel->m_nDamage;
				break;
			case 2:
				return pLevel->m_nDamage + (5 * nSLV1 - 5) / 29;
			case 3:
				return pLevel->m_nDamage + (10 * nSLV1 - 10) / 29;
			case 4:
				return pLevel->m_nDamage + (15 * nSLV1 - 15) / 29;
			case 5:
				return pLevel->m_nDamage + (20 * nSLV1 - 20) / 29;
			case 6:
				return pLevel->m_nDamage + 24;
			case 7:
				return pLevel->m_nDamage + 28;
			case 8:
				return pLevel->m_nDamage + 32;
			case 9:
				return pLevel->m_nDamage + 36;
			case 10:
				return pLevel->m_nDamage + 40;
			default:
				return 100;
			}
			return 100;
		}
		return 0;
	}
	switch (nComboCounter)
	{
	case 1:
		result = pLevel->m_nDamage;
		break;
	case 2:
		result = (24 * nSLV1 - 24) / 29 + pLevel->m_nDamage + 6;
		break;
	case 3:
		result = ((nSLV1 << 6) - 64) / 29 + pLevel->m_nDamage + 16;
		break;
	case 4:
		result = (120 * nSLV1 - 120) / 29 + pLevel->m_nDamage + 30;
		break;
	case 5:
		result = (184 * nSLV1 - 184) / 29 + pLevel->m_nDamage + 46;
		break;
	case 6:
		result = pLevel->m_nDamage + 250;
		break;
	case 7:
		result = pLevel->m_nDamage + 270;
		break;
	case 8:
		result = pLevel->m_nDamage + 290;
		break;
	case 9:
		result = pLevel->m_nDamage + 305;
		break;
	case 10:
		result = pLevel->m_nDamage + 320;
		break;
	default:
		return 100;
	}
	return result;
}

int CalcDamage::GetMesoGuardReduce(User *pUser, double damage)
{
	if (damage <= 1.0)
		damage = 1.0;
	if (damage >= 99999.0)
		damage = 99999.0;
	SkillEntry *pEntry = nullptr;
	int nSLV = SkillInfo::GetInstance()->GetSkillLevel(
		pUser->GetCharacterData(),
		4211005,
		&pEntry
	);
	if (pEntry && nSLV)
	{
		int nX = pEntry->GetLevelData(nSLV)->m_nX;
		if (((int)damage / 2) * nX / 100 > QWUser::GetMoney(pUser))
			return (int)(100 * QWUser::GetMoney(pUser) / nX);

		return (int)(damage / 2);
	}
	return (int)damage;
}

void CalcDamage::DecInvalidCount()
{
	if (m_nInvalidCount >= -5)
	--m_nInvalidCount;
}

void CalcDamage::IncInvalidCount()
{
	++m_nInvalidCount;
	if (m_nInvalidCount >= 20) 
	{
		m_nInvalidCount = 0;
		m_pUser->SendChatMessage(0, "You have over 10 times of invalid attacking!");
	}
}

int CalcDamage::GetInvalidCount() const
{
	return m_nInvalidCount;
}

bool CalcDamage::CheckMDamageMiss(MobStat *ms, unsigned int nRnd)
{
	auto &ss = m_pUser->GetSecondaryStat();
	auto &bs = m_pUser->GetBasicStat();
	int nACC = std::min(999, std::max(0, ss->nEVA + ss->nEVA_)),
		nMobACC = 0;
	if (bs->nLevel >= ms->nLevel || (nACC -= (ms->nLevel - bs->nLevel), nACC > 0))
		nMobACC = nACC;
	
	double accLow = (double)(nMobACC) * 0.1;
	double calc = accLow + ((double)nMobACC - accLow) * CURRENT_RAND;
	nMobACC = std::min(999, std::max(0, ms->nACC + ms->nACC_));
	return calc >= (double)nMobACC;
}

int CalcDamage::MDamage(MobStat *ms, void *pMobAttackInfo_, unsigned int nRandForMissCheck, int *pnReduce, int *pnRand)
{
	auto &ss = m_pUser->GetSecondaryStat();
	auto &bs = m_pUser->GetBasicStat();
	unsigned int nRnd = (unsigned int)m_RndGenForMob.Random();
	if (pnRand)
		*pnRand = (int)nRnd;
	int nMobMDD = std::min(1999, std::max(0, ms->nMAD + ms->nMAD_));
	double damage = (double)nMobMDD * 0.75,
		highDamage = (double)nMobMDD * 0.8;

	double calc = damage + (highDamage - damage) * CURRENT_RAND;
	calc *= (double)nMobMDD * 0.01;
	nMobMDD = std::min(1999, std::max(0, ss->nMDD + ss->nMDD_));
	damage = (double)bs->nSTR * 0.14285714285714
		+ (double)bs->nLUK * 0.2
		+ (double)bs->nDEX * 0.1666666666667
		+ nMobMDD;

	if (bs->nJob / 100 == 2)
		damage *= 0.3;
	else
		damage *= 0.25;

	damage = calc - damage;
	if (ss->nMesoGuard_ && pnReduce)
	{
		*pnReduce = GetMesoGuardReduce(m_pUser, damage);
		damage = (double)*pnReduce;
	}

	if (ms->nMagicUp_)
		damage *= (double)ms->nMagicUp_ * 0.01;
	if (damage <= 1.0)
		damage = 1.0;
	if (damage >= 99999.0)
		damage = 99999.0;

	return (int)damage;
}

void CalcDamage::MDamage(Mob *pMob, MobStat *ms, int nDamagePerMob, int nWeaponItemID, int nAction, const SkillEntry * pSkill, int nSLV, int *aDamage, bool *abCritical, int nMobCount, int tKeyDown)
{
	unsigned int aRandom[7], nRndIdx = 0, nRnd = 0;
	for (auto& nRnd : aRandom)
		nRnd = (unsigned int)m_RndGenForCharacter.Random();

	int nSkillID = pSkill ? pSkill->GetSkillID() : 0, 
		nCriticalAttackProp = 0, 
		nCriticalAttackParam = 0;
	auto &ss = m_pUser->GetSecondaryStat();
	auto &bs = m_pUser->GetBasicStat();
	auto &cd = m_pUser->GetCharacterData();

	int nMAD = std::min(1999, std::max(0, ss->nMAD + ss->nMAD_)), nMobPDD = 0;
	if (ss->nMaxLevelBuff_)
		nMAD = std::min(1999, nMAD + nMAD * ss->nMaxLevelBuff_ / 100);

	int nWT = ItemInfo::GetWeaponType(nWeaponItemID),
		nAmp = 5 * (bs->nINT / 10 + bs->nLUK / 10);

	double s = (double)nAmp * 100 / ((double)std::max(0, ms->nLevel - bs->nLevel) * 10.0 + 255.0),
		M = ((double)pSkill->GetLevelData(nSLV)->m_nMastery  * 5.0 + 10.0) * 0.009,
		damage = 0,
		calc = 0,
		highDamage = 0;

	nAmp = SkillInfo::GetInstance()->GetAmplification(cd, bs->nJob, 0, nullptr);
	//SharpEyes
	int nSharpEyes = ss->nSharpEyes_;
	nCriticalAttackProp += std::min(100, std::max(0, nSharpEyes >> 8));
	if (nSharpEyes)
		if (nCriticalAttackParam)
			nCriticalAttackParam += nSharpEyes;
		else
			nCriticalAttackParam = nSharpEyes + 100;

	for (int i = 0; i < nDamagePerMob; ++i)
	{
		aDamage[i] = 0;
		if (ms->bInvincible)
			continue;

		NEXT_RAND;
		if (ms->nMImmune_ && ss->nRespectMImmune_ < (int)(nRnd % 100))
		{
			aDamage[i] = 1;
			continue;
		}

		nMobPDD = std::min(999, std::max(0, ms->nEVA + ms->nEVA_));
		if (pSkill && pSkill->GetLevelData(nSLV)->m_nFixDamage == 0)
		{
			NEXT_RAND;
			damage = s * 0.5;
			highDamage = s * 1.2;

			if (damage + (highDamage - damage) * CURRENT_RAND < nMobPDD)
				continue;

			if (nSkillID == 2301002)
			{
				damage = (double)bs->nINT * 0.2;
				highDamage = (double)bs->nINT * 0.8;

				NEXT_RAND;
				damage += (highDamage - damage) * CURRENT_RAND;
				calc = ((double)pSkill->GetLevelData(nSLV)->m_nHp * 0.01)
					* (damage * 1.5 + (double)bs->nLUK)
					* ((double)nMobCount * 0.3 + 1.0)
					* (double)nMAD
					* 0.005
					/ (double)nMobCount;
			}
			else
			{
				damage = nMAD;
				highDamage = (double)nMAD * M;
				if (damage > highDamage)
					std::swap(damage, highDamage);

				NEXT_RAND;
				damage += (highDamage - damage) * CURRENT_RAND;
				calc = (double)pSkill->GetLevelData(nSLV)->m_nMad
					* ((double)bs->nINT * 0.5 + std::pow((double)nMAD * 0.058, 2) + damage * 3.3)
					* 0.01;
			}

			damage = GetDamageAdjustedByElemAttr(
				(double)nAmp * calc * 0.01,
				pSkill,
				ms->aDamagedElemAttr,
				nSLV
			);

			nMobPDD = std::min(1999, std::max(0, ms->nMDD + ms->nMDD_));
			highDamage = (double)nMobPDD * 0.6;
			calc = (double)nMobPDD * 0.5;
			NEXT_RAND;
			damage -= (calc + (highDamage - calc) * CURRENT_RAND);
			if (ms->nMGuardUp_)
				damage *= (double)ms->nMGuardUp_ * 0.01;

			//Critical Damage
			if (nCriticalAttackParam > 0
				&& nCriticalAttackProp > 0
				&& (NEXT_RAND, (double)(nRnd % 10000000) * 0.00001 < nCriticalAttackProp))
			{
				abCritical[i] = true;
				damage *= (((double)nCriticalAttackParam - 100.0) * 0.01 + 1.0);
			}

			if (tKeyDown)
				damage *= (90.0 * (double)tKeyDown / 2000.0 + 10.0) * 0.01;

			if (!ms->nHardSkin_ || abCritical[i])
			{
				damage = std::max(1.0, std::min(99999.0, damage));
				aDamage[i] = (int)damage;
			}
		}
		else
		{
			//Calc FixDamage
			int nLevelDiff = ms->nLevel - bs->nLevel;
			if (nLevelDiff > 0)
			{
				calc = (((double)bs->nLevel * 100.0) / ((double)(150 * nLevelDiff) + 255.0));
				damage = calc * 0.7;
				highDamage = calc * 1.3;
				NEXT_RAND;
				if ((damage + (highDamage - damage) * CURRENT_RAND >= nMobPDD))
					aDamage[i] = pSkill->GetLevelData(nSLV)->m_nFixDamage;
			}
		}
	}
}

int CalcDamage::MDamage(MobStat * ms, const SkillEntry * pSkill, int nSLV)
{
	int nMAD = pSkill ? pSkill->GetLevelData(nSLV)->m_nMad : 0;
	auto &ss = m_pUser->GetSecondaryStat();
	auto &bs = m_pUser->GetBasicStat();
	unsigned int nRnd = (unsigned int)m_RndGenForMob.Random();
	double MS = 5 * (bs->nINT / 10 + bs->nLUK / 10);
	int nLevelDiff = std::max(0, ms->nLevel - bs->nLevel);
	MS *= 100.0 / ((double)nLevelDiff * 10.0 + 255.0);
	int nMobEVA = std::min(999, ms->nEVA + ms->nEVA_);
	double dCalcMin = MS * 0.5, dCalcMax = MS * 1.2;
	dCalcMin += (dCalcMax - dCalcMin) * ((double)(nRnd % 10000000) * 0.0000001);

	if (dCalcMin >= nMobEVA)
	{
		double dUserMAD = std::min(1999, ss->nMAD + ss->nMAD_);
		auto pLevel = pSkill->GetLevelData(nSLV);
		MS = ((double)pLevel->m_nMastery * 5.0 + 10.0) * 0.009;
		if (!ms->bInvincible)
		{
			nRnd = (unsigned int)m_RndGenForMob.Random();
			if (ms->nMImmune_)
				return 1;
			MS *= (double)dUserMAD;
			if (dUserMAD > MS)
				MS += (dUserMAD - MS) * ((double)(nRnd % 10000000) * 0.0000001);

			return (int)GetDamageAdjustedByElemAttr(
				(double)bs->nINT * 0.6 + std::pow((double)dUserMAD * 0.058, 2) + (double)MS * 3.3 * ((double)nMAD) * 0.015,
				pSkill,
				ms->aDamagedElemAttr,
				nSLV
			);
		}
	}
	return 0;
}

int CalcDamage::MDamageSummoned(MobStat *ms, int nSLV)
{
	auto &ss = m_pUser->GetSecondaryStat();
	auto &bs = m_pUser->GetBasicStat();
	unsigned int nRnd = (unsigned int)m_pRndGen->Random();
	int nMobMAD = ms->nPAD + ms->nPAD_;
	double dCalcMin = (double)nMobMAD * 0.1;
	double dCalcMax = (double)nMobMAD * 0.85;
	int nUserMAD = std::min(999, std::max(0, ss->nMAD + ss->nMAD_));

	dCalcMin += ((double)(nRnd % 10000000) * 0.0000001) * (dCalcMax - dCalcMin);
	return (int)(40.0 / ((double)nSLV + 70) * (double)nMobMAD * dCalcMin * 0.01);
}

bool CalcDamage::CheckPDamageMiss(MobStat *ms, unsigned int nRnd)
{
	auto &ss = m_pUser->GetSecondaryStat();
	auto &bs = m_pUser->GetBasicStat();
	int nEVA = std::min(999, std::max(0, ss->nEVA + ss->nEVA_)),
		nMobEVA = 0;
	if (bs->nLevel >= ms->nLevel || (nEVA -= (ms->nLevel - bs->nLevel) / 2, nEVA > 0))
		nMobEVA = nEVA;

	int nMobACC = std::min(999, std::max(0, ms->nACC + ms->nACC_));
	double calc = (double)nMobEVA / ((double)nMobACC * 4.5) * 100.0;
	if (bs->nJob / 100 == 4)
		calc = std::max(5.0, std::min(95.0, calc));
	else
		calc = std::max(2.0, std::min(80.0, calc));

	return calc > CURRENT_RAND;
}

int CalcDamage::PDamage(MobStat *ms, void *pMobAttackInfo_, unsigned int nRandForMissCheck, int *pnReduce, int *pnRand)
{
	auto &ss = m_pUser->GetSecondaryStat();
	auto &bs = m_pUser->GetBasicStat();
	unsigned int nRnd = (unsigned int)m_RndGenForMob.Random();
	if (pnRand)
		*pnRand = (int)nRnd;
	int nMobPDD = std::min(
		1999, std::max(0, 
			(pMobAttackInfo_ ? ((MobTemplate::MobAttackInfo*)pMobAttackInfo_)->nPAD : ms->nPAD) + ms->nPAD_
		)
	);
	double damage = (double)nMobPDD * 0.8,
		highDamage = (double)nMobPDD * 0.85,
		calc1 = 0, 
		calc2 = 0;

	double calc = damage + (highDamage - damage) * CURRENT_RAND;
	calc *= (double)nMobPDD * 0.01;

	nMobPDD = std::min(1999, std::max(0, ss->nPDD + ss->nPDD_));
	int nJobCategory = bs->nJob / 100;
	int nStandardPDD = GetStandardPDD(nJobCategory, bs->nLevel),
		nBase = 0;

	if (nJobCategory == 1)
		nBase = (int)(((double)bs->nLUK + (double)bs->nDEX) * 0.25 
		+ (double)bs->nINT * 0.1111111111111111
		+ (double)bs->nSTR * 0.2857142857142857);
	else
		nBase = (int)((double)bs->nINT * 0.1111111111111111
		+ (double)bs->nDEX * 0.2857142857142857 
		+ (double)bs->nSTR * 0.4
		+ (double)bs->nLUK * 0.25);

	if (nStandardPDD > nMobPDD)
	{
		calc1 = (double)bs->nLevel * 0.001818181818181818 + ((double)nBase * 0.00125) + 0.28;
		if (bs->nLevel >= ms->nLevel)
			calc2 = calc1 * (double)(nMobPDD - nStandardPDD) * 13.0 / ((double)(bs->nLevel - ms->nLevel) + 13.0);
		else
			calc2 = calc1 * (double)(nMobPDD - nStandardPDD) * 1.3;
	}
	else
		calc2 = ((double)nBase * 0.0011111111111111 + (double)bs->nLevel * 0.0007692307692307692 + 0.28) * (nMobPDD - nStandardPDD) * 0.7;
	calc1 = ((double)nBase * 0.00125);
	damage = calc
		- (calc2 + (calc1 + 0.28) * (double)nMobPDD)
		- (double)ss->nInvincible_ * (calc - (calc2 + (calc1 + 0.28) * (double)nMobPDD)) * 0.01;

	if (ss->nMesoGuard_ && pnReduce)
	{
		*pnReduce = GetMesoGuardReduce(m_pUser, damage);
		damage = (double)*pnReduce;
	}

	if (ms->nPowerUp_)
		damage *= (double)ms->nPowerUp_ * 0.01;
	if (damage <= 1.0)
		damage = 1.0;
	if (damage >= 99999.0)
		damage = 99999.0;

	return (int)damage;
}

void CalcDamage::PDamage(Mob *pMob, MobStat* ms, int nDamagePerMob, int nWeaponItemID, int nBulletItemID, int nAttackType, int nAction, bool bShadowPartner, const SkillEntry* pSkill, int nSLV, int *aDamage, bool *abCritical, int tKeyDown, int nBerserkDamage, int nAdvancedChargeDamage)
{
	unsigned int aRandom[7], nRndIdx = 0, nRnd = 0;
	int nSkillID = pSkill ? pSkill->GetSkillID() : 0, 
		nCriticalAttackProp = 0, 
		nCriticalAttackParam = 0;
	auto &ss = m_pUser->GetSecondaryStat();
	auto &bs = m_pUser->GetBasicStat();
	auto &cd = m_pUser->GetCharacterData();

	SkillEntry *pSPSkill;
	int nSPSLV = SkillInfo::GetInstance()->GetSkillLevel(
		cd,
		4111002,
		&pSPSkill
	);
	for (auto& nRnd : aRandom)
		nRnd = (unsigned int)m_RndGenForCharacter.Random();
	int nWT = ItemInfo::GetWeaponType(nWeaponItemID);

	//Basic PDD
	int nMobPDD = std::min(999, std::max(0, ss->nACC + ss->nACC_));
	long double a = (long double)nMobPDD * 100.0 / ((long double)std::max(0, pMob->GetMobTemplate()->m_nLevel - bs->nLevel) * 10.0 + 255.0),
		highDamage = 0,
		calc = 0,
		damage = 0;

	//Basic PAD
	int nBulletPAD = nBulletItemID ? ItemInfo::GetInstance()->GetBulletPAD(nBulletItemID) : 0;
	int nPAD = std::min(1999, std::max(0, ss->nPAD + ss->nPAD_ + nBulletPAD));
	if (ss->nMaxLevelBuff_)
		nPAD = std::min(1999, nPAD + nPAD / ss->nMaxLevelBuff_ * 100);

	//Critical Skill
	int nCSLV = ItemInfo::GetWeaponMastery(cd, nWeaponItemID, nSkillID, nAttackType, nullptr, nullptr);
	long double M = ((long double)nCSLV * 5.0 + 10.0) * 0.009000000000000001;

	if (nAction == 59)
	{
		nCSLV = nSLV;
		nCriticalAttackProp = pSkill->GetLevelData(nSLV)->m_nProp;
		nCriticalAttackParam = pSkill->GetLevelData(nSLV)->m_nCriticalDamage;
	}
	else
		nCSLV = ItemInfo::GetCriticalSkillLevel(cd, nWeaponItemID, nAttackType, &nCriticalAttackProp, &nCriticalAttackParam);

	//SharpEyes
	int nSharpEyes = ss->nSharpEyes_;
	nCriticalAttackProp += std::min(100, std::max(0, nSharpEyes >> 8));
	if (nSharpEyes)
		if (nCriticalAttackParam)
			nCriticalAttackParam += (nSharpEyes & 0xFF);
		else
			nCriticalAttackParam = (nSharpEyes & 0xFF) + 100;

	int nNinjaStormProp = (nSkillID == 4121008 ? pSkill->GetLevelData(nSLV)->m_nProp : 0);
	bool bHighDamageAction = false;

	for (int i = 0; i < nDamagePerMob; ++i)
	{
		bHighDamageAction = false;

		aDamage[i] = 0;
		if (ms->bInvincible)
			continue;

		NEXT_RAND;
		if (nSkillID == 1221011 || (ms->nPImmune_ && (int)(nRnd % 100) > ss->nRespectPImmune_))
		{
			aDamage[i] = 1;
			continue;
		}

		if (nSkillID == 3221007 && nAttackType == 1)
		{
			highDamage = 5000.0;
			damage = 0.0;
			NEXT_RAND;
			aDamage[i] = (int)std::round(99999.0 - ((highDamage - damage) * CURRENT_RAND + damage));
			continue;
		}
		else if (nSkillID == 4121008)
		{
			highDamage = 100.0;
			damage = 0.0;
			NEXT_RAND;
			if ((highDamage - damage) * CURRENT_RAND + damage >= nNinjaStormProp)
			{
				aDamage[i] = 0;
				continue;
			}
		}

		nMobPDD = std::min(999, std::max(0, ms->nEVA + ms->nEVA_));
		if (!pSkill || pSkill->GetLevelData(nSLV)->m_nFixDamage == 0)
		{
			damage = a * 0.7;
			highDamage = a * 1.3;
			NEXT_RAND;
			if (damage + (highDamage - damage) * CURRENT_RAND < nMobPDD)
				continue;

			if (ss->nDarkness_)
			{
				highDamage = 100.0;
				damage = 0.0;
				NEXT_RAND;
				if (damage + (highDamage - damage) * CURRENT_RAND > 20.0)
					continue;
			}

			if ((nSkillID == 1121006) ||
				(nSkillID == 1221007) ||
				(nSkillID == 1321003) ||
				(nSkillID == 1221009) ||
				(nSkillID == 1121008))
			{
				highDamage = 5.0;
				damage = 0.0;
				NEXT_RAND;
				if (damage + (highDamage - damage) * CURRENT_RAND < 3.0 ||
					(nSkillID == 1121008 && i == 1))
					bHighDamageAction = true;
			}

			auto IsShootAction = [&](int nAction) {
				return nAction >= 22 && nAction <= 27
					|| nAction == 55
					|| nAction == 92
					|| nAction == 87
					|| nAction == 76
					|| nAction == 85
					|| nAction == 86
					|| nAction == 98
					|| nAction == 91
					|| nAction == 99;
			};

			if ((nWT == 45 || nWT == 46) && !IsShootAction(nAction))
			{
				damage = bs->nDEX;
				highDamage = damage * M;
				if (highDamage < damage)
					std::swap(damage, highDamage);

				NEXT_RAND;
				calc = ((long double)bs->nSTR + (highDamage - damage) * CURRENT_RAND + damage)
					* ((nSkillID != 3201003 && nSkillID != 3101003) ? 1.0 : 3.4)
					* (long double)nPAD * 0.005;
			}
			else if (nWT == 47
				&& !IsShootAction(nAction)
				&& nSkillID != 4221001
				&& nSkillID != 4121008)
			{
				NEXT_RAND;
				damage = bs->nLUK;
				highDamage = damage * M;
				if (highDamage < damage)
					std::swap(highDamage, damage);
				calc = (highDamage - damage) * CURRENT_RAND + damage;
				calc = ((long double)(bs->nSTR + bs->nDEX) + calc) * (long double)nPAD * 0.006666666666666667;
			} 
			else if (nWT == 49
				&& (nAction < 22 || nAction > 27)
				&& (nAction != 94 && nAction != 95)
				&& nSkillID != 5201002
				&& nSkillID != 5221003
				&& nSkillID != 5221007
				&& nSkillID != 5221008)
			{
				damage = bs->nDEX;
				highDamage = damage * M;
				if (highDamage < damage)
					std::swap(damage, highDamage);
				NEXT_RAND;
				if(nAction != 77 && nAction != 78)
					calc = (((highDamage - damage) * CURRENT_RAND + damage) + bs->nSTR) * (long double)nPAD * 0.005;
				else
					calc = (((highDamage - damage) * CURRENT_RAND + damage) * 1.8 + bs->nSTR) * (long double)nPAD * 0.01;
			}
			else if (nAction == 32)
			{
				damage = bs->nSTR;
				highDamage = damage * M;
				if (highDamage < damage)
					std::swap(damage, highDamage);
				NEXT_RAND;
				calc = (((highDamage - damage) * CURRENT_RAND + damage) + bs->nDEX) * (long double)nPAD * 0.05;
			}
			else
			{
				bHighDamageAction = bHighDamageAction || (nAction >= 5 && nAction <= 15);
				NEXT_RAND;
				switch (nWT)
				{
					case 45:
						damage = bs->nDEX;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * 3.4;
						calc = (damage + bs->nSTR) * (long double)nPAD * 0.01;
						break;
					case 46:
						damage = bs->nDEX;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * 3.6;
						calc = (damage + bs->nSTR) * (long double)nPAD * 0.01;
						break;
					case 41:
					case 42:
						damage = bs->nSTR;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * (bHighDamageAction ? 3.4 : 4.8);
						calc = (damage + bs->nDEX) * (long double)nPAD * 0.01;
						break;
					case 43:
					case 44:
						damage = bs->nSTR;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * (nSkillID == 1311006 ? 4.0 : (bHighDamageAction != (nWT == 43)) ? 5.0 : 3.0);
						calc = (damage + bs->nDEX) * (long double)nPAD * 0.01;
						break;
					case 40:
						damage = bs->nSTR;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * 4.6;
						calc = (damage + bs->nDEX) * (long double)nPAD * 0.01;
						break;
					case 31:
					case 32:
					case 37:
					case 38:
						damage = bs->nSTR;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * (bHighDamageAction ? 4.4 : 3.2);
						calc = (damage + bs->nDEX) * (long double)nPAD * 0.01;
						break;
					case 30:
					case 33:
						if (bs->nJob / 100 == 4 && nWT == 33)
						{
							damage = bs->nLUK;
							highDamage = damage * M;
							if (damage > highDamage)
								std::swap(damage, highDamage);
							damage = ((highDamage - damage) * CURRENT_RAND + damage) * 3.6;
							calc = (long double)(damage + bs->nDEX + bs->nSTR) * (long double)nPAD * 0.01;
						}
						else
						{
							damage = bs->nSTR;
							highDamage = damage * M;
							if (damage > highDamage)
								std::swap(damage, highDamage);
							damage = ((highDamage - damage) * CURRENT_RAND + damage) * 4.0;
							calc = (damage + bs->nDEX) * (long double)nPAD * 0.01;
						}
						break;
					case 47:
						if (nSkillID != 4001344 && nSkillID != 4121007)
						{
							if (nSkillID != 4111004)
							{
								damage = bs->nLUK;
								highDamage = damage * M;
								if (damage > highDamage)
									std::swap(damage, highDamage);
								damage = ((highDamage - damage) * CURRENT_RAND + damage) * 3.6;
								calc = (damage + bs->nSTR + bs->nDEX) * (double)nPAD * 0.01;
							}
							else
							{
								int nMoneyCon = pSkill->GetLevelData(nSLV)->m_nMoneyCon;
								damage = nMoneyCon * 0.5;
								highDamage = nMoneyCon * 1.5;
								calc = (damage + (highDamage - damage) * CURRENT_RAND) * 10;
								NEXT_RAND;
								if (CURRENT_RAND < pSkill->GetLevelData(nSLV)->m_nProp)
								{
									calc *= (long double)(pSkill->GetLevelData(nSLV)->m_nX + 100) * 0.01;
									abCritical[i] = 1;
								}
							}
						}
						else
						{
							damage = bs->nLUK;
							highDamage = damage * 0.5;
							if (damage > highDamage)
								std::swap(damage, highDamage);
							damage = ((highDamage - damage) * CURRENT_RAND + damage) * 5.0;
							calc = (damage) * (long double)nPAD * 0.01;
						}
						break;
					case 39:
						damage = bs->nSTR;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);

						damage = ((highDamage - damage) * CURRENT_RAND + damage) * (bs->nJob == 500 ? 3.0: 4.2);
						calc = (damage + bs->nDEX) * (long double)nPAD * 0.01;
						break;
					case 48:
						damage = bs->nSTR;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * 4.8;
						calc = (damage + bs->nDEX) * (long double)nPAD * 0.01;
						break;
					case 49:
						damage = bs->nDEX;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * 3.6;
						calc = (damage + bs->nSTR) * (long double)nPAD * 0.01;
						if (nSkillID == 5211004 && nBulletItemID / 1000 != 2331)
							calc *= 0.5;
						if (nSkillID == 5211005 && nBulletItemID / 1000 != 2332)
							calc *= 0.5;
						break;
				}

			}

			if (bs->nLevel < ms->nLevel)
				calc = (100.0 - (long double)(ms->nLevel - bs->nLevel)) * calc * 0.01;

			//Attack Elem Attribute
			damage = GetDamageAdjustedByChargedElemAttr(
				GetDamageAdjustedByElemAttr(calc, pSkill, ms->aDamagedElemAttr, nSLV),
				ms->aDamagedElemAttr,
				m_pUser
			);

			if (nSkillID != 1311005 && nSkillID != 4111004 && nSkillID != 4211002 && nSkillID != 5121004)
			{
				nMobPDD = std::min(1999, std::max(0, ms->nPDD + ms->nPDD_));
				highDamage = (long double)nMobPDD * 0.6;
				calc = (long double)nMobPDD * 0.5;
				NEXT_RAND;
				damage -= (highDamage - calc) * CURRENT_RAND + calc;
			}

			//Basic Damage
			highDamage = damage;
			int nDamage = pSkill ? pSkill->GetLevelData(nSLV)->m_nDamage : 0;
			int nComboParam = GetComboDamageParam(m_pUser, nSkillID, ss->nComboCounter_ - 1);

			/*
			Pirate stuffs here
			*/
			if (nSkillID == 5211004 || nSkillID == 5211005)
			{
				SkillEntry *pEntry = nullptr;
				int nPirateSkillLV = SkillInfo::GetInstance()->GetSkillLevel(
					cd, 5220001, &pEntry
				);
				if (nPirateSkillLV && pEntry)
					nDamage += pEntry->GetLevelData(nPirateSkillLV)->m_nDamage;
			}

			if (nAdvancedChargeDamage && nSkillID == 1211002)
				damage *= (long double)nAdvancedChargeDamage * 0.01;

			if (nDamage > 0)
				damage *= (long double)nDamage * 0.01;

			if (nComboParam)
				damage *= (long double)nComboParam * 0.01;

			//Critical Damage
			if ((nSkillID != 3211003 && nSkillID != 4111004 && nSkillID != 4221001 || nAction == 59)
				&& nCriticalAttackParam > 0
				&& nCriticalAttackProp > 0
				&& (nCSLV > 0 || ss->nSharpEyes_)
				&& (NEXT_RAND, (long double)(nRnd % 10000000) * 0.00001 < nCriticalAttackProp))
			{
				abCritical[i] = true;
				damage += ((long double)nCriticalAttackParam - 100.0) * 0.01 * (long double)(int)highDamage;
			}

			if (nSkillID != 1311005 && ms->nPGuardUp_)
				damage *= (long double)ms->nPGuardUp_ * 0.01;

			//Shadow Partner
			if (bShadowPartner && nSkillID != 4121003)
			{
				int nRealPartIdx = i - (nDamagePerMob / 2);
				if (i >= nDamagePerMob / 2)
				{
					if (pSkill)
						highDamage = aDamage[nRealPartIdx] * (long double)pSPSkill->GetLevelData(nSPSLV)->m_nY / 100.0;
					else
						highDamage = aDamage[nRealPartIdx] * (long double)pSPSkill->GetLevelData(nSPSLV)->m_nX / 100.0;
					damage = highDamage;
					abCritical[i] = abCritical[nRealPartIdx];
				}
			}
			if (nSkillID == 4221001)
			{
				if ((ss->nDarkSight_))
					damage *= (long double)(pSkill->GetLevelData(nSLV)->m_nTime) / 3.0 + 1.0;
				else
					damage = 0.0;
			}
			if (tKeyDown)
				if(nSkillID == 5101004)
					damage *= (90.0 * (long double)tKeyDown / 1000.0 + 10.0) * 0.01;
				else
					damage *= (90.0 * (long double)tKeyDown / 2000.0 + 10.0) * 0.01;

			if (!ms->nHardSkin_ || abCritical[i])
			{
				if (nBerserkDamage)
					damage *= (long double)(nBerserkDamage + 100.0) * 0.01;

				if (nSkillID == 5121007)
					damage *= (i == 4 ? 2.0 : (i == 5 ? 4.0 : 1.0));

				damage = std::max((long double)1.0, std::min((long double)99999.0, damage));
				aDamage[i] = (int)(damage);
			}
		}
		else
		{
			//Calc FixDamage
			int nLevelDiff = ms->nLevel - bs->nLevel;
			if (nLevelDiff > 0)
			{
				calc = (((long double)bs->nLevel * 100.0) / ((long double)(150 * nLevelDiff) + 255.0));
				damage = calc * 0.7;
				highDamage = calc * 1.3;
				NEXT_RAND;
				if ((damage + (highDamage - damage) * CURRENT_RAND >= nMobPDD))
					aDamage[i] = pSkill->GetLevelData(nSLV)->m_nFixDamage;
			}
		}
	}
}

int CalcDamage::PDamage(MobStat * ms, const SkillEntry * pSkill, int nSLV)
{
	int nPAD = pSkill ? pSkill->GetLevelData(nSLV)->m_nPad : 0;
	auto &ss = m_pUser->GetSecondaryStat();
	auto &bs = m_pUser->GetBasicStat();
	unsigned int nRnd = (unsigned int)m_RndGenForMob.Random();

	int nUserACC = std::min(999, ms->nACC + ms->nACC_);
	int nLevelDiff = std::max(0, ms->nLevel - bs->nLevel);
	double MS = (double)nUserACC * 100.0 / ((double)nLevelDiff * 10.0 + 255.0);
	int nMobEVA = std::min(999, ms->nEVA + ms->nEVA_);
	double dCalcMin = MS * 0.7, dCalcMax = MS * 1.3;
	dCalcMin += (dCalcMax - dCalcMin) * ((double)(nRnd % 10000000) * 0.0000001);

	if (dCalcMin >= nMobEVA)
	{
		auto pLevel = pSkill->GetLevelData(nSLV);
		if (!ms->bInvincible)
		{
			nRnd = (unsigned int)m_pRndGen->Random();
			if (ms->nPImmune_)
				return 1;

			return (int)GetDamageAdjustedByElemAttr(
				((double)bs->nDEX * ((double)(nRnd % 10000000) * 0.00000003 + 0.7) * 2.5 + (double)bs->nSTR) * (double)nPAD * 0.015,
				pSkill,
				ms->aDamagedElemAttr,
				nSLV
			);
		}
	}
	return 0;
}

int CalcDamage::PDamageSummoned(MobStat *ms, int nSLV)
{
	unsigned int nRnd = (unsigned int)m_pRndGen->Random();
	int nMobPAD = ms->nPAD + ms->nPAD_;
	double dCalcMin = (double)nMobPAD * 0.1;
	double dCalcMax = (double)nMobPAD * 0.85;
	dCalcMin += ((double)(nRnd % 10000000) * 0.0000001) * (dCalcMax - dCalcMin);

	return (int)(40.0 / ((double)nSLV + 70) * (double)nMobPAD * dCalcMin * 0.01);
}

void CalcDamage::MesoExplosionDamage(MobStat *ms, int *anMoneyAmount, int nDropCount, int *aDamage)
{
	unsigned int aRandom[7], nRndIdx = 0, nRnd = 0;
	auto &ss = m_pUser->GetSecondaryStat();
	auto &bs = m_pUser->GetBasicStat();
	auto &cd = m_pUser->GetCharacterData();

	SkillEntry *pEntry;
	int nSLV = SkillInfo::GetInstance()->GetSkillLevel(
		cd,
		4211006,
		&pEntry
	);
	auto pLevel = pEntry->GetLevelData(nSLV);
	for (auto& nRnd : aRandom)
		nRnd = (unsigned int)m_RndGenForCharacter.Random();

	double dRatio = 0;
	for (int i = 0; i < nDropCount; ++i)
	{
		aDamage[i] = 0;
		if (ms->bInvincible)
			continue;
		else
		{
			NEXT_RAND;
			if (ms->nPImmune_ && (int)(nRnd % 100) > ss->nRespectPImmune_)
			{
				aDamage[i] = 1;
				continue;
			}

			NEXT_RAND;
			if (anMoneyAmount[i] <= 1000)
				dRatio = ((double)anMoneyAmount[i] * 0.82 + 28.0) * 0.0001886792452830189;
			else
				dRatio = (double)anMoneyAmount[i] / ((double)anMoneyAmount[i] + 5250);
			dRatio *= ((double)(nRnd % 10000000) * 0.0000000500000050000005 + 0.5);
			aDamage[i] = (int)((double)(50 * pLevel->m_nX) * dRatio);
		}
	}
}

void CalcDamage::InspectAttackDamage(AttackInfo::DamageInfo & dmgInfo, int nDamageCount)
{	
	//Inspect Damage Values Between Server and Client
	long long int liTotalDmgClient = 0, liTotalDmgSrv = 0;
	for (int i = 0; i < nDamageCount; ++i)
	{
		liTotalDmgClient += dmgInfo.anDamageClient[i];
		liTotalDmgSrv += dmgInfo.anDamageSrv[i];

		if ((double)dmgInfo.anDamageClient[i] >= (double)dmgInfo.anDamageSrv[i] * 1.3)
		{
			if (dmgInfo.anDamageClient[i] >= dmgInfo.anDamageSrv[i] * 3) //Might probably be hacking 
			{
				IncInvalidCount();
				if (dmgInfo.anDamageClient[i] >= dmgInfo.anDamageSrv[i] * 6)
					dmgInfo.anDamageClient[i] = dmgInfo.anDamageSrv[i];
			}
			else //Possible that critial damages were yielded in the client but not in the server. 
				dmgInfo.abDamageCriticalClient[i] = dmgInfo.abDamageCriticalSrv[i] = true;
		}
		//If not, that means the calculations are very close
		else if (dmgInfo.anDamageClient[i] > (double)dmgInfo.anDamageSrv[i]
			|| dmgInfo.anDamageSrv[i] >= (double)dmgInfo.anDamageClient[i] * 1.4)
			dmgInfo.abDamageCriticalClient[i] = dmgInfo.abDamageCriticalSrv[i];
		dmgInfo.anDamageSrv[i] = dmgInfo.anDamageClient[i];
	}

	if (liTotalDmgClient > liTotalDmgSrv * 2.2)
		IncInvalidCount();
	else
		DecInvalidCount();
}
