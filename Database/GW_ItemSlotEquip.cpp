#include "GW_ItemSlotEquip.h"
#include "WvsUnified.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

#include "Poco\Data\MySQL\MySQLException.h"

#define ADD_EQUIP_FLAG(name, container)\
if(n##name != 0) {\
	nFlag |= EQP_##name;\
	VALUE_HOLDER vh;\
	vh.type = sizeof(n##name);\
	if((vh.type) == 1)vh.cValue = n##name;\
	if ((vh.type) == 2)vh.sValue = n##name;\
	if ((vh.type) == 4)vh.iValue = n##name;\
	if ((vh.type) == 8)vh.liValue = n##name;\
	container.push_back(vh);\
}

#define DECODE_EQUIP_FLAG(name)\
if (nFlag & EQP_##name) {\
	if (sizeof(n##name) == 1) n##name = iPacket->Decode1();\
	if (sizeof(n##name) == 2) n##name = iPacket->Decode2();\
	if (sizeof(n##name) == 4) n##name = iPacket->Decode4();\
	if (sizeof(n##name) == 8) n##name = iPacket->Decode8();\
}

GW_ItemSlotEquip::GW_ItemSlotEquip()
{
	nInstanceType = GW_ItemSlotInstanceType::GW_ItemSlotEquip_Type;
}


GW_ItemSlotEquip::~GW_ItemSlotEquip()
{
}

void GW_ItemSlotEquip::LoadAll(int nCharacterID, bool bIsCash, std::map<int, ZSharedPtr<GW_ItemSlotBase>>& mRes)
{
	std::string sTableName = "ItemSlot_EQP";
	if (bIsCash)
		sTableName = "CashItem_EQP";

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM " << sTableName << " Where CharacterID = " << nCharacterID << " AND POS < " << GW_ItemSlotBase::LOCK_POS;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		auto pItem = MakeShared<GW_ItemSlotEquip>();
		pItem->nCharacterID = recordSet["CharacterID"];

		if (bIsCash)
			pItem->liCashItemSN = recordSet["CashItemSN"];
		else
			pItem->liItemSN = recordSet["ItemSN"];

		pItem->nItemID = recordSet["ItemID"];
		pItem->liExpireDate = recordSet["ExpireDate"];
		pItem->nAttribute = recordSet["Attribute"];
		pItem->nPOS = recordSet["POS"];
		pItem->sTitle = recordSet["Title"].toString();
		pItem->nRUC = (unsigned char)(unsigned short)recordSet["RUC"];
		pItem->nCUC = (unsigned char)(unsigned short)recordSet["CUC"];
		pItem->nCuttable = recordSet["Cuttable"];
		pItem->nSTR = recordSet["I_STR"];
		pItem->nDEX = recordSet["I_DEX"];
		pItem->nINT = recordSet["I_INT"];
		pItem->nLUK = recordSet["I_LUK"];
		pItem->nMaxHP = recordSet["I_MaxHP"];
		pItem->nMaxMP = recordSet["I_MaxMP"];
		pItem->nPAD = recordSet["I_PAD"];
		pItem->nMAD = recordSet["I_MAD"];
		pItem->nPDD = recordSet["I_PDD"];
		pItem->nMDD = recordSet["I_MDD"];
		pItem->nACC = recordSet["I_ACC"];
		pItem->nEVA = recordSet["I_EVA"];
		pItem->nSpeed = recordSet["I_Speed"];
		pItem->nCraft = recordSet["I_Craft"];
		pItem->nJump = recordSet["I_Jump"];
		pItem->nType = GW_ItemSlotType::EQUIP;

		pItem->bIsCash = (pItem->liCashItemSN != -1);
		mRes[pItem->nPOS] = pItem;
	}
}

void GW_ItemSlotEquip::Load(ATOMIC_COUNT_TYPE SN)
{
	std::string sColumnName = "ItemSN", sTableName = "ItemSlot_EQP";
	if (bIsCash) 
	{
		sColumnName = "CashItemSN";
		sTableName = "CashItem_EQP";
	}
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM " << sTableName << " Where " + sColumnName + " = " << SN;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	nCharacterID = recordSet["CharacterID"];

	if(bIsCash)
		liCashItemSN = recordSet["CashItemSN"];
	else
		liItemSN = recordSet["ItemSN"];

	nItemID = recordSet["ItemID"];
	liExpireDate = recordSet["ExpireDate"];
	nAttribute = recordSet["Attribute"];
	nPOS = recordSet["POS"];
	sTitle = recordSet["Title"].toString();
	nRUC = (unsigned char)(unsigned short)recordSet["RUC"];
	nCUC = (unsigned char)(unsigned short)recordSet["CUC"];
	nCuttable = recordSet["Cuttable"];
	nSTR = recordSet["I_STR"];
	nDEX = recordSet["I_DEX"];
	nINT = recordSet["I_INT"];
	nLUK = recordSet["I_LUK"];
	nMaxHP = recordSet["I_MaxHP"];
	nMaxMP = recordSet["I_MaxMP"];
	nPAD = recordSet["I_PAD"];
	nMAD = recordSet["I_MAD"];
	nPDD = recordSet["I_PDD"];
	nMDD = recordSet["I_MDD"];
	nACC = recordSet["I_ACC"];
	nEVA = recordSet["I_EVA"];
	nSpeed = recordSet["I_Speed"];
	nCraft = recordSet["I_Craft"];
	nJump = recordSet["I_Jump"];
	nType = GW_ItemSlotType::EQUIP;

	bIsCash = (liCashItemSN != -1);
}

void GW_ItemSlotEquip::Save(int nCharacterID, bool bRemoveRecord)
{
	if (nType != GW_ItemSlotType::EQUIP)
		throw std::runtime_error("Invalid Equip Type.");
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	std::string sQuery = "";

	try 
	{
		if (liItemSN < -1 && bRemoveRecord) //DROPPED or DELETED
		{
			liItemSN *= -1;
			queryStatement << "UPDATE ItemSlot_EQP Set CharacterID = -1 Where CharacterID = " << nCharacterID << " and ItemSN = " << liItemSN;

			//WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Del SQL = %s\n", queryStatement.toString().c_str());
			queryStatement.execute();
			return;
		}
		else
		{
			if(liItemSN <= 0)
				liItemSN = IncItemSN(GW_ItemSlotType::EQUIP);
			if (bIsCash && liCashItemSN == -1)
				liCashItemSN = IncItemSN(GW_ItemSlotType::CASH);

			std::string sTableName = (bIsCash ? "CashItem_EQP" : "ItemSlot_EQP");
			std::string sColumnName = (bIsCash ? "CashItemSN" : "ItemSN");

			queryStatement << "INSERT INTO " << sTableName << " (" << sColumnName <<", ItemID, CharacterID, ExpireDate, Attribute, POS, RUC, CUC, Cuttable, I_STR, I_DEX, I_INT, I_LUK, I_MaxHP, I_MaxMP, I_PAD, I_MAD, I_PDD, I_MDD, I_ACC, I_EVA, I_Speed, I_Craft, I_Jump) VALUES("
				<< (bIsCash ? liCashItemSN : liItemSN) << ", "
				<< nItemID << ", "
				<< nCharacterID << ", "
				<< liExpireDate << ", "
				<< nAttribute << ", "
				<< nPOS << ", "
				<< (unsigned short)nRUC << ", "
				<< (unsigned short)nCUC << ", "
				<< (unsigned short)nCuttable << ", "
				<< nSTR << ", "
				<< nDEX << ", "
				<< nINT << ", "
				<< nLUK << ", "
				<< nMaxHP << ", "
				<< nMaxMP << ", "
				<< nPAD << ", "
				<< nMAD << ", "
				<< nPDD << ", "
				<< nMDD << ", "
				<< nACC << ", "
				<< nEVA << ", "
				<< nSpeed << ", "
				<< nCraft << ", "
				<< nJump << ")";

			queryStatement << " ON DUPLICATE KEY UPDATE " 
				<< sColumnName << " = " << (bIsCash ? liCashItemSN : liItemSN) << ", "
				<< "CharacterID = " << nCharacterID << ", "
				<< "ExpireDate = '" << liExpireDate << "', "
				<< "Attribute = " << nAttribute << ", "
				<< "POS = " << nPOS << ", "
				<< "Title = '" << sTitle << "', "
				<< "RUC = '" << (unsigned short)nRUC << "', "
				<< "CUC = '" << (unsigned short)nCUC << "', "
				<< "Cuttable = '" << (unsigned short)nCuttable << "', "
				<< "I_STR = " << nSTR << ", "
				<< "I_DEX = " << nDEX << ", "
				<< "I_INT = " << nINT << ", "
				<< "I_LUK = " << nLUK << ", "
				<< "I_MaxHP = " << nMaxHP << ", "
				<< "I_MaxMP = " << nMaxMP << ", "
				<< "I_PAD = " << nPAD << ", "
				<< "I_MAD = " << nMAD << ", "
				<< "I_PDD = " << nPDD << ", "
				<< "I_MDD = " << nMDD << ", "
				<< "I_ACC = " << nACC << ", "
				<< "I_EVA = " << nEVA << ", "
				<< "I_Speed = " << nSpeed << ", "
				<< "I_Craft = " << nCraft << ", "
				<< "I_Jump = '" << nJump << "'";
		}
		queryStatement.execute();
	}
	catch (Poco::Data::MySQL::StatementException & se) 
	{
		WvsLogger::LogFormat("SQL Exception Occurred: %s\nRaw Query = %s\n", se.message(), sQuery.c_str());
	}
}

/*
Equip Encoder Entry (Then Encoding Position Info And Equip Attributes Info.)
*/
void GW_ItemSlotEquip::Encode(OutPacket *oPacket, bool bForInternal) const
{
	EncodeInventoryPosition(oPacket);
	if (bForInternal)
		oPacket->Encode8(liItemSN);
	RawEncode(oPacket);
}

/*
Encode Equip Information
*/
void GW_ItemSlotEquip::RawEncode(OutPacket *oPacket) const
{
	GW_ItemSlotBase::RawEncode(oPacket);
	EncodeEquipBase(oPacket);
	EncodeEquipAdvanced(oPacket);
}

/*
Encode Equip Basic Information
*/
//ADD_EQUIP_FLAG cause compiler think it will convert short to char, but it actually wont.
#pragma warning(disable:4244)  
void GW_ItemSlotEquip::EncodeEquipBase(OutPacket *oPacket) const
{
	oPacket->Encode1(nRUC);
	oPacket->Encode1(nCUC);
	oPacket->Encode2(nSTR);
	oPacket->Encode2(nDEX);
	oPacket->Encode2(nINT);
	oPacket->Encode2(nLUK);
	oPacket->Encode2(nMaxHP);
	oPacket->Encode2(nMaxHP);
	oPacket->Encode2(nPAD);
	oPacket->Encode2(nMAD);
	oPacket->Encode2(nPDD);
	oPacket->Encode2(nMDD);
	oPacket->Encode2(nACC);
	oPacket->Encode2(nEVA);
	oPacket->Encode2(nCraft);
	oPacket->Encode2(nSpeed);
	oPacket->Encode2(nJump);
}
#pragma warning(default:4244)  

/*
Encode Equip Advanced Information, Including Potential, Sockets, And Etc.
*/
void GW_ItemSlotEquip::EncodeEquipAdvanced(OutPacket *oPacket) const
{
	oPacket->EncodeStr(sTitle);
	oPacket->Encode2(nAttribute);

	if (liCashItemSN == -1)
		oPacket->Encode8(liItemSN);
}

void GW_ItemSlotEquip::Decode(InPacket *iPacket, bool bForInternal)
{
	if (bForInternal)
		liItemSN = iPacket->Decode8();
	RawDecode(iPacket);
}

void GW_ItemSlotEquip::RawDecode(InPacket *iPacket)
{
	GW_ItemSlotBase::RawDecode(iPacket);
	DecodeEquipBase(iPacket);
	DecodeEquipAdvanced(iPacket);
}

#pragma warning(disable:4244)  
void GW_ItemSlotEquip::DecodeEquipBase(InPacket *iPacket)
{
	nRUC = iPacket->Decode1();
	nCUC = iPacket->Decode1();
	nSTR = iPacket->Decode2();
	nDEX = iPacket->Decode2();
	nINT = iPacket->Decode2();
	nLUK = iPacket->Decode2();
	nMaxHP = iPacket->Decode2();
	nMaxHP = iPacket->Decode2();
	nPAD = iPacket->Decode2();
	nMAD = iPacket->Decode2();
	nPDD = iPacket->Decode2();
	nMDD = iPacket->Decode2();
	nACC = iPacket->Decode2();
	nEVA = iPacket->Decode2();
	nSpeed = iPacket->Decode2();
	nCraft = iPacket->Decode2();
	nJump = iPacket->Decode2();
}
#pragma warning(default:4244) 

void GW_ItemSlotEquip::DecodeEquipAdvanced(InPacket *iPacket)
{
	sTitle = iPacket->DecodeStr();
	nAttribute = iPacket->Decode2();

	if (liCashItemSN == -1)
		liItemSN = iPacket->Decode8();
}

GW_ItemSlotBase * GW_ItemSlotEquip::MakeClone() const
{
	GW_ItemSlotEquip* ret = AllocObj(GW_ItemSlotEquip);
	*ret = *this;
	ret->liItemSN = -1;
	return ret;
}
