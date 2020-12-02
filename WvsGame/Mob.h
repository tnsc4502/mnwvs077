#pragma once
#include <map>
#include <vector>
#include "FieldObj.h"

struct MobSkillLevelData;
class SkillEntry;
class MobTemplate;
class Controller;
class MobStat;
class User;
class AffectedArea;
class InPacket;

class Mob : public FieldObj
{
public:
	enum MobType
	{
		e_MobType_Normal = 0,
		e_MobType_SubMob = 1,
		e_MobType_Suspend = 2
	};

	struct DamageLog
	{
		struct Info
		{
			int nCharacterID = 0,
				nDamage = 0;
		};
		std::map<int, Info> mInfo;
		int nFieldID = 0, nLastHitCharacter = 0; 
		long long int liTotalDamage = 0, liInitHP = 0;
	};

	struct PartyDamage
	{
		int nDamage = 0,
			nMaxDamage = 0,
			nMaxDamageCharacter = 0,
			nMaxDamageLevel = 0,
			nMinLevel = 255,
			nParty = -1;
	};

private:

	MobStat* m_pStat;
	MobTemplate* m_pMobTemplate;
	DamageLog m_damageLog;
	Controller* m_pController; 
	long long int m_liHP, m_liMP;

	int m_nSkillSummoned = 0,
		m_nSummonType = 0,
		m_nMobType = 0,
		m_nSummonOption = 0,
		m_nCtrlPriority = 0,
		m_nSkillCommand = 0;

	unsigned int m_tLastMove = 0,
		m_tLastSkillUse = 0,
		m_tLastUpdatePoison = 0,
		m_tLastUpdateVenom = 0,
		m_tLastUpdateAmbush = 0,
		m_tLastSendMobHP = 0,
		m_nItemIDStolen = 0;

	bool m_bNextAttackPossible = false, m_bAlreadyStealed = false;
	void* m_pMobGen = nullptr;

public:
	Mob();
	~Mob();

	//GameObj
	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	void EncodeInitData(OutPacket *oPacket, bool bIsControl = false);
	void SetMovePosition(int x, int y, char bMoveAction, short nSN);

	//Controller
	void SendChangeControllerPacket(User* pUser, int nLevel);
	void SendReleaseControllPacket(User* pUser, int dwMobID);
	void SetController(Controller* pController);
	Controller* GetController();
	void SendSuspendReset(bool bResetSuspend);
	void OnApplyCtrl(User *pUser, InPacket *iPacket);

	//Template
	void SetMobTemplate(MobTemplate *pTemplate);
	const MobTemplate* GetMobTemplate() const;

	//Move& Skill
	bool OnMobMove(bool bNextAttackPossible, int nAction, int nData, unsigned char *nSkillCommand, unsigned char *nSLV, bool *bShootAttack);
	bool DoSkill(int nSkillID, int nSLV, int nOption);
	void DoSkill_AffectArea(int nSkillID, int nSLV, const MobSkillLevelData *pLevel, int tDelay);
	void DoSkill_StateChange(int nSkillID, int nSLV, const MobSkillLevelData *pLevel, int tDelay, bool bResetBySkill = false);
	void DoSkill_UserStatChange(int nArg, int nSkillID, int nSLV, const MobSkillLevelData *pLevel, int tDelay);
	void DoSkill_PartizanStatChange(int nSkillID, int nSLV, const MobSkillLevelData *pLevel, int tDelay);
	void DoSkill_PartizanOneTimeStatChange(int nSkillID, int nSLV, const MobSkillLevelData *pLevel, int tDelay);
	void DoSkill_Summon(const MobSkillLevelData *pLevel, int tDelay);
	void PrepareNextSkill(unsigned char *nSkillCommand, unsigned char *nSLV, unsigned int tCur);

	//Stat
	void SetMobStat(MobStat *pStat);
	MobStat *GetMobStat();
	void ResetStatChangeSkill(int nSkillID);
	void OnMobStatChangeSkill(User *pUser, const SkillEntry *pSkill, int nSLV, int nDamageSum, int tDelay);
	void SendMobTemporaryStatSet(int nSet, int tDelay);
	void SendMobTemporaryStatReset(int nSet);
	void SendDamagedPacket(int nType, int nDecHP);
	void SendMobHPChange(int nHP, int nColor, int nBgColor, bool bEnforce);
	void BroadcastHP();
	void OnMobHit(User* pUser, long long int nDamage, int nAttackType);
	void OnMobDead(int nHitX, int nHitY, int nMesoUp, int nMesoUpByItem);
	void Heal(int nRangeMin, int nRangeMax);
	DamageLog& GetDamageLog();

	//Rewards
	int DistributeExp(int& refOwnType, int& refOwnParyID, int& refLastDamageCharacterID);
	void GiveExp(const std::vector<PartyDamage>& aPartyDamage);
	void GiveReward(unsigned int dwOwnerID, unsigned int dwOwnPartyID, int nOwnType, int nX, int nY, int tDelay, int nMesoUp, int nMesoUpByItem, bool bSteal = false);
	void GiveMoney(User *pUser, void *pDamageInfo, int nAttackCount);

	//MobGen
	void* GetMobGen() const;
	void SetMobGen(void* pGen);

	//Types
	void SetSummonType(int nType);
	void SetSummonOption(int nOption);
	void SetMobType(int nMobType);
	int GetMobType() const;

	//HP&MP
	void SetHP(long long int liHP);
	void SetMP(long long int liMP);
	long long int GetHP() const;
	long long int GetMP() const;
	void SetMobHP(long long int liHP);

	//Update
	void Update(unsigned int tCur);
	void UpdateMobStatChange(unsigned int tCur, int nVal, unsigned int tVal, unsigned int &nLastUpdateTime);
	void OnMobInAffectedArea(AffectedArea *pArea, unsigned int tCur);
};

