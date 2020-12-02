#pragma once
#include "FieldRect.h"
#include <vector>
#include <map>

struct RewardInfo;
class User;

class MobTemplate
{
	static std::map<int, MobTemplate*>* m_MobTemplates;

public:
	const static int MAX_DAMAGED_ELEM_ATTR = 8;

	struct MobSkillContext
	{
		int nSkillID = 0, 
			nSLV = 0, 
			nSummoned = 0;

		unsigned int tLastSkillUse = 0;
	};

	struct MobAttackInfo
	{
		int nType = 0,
			nConMP = 0,
			nPAD = 0,
			nLevel = 0,
			nSkillLevel = 0,
			nDisease = 0,
			nMPBurn = 0,
			nMagicElemAttr = 0;

		bool bDeadlyAttack = false,
			bDoFirst = false,
			bMagicAttack = false,
			bKnockBack = false;

		FieldRect rc;
	};

	const std::vector<RewardInfo*> *m_paMobReward;
	std::vector<MobSkillContext> m_aSkillContext;
	std::vector<MobAttackInfo> m_aAttackInfo;
	std::vector<std::pair<int, int>> m_aMobSkill;
	std::vector<int> m_aReviveTemplateID;

	unsigned int m_unTotalRewardProb = 0;

	MobTemplate();
	~MobTemplate();
	const std::vector<RewardInfo*>& GetMobReward();
	static MobTemplate* GetMobTemplate(int dwTemplateID);
	static void RegisterMob(int dwTemplateID);

	//ElemAttr
	static int GetElementAttribute(const std::string& s, int *aElemAttr);
	static int GetMagicAttackElementAttribute(const char *s, int *nElemAttr);

	//Skill
	void MakeSkillContext();
	int GetSkillIndex(int nSkillID, int nSLV);

	//Quest
	void SetMobCountQuestInfo(User *pUser) const;
	
	long long int m_liMaxHP, m_liMaxMP;

	int 
		m_nTemplateID,
		m_nLevel, 
		m_nSpeed, 
		m_nPAD, 
		m_nPDD, 
		m_nPDRate,
		m_nMAD, 
		m_nMDD, 
		m_nMDRate,
		m_nACC, 
		m_nEVA, 
		m_nPushed, 
		m_nEXP, 
		m_nGetCP,
		m_nCategory, 
		m_nFixedDamage,
		m_nFlySpeed,
		m_nSummonType, 
		m_nMoveAbility,
		m_nHPTagColor = 0,
		m_nHPTagBgColor = 0,
		m_aDamagedElemAttr[MAX_DAMAGED_ELEM_ATTR] = { 0 }
	;

	bool 
		m_bIsBodyAttack, 
		m_bIsChangeableMob, 
		m_bIsDamagedByMob, 
		m_bIsSmartPhase, 
		m_bInvincible,
		m_bOnlyNormalAttack,
		m_bIsExplosiveDrop,
		m_bHPgaugeHide = false,
		m_bIsBoss = false,
		m_bIsTargetFromSrv = true;

	double m_dFs;
	std::string m_strElemAttr, m_strMobType;

	/*void SetMaxHP(long long int maxHp);
	void SetMaxMP(long long int maxMp);

	void SetLevel(int level);
	void SetSpeed(int speed);
	void SetPAD(int PAD);
	void SetPDD(int PDD);
	void SetPDRate(int rate);
	void SetMAD(int MAD);
	void SetMDD(int MDD);
	void SetMDRate(int rate);
	void SetACC(int ACC);
	void SetEVA(int EVA);
	void SetPushed(int pushed);
	void SetEXP(int EXP);
	void SetCategory(int category);

	void SetElementAttribute(const std::string& elemAttr);
	void SetMobType(const std::string& mobType);

	long long int GetMaxHP() const;
	long long int GetMaxMP() const;
		
	int GetLevel() const;
	int GetSpeed() const;
	int GetPAD() const;
	int GetPDD() const;
	int GetPDRate() const;
	int GetMAD() const;
	int GetMDD() const;
	int GetMDRate() const;
	int GetACC() const;
	int GetEVA() const;
	int GetPushed() const;
	int GetEXP() const;
	int GetCategory() const;

	const std::string& GetElementAttribute() const;
	const std::string& GetMobType() const;*/

};

