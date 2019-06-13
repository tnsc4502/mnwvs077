#include "GW_ItemSlotBundle.h"
#include "WvsUnified.h"
#include "Poco\Data\MySQL\MySQLException.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"

GW_ItemSlotBundle::GW_ItemSlotBundle()
{
	nInstanceType = GW_ItemSlotInstanceType::GW_ItemSlotBundle_Type;
}


GW_ItemSlotBundle::~GW_ItemSlotBundle()
{
}

void GW_ItemSlotBundle::Load(ATOMIC_COUNT_TYPE SN)
{
	std::string strTableName = "",
				sSNColumnName = (nType == GW_ItemSlotType::CASH ? "CashItemSN" : "ItemSN");

	if (nType == GW_ItemSlotType::CONSUME)
		strTableName = "ItemSlot_CON";
	else if (nType == GW_ItemSlotType::ETC)
		strTableName = "ItemSlot_ETC";
	else if (nType == GW_ItemSlotType::INSTALL)
		strTableName = "ItemSlot_INS";
	else if (nType == GW_ItemSlotType::CASH)
		strTableName = "CashItem_Bundle";
	else
		throw std::runtime_error("Invalid Item Slot Type.");
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM " << strTableName << " Where " + sSNColumnName + " = " << SN;
	queryStatement.execute();

	Poco::Data::RecordSet recordSet(queryStatement);
	nCharacterID = recordSet["CharacterID"];

	if (nType == GW_ItemSlotType::CASH)
		liCashItemSN = recordSet[sSNColumnName];
	else
		liItemSN = recordSet[sSNColumnName];

	nItemID = recordSet["ItemID"];
	liExpireDate = recordSet["ExpireDate"];
	nAttribute = recordSet["Attribute"];
	nNumber = recordSet["Number"];
	nPOS = recordSet["POS"];

	if (nType == GW_ItemSlotType::CASH)
		bIsCash = true;
}

void GW_ItemSlotBundle::Save(int nCharacterID, bool bRemoveRecord)
{

	std::string strTableName = "",
		sSNColumnName = (nType == GW_ItemSlotType::CASH ? "CashItemSN" : "ItemSN"),
		sQuery = "";

	if (nType == GW_ItemSlotType::CONSUME)
		strTableName = "ItemSlot_CON";
	else if (nType == GW_ItemSlotType::ETC)
		strTableName = "ItemSlot_ETC";
	else if (nType == GW_ItemSlotType::INSTALL)
		strTableName = "ItemSlot_INS";
	else if (nType == GW_ItemSlotType::CASH)
		strTableName = "CashItem_Bundle";
	else
		throw std::runtime_error("Invalid Item Slot Type.");
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	try 
	{
		if (liItemSN < -1 && bRemoveRecord) //DROPPED or DELETED
		{
			liItemSN *= -1;
			queryStatement << "UPDATE " << strTableName
				<< " Set CharacterID = -1 Where CharacterID = " << nCharacterID
				<< " and " + sSNColumnName + " = " << (nType == GW_ItemSlotType::CASH ? liCashItemSN : liItemSN);
			queryStatement.execute();
			return;
		}
		else
		{
			if (liItemSN <= 0)
				liItemSN = IncItemSN(nType);
			if (nType == GW_ItemSlotType::CASH && liCashItemSN == -1)
				liCashItemSN = liItemSN;
			queryStatement << "INSERT INTO " << strTableName << " (" + sSNColumnName + ", ItemID, CharacterID, ExpireDate, Attribute, POS, Number) VALUES("
				<< (nType == GW_ItemSlotType::CASH ? liCashItemSN : liItemSN) << ", "
				<< nItemID << ", "
				<< nCharacterID << ", "
				<< liExpireDate << ", "
				<< nAttribute << ", "
				<< nPOS << ", "
				<< nNumber << ")";
			queryStatement << " ON DUPLICATE KEY UPDATE "
				<< "ItemID = '" << nItemID << "', "
				<< "CharacterID = '" << nCharacterID << "', "
				<< "ExpireDate = '" << liExpireDate << "', "
				<< "Attribute = '" << nAttribute << "', "
				<< "POS ='" << nPOS << "', "
				<< "Number = '" << nNumber << "'";
				//<< "' WHERE " + sSNColumnName + " = " << (nType == GW_ItemSlotType::CASH ? liCashItemSN : liItemSN);
		}
		sQuery = queryStatement.toString();
		queryStatement.execute();
	}
	catch (Poco::Data::MySQL::StatementException &se) 
	{
		WvsLogger::LogFormat("SQL Exception Occurred: %s\nRaw Query = %s\n", se.message().c_str(), sQuery.c_str());
	}
}

void GW_ItemSlotBundle::Encode(OutPacket *oPacket, bool bForInternal) const
{
	EncodeInventoryPosition(oPacket);
	if (bForInternal)
		oPacket->Encode8(liItemSN);
	RawEncode(oPacket);
}

/*
Encode Bundle Item Info.
*/
void GW_ItemSlotBundle::RawEncode(OutPacket *oPacket) const
{
	GW_ItemSlotBase::RawEncode(oPacket);
	oPacket->Encode2(nNumber);
	oPacket->EncodeStr("");
	oPacket->Encode2(nAttribute);

	//          Throwing Start            Bullet
	if ((nItemID / 10000 == 207) || (nItemID / 10000 == 233))
		oPacket->Encode8(liItemSN);
}

void GW_ItemSlotBundle::Decode(InPacket *iPacket, bool bForInternal)
{
	if (bForInternal)
		liItemSN = iPacket->Decode8();
	RawDecode(iPacket);
}

void GW_ItemSlotBundle::RawDecode(InPacket *iPacket)
{
	GW_ItemSlotBase::RawDecode(iPacket);
	nNumber = iPacket->Decode2();
	std::string strTitle = iPacket->DecodeStr();
	nAttribute = iPacket->Decode2();
	if ((nItemID / 10000 == 207) || (nItemID / 10000 == 233))
		liItemSN = iPacket->Decode8();
}

void GW_ItemSlotBundle::Release()
{
	FreeObj(this);
}

GW_ItemSlotBase * GW_ItemSlotBundle::MakeClone()
{
	GW_ItemSlotBundle* ret = AllocObj(GW_ItemSlotBundle);
	*ret = *this;
	ret->liItemSN = -1;
	ret->liCashItemSN = -1;

	return ret;
}