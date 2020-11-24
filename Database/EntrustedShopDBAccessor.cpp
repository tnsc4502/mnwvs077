#include "EntrustedShopDBAccessor.h"
#include "WvsUnified.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\WvsLib\Logger\WvsLogger.h"

const static std::string asTableName[] = { "", "EQP", "CON", "INS", "ETC" };

void EntrustedShopDBAccessor::MoveItemToShop(int nCharacterID, int nTI, long long int liItemSN)
{
	if (nTI < 1 || nTI > 4)
		return;

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "INSERT INTO EntrustedShop_" << asTableName[nTI] << " (CharacterID, SN, Locked) VALUES(";
	queryStatement << nCharacterID << ", "
		<< liItemSN << ", 0) ON DUPLICATE KEY UPDATE Locked = 0";
	queryStatement.execute();
}

void EntrustedShopDBAccessor::RestoreItemFromShop(int nCharacterID, int nTI, long long int liItemSN, bool bRemoveFromItemSlot)
{
	if (nTI < 1 || nTI > 4)
		return;

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

void EntrustedShopDBAccessor::LoadEntrustedShopItem(std::vector<ZUniquePtr<GW_ItemSlotBase>>& aItemRet, int nCharacterID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	for (int nTI = 1; nTI <= 4; ++nTI)
	{
		queryStatement.reset(GET_DB_SESSION);
		queryStatement << "SELECT SN FROM EntrustedShop_" << asTableName[nTI] << " Where CharacterID = " << nCharacterID;
		queryStatement.execute();
		Poco::Data::RecordSet recordSet(queryStatement);
		for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
		{
			ZUniquePtr<GW_ItemSlotBase> pItem = GW_ItemSlotBase::CreateItem(
				nTI == GW_ItemSlotBase::EQUIP ?
				GW_ItemSlotBase::GW_ItemSlotEquip_Type :
				GW_ItemSlotBase::GW_ItemSlotBundle_Type
			);
			pItem->nType = (GW_ItemSlotBase::GW_ItemSlotType)nTI;
			pItem->Load(recordSet["SN"]);
			aItemRet.push_back(std::move(pItem));
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
	queryStatement << "INSERT INTO EntrustedShop VALUES("
		<< nCharacterID << ", "
		<< nMoney << ") ON DUPLICATE KEY UPDATE "
		<< "Money = " << nMoney;
	queryStatement.execute();
}

std::vector<std::pair<int, long long int>> EntrustedShopDBAccessor::QueryItemExistence(int nWorldID, int nItemID)
{
	std::vector<std::pair<int, long long int>> aRet;
	int nTI = nItemID / 1000000;
	if (nTI < 1 || nTI > 4)
		return aRet;

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT Character.CharacterID, EntrustedShop_" << asTableName[nTI] << ".SN as ItemSN FROM EntrustedShop_" << asTableName[nTI] 
		<< " INNER JOIN `Character` ON Character.CharacterID = EntrustedShop_" << asTableName[nTI] << ".CharacterID" 
		<< " INNER JOIN ItemSlot_" << asTableName[nTI] << " ON ItemSlot_" << asTableName[nTI] << ".ItemSN = EntrustedShop_" << asTableName[nTI] << ".SN"
		<< " WHERE Character.WorldID = " << nWorldID << " AND ItemSlot_" << asTableName[nTI] << ".ItemID = " << nItemID;

	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
		aRet.push_back({ (int)recordSet["CharacterID"], (long long int)recordSet["ItemSN"] });

	return aRet;
}
