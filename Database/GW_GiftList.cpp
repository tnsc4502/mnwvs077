#include "GW_GiftList.h"
#include "WvsUnified.h"

#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

void GW_GiftList::Load(void * pRecordSet)
{
	Poco::Data::RecordSet &recordSet = *((Poco::Data::RecordSet*)pRecordSet);
	liItemSN = recordSet["ItemSN"];
	nCharacterID = recordSet["CharacterID"];
	nItemID = recordSet["ItemID"];
	sBuyCharacterName = recordSet["BuyCharacterName"].toString();
	sText = recordSet["Text"].toString();
}

void GW_GiftList::Save()
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "INSERT INTO GiftList VALUES(";
	if (liSN != -1)
		queryStatement << liSN << ", ";
	else
		queryStatement << "null, ";

	queryStatement << nCharacterID << ", "
		<< liItemSN << ", "
		<< nItemID << ", "
		<< "\'" << sBuyCharacterName << "\', "
		<< "\'" << sText << "\', 1) ON DUPLICATE KEY UPDATE "
		<< "State = " << nState;

	queryStatement.execute();
}

void GW_GiftList::Encode(OutPacket * oPacket)
{
	oPacket->Encode8(liItemSN);
	oPacket->Encode4(nItemID);
	oPacket->EncodeBuffer(
		(unsigned char*)sBuyCharacterName.c_str(), 
		std::min((int)sBuyCharacterName.size(), 13), 
		std::max(0, 13 - (int)sBuyCharacterName.size())
	);
	oPacket->EncodeBuffer(
		(unsigned char*)sText.c_str(), 
		std::min((int)sText.size(), 74),
		std::max(0, 74 - (int)sText.size()));
}

void GW_GiftList::Decode(InPacket * iPacket)
{
	char aName[16] = { 0 }, aText[75] = { 0 };
	liItemSN = iPacket->Decode8();
	nItemID = iPacket->Decode4();
	iPacket->DecodeBuffer((unsigned char*)aName, 13);
	iPacket->DecodeBuffer((unsigned char*)aText, 74);

	sBuyCharacterName = aName;
	sText = aText;
}

std::vector<ZUniquePtr<GW_GiftList>> GW_GiftList::Load(int nCharacterID)
{
	std::vector<ZUniquePtr<GW_GiftList>> aGiftList;

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM GiftList Where CharacterID = " << nCharacterID << " AND State = 1";
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);

	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		GW_GiftList* pGList = AllocObj(GW_GiftList);
		pGList->Load((void*)&recordSet);
		aGiftList.push_back(pGList);
	}

	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "UPDATE GiftList SET State = 2 Where CharacterID = " << nCharacterID << " AND State = 1";
	queryStatement.execute();

	return aGiftList;
}
