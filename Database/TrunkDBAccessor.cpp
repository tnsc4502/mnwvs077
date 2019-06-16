#include "TrunkDBAccessor.h"
#include "GW_ItemSlotBundle.h"
#include "GW_ItemSlotEquip.h"
#include "WvsUnified.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"

std::pair<int, int> TrunkDBAccessor::LoadTrunk(int nAccountID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT SlotCount, Money FROM Trunk Where AccountID = " << nAccountID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	if (recordSet.rowCount() == 0)
		return{ -1, -1 };

	return std::pair<int, int> { recordSet["SlotCount"], recordSet["Money"] };
}

void TrunkDBAccessor::UpdateTrunk(int nAccountID, int nMoney, int nSlotCount)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "INSERT INTO Trunk VALUES("
		<< nAccountID << ", "
		<< nSlotCount << ", "
		<< nMoney << ") ON DUPLICATE KEY UPDATE "
		<< "SlotCount = " << nSlotCount << ", "
		<< "Money = " << nMoney;

	queryStatement.execute();
}

std::vector<ZSharedPtr<GW_ItemSlotBase>> TrunkDBAccessor::LoadTrunkEquip(int nAccountID)
{
	std::vector<ZSharedPtr<GW_ItemSlotBase>> aRet;
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT SN FROM Trunk_EQP Where AccountID = " << nAccountID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	GW_ItemSlotEquip* pEquip = nullptr;
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		pEquip = AllocObj(GW_ItemSlotEquip);
		pEquip->nType = GW_ItemSlotBase::EQUIP;
		pEquip->Load(recordSet["SN"]);
		aRet.push_back(pEquip);
	}
	return aRet;
}

std::vector<ZSharedPtr<GW_ItemSlotBase>> TrunkDBAccessor::LoadTrunkEtc(int nAccountID)
{
	std::vector<ZSharedPtr<GW_ItemSlotBase>> aRet;
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT SN FROM Trunk_ETC Where AccountID = " << nAccountID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	GW_ItemSlotBundle* pBundle = nullptr;
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		pBundle = AllocObj(GW_ItemSlotBundle);
		pBundle->nType = GW_ItemSlotBase::ETC;
		pBundle->Load(recordSet["SN"]);
		aRet.push_back(pBundle);
	}
	return aRet;
}

std::vector<ZSharedPtr<GW_ItemSlotBase>> TrunkDBAccessor::LoadTrunkInstall(int nAccountID)
{
	std::vector<ZSharedPtr<GW_ItemSlotBase>> aRet;
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT SN FROM Trunk_INS Where AccountID = " << nAccountID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	GW_ItemSlotBundle* pBundle = nullptr;
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		pBundle = AllocObj(GW_ItemSlotBundle);
		pBundle->nType = GW_ItemSlotBase::INSTALL;
		pBundle->Load(recordSet["SN"]);
		aRet.push_back(pBundle);
	}
	return aRet;
}

std::vector<ZSharedPtr<GW_ItemSlotBase>> TrunkDBAccessor::LoadTrunkConsume(int nAccountID)
{
	std::vector<ZSharedPtr<GW_ItemSlotBase>> aRet;
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT SN FROM Trunk_CON Where AccountID = " << nAccountID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);
	GW_ItemSlotBundle* pBundle = nullptr;
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		pBundle = AllocObj(GW_ItemSlotBundle);
		pBundle->nType = GW_ItemSlotBase::CONSUME;
		pBundle->Load(recordSet["SN"]);
		aRet.push_back(pBundle);
	}
	return aRet;
}

void TrunkDBAccessor::MoveSlotToTrunk(int nAccountID, long long int liItemSN, int nTI)
{
	std::string sSuffix = "";
	if (nTI == GW_ItemSlotBase::EQUIP)
		sSuffix = "EQP";
	else if (nTI == GW_ItemSlotBase::CONSUME)
		sSuffix = "CON";
	else if (nTI == GW_ItemSlotBase::ETC)
		sSuffix = "ETC"; 
	else if (nTI == GW_ItemSlotBase::INSTALL)
		sSuffix = "INS";

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "INSERT INTO Trunk_" << sSuffix << " VALUES ( " << nAccountID << ", " << liItemSN << " )";
	queryStatement.execute();
}

void TrunkDBAccessor::MoveTrunkToSlot(int nAccountID, long long int liItemSN, int nTI, bool bTreatSingly)
{
	std::string sSuffix = "";
	if (nTI == GW_ItemSlotBase::EQUIP)
		sSuffix = "EQP";
	else if (nTI == GW_ItemSlotBase::CONSUME)
		sSuffix = "CON";
	else if (nTI == GW_ItemSlotBase::ETC)
		sSuffix = "ETC";
	else if (nTI == GW_ItemSlotBase::INSTALL)
		sSuffix = "INS";

	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "DELETE FROM Trunk_" << sSuffix << " WHERE AccountID = " << nAccountID << " AND SN = " << liItemSN;
	queryStatement.execute();
	if (!bTreatSingly)
	{
		queryStatement.reset(GET_DB_SESSION);
		queryStatement << "DELETE FROM ItemSlot_" << sSuffix << " WHERE ItemSN = " << liItemSN;
		queryStatement.execute();
	}
}
