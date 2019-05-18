#pragma once
#include <map>
#include <string>
#include <mutex>

#ifdef _WVSCENTER
#include <atomic>
#endif

class InPacket;
class OutPacket;
class User;

class PartyMan
{
public:
	enum PartyRequest
	{
		rq_Party_Load = 0,
		rq_Party_Create = 1,
		rq_Party_Withdraw = 2,
		rq_Party_Join = 3,
		rq_Party_Invite = 4,
		rq_Party_Withdraw_Kick = 5,
		rq_Party_ChangeBoss = 6,
		rq_Guild_LevelOrJobChanged = 7,
	};

	enum PartyResult
	{
		res_Party_Load = 0,
		res_Party_Invite = 4,
		res_Party_Update = 7,
		res_Party_Create = 8,

		res_Party_Withdraw = 12,
		res_Party_Join = 15,
		res_Party_Failed_AlreadyInParty = 16,
		res_Party_Failed_UnableToProcess = 18,
		res_Party_ChangeBoss = 25,
		res_Party_MigrateIn = 31,
		res_Party_ChangeLevelOrJob = 32,
	};

	enum PartyRejectReason
	{
		rs_Party_Reject_Invitation = 0x16,
	};

	std::recursive_mutex m_mtxPartyLock;

#ifdef _WVSCENTER
	std::atomic<int> m_atiPartyIDCounter;
#endif

	const static int MAX_PARTY_MEMBER_COUNT = 6;

	struct PartyTownPortal
	{
		int m_TownPortalReturnFieldID = 999999999,
			m_TownPortalFieldID = 999999999,
			m_nFieldPortalX = 0,
			m_nFieldPortalY = 0;
	};

	struct PartyMember
	{
		std::string asCharacterName[MAX_PARTY_MEMBER_COUNT];

		int nPartyBossCharacterID = 0;
		int anFieldID[MAX_PARTY_MEMBER_COUNT],
			anLevel[MAX_PARTY_MEMBER_COUNT],
			anChannelID[MAX_PARTY_MEMBER_COUNT],
			anJob[MAX_PARTY_MEMBER_COUNT],
			anCharacterID[MAX_PARTY_MEMBER_COUNT];

		PartyTownPortal aTownPortal[MAX_PARTY_MEMBER_COUNT];

		void Encode(OutPacket *oPacket);
		void Decode(InPacket *iPacket);
		void Initialize(int nIdx);
	};

	struct PartyData
	{
		PartyMember party;
		int nPartyID = 0;

		void Encode(OutPacket *oPacket);
		void Decode(InPacket *iPacket);
	};

private:

	std::map<int, int> m_mCharacterIDToPartyID;
	std::map<int, PartyData*> m_mParty;
	PartyMan();
	~PartyMan();

public:
	static int FindUser(int nUserID, PartyData *pParty);
	static PartyMan* GetInstance();
	int GetPartyIDByCharID(int nUserID);
	PartyData* GetPartyByCharID(int nUserID);
	PartyData* GetParty(int nPartyID);
	void RemoveParty(PartyData *pParty);
	bool IsPartyBoss(int nPartyID, int nUserID);
	bool IsPartyMember(int nPartyID, int nUserID);
	void Broadcast(OutPacket *oPacket, int *anCharacterID, int nPlusOne);
	void GetSnapshot(int nPartyID, int anCharacterID[MAX_PARTY_MEMBER_COUNT]);

#ifdef _WVSGAME
	void OnPartyRequest(User *pUser, InPacket *iPacket);
	void OnPartyRequestRejected(User *pUser, InPacket *iPacket);
	void OnPacket(InPacket *iPacket);

	void OnCreateNewPartyRequest(User *pUser, InPacket *iPacket);
	void OnCreateNewPartyDone(InPacket *iPacket);
	void OnLoadPartyDone(InPacket *iPacket);
	void OnInvitePartyRequest(User *pUser, InPacket *iPacket);
	void OnJoinPartyRequest(User *pUser, InPacket *iPacket);
	void OnJoinPartyDone(InPacket *iPacket);
	void OnWithdrawPartyRequest(int nType, User *pUser, InPacket *iPacket);
	void OnWithdrawPartyDone(InPacket *iPacket);
	void OnChangePartyBossRequest(User *pUser, InPacket *iPacket);
	void OnChangePartyBossDone(InPacket *iPacket);
	void OnLeave(User *pUser, bool bMigrate);
	void OnUserMigration(InPacket *iPacket);
	void NotifyTransferField(int nCharacterID, int nFieldID);

	void OnRejectInvitation(User *pUser, InPacket *iPacket);
	void PostChangeLevelOrJob(User *pUser, int nVal, bool bLevelChanged);
	void OnChangeLevelOrJob(InPacket *iPacket);
#endif

#ifdef _WVSCENTER
	void CreateNewParty(InPacket *iPacket, OutPacket *oPacket);
	void LoadParty(InPacket *iPacket, OutPacket *oPacket);
	void JoinParty(InPacket *iPacket, OutPacket *oPacket);
	void WithdrawParty(InPacket *iPacket, OutPacket *oPacket);
	void ChangePartyBoss(InPacket *iPacket, OutPacket *oPacket);
	void NotifyMigrateIn(int nCharacterID, int nChannelID);
	void ChangeLevelOrJob(InPacket *iPacket, OutPacket *oPacket);
	void SendPacket(OutPacket *oPacket, PartyData *pParty);
#endif

};

