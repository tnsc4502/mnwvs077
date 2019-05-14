#include "WvsUnified.h"
#include "GW_Friend.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

GW_Friend::GW_Friend()
{
}


GW_Friend::~GW_Friend()
{
}

void GW_Friend::Encode(OutPacket * oPacket)
{
	oPacket->Encode4(nFriendID);
	sFriendName.reserve(15);
	oPacket->EncodeBuffer((unsigned char*)sFriendName.c_str(), 13);
	oPacket->Encode1(nFlag);
	oPacket->Encode4(nChannelID);
}

void GW_Friend::Decode(InPacket * iPacket)
{
	nFriendID = iPacket->Decode4();
	char aBuffer[15]{ 0 };
	iPacket->DecodeBuffer((unsigned char*)aBuffer, 13);
	sFriendName = aBuffer;
	nFlag = iPacket->Decode1();
	nChannelID = iPacket->Decode4();
}

void GW_Friend::Load(int nCharacterID, int nFriendID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * From Friend Where CharacterID = " << nCharacterID << " AND FriendID = " << nFriendID;

	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	
	this->nCharacterID = nCharacterID;
	this->nFriendID = nFriendID;
	this->nChannelID = -2;
	this->sFriendName = recordSet["FriendName"].toString();
	this->nFlag = recordSet["Flag"];
}

std::vector<GW_Friend*> GW_Friend::LoadAll(int nCharacterID)
{
	std::vector<GW_Friend*> aFriend;
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT FriendID From Friend Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);

	GW_Friend *pFriend = nullptr;
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		pFriend = AllocObj(GW_Friend);
		pFriend->Load(nCharacterID, (int)recordSet["FriendID"]);
		aFriend.push_back(pFriend);
	}

	return aFriend;
}

void GW_Friend::Save()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "REPLACE INTO Friend(CharacterID, FriendID, FriendName, Flag) VALUES (" 
		<< nCharacterID << ", "
		<< nFriendID << ", "
		<< "'" << sFriendName << "',"
		<< nFlag << ")";
	queryStatement.execute();
}

void GW_Friend::Delete()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "DELETE FROM Friend WHERE CharacterID = "
		<< nCharacterID << " AND FriendID = " << nFriendID;
	queryStatement.execute();
}
