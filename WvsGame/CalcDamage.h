#pragma once

class User;
class MobStat;
class SkillEntry;
class Mob;
class Rand32;

class CalcDamage
{
	User *m_pUser = nullptr;
	Rand32 *m_pRndGenForCharacter = nullptr;
public:
	CalcDamage(User *pUser);
	~CalcDamage();
	static double GetDamageAdjustedByElemAttr(long double damage, int nAttr, long double dAdjust);
	static double GetDamageAdjustedByElemAttr(double damage, const SkillEntry* pSkill, int *aDamagedElemAttr, int nSLV);
	static double GetDamageAdjustedByChargedElemAttr(double damage, int *aDamagedElemAttr, User* pUser);
	static int GetComboDamageParam(User *pUser, int nSkillID, int nComboCounter);
	void PDamage(Mob *pMob, MobStat* ms, int nDamagerPerMob, int nWeaponItemID, int nBulletItemID, int nAttackType, int nAction, bool bShadowPartner, const SkillEntry* pSkill, int nSLV, int *aDamage, int *abCritical, int tKeyDown, int nBerserkDamage, int nAdvancedChargeDamage);
};

