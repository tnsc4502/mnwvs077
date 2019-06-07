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
#include "..\WvsLib\Wz\ImgAccessor.h"

static int ms_annStandardPDD[5][255];

#undef max
#undef min

#define NEXT_RAND nRnd = aRandom[nRndIdx++ % 7]
#define CURRENT_RAND (double)(nRnd % 10000000) * 0.0000001

CalcDamage::CalcDamage(User* pUser)
{
	m_pUser = pUser;
	m_pRndGen = AllocObj(Rand32);
}

CalcDamage::~CalcDamage()
{
	FreeObj(m_pRndGen);
}

void CalcDamage::LoadStandardPDD()
{
	static WZ::ImgAccessor img("./DataSrv/StandardPDD");
	static auto empty = WZ::Node();
	int nJobCategory = 0, nLastValue = 0;
	for (auto& node : img)
	{
		nJobCategory = atoi(node.Name().c_str());
		if (nJobCategory < 0 || nJobCategory > 5)
			continue;
		for (int i = 0; i < 256; ++i)
		{
			auto& v = node[std::to_string(i)];
			if (v.Name() != "" && v != empty)
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
	auto ss = pUser->GetSecondaryStat();
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
		&pEntry,
		0,
		0, 
		0, 
		0
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
	int nSLV1 = SkillInfo::GetInstance()->GetSkillLevel(pUser->GetCharacterData(), 1111002, &pEntry1, 0, 0, 0, 0);
	int nSLV2 = SkillInfo::GetInstance()->GetSkillLevel(pUser->GetCharacterData(), 1120003, &pEntry2, 0, 0, 0, 0);

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
		&pEntry,
		0,
		0,
		0,
		0
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

bool CalcDamage::CheckMDamageMiss(MobStat *ms, unsigned int nRandForMissCheck)
{
	unsigned int nRnd = (unsigned int)m_pRndGen->Random();
	auto ss = m_pUser->GetSecondaryStat();
	auto bs = m_pUser->GetBasicStat();
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
	auto ss = m_pUser->GetSecondaryStat();
	auto bs = m_pUser->GetBasicStat();
	unsigned int nRnd = (unsigned int)m_pRndGen->Random();
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
		nRnd = (unsigned int)m_pRndGen->Random();

	int nSkillID = pSkill ? pSkill->GetSkillID() : 0, 
		nCriticalAttackProp = 0, 
		nCriticalAttackParam = 0;
	auto ss = m_pUser->GetSecondaryStat();
	auto bs = m_pUser->GetBasicStat();
	auto cd = m_pUser->GetCharacterData();

	int nMAD = std::min(1999, std::max(0, ss->nMAD + ss->nMAD_)), nMobPDD = 0;
	if (ss->nMaxLevelBuff_)
		nMAD = std::min(1999, nMAD + nMAD / ss->nMaxLevelBuff_ * 100);

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
		NEXT_RAND;
		aDamage[i] = 0;
		if (ms->bInvincible)
			continue;

		nMobPDD = std::min(999, std::max(0, ms->nEVA + ms->nEVA_));
		if (pSkill && pSkill->GetLevelData(nSLV)->m_nFixDamage == 0)
		{
			damage = s * 0.5;
			highDamage = s * 1.2;

			if (damage + (highDamage - damage) * CURRENT_RAND < nMobPDD)
				continue;

			if (nSkillID == 2301002)
			{
				damage = bs->nINT * 0.2;
				highDamage = bs->nINT * 0.8;

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
				highDamage = nMAD * M;
				if (damage > highDamage)
					std::swap(damage, highDamage);

				damage += (highDamage - damage) * CURRENT_RAND;
				calc = pSkill->GetLevelData(nSLV)->m_nMad
					* ((double)bs->nINT * 0.5 + std::pow(nMAD * 0.058, 2) + damage * 3.3)
					* 0.01;
			}

			damage = GetDamageAdjustedByElemAttr(
				(double)nAmp * calc * 0.01,
				pSkill,
				ms->aDamagedElemAttr,
				nSLV
			);

			nMobPDD = std::min(1999, std::max(0, ms->nMDD + ms->nMDD_));
			highDamage = nMobPDD * 0.5;
			calc = nMobPDD * 0.6;
			damage -= (highDamage + (highDamage - calc) * CURRENT_RAND);

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
				aDamage[i] = (int)std::round(damage);
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
				if ((damage + (highDamage - damage) * CURRENT_RAND >= nMobPDD))
					aDamage[i] = pSkill->GetLevelData(nSLV)->m_nFixDamage;
			}
		}
	}
}

bool CalcDamage::CheckPDamageMiss(MobStat *ms, unsigned int nRandForMissCheck)
{
	unsigned int nRnd = (unsigned int)m_pRndGen->Random();
	auto ss = m_pUser->GetSecondaryStat();
	auto bs = m_pUser->GetBasicStat();
	int nEVA = std::min(999, std::max(0, ss->nEVA + ss->nEVA_)),
		nMobEVA = 0;
	if (bs->nLevel >= ms->nLevel || (nEVA -= (ms->nLevel - bs->nLevel), nEVA > 0))
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
	auto ss = m_pUser->GetSecondaryStat();
	auto bs = m_pUser->GetBasicStat();
	unsigned int nRnd = (unsigned int)m_pRndGen->Random();
	if (pnRand)
		*pnRand = (int)nRnd;
	int nMobPDD = std::min(1999, std::max(0, ms->nPAD + ms->nPAD_));
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
		nBase = (int)((double)bs->nLUK 
		+ (double)bs->nDEX * 0.25 
		+ (double)bs->nINT * 0.111111111
		+ (double)bs->nSTR * 0.2857142857142857);
	else
		nBase = (int)((double)bs->nINT * 0.111111111111 
		+ (double)bs->nDEX * 0.2857142857142857 
		+ (double)bs->nSTR * 0.4
		+ (double)bs->nLUK * 0.25);

	if (nStandardPDD > nMobPDD)
	{
		calc1 = (double)ms->nLevel * 0.00181818181 + ((double)nBase * 0.00125) + 0.28;
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
	auto ss = m_pUser->GetSecondaryStat();
	auto bs = m_pUser->GetBasicStat();
	auto cd = m_pUser->GetCharacterData();

	SkillEntry *pSPSkill;
	int nSPSLV = SkillInfo::GetInstance()->GetSkillLevel(
		cd,
		4111002,
		&pSPSkill,
		0,
		0,
		0,
		0
	);
	for (auto& nRnd : aRandom)
		nRnd = (unsigned int)m_pRndGen->Random();
	int nWT = ItemInfo::GetWeaponType(nWeaponItemID);

	//Basic PDD
	int nMobPDD = std::min(999, std::max(0, ss->nACC + ss->nACC_));
	double a = (double)nMobPDD * 100.0 / ((double)std::max(0, pMob->GetMobTemplate()->m_nLevel - bs->nLevel) * 10.0 + 255.0), 
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
	double M = ((double)nCSLV * 5.0 + 10.0) * 0.009;

	if (nAction == 58)
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
		NEXT_RAND;
		bHighDamageAction = false;

		aDamage[i] = 0;
		if (ms->bInvincible)
			continue;

		if (ms->nPImmune_ || nSkillID == 1221011)
		{
			aDamage[i] = 1;
			continue;
		}

		if (nSkillID == 3221007 && nAttackType == 1)
		{
			highDamage = 5000.0;
			damage = 0.0;
			aDamage[i] = (int)std::round(99999.0 - ((highDamage - damage) * CURRENT_RAND + damage));
			continue;
		}
		else if (nSkillID == 4121008)
		{
			highDamage = 100.0;
			damage = 0.0;
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
			if (damage + (highDamage - damage) * CURRENT_RAND < nMobPDD)
				continue;

			if (ss->nDarkness_)
			{
				highDamage = 100.0;
				damage = 0.0;
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
				if (damage + (highDamage - damage) * CURRENT_RAND < 3.0 ||
					(nSkillID == 1121008 && i == 1))
					bHighDamageAction = true;
			}

			if ((nWT == 45 || nWT == 46) &&
				((nAction < 22 || nAction > 27) && nAction != 54))
			{
				damage = bs->nDEX;
				highDamage = damage * M;
				if (highDamage < damage)
					std::swap(damage, highDamage);

				calc = (bs->nSTR + (highDamage - damage) * CURRENT_RAND + damage)
					* (nSkillID != 3201003 && nSkillID != 3101003) ? 1.0 : 3.4
					* (double)nPAD * 0.005;
			}
			else if (nWT == 47
				&& (nAction < 22 || nAction > 27)
				&& nAction != 55
				&& nSkillID != 4221001
				&& nSkillID != 4121008)
			{
				NEXT_RAND;
				damage = bs->nLUK;
				highDamage = damage * M;
				if (highDamage < damage)
					std::swap(highDamage, damage);
				calc = (highDamage - damage) * CURRENT_RAND + damage;
				calc = ((double)(bs->nSTR + bs->nDEX) + calc) * (double)nPAD * 0.0067;
			} 
			else if (nWT == 49
				&& (nAction < 22 || nAction > 27)
				&& (nAction != 94 && nAction != 95)
				&& nSkillID != 5201002
				&& nSkillID != 5221003
				&& nSkillID != 5221007
				&& nSkillID != 45221008)
			{
				damage = bs->nDEX;
				highDamage = damage * M;
				if (highDamage < damage)
					std::swap(damage, highDamage);
				if(nAction != 77 && nAction != 78)
					calc = (((highDamage - damage) * CURRENT_RAND + damage) + bs->nSTR) * (double)nPAD * 0.005;
				else
					calc = (((highDamage - damage) * CURRENT_RAND + damage) * 1.8 + bs->nSTR) * (double)nPAD * 0.01;
			}
			else if (nAction == 32)
			{
				damage = bs->nSTR;
				highDamage = damage * M;
				if (highDamage < damage)
					std::swap(damage, highDamage);
				calc = (((highDamage - damage) * CURRENT_RAND + damage) + bs->nDEX) * (double)nPAD * 0.05;
			}
			else
			{
				bHighDamageAction = bHighDamageAction || (nAction >= 5 && nAction <= 15);
				switch (nWT)
				{
					case 45:
						damage = bs->nDEX;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * 3.4;
						calc = (damage + bs->nSTR) * (double)nPAD * 0.01;
						break;
					case 46:
						damage = bs->nDEX;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * 3.6;
						calc = (damage + bs->nSTR) * (double)nPAD * 0.01;
						break;
					case 41:
					case 42:
						damage = bs->nSTR;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * (bHighDamageAction ? 3.4 : 4.8);
						calc = (damage + bs->nDEX) * (double)nPAD * 0.01;
						break;
					case 43:
					case 44:
						damage = bs->nSTR;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * (nSkillID == 1311006 ? 4.0 : (bHighDamageAction != (nWT == 43)) ? 5.0 : 3.0);
						calc = (damage + bs->nDEX) * (double)nPAD * 0.01;
						break;
					case 40:
						damage = bs->nSTR;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * 4.6;
						calc = (damage + bs->nDEX) * (double)nPAD * 0.01;
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
						calc = (damage + bs->nDEX) * (double)nPAD * 0.01;
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
							calc = (double)(damage + bs->nDEX + bs->nSTR) * (double)nPAD * 0.01;
						}
						else
						{
							damage = bs->nSTR;
							highDamage = damage * M;
							if (damage > highDamage)
								std::swap(damage, highDamage);
							damage = ((highDamage - damage) * CURRENT_RAND + damage) * 4.0;
							calc = (damage + bs->nDEX) * (double)nPAD * 0.01;
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
								if (CURRENT_RAND < pSkill->GetLevelData(nSLV)->m_nProp)
								{
									calc *= (double)(pSkill->GetLevelData(nSLV)->m_nX + 100) * 0.01;
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
							calc = (damage) * (double)nPAD * 0.01;
						}
						break;
					case 39:
						damage = bs->nSTR;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);

						damage = ((highDamage - damage) * CURRENT_RAND + damage) * (bs->nJob == 500 ? 3.0: 4.2);
						calc = (damage + bs->nDEX) * (double)nPAD * 0.01;
						break;
					case 48:
						damage = bs->nSTR;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * 4.8;
						calc = (damage + bs->nDEX) * (double)nPAD * 0.01;
						break;
					case 49:
						damage = bs->nDEX;
						highDamage = damage * M;
						if (damage > highDamage)
							std::swap(damage, highDamage);
						damage = ((highDamage - damage) * CURRENT_RAND + damage) * 3.6;
						calc = (damage + bs->nSTR) * (double)nPAD * 0.01;
						if (nSkillID == 5211004 && nBulletItemID / 1000 != 2331)
							calc *= 0.5;
						if (nSkillID == 5211005 && nBulletItemID / 1000 != 2332)
							calc *= 0.5;
						break;
				}

			}

			if (bs->nLevel < ms->nLevel)
				calc = (100.0 - (double)(ms->nLevel - bs->nLevel)) * calc * 0.01;

			//Attack Elem Attribute
			damage = GetDamageAdjustedByChargedElemAttr(
				GetDamageAdjustedByElemAttr(calc, pSkill, ms->aDamagedElemAttr, nSLV),
				ms->aDamagedElemAttr,
				m_pUser
			);

			if (nSkillID != 1311005 && nSkillID != 4111004 && nSkillID != 4211002)
			{
				nMobPDD = std::min(1999, std::max(0, ms->nPDD + ms->nPDD_));
				highDamage = nMobPDD * 0.5;
				calc = nMobPDD * 0.6;
				damage -= (highDamage - calc) * CURRENT_RAND + highDamage;
			}

			//Basic Damage
			highDamage = damage;
			nMobPDD = pSkill ? pSkill->GetLevelData(nSLV)->m_nDamage : 0;
			int nComboParam = GetComboDamageParam(m_pUser, nSkillID, ss->nComboCounter_ - 1);
			if (nAdvancedChargeDamage && nSkillID == 1211002)
				damage *= (double)nAdvancedChargeDamage * 0.01;

			if (nMobPDD > 0)
				damage *= (double)nMobPDD * 0.01;

			if (nComboParam)
				damage *= (double)nComboParam * 0.01;

			//Critical Damage
			if ((nSkillID != 3211003 && nSkillID != 4111004 && nSkillID != 4221001 || nAction == 58)
				&& nCriticalAttackParam > 0
				&& nCriticalAttackProp > 0
				&& (nCSLV > 0 || ss->nSharpEyes_)
				&& (NEXT_RAND, (double)(nRnd % 10000000) * 0.00001 < nCriticalAttackProp))
			{
				abCritical[i] = true;
				damage += ((double)nCriticalAttackParam - 100.0) * 0.01 * highDamage;
			}

			if (nSkillID != 1311005 && ms->nPGuardUp_)
				damage *= (double)ms->nPGuardUp_ * 0.01;

			//Shadow Partner
			if (bShadowPartner && nSkillID != 4121003)
			{
				int nRealPartIdx = i - (nDamagePerMob / 2);
				if (i >= nDamagePerMob / 2)
				{
					if (pSkill)
						highDamage = aDamage[nRealPartIdx] * (double)pSPSkill->GetLevelData(nSPSLV)->m_nY / 100.0;
					else
						highDamage = aDamage[nRealPartIdx] * (double)pSPSkill->GetLevelData(nSPSLV)->m_nX / 100.0;
					damage = highDamage;
					abCritical[i] = abCritical[nRealPartIdx];
				}
			}
			if (nSkillID == 4221001)
			{
				if ((ss->nDarkSight_))
					damage *= (double)(pSkill->GetLevelData(nSLV)->m_nTime) / 3.0 + 1.0;
				else
					damage = 0.0;
			}
			if (tKeyDown)
				if(nSkillID == 5101004)
					damage *= (90.0 * (double)tKeyDown / 1000.0 + 10.0) * 0.01;
				else
					damage *= (90.0 * (double)tKeyDown / 2000.0 + 10.0) * 0.01;

			if (!ms->nHardSkin_ || abCritical[i])
			{
				if (nBerserkDamage)
					damage *= (double)(nBerserkDamage + 100.0) * 0.01;

				if (nSkillID == 5121007)
					damage *= (i == 4 ? 2.0 : (i == 5 ? 4.0 : 1.0));

				damage = std::max(1.0, std::min(99999.0, damage));
				aDamage[i] = (int)std::round(damage);
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
				if ((damage + (highDamage - damage) * CURRENT_RAND >= nMobPDD))
					aDamage[i] = pSkill->GetLevelData(nSLV)->m_nFixDamage;
			}
		}
	}
}
