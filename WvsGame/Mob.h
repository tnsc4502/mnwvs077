#pragma once
#include <map>
#include <vector>
#include "FieldObj.h"

struct MobSkillLevelData;
class MobTemplate;
class Controller;
class MobStat;
class User;
class AffectedArea;

class Mob : public FieldObj
{
public:
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

	int m_tLastMove = 0, 
		m_tLastSkillUse = 0,
		m_tLastUpdatePoison = 0,
		m_tLastUpdateVenom = 0,
		m_tLastUpdateAmbush = 0,
		m_nSkillSummoned = 0,
		m_nSummonType = 0,
		m_nSummonOption = 0,
		m_nSkillCommand = 0;

	bool m_bNextAttackPossible = false;
	void* m_pMobGen = nullptr;

public:
	Mob();
	~Mob();

	void MakeEnterFieldPacket(OutPacket *oPacket);
	void MakeLeaveFieldPacket(OutPacket *oPacket);
	void EncodeInitData(OutPacket *oPacket, bool bIsControl = false);
	void SendChangeControllerPacket(User* pUser, int nLevel);
	void SendReleaseControllPacket(User* pUser, int dwMobID);
	void SetMobTemplate(MobTemplate *pTemplate);
	const MobTemplate* GetMobTemplate() const;

	void SetController(Controller* pController);
	Controller* GetController();
	void SetMovePosition(int x, int y, char bMoveAction, short nSN);

	//解析怪物移動時，Lucid有些怪物移動封包多兩個bytes
	static bool IsLucidSpecialMob(int dwTemplateID);
	bool OnMobMove(bool bNextAttackPossible, int nAction, int nData, unsigned char *nSkillCommand, unsigned char *nSLV, bool *bShootAttack);
	bool DoSkill(int nSkillID, int nSLV, int nOption);
	void DoSkill_AffectArea(int nSkillID, int nSLV, const MobSkillLevelData *pLevel, int tDelay);
	void DoSkill_StateChange(int nSkillID, int nSLV, const MobSkillLevelData *pLevel, int tDelay, bool bResetBySkill = false);
	void DoSkill_UserStatChange(int nArg, int nSkillID, int nSLV, const MobSkillLevelData *pLevel, int tDelay);
	void DoSkill_PartizanStatChange(int nSkillID, int nSLV, const MobSkillLevelData *pLevel, int tDelay);
	void DoSkill_PartizanOneTimeStatChange(int nSkillID, int nSLV, const MobSkillLevelData *pLevel, int tDelay);
	void DoSkill_Summon(const MobSkillLevelData *pLevel, int tDelay);
	void PrepareNextSkill(unsigned char *nSkillCommand, unsigned char *nSLV, int tCur);
	void ResetStatChangeSkill(int nSkillID);
	void OnMobInAffectedArea(AffectedArea *pArea, int tCur);
	void SendMobTemporaryStatSet(int nSet, int tDelay);
	void SendMobTemporaryStatReset(int nSet);
	void OnMobHit(User* pUser, long long int nDamage, int nAttackType);
	void OnMobDead(int nHitX, int nHitY, int nMesoUp, int nMesoUpByItem);
	int DistributeExp(int& refOwnType, int& refOwnParyID, int& refLastDamageCharacterID);
	void GiveExp(const std::vector<PartyDamage>& aPartyDamage);
	void GiveReward(unsigned int dwOwnerID, unsigned int dwOwnPartyID, int nOwnType, int nX, int nY, int tDelay, int nMesoUp, int nMesoUpByItem);
	void SetHP(long long int liHP);
	void SetMP(long long int liMP);
	void SetMobStat(MobStat *pStat);
	MobStat *GetMobStat();
	void* GetMobGen() const;
	void SetMobGen(void* pGen);
	void SetSummonType(int nType);
	void SetSummonOption(int nOption);

	long long int GetHP() const;
	long long int GetMP() const;

	DamageLog& GetDamageLog();
	void Update(int tCur);
	void UpdatePoison(int tCur);
	void UpdateVenom(int tCur);
	void UpdateAmbush(int tCur);
};

