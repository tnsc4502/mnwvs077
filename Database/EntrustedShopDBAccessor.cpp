#include "EntrustedShopDBAccessor.h"
#include "WvsUnified.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\WvsLib\Logger\WvsLogger.h"

void EntrustedShopDBAccessor::MoveItemToShop(int nCharacterID, int nTI, long long int liItemSN)
{
	if (nTI < 1 || nTI > 4)
		return;

	std::string asTableName[] = { "", "EQP", "CON", "INS", "ETC" };
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "INSERT INTO EntrustedShop_" << asTableName[nTI] << " (CharacterID, SN, Locked) VALUES(";
	queryStatement << nCharacterID << ", "
		<< liItemSN << ", 0) ";
	queryStatement.execute();
}

void EntrustedShopDBAccessor::RestoreItemFromShop(int nCharacterID, int nTI, long long int liItemSN, bool bRemoveFromItemSlot)
{
	if (nTI < 1 || nTI > 4)
		return;

	std::string asTableName[] = { "", "EQP", "CON", "INS", "ETC" };
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "DELETE FROM EntrustedShop_" << asTableName[nTI] << " WHERE CharacterID = " << nCharacterID << " AND SN = " << liItemSN;
	queryStatement.execute();
	if (bRemoveFromItemSlot)
	{
		queryStatement.reset(GET_DB_SESSION);
		queryStatement << "DELETE FROM ItemSlot_" << asTableName[nTI] << " WHERE ItemSN = " << liItemSN;
		queryStatement.execute();
	}
}

void EntrustedShopDBAccessor::LoadEntrustedShopItem(std::vector<GW_ItemSlotBase*>& aItemRet, int nCharacterID)
{
	std::string asTableName[] = { "", "EQP", "CON", "INS", "ETC" };
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	for (int nTI = 1; nTI <= 4; ++nTI)
	{
		queryStatement.reset(GET_DB_SESSION);
		queryStatement << "SELECT SN FROM EntrustedShop_" << asTableName[nTI] << " Where CharacterID = " << nCharacterID;
		queryStatement.execute();
		Poco::Data::RecordSet recordSet(queryStatement);
		GW_ItemSlotBase* pItem = nullptr;
		for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
		{
			pItem = GW_ItemSlotBase::CreateItem(
				nTI == GW_ItemSlotBase::EQUIP ?
				GW_ItemSlotBase::GW_ItemSlotEquip_Type :
				GW_ItemSlotBase::GW_ItemSlotBundle_Type
			);
			pItem->nType = (GW_ItemSlotBase::GW_ItemSlotType)nTI;
			pItem->Load(recordSet["SN"]);
			aItemRet.push_back(pItem);
		}
	}
}

long long int EntrustedShopDBAccessor::QueryEntrustedShopMoney(int nCharacterID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT Money FROM EntrustedShop Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0)
		return 0;

	return recordSet["Money"];
}

void EntrustedShopDBAccessor::UpdateEntrustedShopMoney(int nCharacterID, int nMoney)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "UPDATE EntrustedShop Set Money = " << nMoney << " WHERE CharacterID = " << nCharacterID;
	queryStatement.execute();
}
