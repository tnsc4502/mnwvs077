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
#include "MobTemplate.h"
#include "..\Database\GA_Character.hpp"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"

#undef max
#undef min

CalcDamage::CalcDamage(User* pUser)
{
	m_pUser = pUser;
	m_pRndGenForCharacter = AllocObj(Rand32);
}

CalcDamage::~CalcDamage()
{
	FreeObj(m_pRndGenForCharacter);
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

void CalcDamage::PDamage(Mob *pMob, MobStat* ms, int nDamagerPerMob, int nWeaponItemID, int nBulletItemID, int nAttackType, int nAction, bool bShadowPartner, const SkillEntry* pSkill, int nSLV, int *aDamage, int *abCritical, int tKeyDown, int nBerserkDamage, int nAdvancedChargeDamage)
{
	unsigned int aRandom[7], nRndIdx = 0, nRnd = 0;
	int nSkillID = pSkill ? pSkill->GetSkillID() : 0;
	auto ss = m_pUser->GetSecondaryStat();
	auto bs = m_pUser->GetBasicStat();
	auto cd = m_pUser->GetCharacterData();
	auto mt = pMob->GetMobTemplate();

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
		nRnd = (unsigned int)m_pRndGenForCharacter->Random();

	int nCriticalAttackProp = 0, nCriticalAttackParam = 0;
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
	int nCSLV = ItemInfo::GetWeaponMastery(cd, nWeaponItemID, nAttackType, nullptr, nullptr);
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
			nCriticalAttackParam += nSharpEyes;
		else
			nCriticalAttackParam = nSharpEyes + 100;

	int nNinjaStormProp = (nSkillID == 4121008 ? pSkill->GetLevelData(nSLV)->m_nProp : 0);
	bool bHighDamageAction = false;

#define NEXT_RAND nRnd = aRandom[nRndIdx++ % 7]
#define CURRENT_RAND (double)(nRnd % 10000000) * 0.0000001

	for (int i = 0; i < nDamagerPerMob; ++i)
	{
		NEXT_RAND;
		bHighDamageAction = false;

		aDamage[i] = 0;
		if (ms->bInvincible)
			continue;

		if (ms->nPImmune || nSkillID == 1221011)
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

		nMobPDD = std::min(999, std::max(0, mt->m_nEVA + ms->nEVA));
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

			if (nWT == 45 || nWT == 46)
			{
				damage = bs->nDEX;
				highDamage = damage * M;
				if (highDamage < damage)
					std::swap(damage, highDamage);

				calc = (bs->nSTR + (highDamage - damage) * CURRENT_RAND + damage)
					* ((nAction < 22 || nAction > 27) && nAction != 54) ? 1.0 : 3.4
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
				bHighDamageAction = bHighDamageAction | (nAction >= 5 && nAction <= 15);
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
				}
			}

			if (bs->nLevel < mt->m_nLevel)
				calc = (100.0 - (double)(mt->m_nLevel - bs->nLevel)) * calc * 0.01;

			//Attack Elem Attribute
			damage = GetDamageAdjustedByChargedElemAttr(
				GetDamageAdjustedByElemAttr(calc, pSkill, ms->aDamagedElemAttr, nSLV),
				ms->aDamagedElemAttr,
				m_pUser
			);

			if (nSkillID != 1311005 && nSkillID != 4111004 && nSkillID != 4211002)
			{
				nMobPDD = std::min(1999, std::max(0, mt->m_nPDD + ms->nPDD));
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
				abCritical[i] = 1;
				damage += ((double)nCriticalAttackParam - 100.0) * 0.01 * highDamage;
			}

			if (nSkillID != 1311005 && ms->nPGuardUp)
				damage *= (double)ms->nPGuardUp * 0.01;

			//Shadow Partner
			if (bShadowPartner && nSkillID != 4121003)
			{
				int nRealPartIdx = i - (nDamagerPerMob / 2);
				if (i >= nDamagerPerMob / 2)
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
				damage *= (90.0 * (double)tKeyDown / 2000.0 + 10.0) * 0.01;

			if (!ms->nHardSkin || abCritical[i])
			{
				if (nBerserkDamage)
					damage *= (double)(nBerserkDamage + 100.0) * 0.01;
				damage = std::max(1.0, std::min(99999.0, damage));
				aDamage[i] = (int)std::round(damage);
			}
		}
		else
		{
			//Calc FixDamage
			int nLevelDiff = mt->m_nLevel - bs->nLevel;
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
