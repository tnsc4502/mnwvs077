#include "GW_QuestRecord.h"
#include "WvsUnified.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\String\StringUtility.h"

void GW_QuestRecord::Load(void* pRecordSet)
{
	Poco::Data::RecordSet &recordSet = *((Poco::Data::RecordSet*)pRecordSet);
	nQuestID = recordSet["QuestID"];
	nCharacterID = recordSet["CharacterID"];
	nState = recordSet["State"];
	tTime = recordSet["Time"];
	sStringRecord = recordSet["StrRecord"].toString();
	std::string sMobRecord = recordSet["MobRecord"].toString();
}

void GW_QuestRecord::Save()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	std::string mobRecord = StringUtility::VectorToString(aMobRecord, ",");
	queryStatement << "INSERT INTO QuestRecord VALUES("
		<< nCharacterID << ", "
		<< nQuestID << ", "
		<< nState << ", "
		<< tTime << ", "
		<< "\'" << sStringRecord << "\', "
		<< "\'" << mobRecord << "\') ON DUPLICATE KEY UPDATE "
		<< "State = " << nState << ", "
		<< "StrRecord = \'" << sStringRecord << "\', "
		<< "Time = " << tTime;

	queryStatement.execute();
}

void GW_QuestRecord::Encode(OutPacket * oPacket)
{
	oPacket->Encode2(nQuestID);
	if (nState == 1)
	{
		oPacket->EncodeStr(sStringRecord);
	}
	else if (nState == 2)
		oPacket->Encode8(tTime);
}

void GW_QuestRecord::Decode(InPacket * iPacket, int nState)
{
	this->nState = nState;
	nQuestID = iPacket->Decode2();

	//not done yet.
	if (nState == 1)
	{
		sStringRecord = iPacket->DecodeStr();
	}
	else if (nState == 2)
		tTime = iPacket->Decode8();
}
