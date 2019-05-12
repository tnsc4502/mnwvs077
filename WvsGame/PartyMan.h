#pragma once
#include <map>
#include <string>
#include <mutex>
#include <atomic>

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
		rq_Party_Join = 3,
		rq_Party_Invite = 4
	};

	enum PartyResult
	{
		res_Party_Load = 0,
		res_Party_MigrateIn = 1,
		res_Party_Invite = 4,
		res_Party_Update = 7,
		res_Party_Create = 8,

		res_Party_Join = 15,
		res_Party_Invite_Failed_AlreadyInParty = 16,
		res_Party_Invite_Failed_UnableToProcess = 18,
	};

	std::recursive_mutex m_mtxPartyLock;
	std::atomic<int> m_atiPartyIDCounter;

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
	static int FindUser(int nUserID, PartyData *pParty);
	PartyMan();
	~PartyMan();

public:
	static PartyMan* GetInstance();
	int GetPartyIDByCharID(int nUserID);
	PartyData* GetPartyByCharID(int nUserID);
	PartyData* GetParty(int nPartyID);
	bool IsPartyBoss(int nPartyID, int nUserID);
	void Broadcast(OutPacket *oPacket, int *anCharacterID, int nPlusOne);


#ifdef _WVSGAME
	void OnPartyRequest(User *pUser, InPacket *iPacket);
	void OnPacket(InPacket *iPacket);
	void OnCreateNewPartyDone(InPacket *iPacket);
	void OnLoadPartyDone(InPacket *iPacket);
	void OnJoinPartyDone(InPacket *iPacket);
	void OnLeave(User *pUser, bool bMigrate);
	void OnUserMigration(InPacket *iPacket);
	void NotifyTransferField(int nCharacterID, int nFieldID);
#endif

#ifdef _WVSCENTER
	void OnCreateNewPartyRequest(InPacket *iPacket, OutPacket *oPacket);
	void OnLoadPartyRequest(InPacket *iPacket, OutPacket *oPacket);
	void OnJoinPartyRequest(InPacket *iPacket, OutPacket *oPacket);
	void NotifyMigrateIn(int nCharacterID, int nChannelID);
	void SendPacket(OutPacket *oPacket, PartyData *pParty);
#endif

};

