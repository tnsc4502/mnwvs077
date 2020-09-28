#include "InventoryManipulator.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterSlotCount.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\Database\GW_CharacterMoney.h"
#include "BackupItem.h"
#include "ExchangeElement.h"
#include "ItemInfo.h"
#include "SkillInfo.h"

#include "..\WvsGame\UserPacketTypes.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

InventoryManipulator::InventoryManipulator()
{
}


InventoryManipulator::~InventoryManipulator()
{
}

void InventoryManipulator::SwapSlot(GA_Character* pCharacterData, std::vector<ChangeLog>& aChangeLog, int nTI, int nPOS1, int nPOS2)
{
	auto pItemSrc = pCharacterData->mItemSlot[nTI][nPOS1];
	auto pItemDst = pCharacterData->mItemSlot[nTI][nPOS2];
	pItemSrc->nPOS = nPOS2;
	if(pItemDst)
		pItemDst->nPOS = nPOS1;

	if (pItemDst)
		pCharacterData->mItemSlot[nTI][nPOS1] = pItemDst;
	else
		pCharacterData->mItemSlot[nTI].erase(nPOS1);
	pCharacterData->mItemSlot[nTI][nPOS2] = pItemSrc;
	InventoryManipulator::InsertChangeLog(aChangeLog, ChangeType::Change_SlotPOSChanged, nTI, nPOS1, nullptr, nPOS2, 1);
}

bool InventoryManipulator::IsItemExist(GA_Character* pCharacterData, int nTI, int nItemID)
{
	return pCharacterData->GetItemCount(nTI, nItemID) > 0;
}

bool InventoryManipulator::RawIncMoney(GA_Character * pCharacterData, int nMoney)
{
	if(pCharacterData->mMoney->nMoney + nMoney < 0)
		return false;
	pCharacterData->mMoney->nMoney += nMoney;
	return true;
}

/*
Lock by caller
*/
bool InventoryManipulator::RawAddItem(GA_Character *pCharacterData, int nTI, ZSharedPtr<GW_ItemSlotBase>& pItem, std::vector<ChangeLog>* aChangeLog, int * nIncRet, std::vector<BackupItem>* paBackupItem)
{
	if (nTI < GW_ItemSlotBase::EQUIP || nTI > GW_ItemSlotBase::CASH)
		return false;
	auto& itemSlot = pCharacterData->mItemSlot[nTI];
	if (ItemInfo::IsTreatSingly(pItem->nItemID, pItem->liExpireDate))
	{
		short nPOS = pCharacterData->FindEmptySlotPosition(nTI);
		if (nPOS > 0) 
		{
			//09/12/2019 modified, record restoration of cash items is ignored.
			if (pItem->liItemSN != -1)
				pCharacterData->mItemRemovedRecord[nTI].erase({ pItem->liItemSN, false });
			itemSlot[nPOS] = pItem;
			pItem->nPOS = nPOS;
			if (pItem->liItemSN <= 0)
				pItem->liItemSN = GW_ItemSlotBase::GetNextSN(nTI);

			if (paBackupItem)
				(*paBackupItem).push_back({ nTI, nPOS, nullptr });
			if(aChangeLog)
				InsertChangeLog(*aChangeLog, ChangeType::Change_AddToSlot, nTI, nPOS, pItem, 0, 0);
			*nIncRet = 1;
			return true;
		}
		return false;
	}

	if (ItemInfo::GetInstance()->GetBundleItem(pItem->nItemID) != nullptr)
	{
		int nMaxPerSlot = SkillInfo::GetInstance()->GetBundleItemMaxPerSlot(pItem->nItemID, pCharacterData),
			nPOS = 1,
			nOnTrading = 0,
			nRemaining = 0,
			nSlotInc = 0,
			nTotalInc = 0,
			nNumber = ((GW_ItemSlotBundle*)pItem)->nNumber;

		for(auto& pos : itemSlot)
		{
			nPOS = pos.first;
			if (nPOS <= 0)
				continue;
			if (nPOS > pCharacterData->mSlotCount->aSlotCount[nTI])
				break;

			ZSharedPtr<GW_ItemSlotBase> pItemInSlot = (pCharacterData->GetItem(nTI, nPOS));
			
			//Item at slot[pos] has same item id as pItem.
			if (!ItemInfo::IsRechargable(pItem->nItemID) && 
				pItemInSlot != nullptr && 
				pItemInSlot->nItemID == pItem->nItemID)
			{
				//To calculate how many spaces (of the slot) are available.
				nRemaining = (nMaxPerSlot - ((GW_ItemSlotBundle*)pItemInSlot)->nNumber);
				
				//This slot is full, continue to find next item with the same item id.
				if (nRemaining <= 0)
					continue;
				nOnTrading = pCharacterData->mItemTrading[nTI][nPOS];
				nSlotInc = nNumber > nRemaining ? nRemaining : (nNumber); //The number allowed to put into slot.
				if (nSlotInc - nOnTrading > 0)
				{
					if (paBackupItem) 
					{
						auto pBackup = pItemInSlot->MakeClone();
						pBackup->liCashItemSN = pItemInSlot->liCashItemSN;
						pBackup->liItemSN = pItemInSlot->liItemSN;
						(*paBackupItem).push_back({ nTI, nPOS, pBackup });
					}
					((GW_ItemSlotBundle*)pItemInSlot)->nNumber += (nSlotInc - nOnTrading);
					if(aChangeLog)
						InsertChangeLog(*aChangeLog, ChangeType::Change_QuantityChanged, nTI, nPOS, pItemInSlot, 0, ((GW_ItemSlotBundle*)pItemInSlot)->nNumber);
				}
				else
				{
					//Official server separates trading items here; however, the difference between our's and official's impl. seems enable us to ignore this.
					//When players are trading, picking up item is denied as well, entrusted shop is also nothing to do with trading (while in official server it does).
				}
				nNumber -= nSlotInc;
				nTotalInc += nSlotInc;
				if (nNumber <= 0)
					break;
			}
		}

		//There are no available items with the same item id that still have empty space, find an empty slot if any.
		while (nNumber > 0)
		{
			WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "InventoryManipulator::RawAddItem nItemID = %d nNumber = %d nMaxPerSlot = %d Test = %d\n", pItem->nItemID, nNumber, nMaxPerSlot, (int)(ItemInfo::GetInstance()->GetBundleItem(pItem->nItemID) == nullptr));
			nPOS = pCharacterData->FindEmptySlotPosition(nTI);

			//Inventory is full, post failure notification.
			if (nPOS <= 0 || nPOS > pCharacterData->mSlotCount->aSlotCount[nTI])
			{
				((GW_ItemSlotBundle*)pItem)->nNumber = nNumber;
				*nIncRet = nTotalInc;
				return false;
			}

			/*
			If the remaining number [nSlotInc] of pItem is less or equal to nMaxPerSlot (The maximum number of the item per slot allowed), just set pItem at [slot].
			Otherwise, clone a pItem named [pClone] with the nNumber [nMaxPerSlot], and modify the nNumber of pItem to [nSlotInc - nMaxPerSlot].
			*/
			ZSharedPtr<GW_ItemSlotBase> pClone = nNumber > nMaxPerSlot ? 
				ZSharedPtr<GW_ItemSlotBase>(pItem->MakeClone()) : pItem;

			nSlotInc = nNumber > nMaxPerSlot ? nMaxPerSlot : (nNumber); //The maximum quantity allowd to set.
			((GW_ItemSlotBundle*)pClone)->nNumber = nSlotInc;

			//09/12/2019 modified, record restoration of cash items is ignored.
			if(pClone->liItemSN != -1)
				pCharacterData->mItemRemovedRecord[nTI].erase({ pClone->liItemSN, false });
			if (paBackupItem)
				(*paBackupItem).push_back({ nTI, nPOS, nullptr });
			itemSlot[nPOS] = pClone;
			pClone->nPOS = nPOS;
			if (pClone->liItemSN <= 0)
				pClone->liItemSN = GW_ItemSlotBase::GetNextSN(nTI);

			if(aChangeLog)
				InsertChangeLog(*aChangeLog, ChangeType::Change_AddToSlot, nTI, nPOS, pClone, 0, 0);
			nNumber -= nSlotInc;
			nTotalInc += nSlotInc;
		}
		*nIncRet = nTotalInc;
	}
	return true;
}

/*
Lock by caller
*/
bool InventoryManipulator::RawAddItem(GA_Character *pCharacterData, int nTI, int nItemID, int nCount, std::vector<ChangeLog>* aChangeLog, int * nIncRet, std::vector<BackupItem>* paBackupItem)
{
	if (nTI < GW_ItemSlotBase::EQUIP || nTI > GW_ItemSlotBase::CASH)
		return false;
	ZSharedPtr<GW_ItemSlotBase> pItem = (ItemInfo::GetInstance()->GetItemSlot(nItemID, ItemInfo::ITEMVARIATION_NORMAL));
	if (pItem == nullptr)
		return false;
	if (nTI != GW_ItemSlotBase::EQUIP)
		((GW_ItemSlotBundle*)pItem)->nNumber = nCount;
	return RawAddItem(pCharacterData, nTI, pItem, aChangeLog, nIncRet, paBackupItem);
}

void InventoryManipulator::InsertChangeLog(std::vector<ChangeLog>& aChangeLog, int nChange, int nTI, int nPOS, const ZSharedPtr<GW_ItemSlotBase>& pi, int nPOS2, int nNumber)
{
	ChangeLog newLog;
	newLog.nChange = nChange;
	newLog.nTI = nTI;
	newLog.nPOS = nPOS;
	newLog.nPOS2 = nPOS2;
	newLog.nNumber = nNumber;
	newLog.pItem = pi;
	aChangeLog.push_back(newLog);
}

bool InventoryManipulator::MakeInventoryOperation(OutPacket *oPacket, int bOnExclResult, std::vector<InventoryManipulator::ChangeLog>& aChangeLog)
{
	bool bAddMovementInfo = false;
	oPacket->Encode2((short)UserSendPacketType::UserLocal_OnInventoryOperation);
	oPacket->Encode1(bOnExclResult);
	oPacket->Encode1((char)aChangeLog.size());
	for (auto& change : aChangeLog)
	{
		oPacket->Encode1((char)change.nChange);
		oPacket->Encode1((char)change.nTI);
		oPacket->Encode2((short)change.nPOS);
		if (change.nChange)
		{
			if (change.nChange == ChangeType::Change_QuantityChanged)
				oPacket->Encode2((short)change.nNumber);
			else if (change.nChange == ChangeType::Change_SlotPOSChanged)
			{
				if (change.nTI == GW_ItemSlotBase::EQUIP && change.nPOS < 0 || change.nPOS2 < 0)
					bAddMovementInfo = true;
				oPacket->Encode2((short)change.nPOS2);
			}
			else if (change.nChange == 3 && change.nTI == GW_ItemSlotBase::EQUIP && change.nPOS < 0)
				bAddMovementInfo = true;
		}
		else 
			change.pItem->RawEncode(oPacket);
	}
	return bAddMovementInfo;
}

void InventoryManipulator::MakeItemUpgradeEffect(OutPacket *oPacket, int nCharacterID, int nEItemID, int nUItemID, bool bSuccess, bool bCursed, bool bEnchant)
{
	oPacket->Encode2((short)UserSendPacketType::UserCommon_ShowItemUpgradeEffect);
	oPacket->Encode4(nCharacterID);
	oPacket->Encode1(bSuccess ? 1 : (bCursed ? 2 : 0));
	oPacket->Encode1(bEnchant);
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode1(0);
	oPacket->Encode1(0);
}

/*
Lock by caller
Warning: 
ppBackupItem is given exclusively only by "RawExchange" and it requires "ItemSN" to be the original ItemSN, while ppItemRemove does not require such consistency.
Both ppBackupItem and ppItemRemoved can't be non-nullptr at same time or it would bring undefined results.
*/
bool InventoryManipulator::RawRemoveItem(GA_Character *pCharacterData, int nTI, int nPOS, int nCount, std::vector<ChangeLog>* aChangeLog, int * nDecRet, ZSharedPtr<GW_ItemSlotBase>* ppItemRemoved, ZSharedPtr<GW_ItemSlotBase>* ppBackupItem)
{
	auto pItem = pCharacterData->GetItem(nTI, nPOS);
	auto pClone = ppItemRemoved ? ppItemRemoved : (ppBackupItem ? ppBackupItem : nullptr);
	bool bSufficientCount = true;
	if (pItem != nullptr)
	{
		int nRemaining = 0;
		if (ItemInfo::IsTreatSingly(pItem->nItemID, pItem->liExpireDate) && nTI == 1)
			pCharacterData->RemoveItem(nTI, nPOS);
		else if (nCount >= 1)
		{
			GW_ItemSlotBundle* pBundle = (GW_ItemSlotBundle*)pItem;
			int nInSlotCount = pBundle->nNumber;
			if (ItemInfo::IsRechargable(pItem->nItemID))
				nCount = pBundle->nNumber;

			if (nCount > nInSlotCount)
			{
				bSufficientCount = false;
				nCount = nInSlotCount;
			}

			pBundle->nNumber -= nCount;
			nRemaining = pBundle->nNumber;
			if (nRemaining <= 0)
				pCharacterData->RemoveItem(nTI, nPOS);
		}

		//If pClone is non-nullptr, then point it to pItem.
		//Notice that if pItem is valid (nNumber > 0), we need a cloned pItem.
		//If ppItemBackup is passed, set ItemSN/CashItemSN to original SNs for fast backup.
		if (pClone)
		{
			if (nRemaining <= 0)
				*pClone = pItem;
			else
				pClone->reset(pItem->MakeClone());

			if (pClone == ppBackupItem) 
			{
				(*pClone)->liItemSN = pItem->liItemSN;
				(*pClone)->liCashItemSN = pItem->liCashItemSN;
			}
		}

		if (pClone && nCount >= 1 && nTI != GW_ItemSlotBase::EQUIP)
			((GW_ItemSlotBundle*)*pClone)->nNumber = nCount;

		*nDecRet = nCount;
		if (nRemaining > 0 && aChangeLog)
			InsertChangeLog(*aChangeLog, ChangeType::Change_QuantityChanged, nTI, nPOS, pItem, 0, nRemaining);
		else if(aChangeLog)
			InsertChangeLog(*aChangeLog, ChangeType::Change_RemoveFromSlot, nTI, nPOS, pItem, 0, *nDecRet);
	}
	else
		return false;
	return bSufficientCount;
}

bool InventoryManipulator::RawWasteItem(GA_Character * pCharacterData, int nPOS, int nCount, std::vector<ChangeLog>* aChangeLog)
{
	GW_ItemSlotBundle* pItem = (GW_ItemSlotBundle*)pCharacterData->GetItem(GW_ItemSlotBase::CONSUME, nPOS);
	if (!pItem || pItem->nNumber < nCount)
		return false;
	if (!ItemInfo::GetInstance()->IsRechargable(pItem->nItemID))
		return false;
	pItem->nNumber -= nCount;
	if (aChangeLog)
		InventoryManipulator::InsertChangeLog(
			*aChangeLog, ChangeType::Change_QuantityChanged, GW_ItemSlotBase::CONSUME, nPOS, nullptr, 0, pItem->nNumber
		);
	return true;
}

bool InventoryManipulator::RawRechargeItem(GA_Character * pCharacterData, int nPOS, std::vector<ChangeLog>* aChangeLog)
{
	GW_ItemSlotBundle* pItem = (GW_ItemSlotBundle*)pCharacterData->GetItem(GW_ItemSlotBase::CONSUME, nPOS);
	int nMaxPerSlot = 0;
	if (!pItem ||
		!ItemInfo::GetInstance()->IsRechargable(pItem->nItemID) ||
		!(nMaxPerSlot = SkillInfo::GetInstance()->GetBundleItemMaxPerSlot(pItem->nItemID, pCharacterData)) ||
			pItem->nNumber > nMaxPerSlot)
		return false;
	pItem->nNumber = nMaxPerSlot;
	if (aChangeLog)
		InsertChangeLog(
			*aChangeLog,
			ChangeType::Change_QuantityChanged,
			GW_ItemSlotBase::CONSUME,
			nPOS,
			nullptr,
			0,
			nMaxPerSlot);
	return true;
}

int InventoryManipulator::RawExchange(GA_Character *pCharacterData, int nMoney, std::vector<ExchangeElement>& aExchange, std::vector<ChangeLog>* aLogAdd, std::vector<ChangeLog>* aLogRemove, std::vector<ChangeLog>* aLogDefault, std::vector<BackupItem>& aBackupItem)
{
	int nDel = 0, nAdd = 0, nRemovedAtCurrentSlot = 0, nCount = 0;
	for (auto& elem : aExchange)
	{
		if (elem.m_nCount < 0)
		{
			nCount = elem.m_nCount * -1;
			while (nCount > 0)
			{
				nRemovedAtCurrentSlot = 0;
				if (elem.m_pItem) //Exchange by a GW_ItemSlotBase instance.
				{
					aBackupItem.push_back({ elem.m_pItem->nType, elem.m_pItem->nPOS });
					RawRemoveItem(
						pCharacterData,
						elem.m_pItem->nType,
						elem.m_pItem->nPOS,
						nCount,
						aLogRemove ? aLogRemove : aLogDefault,
						&nRemovedAtCurrentSlot, 
						nullptr,
						&(aBackupItem.back().m_pItem));
				}
				else //Exchange by a specified nItemID.
				{
					auto pItem = pCharacterData->GetItemByID(elem.m_nItemID);
					if (pItem == nullptr)
						break;
					aBackupItem.push_back({ pItem->nType, pItem->nPOS });
					RawRemoveItem(
						pCharacterData, 
						pItem->nType, 
						pItem->nPOS, 
						nCount, 
						aLogRemove ? aLogRemove : aLogDefault,
						&nRemovedAtCurrentSlot,
						nullptr,
						&(aBackupItem.back().m_pItem));
				}
				nCount -= nRemovedAtCurrentSlot;
			}
			if(nCount != 0)
			{
				RestoreBackupItem(pCharacterData, aBackupItem);
				return Exchange_InsufficientItemCount;
			}
		}
		else
		{
			if ((elem.m_pItem != nullptr && !RawAddItem(pCharacterData, elem.m_pItem->nType, elem.m_pItem, aLogAdd ? aLogAdd : aLogDefault, &nAdd, &aBackupItem)) //Exchange by a GW_ItemSlotBase instance.
				|| (elem.m_pItem == nullptr && !RawAddItem(pCharacterData, elem.m_nItemID / 1000000, elem.m_nItemID, elem.m_nCount, aLogAdd ? aLogAdd : aLogDefault, &nAdd, &aBackupItem))) //Exchange by a specified nItemID.
			{
				RestoreBackupItem(pCharacterData, aBackupItem);
				return Exchange_InsufficientSlotCount;
			}
			elem.m_nCount = 0;
		}
	}
	if (!RawIncMoney(pCharacterData, nMoney))
	{
		RestoreBackupItem(pCharacterData, aBackupItem);
		return Exchange_InsufficientMeso;
	}
	return Exchange_Success;
}

//Rolling back modified slots when "Exchange" failed.
void InventoryManipulator::RestoreBackupItem(GA_Character * pCharacterData, std::vector<BackupItem>& aBackupItem)
{
	for(int i = 0; i < aBackupItem.size(); )
	{
		auto& refItemBackup = (aBackupItem)[i];
		if (refItemBackup.m_pItem == nullptr)
			pCharacterData->mItemSlot[refItemBackup.m_nTI].erase(refItemBackup.m_nPOS);
		else 
		{
			pCharacterData->SetItem(refItemBackup.m_nTI, refItemBackup.m_nPOS, refItemBackup.m_pItem);
			//09/12/2019 modified, record restoration of cash items is ignored.
			pCharacterData->mItemRemovedRecord[refItemBackup.m_nTI].erase({ refItemBackup.m_pItem->liItemSN, false });
			aBackupItem.erase(aBackupItem.begin() + i);
			continue;
		}
		++i;
	}
}

void InventoryManipulator::RestoreTradingInventory(GA_Character *pCharacterData, std::map<int, int> mBackupItemTrading[6], std::vector<InventoryManipulator::ChangeLog> &aChangeLog)
{
	for (int nTI = 1; nTI <= 5; ++nTI)
		for (auto& prItem : mBackupItemTrading[nTI])
		{
			auto pItem = pCharacterData->GetItem(nTI, prItem.first);
			if (pItem)
				InventoryManipulator::InsertChangeLog(
					aChangeLog,
					InventoryManipulator::ChangeType::Change_AddToSlot,
					nTI,
					prItem.first,
					pItem,
					0,
					pItem->nType == GW_ItemSlotBase::EQUIP ?
					1 : ((GW_ItemSlotBundle*)pItem)->nNumber
				);
		}
}
