#pragma once
#include "Field.h"
#include <vector>
#include <map>
#include <string>

class InPacket;
struct MCSkillEntry;

class Field_MonsterCarnival
	: public Field
{
	struct MemberData
	{
		bool bIsDead = false;
		int nTotCP = 0,
			nRestCP = 0,
			nTeam = -1;

		std::string sName;
	};

	struct RewardRate
	{
		double dWinCoin,
			dWinRecovery,
			dWinNuff,
			dWinCP,
			dLoseCoin,
			dLoseRecovery,
			dLoseNuff,
			dLoseCP;
	};

	struct GuardianGenPos
	{
		int nX = 0,
			nY = 0,
			nF = 0,
			nOccupier = 0,
			nTeam = 0;
	};

	struct MCMob
	{
		int nID = 0,
			nSpendCP = 0,
			nMobTime = 0;
	};

	struct MobGenPos
	{
		int nX = 0,
			nY = 0,
			nCy = 0,
			nFh = 0,
			nTeam = 0;
	};

	std::vector<void*> m_apReactorGen;
	std::vector<MCMob> m_aMob;
	std::vector<GuardianGenPos> m_aGuardianGenPos;
	std::vector<MobGenPos> m_aMobGenPosUnited, m_aMobGenPosDivided[2];
	std::vector<int> m_anSkillID, m_anGuardianID, m_anRewardRange, m_aSummonedMobCount[2];
	std::vector<RewardRate> m_aRewardRate;
	std::map<int, MemberData> m_mMember;
	std::map<int, int> m_mCharacterOut;
	std::vector<bool> m_abGuardianSummoned[2];

	int m_aMobGenMax[2] = { 0 },
		m_aGuardianGenMax[2] = { 0 },
		m_nGuardianID[2],
		m_anRestCP[2] = { 0 },
		m_anTotCP[2] = { 0 },
		m_nMobGenAvailable[2] = { 0 },
		m_nMobGenAvailableUnited = 0,
		m_nGuardianGenAvailable[2] = { 0 },
		m_nGuardianGenAvailableUnited = 0,
		m_nMobGenPosCount = 0,
		m_nState = 0, 
		m_tFinishMessage = 0,
		m_nTimeDefault = 0,
		m_nTimeExpand = 0,
		m_nTimeMessage = 0,
		m_nTimeFinish = 0,
		m_nRewardMapWin = 0,
		m_nRewardMapLose = 0,
		m_nDeathCP = 0,
		m_tEndTime = -1;

	double m_dClimaxRate;
	bool m_bIsMapDivided = false, m_bCanceled = false;

	std::string m_sEffectWin, m_sEffectLose, m_sSoundWin, m_sSoundLose;

public:
	Field_MonsterCarnival(void *pData, int nFieldID);
	~Field_MonsterCarnival();

	int OnUseSkill(MemberData& rmd, int nIndex);
	int DoNuffToOpponent(int nTeam, MCSkillEntry* pEntry);
	int OnSummonMob(MemberData& rmd, int nIndex);
	int OnSummonGuardian(MemberData& rmd, int nIndex);
	void AddCP(int nLastDamageCharacterID, int nAddCP);
	void SendCP(MemberData& rmd, User *pUser);
	void OnEnter(User *pUser);
	void OnLeave(User *pUser);
	void OnPacket(User *pUser, InPacket *iPacket); 
	void OnRequest(User *pUser, InPacket *iPacket);
	void Reset(bool bShuffleReactor);
	void OnReactorDestroyed(Reactor *pReactor);
	void ProcessTimeFinish();
	void Update();
};

