#include "GW_ItemSlotPet.h"
#include "WvsUnified.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "Poco\Data\MySQL\MySQLException.h"
#include "..\WvsLib\Logger\WvsLogger.h"

GW_ItemSlotPet::GW_ItemSlotPet()
{
	strPetName.resize(32);
	bIsPet = true;
	bIsCash = true;
	nInstanceType = GW_ItemSlotInstanceType::GW_ItemSlotPet_Type;
}


GW_ItemSlotPet::~GW_ItemSlotPet()
{
}

void GW_ItemSlotPet::LoadAll(int nCharacterID, std::map<int, ZSharedPtr<GW_ItemSlotBase>>& mRes)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM CashItem_Pet Where CharacterID = " << nCharacterID << " AND POS < " << GW_ItemSlotBase::LOCK_POS;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		auto pItem = MakeShared<GW_ItemSlotPet>();
		pItem->nCharacterID = recordSet["CharacterID"];
		pItem->liCashItemSN = recordSet["CashItemSN"];
		pItem->nItemID = recordSet["ItemID"];
		pItem->liExpireDate = recordSet["ExpireDate"];
		pItem->nAttribute = recordSet["Attribute"];
		pItem->nPetAttribute = (short)recordSet["PetAttribute"];
		pItem->nPOS = recordSet["POS"];
		pItem->nLevel = (unsigned char)(unsigned short)recordSet["Level"];
		pItem->nRepleteness = (unsigned char)(unsigned short)recordSet["Repleteness"];
		pItem->nTameness = (short)recordSet["Tameness"];
		pItem->usPetSkill = (unsigned short)recordSet["PetSkill"];
		pItem->strPetName = recordSet["PetName"].toString();
		pItem->nActiveState = (unsigned char)(unsigned short)recordSet["ActiveState"];
		pItem->nAutoBuffSkill = recordSet["AutoBuffSkill"];
		pItem->nPetHue = recordSet["PetHue"];
		pItem->nGiantRate = recordSet["GiantRate"];

		pItem->nType = GW_ItemSlotType::CASH;
		mRes[pItem->nPOS] = pItem;
	}
}

void GW_ItemSlotPet::Load(ATOMIC_COUNT_TYPE SN)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM CashItem_Pet Where CashItemSN = " << SN;
	queryStatement.execute();

	Poco::Data::RecordSet recordSet(queryStatement);
	nCharacterID = recordSet["CharacterID"];
	liCashItemSN = recordSet["CashItemSN"];
	nItemID = recordSet["ItemID"];
	liExpireDate = recordSet["ExpireDate"];
	nAttribute = recordSet["Attribute"];
	nPetAttribute = (short)recordSet["PetAttribute"];
	nPOS = recordSet["POS"];
	nLevel = (unsigned char)(unsigned short)recordSet["Level"];
	nRepleteness = (unsigned char)(unsigned short)recordSet["Repleteness"];
	nTameness = (short)recordSet["Tameness"];
	usPetSkill = (unsigned short)recordSet["PetSkill"];
	strPetName = recordSet["PetName"].toString();
	nActiveState = (unsigned char)(unsigned short)recordSet["ActiveState"];
	nAutoBuffSkill = recordSet["AutoBuffSkill"];
	nPetHue = recordSet["PetHue"];
	nGiantRate = recordSet["GiantRate"];

	nType = GW_ItemSlotType::CASH;
}

void GW_ItemSlotPet::Save(int nCharacterID, bool bRemoveRecord)
{
	if (nType != GW_ItemSlotType::CASH)
		throw std::runtime_error("Invalid Equip Type.");

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	std::string sQuery = "";
	try 
	{
		if (liItemSN < -1 && bRemoveRecord) //DROPPED or DELETED
		{
			liItemSN *= -1;
			queryStatement << "UPDATE CashItem_Pet "
				<< " Set CharacterID = -1 Where CharacterID = " << nCharacterID
				<< " and  CashItemSN = " << liCashItemSN;
			queryStatement.execute();
			return;
		}
		else
		{
			if(liCashItemSN <= 0)
				liCashItemSN = IncItemSN(GW_ItemSlotBase::CASH);
			queryStatement << "INSERT INTO CashItem_Pet (CashItemSN, ItemID, CharacterID, ExpireDate, Attribute, PetAttribute, POS, Level, Repleteness, Tameness, PetSkill, PetName, RemainLife, ActiveState, AutoBuffSkill, PetHue, GiantRate) VALUES("
				<< liCashItemSN << ", "
				<< nItemID << ", "
				<< nCharacterID << ", "
				<< liExpireDate << ", "
				<< nAttribute << ", "
				<< nPetAttribute << ", "
				<< nPOS << ", "
				<< (int)nLevel << ", "
				<< (int)nRepleteness << ", "
				<< nTameness << ", "
				<< usPetSkill << ", "
				<< "'" << strPetName << "', "
				<< nRemainLife << ", "
				<< (int)nActiveState << ", "
				<< nAutoBuffSkill << ", "
				<< nPetHue << ", "
				<< nGiantRate << ")";

			queryStatement << " ON DUPLICATE KEY UPDATE "
				<< "ItemID = " << nItemID << ", "
				<< "CharacterID = " << nCharacterID << ", "
				<< "ExpireDate = '" << liExpireDate << "', "
				//<< "Attribute = " << nAttribute << ", "
				<< "POS = " << nPOS << ", "
				//<< "Attribute = " << nAttribute << ", "
				<< "PetAttribute = " << nPetAttribute << ", "
				<< "Level = " << (int)nLevel << ", "
				<< "Repleteness = " << (int)nRepleteness << ", "
				<< "Tameness = " << (int)nTameness << ", "
				<< "PetSkill = " << (int)usPetSkill << ", "
				//<< "PetName = '" << strPetName << "', "
				//<< "RemainLife = " << (int)nRemainLife << ", "
				<< "ActiveState = " << (int)nActiveState << " "
				//<< "AutoBuffSkill = " << (int)nAutoBuffSkill << ", "
				//<< "PetHue = " << (int)nPetHue << ", "
				//<< "GiantRate = " << (int)nGiantRate << 
				;
		}
		sQuery = queryStatement.toString();
		queryStatement.execute();
	}
	catch (Poco::Data::MySQL::StatementException &se) 
	{
		WvsLogger::LogFormat("SQL Exception Occurred: %s\nRaw Query = %s\n", se.message().c_str(), sQuery.c_str());
	}
}

void GW_ItemSlotPet::Encode(OutPacket * oPacket, bool bForInternal) const
{
	EncodeInventoryPosition(oPacket);
	RawEncode(oPacket);
	if (bForInternal) 
		oPacket->Encode1(nActiveState);
}

void GW_ItemSlotPet::RawEncode(OutPacket * oPacket) const
{
	GW_ItemSlotBase::RawEncode(oPacket);	
	oPacket->EncodeBuffer(
		(unsigned char*)strPetName.data(),
		std::min(13, (int)strPetName.size()),
		std::min(13, std::max(0, 13 - (int)strPetName.size())));

	oPacket->Encode1(nLevel);
	oPacket->Encode2(nTameness);
	oPacket->Encode1(nRepleteness);
	oPacket->Encode8(liExpireDate);
	oPacket->Encode2(nPetAttribute);
	oPacket->Encode2(usPetSkill);
}

void GW_ItemSlotPet::Decode(InPacket * iPacket, bool bForInternal)
{
	RawDecode(iPacket);
	if (bForInternal) 
		nActiveState = iPacket->Decode1();
}

void GW_ItemSlotPet::RawDecode(InPacket * iPacket)
{
	GW_ItemSlotBase::RawDecode(iPacket);
	char cStrNameBuff[16]{ 0 };
	iPacket->DecodeBuffer((unsigned char*)cStrNameBuff, 13);
	strPetName = cStrNameBuff;

	nLevel = iPacket->Decode1();
	nTameness = iPacket->Decode2();
	nRepleteness = iPacket->Decode1();
	liExpireDate = iPacket->Decode8();
	nPetAttribute = iPacket->Decode2();
	usPetSkill = iPacket->Decode2();
}

void GW_ItemSlotPet::Release()
{
	FreeObj(this);
}

GW_ItemSlotBase * GW_ItemSlotPet::MakeClone() const
{
	GW_ItemSlotBase *pPet = AllocObj(GW_ItemSlotPet);
	*pPet = *this;
	pPet->liCashItemSN = -1;
	return pPet;
}
