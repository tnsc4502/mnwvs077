#pragma once
#include "AttackInfo.h"
#include "..\WvsLib\Random\Rand32.h"

class User;
class MobStat;
class SkillEntry;
class Mob;
class Rand32;

class CalcDamage
{
	User *m_pUser = nullptr;
	SynchronizedRand32 m_RndGenForCharacter, m_RndGenForSummoned, m_RndForCheckDamageMiss, m_RndGenForMob;
	int m_nInvalidCount = 0;

public:
	CalcDamage(User *pUser);
	~CalcDamage();
	void SetSeed(unsigned int uS1, unsigned int uS2, unsigned int uS3);
	SynchronizedRand32& GetRndGenForMob();
	SynchronizedRand32& GetRndGenForCharacter();
	SynchronizedRand32& GetRndGenForSummoned();
	SynchronizedRand32& GetRndGenForForCheckDamageMiss();

	//Static helper funcs
	static void LoadStandardPDD();
	static int GetStandardPDD(int nJobCategory, int nLevel);
	static double GetDamageAdjustedByElemAttr(double damage, int nAttr, double dAdjust);
	static double GetDamageAdjustedByElemAttr(double damage, const SkillEntry* pSkill, int *aDamagedElemAttr, int nSLV);
	static double GetDamageAdjustedByChargedElemAttr(double damage, int *aDamagedElemAttr, User* pUser);
	static int GetComboDamageParam(User *pUser, int nSkillID, int nComboCounter);
	static int GetMesoGuardReduce(User *pUser, double damage);

	//Funcs for damage calculation
	void DecInvalidCount();
	void IncInvalidCount();
	int GetInvalidCount() const;
	bool CheckMDamageMiss(MobStat *ms, unsigned int nRandForMissCheck);
	int MDamage(MobStat *ms, void *pMobAttackInfo_, unsigned int nRandForMissCheck, int *pnReduce, int *pnRand);
	void MDamage(Mob *pMob, MobStat* ms, int nDamagePerMob, int nWeaponItemID, int nAction, const SkillEntry* pSkill, int nSLV, int *aDamage, bool *abCritical, int nMobCount, int tKeyDown);
	int MDamage(MobStat *ms, const SkillEntry* pSkill, int nSLV);
	int MDamageSummoned(MobStat *ms, int nSLV);
	bool CheckPDamageMiss(MobStat *ms, unsigned int nRnd);
	int PDamage(MobStat *ms, void *pMobAttackInfo_, unsigned int nRandForMissCheck, int *pnReduce, int *pnRand);
	void PDamage(Mob *pMob, MobStat* ms, int nDamagePerMob, int nWeaponItemID, int nBulletItemID, int nAttackType, int nAction, bool bShadowPartner, const SkillEntry* pSkill, int nSLV, int *aDamage, bool *abCritical, int tKeyDown, int nBerserkDamage, int nAdvancedChargeDamage);
	int PDamage(MobStat *ms, const SkillEntry* pSkill, int nSLV);
	int PDamageSummoned(MobStat *ms, int nSLV);
	void MesoExplosionDamage(MobStat *ms, int *anMoneyAmount, int nDropCount, int *aDamage);
	void InspectAttackDamage(AttackInfo::DamageInfo& dmgInfo, int nDamageCount);
};

