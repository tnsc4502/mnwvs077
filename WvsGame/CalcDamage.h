#pragma once

class User;
class MobStat;
class SkillEntry;
class Mob;
class Rand32;

class CalcDamage
{
	User *m_pUser = nullptr;
	Rand32 *m_pRndGen = nullptr;
	int m_nInvalidCount = 0;

public:
	CalcDamage(User *pUser);
	~CalcDamage();
	static void LoadStandardPDD();
	static int GetStandardPDD(int nJobCategory, int nLevel);
	static double GetDamageAdjustedByElemAttr(long double damage, int nAttr, long double dAdjust);
	static double GetDamageAdjustedByElemAttr(double damage, const SkillEntry* pSkill, int *aDamagedElemAttr, int nSLV);
	static double GetDamageAdjustedByChargedElemAttr(double damage, int *aDamagedElemAttr, User* pUser);
	static int GetComboDamageParam(User *pUser, int nSkillID, int nComboCounter);
	void DecInvalidCount();
	void IncInvalidCount();
	int GetInvalidCount() const;
	void MDamage(Mob *pMob, MobStat* ms, int nDamagePerMob, int nWeaponItemID, int nAction, const SkillEntry* pSkill, int nSLV, int *aDamage, bool *abCritical, int nMobCount, int tKeyDown);
	void PDamage(Mob *pMob, MobStat* ms, int nDamagePerMob, int nWeaponItemID, int nBulletItemID, int nAttackType, int nAction, bool bShadowPartner, const SkillEntry* pSkill, int nSLV, int *aDamage, bool *abCritical, int tKeyDown, int nBerserkDamage, int nAdvancedChargeDamage);
};

