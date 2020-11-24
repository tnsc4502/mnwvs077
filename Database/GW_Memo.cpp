#include "GW_Memo.h"
#include "WvsUnified.h"

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#if defined(DBLIB) || defined(_WVSCENTER)
void GW_Memo::Load(void * pRecordSet)
{
	Poco::Data::RecordSet &recordSet = *((Poco::Data::RecordSet*)pRecordSet);
	liSN = recordSet["SN"];
	sSender = recordSet["Sender"].toString();
	sContent = recordSet["Content"].toString();
	nFlag = recordSet["Flag"];
	nState = recordSet["State"];
	liDateSent = recordSet["SendDate"];
}

void GW_Memo::Save()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "INSERT INTO Memo VALUES(";
	if (liSN != -1)
		queryStatement << liSN << ", ";
	else
		queryStatement << "null, ";

	queryStatement << nCharacterID << ", "
		<< "\'" << sSender << "\', "
		<< "\'" << sContent << "\', "
		<< liDateSent << ", "
		<< nFlag << ", "
		<< nState << ") ON DUPLICATE KEY UPDATE "
		<< "State = " << nState;

	queryStatement.execute();
}
#endif

void GW_Memo::Encode(OutPacket * oPacket)
{
	oPacket->Encode4((int)liSN);
	oPacket->EncodeStr(sSender);
	oPacket->EncodeStr(sContent);
	oPacket->Encode8(liDateSent);
	oPacket->Encode1(nFlag);
}

void GW_Memo::Decode(InPacket * iPacket)
{
	liSN = iPacket->Decode4();
	sSender = iPacket->DecodeStr();
	sContent = iPacket->DecodeStr();
	liDateSent = iPacket->Decode8();
	nFlag = iPacket->Decode1();
}

std::vector<ZUniquePtr<GW_Memo>> GW_Memo::Load(int nCharacterID)
{
	std::vector<ZUniquePtr<GW_Memo>> aMemo;

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM Memo Where CharacterID = " << nCharacterID << " AND State = 1";
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);

	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		GW_Memo* pMemo = AllocObj(GW_Memo);
		pMemo->Load((void*)&recordSet);
		pMemo->nCharacterID = nCharacterID;
		aMemo.push_back(pMemo);
	}

	return aMemo;
}
