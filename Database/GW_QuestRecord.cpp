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
	std::vector<std::string> mobRecords;
	/*StringUtility::Split(sMobRecord, mobRecords, ",");
	for (int i = 0; i < (int)mobRecords.size(); ++i)
		aMobRecord.push_back(atoi(mobRecords[i].c_str()));*/
}

void GW_QuestRecord::Save()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "DELETE FROM QuestRecord Where QuestID = " << nQuestID << " and CharacterID = " << nCharacterID;
	queryStatement.execute();

	std::string mobRecord = StringUtility::VectorToString(aMobRecord, ",");
	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "INSERT INTO QuestRecord VALUES(null, "
		<< nCharacterID << ", "
		<< nQuestID << ", "
		<< nState << ", "
		<< tTime << ", \'"
		<< sStringRecord << "\', \'"
		<< mobRecord << "\')";
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
