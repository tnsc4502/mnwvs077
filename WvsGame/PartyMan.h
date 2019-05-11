#pragma once
#include <map>
#include <string>

class InPacket;
class User;

class PartyMan
{
public:
	enum PartyRequest
	{
		rq_Party_Create = 1
	};

	enum PartyResult
	{

	};

	const static int MAX_PARTY_MEMBER = 6;

	struct PartyMember
	{
		std::string asCharacterName[MAX_PARTY_MEMBER];
		int anFieldID[MAX_PARTY_MEMBER],
			anLevel[MAX_PARTY_MEMBER],
			anChannelID[MAX_PARTY_MEMBER];

		int nPartyBossCharacterID = 0;
	};

	struct PartyData
	{
		PartyMember party;
	};

private:

	std::map<int, int> m_mCharacterIDToPartyID;
	std::map<int, PartyData*> m_mParty;
	PartyMan();
	~PartyMan();

public:
	void OnPartyRequest(User *pUser, InPacket *iPacket);
	PartyMan* GetInstance();
};

