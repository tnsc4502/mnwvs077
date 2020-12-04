#include "QWUInventory.h"
#include  "User.h"
#include "Reward.h"
#include "Field.h"
#include "DropPool.h"
#include "ItemInfo.h"
#include "BackupItem.h"
#include "SkillInfo.h"
#include "QWUser.h"
#include "BasicStat.h"

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_CharacterSlotCount.h"

#include <algorithm>

QWUInventory::QWUInventory()
{
}


QWUInventory::~QWUInventory()
{
}

bool QWUInventory::ChangeSlotPosition(User * pUser, int bOnExclRequest, int nTI, int nPOS1, int nPOS2, int nCount, int tRequestTime)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto &pCharacterData = pUser->GetCharacterData();
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	int nMovedCount = 0;
	if (pCharacterData->mStat->nHP && nPOS1 != nPOS2)
	{
		//Drop out the item from inventory.
		if (nPOS2 == 0 && pUser->GetField())
		{
			auto pItem = pCharacterData->GetItem(nTI, nPOS1);
			if (!pItem || pItem->IsProtectedItem())
				return 0;

			ZSharedPtr<GW_ItemSlotBase> pItemCopyed = nullptr;
			QWUInventory::RemoveItem(pUser, pItem, nCount, true, true, &pItemCopyed);
			if (pItemCopyed)
			{
				auto zpReward = MakeUnique<Reward>();
				zpReward->SetType(1);
				zpReward->SetItem(pItemCopyed);
				pUser->GetField()->GetDropPool()->Create(
					zpReward,
					pUser->GetUserID(),
					0,
					0,
					0,
					pUser->GetPosX(),
					pUser->GetPosY(),
					pUser->GetPosX(),
					pUser->GetPosY(),
					0,
					0,
					0,
					0);
			}
		}
		else if (nPOS1 > 0 && nPOS2 > 0) //Exchange the position of the item inside inventory.
		{
			auto pItemSrc = pCharacterData->GetItem(nTI, nPOS1);
			auto pItemDst = pCharacterData->GetItem(nTI, nPOS2);

			if (pItemSrc == nullptr && pItemDst == nullptr)
				return false;

			//Equip
			if (nTI == GW_ItemSlotBase::EQUIP || 
				(!pItemSrc || ItemInfo::IsRechargable(pItemSrc->nItemID) || pItemSrc->liExpireDate != -1) ||
				(!pItemDst || ItemInfo::IsRechargable(pItemDst->nItemID) || pItemDst->liExpireDate != -1))
				InventoryManipulator::SwapSlot(pCharacterData, aChangeLog, nTI, nPOS1, nPOS2);
			else
			{
				if (pItemDst != nullptr && pItemSrc->nItemID == pItemDst->nItemID)
				{
					int nMaxPerSlot = SkillInfo::GetInstance()->GetBundleItemMaxPerSlot(pItemSrc->nItemID, pCharacterData);
					int nSlotSrcCount = ((GW_ItemSlotBundle*)pItemSrc)->nNumber;
					int nSlotDstCount = ((GW_ItemSlotBundle*)pItemDst)->nNumber;
					if (nSlotSrcCount + nSlotDstCount <= nMaxPerSlot)
					{
						InventoryManipulator::RawRemoveItem(pCharacterData, nTI, nPOS1, nSlotSrcCount, &aChangeLog, &nMovedCount, nullptr);
						((GW_ItemSlotBundle*)pItemDst)->nNumber += nSlotSrcCount;
						InventoryManipulator::InsertChangeLog(aChangeLog, InventoryManipulator::Change_AddToSlot, nTI, nPOS2, pItemDst, 0, ((GW_ItemSlotBundle*)pItemDst)->nNumber);
					}
					else
					{
						((GW_ItemSlotBundle*)pItemDst)->nNumber = nMaxPerSlot;
						((GW_ItemSlotBundle*)pItemSrc)->nNumber = nMaxPerSlot = nSlotSrcCount + nSlotDstCount - nMaxPerSlot;
						InventoryManipulator::InsertChangeLog(aChangeLog, InventoryManipulator::Change_AddToSlot, nTI, nPOS1, pItemSrc, 0, ((GW_ItemSlotBundle*)pItemSrc)->nNumber);
						InventoryManipulator::InsertChangeLog(aChangeLog, InventoryManipulator::Change_AddToSlot, nTI, nPOS2, pItemDst, 0, ((GW_ItemSlotBundle*)pItemDst)->nNumber);
					}
				}
				else
					InventoryManipulator::SwapSlot(pCharacterData, aChangeLog, nTI, nPOS1, nPOS2);
			}
		}
		else if ((nPOS1 < 0 && nPOS2 > 0) || nPOS2 < 0) //Equip/Unequip the item.
		{
			auto pItemSrc = pCharacterData->GetItem(nTI, nPOS1);
			auto pItemDst = pCharacterData->GetItem(nTI, nPOS2);
			if (nTI == 1)
			{
				if (pItemSrc->nAttribute & GW_ItemSlotBase::ItemAttribute::eItemAttr_BlockTradeOnEquipped)
				{
					pItemSrc->nAttribute &= (~GW_ItemSlotBase::ItemAttribute::eItemAttr_BlockTradeOnEquipped);
					pItemSrc->nAttribute |= (GW_ItemSlotBase::ItemAttribute::eItemAttr_Untradable);
					InventoryManipulator::InsertChangeLog(aChangeLog, InventoryManipulator::Change_RemoveFromSlot, nTI, nPOS1, nullptr, nPOS2, 1);
					InventoryManipulator::InsertChangeLog(aChangeLog, InventoryManipulator::Change_AddToSlot, nTI, nPOS1, pItemSrc, nPOS2, 1);
				}
				InventoryManipulator::SwapSlot(pCharacterData, aChangeLog, nTI, nPOS1, nPOS2);
				pUser->OnAvatarModified();
			}
		}
	}
	if (nTI == GW_ItemSlotBase::EQUIP)
		pUser->ValidateStat();

	SendInventoryOperation(pUser, bOnExclRequest, aChangeLog);
	pUser->SendCharacterStat(true, 0);
	return false; //The return value is dispensable.
}

void QWUInventory::OnChangeSlotPositionRequest(User * pUser, InPacket * iPacket)
{
	//0xF7 0x00 [0x71 0xF2 0x4A 0x52] [0x01] [0xF9 0xFF] [0x01 0x00] [0xFF 0xFF]
	int tRequestTime = iPacket->Decode4();
	char nTI = iPacket->Decode1();
	short nPOS1 = iPacket->Decode2();
	short nPOS2 = iPacket->Decode2();
	int nCount = iPacket->Decode2();
	ChangeSlotPosition(pUser, true, nTI, nPOS1, nPOS2, nCount, tRequestTime);
}

bool QWUInventory::PickUpMoney(User* pUser, bool byPet, int nAmount)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	InventoryManipulator::RawIncMoney(pUser->GetCharacterData(), nAmount);

	auto liFlag = BasicStat::BasicStatFlag::BS_Meso;
	pUser->SendCharacterStat(true, liFlag);
	return true;
}

bool QWUInventory::PickUpItem(User * pUser, bool byPet, ZSharedPtr<GW_ItemSlotBase>& pItem)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	int nTotalInc = 0;
	bool result = false;
	if(pItem != nullptr)
		result = InventoryManipulator::RawAddItem(
			pUser->GetCharacterData(),
			pItem->nType,
			pItem,
			&aChangeLog,
			&nTotalInc
		);	
	OutPacket oPacket;
	InventoryManipulator::MakeInventoryOperation(&oPacket, true, aChangeLog);
	pUser->SendPacket(&oPacket);
	pUser->SendCharacterStat(true, 0);
	return result;
}

/*
pUser is locked here.
*/
bool QWUInventory::RawRemoveItemByID(User * pUser, int nItemID, int nCount)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto &pCharacterData = pUser->GetCharacterData();
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	while (nCount > 0)
	{
		auto pItem = pCharacterData->GetItemByID(nItemID);
		if (pItem == nullptr)
			break;
		int nRemovedAtCurrentSlot = 0;
		InventoryManipulator::RawRemoveItem(pCharacterData, pItem->nType, pItem->nPOS, nCount, &aChangeLog, &nRemovedAtCurrentSlot, nullptr);
		nCount -= nRemovedAtCurrentSlot;
	}

	OutPacket oPacket;
	InventoryManipulator::MakeInventoryOperation(&oPacket, true, aChangeLog);
	pUser->SendPacket(&oPacket);
	pUser->SendCharacterStat(true, 0);
	return nCount == 0;
}

bool QWUInventory::RawRemoveItem(User * pUser, int nTI, int nPOS, int nCount, std::vector<InventoryManipulator::ChangeLog>* aChangeLog, int & nDecRet, ZSharedPtr<GW_ItemSlotBase>* ppItemRemoved)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	if (pUser->GetCharacterData()->mStat->nHP != 0)
		return InventoryManipulator::RawRemoveItem(
			pUser->GetCharacterData(),
			nTI,
			nPOS,
			nCount,
			aChangeLog,
			&nDecRet,
			ppItemRemoved
		);
	return false;
}

int QWUInventory::RemoveItem(User * pUser, ZSharedPtr<GW_ItemSlotBase>& pItem, int nCount, bool bSendInventoryOperation, bool bOnExclResult, ZSharedPtr<GW_ItemSlotBase>* ppItemRemoved)
{
	std::vector<InventoryManipulator::ChangeLog> aChange;
	int nDec = 0;
	RawRemoveItem(
		pUser, pItem->nType, pItem->nPOS, nCount, &aChange, nDec, ppItemRemoved
	);

	if (bSendInventoryOperation)
		SendInventoryOperation(pUser, bOnExclResult, aChange);

	return nDec;
}

bool QWUInventory::RawRechargeItem(User * pUser, int nPOS, std::vector<InventoryManipulator::ChangeLog>* aChangeLog)
{
	if (QWUser::GetHP(pUser))
		return InventoryManipulator::RawRechargeItem(
			pUser->GetCharacterData(),
			nPOS,
			aChangeLog
		);
	return false;
}

int QWUInventory::Exchange(User *pUser, int nMoney, std::vector<ExchangeElement>& aExchange)
{
	std::vector<BackupItem> aBackup;
	return Exchange(pUser, nMoney, aExchange, nullptr, nullptr, aBackup);
}

int QWUInventory::Exchange(User *pUser, int nMoney, std::vector<ExchangeElement>& aExchange, std::vector<InventoryManipulator::ChangeLog>* aLogAdd, std::vector<InventoryManipulator::ChangeLog>* aLogRemove, std::vector<BackupItem>& aBackupItem, bool bSendOperation)
{
	std::vector<InventoryManipulator::ChangeLog> aLogDefault;
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nResult = InventoryManipulator::RawExchange(
		pUser->GetCharacterData(),
		nMoney,
		aExchange,
		aLogAdd,
		aLogRemove,
		&aLogDefault,
		aBackupItem
	);
	if (!nResult)
	{
		if (bSendOperation)
		{
			if (nMoney)
				pUser->SendCharacterStat(true, BasicStat::BasicStatFlag::BS_Meso);

			if (!aLogAdd && !aLogRemove)
				QWUInventory::SendInventoryOperation(pUser, false, aLogDefault);
			else
			{
				if (aLogRemove)
					QWUInventory::SendInventoryOperation(pUser, false, *aLogRemove);
				if (aLogAdd)
					QWUInventory::SendInventoryOperation(pUser, false, *aLogAdd);
			}
		}
		for (auto& elem : aExchange)
			if(elem.m_nItemID)
				pUser->ShowItemExchangeEffect(elem.m_nItemID, elem.m_nCount);
	}
	return nResult;
}

void QWUInventory::SendInventoryOperation(User* pUser, int bOnExclResult, std::vector<InventoryManipulator::ChangeLog>& aChangeLog)
{
	OutPacket oPacket;
	oPacket.Encode1(InventoryManipulator::MakeInventoryOperation(&oPacket, bOnExclResult, aChangeLog) ? 1 : 0);
	pUser->SendPacket(&oPacket);
}

void QWUInventory::OnUpgradeItemRequest(User * pUser, InPacket * iPacket)
{
	int tRequestTime = iPacket->Decode4();
	int nUPOS = iPacket->Decode2();
	int nEPOS = iPacket->Decode2();
	int nWhiteScroll = iPacket->Decode2();
	bool bEnchantSkill = iPacket->Decode1() == 1;
	UpgradeEquip(
		pUser,
		nUPOS,
		nEPOS,
		nWhiteScroll,
		bEnchantSkill,
		tRequestTime
	);
}

void QWUInventory::UpgradeEquip(User * pUser, int nUPOS, int nEPOS, int nWhiteScroll, bool bEnchantSkill, int tReqTime)
{
	SkillEntry *ppEntry = nullptr;
	if (!bEnchantSkill ||
		(SkillInfo::GetInstance()->GetSkillLevel(
			pUser->GetCharacterData(),
			1003,
			&ppEntry), ppEntry))
	{
		auto pUItem = pUser->GetCharacterData()->GetItem(GW_ItemSlotBase::CONSUME, nUPOS);
		auto pItem = (pUser->GetCharacterData()->GetItem(GW_ItemSlotBase::EQUIP, nEPOS));
		if (!pUItem || !pItem)
			return;

		auto pEItem = (GW_ItemSlotEquip*)pItem;
		std::vector<InventoryManipulator::ChangeLog> aChangeLog;
		auto pScroll = ItemInfo::GetInstance()->GetUpgradeItem(pUItem->nItemID);
		if (pScroll)
		{
			bool bCheckScroll = false, bSuccess = true, bCursed = false;
			int nDecScroll = 0;
			
			if ((pEItem->nRUC > 0) 
				&& (RawRemoveItem(pUser, GW_ItemSlotBase::CONSUME, nUPOS, 1, &aChangeLog, nDecScroll, nullptr), nDecScroll == 1)
			   )
			{
				--pEItem->nRUC;
				bCheckScroll = true;
				if ((Rand32::GetInstance()->Random() % 100) > (unsigned int)pScroll->nSuccessRate)
				{
					bSuccess = false;
					if (pScroll->nCursedRate 
						&& (Rand32::GetInstance()->Random() % 100) >= (unsigned int)pScroll->nCursedRate)
						bCursed = true;
				}
				else
				{
					++pEItem->nCUC;
					pEItem->nSTR += pScroll->incStat.niSTR;
					pEItem->nDEX += pScroll->incStat.niDEX;
					pEItem->nINT += pScroll->incStat.niINT;
					pEItem->nLUK += pScroll->incStat.niLUK;
					pEItem->nMaxHP += pScroll->incStat.niMaxHP;
					pEItem->nMaxMP += pScroll->incStat.niMaxMP;

					pEItem->nPAD += pScroll->incStat.niPAD;
					pEItem->nMAD += pScroll->incStat.niMAD;
					pEItem->nPDD += pScroll->incStat.niPDD;
					pEItem->nMDD += pScroll->incStat.niMDD;

					pEItem->nACC += pScroll->incStat.niACC;
					pEItem->nEVA += pScroll->incStat.niEVA;
					pEItem->nCraft += pScroll->incStat.niCraft;
					pEItem->nSpeed += pScroll->incStat.niSpeed;
					pEItem->nJump += pScroll->incStat.niJump;
				}
			}
			if (RawRemoveItemByID(pUser, 2340000, 1) && !bSuccess)
				++pEItem->nRUC;
	
			pUser->ValidateStat();
			if (bCheckScroll || bEnchantSkill) 
			{
				OutPacket oPacket;
				InventoryManipulator::MakeItemUpgradeEffect(
					&oPacket,
					pUser->GetUserID(),
					pEItem->nItemID,
					pUItem->nItemID,
					bSuccess,
					bCursed,
					bEnchantSkill
				);
				pUser->GetField()->BroadcastPacket(&oPacket);
			}

			if (bCursed)
				RawRemoveItem(pUser, GW_ItemSlotBase::EQUIP, pEItem->nPOS, 1, &aChangeLog, nDecScroll, nullptr);
			else
				InventoryManipulator::InsertChangeLog(
					aChangeLog,
					InventoryManipulator::Change_AddToSlot,
					GW_ItemSlotBase::EQUIP,
					nEPOS, pItem, 0, 0);
			SendInventoryOperation(pUser, true, aChangeLog);
		}
	}
}

void QWUInventory::RestoreFromTemp(User * pUser, std::map<int, int> mItemTrading[6])
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto &pCharacterData = pUser->GetCharacterData();

	if (pCharacterData->nMoneyTrading > 0)
		pUser->SendCharacterStat(false, QWUser::IncMoney(pUser, 0, false));
	
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	InventoryManipulator::RestoreTradingInventory(pCharacterData, mItemTrading, aChangeLog);
	SendInventoryOperation(pUser, false, aChangeLog);

	pCharacterData->nMoneyTrading = 0;
	for (int i = 1; i <= 5; ++i)
		mItemTrading[i].clear();
}

void QWUInventory::RawMoveItemToTemp(User *pUser, ZSharedPtr<GW_ItemSlotBase>* pItemCopyed, int nTI, int nPOS, int nNumber, std::vector<InventoryManipulator::ChangeLog>& aChangeLog)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto &pCharacterData = pUser->GetCharacterData();
	auto pItem = pCharacterData->GetItem(nTI, nPOS);
	if (!pItem)
		return;

	/*if (ItemInfo::GetInstance()->IsQuestItem(pItem->nItemID) ||
		ItemInfo::GetInstance()->IsTradeBlockItem(pItem->nItemID) ||
		pItem->liCashItemSN != 0)
		return;*/

	if (ItemInfo::IsTreatSingly(pItem->nItemID, 0))
	{
		if (nNumber != 1)
			return;
		pCharacterData->mItemTrading[nTI][nPOS] = nNumber;
		InventoryManipulator::InsertChangeLog(
			aChangeLog,
			InventoryManipulator::Change_RemoveFromSlot,
			nTI,
			nPOS,
			nullptr,
			0,
			1
		);
	}
	else
	{
		int nTradingCount = pCharacterData->GetTradingCount(nTI, nPOS);
		int nCount = ((GW_ItemSlotBundle*)pItem)->nNumber;
		if (nNumber < 1 || nNumber >(nCount - nTradingCount))
			return;
		pCharacterData->mItemTrading[nTI][nPOS] = nNumber + nTradingCount;
		InventoryManipulator::InsertChangeLog(
			aChangeLog,
			(nCount - nTradingCount - nNumber == 0) ?
			InventoryManipulator::Change_RemoveFromSlot :
			InventoryManipulator::Change_QuantityChanged,
			nTI,
			nPOS,
			nullptr,
			0,
			nCount - nTradingCount - nNumber
		);
	}

	if (pItemCopyed) 
	{
		*pItemCopyed = pItem;
		if ((*pItemCopyed)->nType != GW_ItemSlotBase::EQUIP)
			((GW_ItemSlotBundle*)*pItemCopyed)->nNumber = nNumber;
	}
}

void QWUInventory::MoveItemToTemp(User *pUser, ZSharedPtr<GW_ItemSlotBase> *pItemCopyed, int nTI, int nPOS, int nNumber)
{
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	RawMoveItemToTemp(pUser, pItemCopyed, nTI, nPOS, nNumber, aChangeLog);
	SendInventoryOperation(pUser, true, aChangeLog);
}

bool QWUInventory::MoveMoneyToTemp(User * pUser, int nAmount)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	if (nAmount <= 0 || 
		QWUser::GetMoney(pUser) - pUser->GetCharacterData()->nMoneyTrading < nAmount)
		return false;
	pUser->GetCharacterData()->nMoneyTrading = nAmount;
	pUser->SendCharacterStat(true, BasicStat::BS_Meso);
	return true;
}

bool QWUInventory::WasteItem(User *pUser, int nItemID, int nCount, bool bProtected)
{
	if (!ItemInfo::GetInstance()->IsRechargable(nItemID))
		return false;
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	std::vector<BackupItem> aBackup;

	auto &pCharacterData = pUser->GetCharacterData();
	int nDecCount = 0;
	GW_ItemSlotBundle* pItem = nullptr, *pClone;
	for (auto& prItem : pCharacterData->mItemSlot[GW_ItemSlotBase::CONSUME])
	{
		pItem = (GW_ItemSlotBundle*)prItem.second;
		if (pItem && pItem->nItemID == nItemID && (!pItem->IsProtectedItem() || bProtected))
		{
			//Make BackupItem.
			pClone = (GW_ItemSlotBundle*)pItem->MakeClone();
			pClone->liItemSN = pItem->liItemSN;
			pClone->liCashItemSN = pItem->liCashItemSN;
			aBackup.push_back({ GW_ItemSlotBase::CONSUME, pItem->nPOS, pClone });

			nDecCount = pItem->nNumber;
			if (nDecCount > nCount)
				nDecCount = nCount;
			InventoryManipulator::RawWasteItem(pCharacterData, pItem->nPOS, nDecCount, &aChangeLog);
			if ((nCount -= nDecCount) == 0)
				break;
		}
	}

	if (nCount > 0)
	{
		InventoryManipulator::RestoreBackupItem(pCharacterData, aBackup);
		return false;
	}
	else 
	{
		//InventoryManipulator::ReleaseBackupItem(aBackup);
		SendInventoryOperation(pUser, false, aChangeLog);
	}
	return true;
}

bool QWUInventory::RawWasteItem(User *pUser, int nPOS, int nCount, std::vector<InventoryManipulator::ChangeLog>& aChangeLog)
{
	if (QWUser::GetHP(pUser))
		return InventoryManipulator::RawWasteItem(pUser->GetCharacterData(), nPOS, nCount, &aChangeLog);
	return false;
}

void QWUInventory::UpdatePetItem(User * pUser, int nPos)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto pItem = pUser->GetCharacterData()->GetItem(GW_ItemSlotBase::CASH, nPos);
	if (!pItem)
		return;

	std::vector<InventoryManipulator::ChangeLog> aChange;
	InventoryManipulator::InsertChangeLog(
		aChange,
		InventoryManipulator::ChangeType::Change_RemoveFromSlot,
		GW_ItemSlotBase::CASH,
		nPos,
		pItem,
		0,
		0
	);

	InventoryManipulator::InsertChangeLog(
		aChange,
		InventoryManipulator::ChangeType::Change_AddToSlot,
		GW_ItemSlotBase::CASH,
		nPos,
		pItem,
		0,
		0
	);

	SendInventoryOperation(pUser, false, aChange);
}

int QWUInventory::GetSlotCount(User *pUser, int nTI)
{
	if (nTI < GW_ItemSlotBase::EQUIP || nTI > GW_ItemSlotBase::CASH)
		return 0;
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	return pUser->GetCharacterData()->mSlotCount->aSlotCount[nTI];
}

int QWUInventory::GetHoldCount(User * pUser, int nTI)
{
	if (nTI < GW_ItemSlotBase::EQUIP || nTI > GW_ItemSlotBase::CASH)
		return 0;
	int nMaxSlotCount = GetSlotCount(pUser, nTI);
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto& mSlot = pUser->GetCharacterData()->mItemSlot[nTI];
	int nRet = 0;
	for (auto& pItem : mSlot)
		if (pItem.second->nPOS >= 1 && pItem.second->nPOS <= nMaxSlotCount)
			++nRet;
	return nRet;
}

int QWUInventory::GetFreeCount(User * pUser, int nTI)
{
	if (nTI < GW_ItemSlotBase::EQUIP || nTI > GW_ItemSlotBase::CASH)
		return 0;
	int nMaxSlotCount = GetSlotCount(pUser, nTI);
	int nHoldCount = GetHoldCount(pUser, nTI);
	return nMaxSlotCount - nHoldCount;
}

int QWUInventory::GetItemCount(User * pUser, int nItemID, bool bProtected)
{
	int nTI = nItemID / 1000000;
	if (nTI <= 0 || nTI > 5)
		return 0;

	int nResult = 0;
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	auto& mSlot = pUser->GetCharacterData()->mItemSlot[nTI];
	GW_ItemSlotBase *pItem = nullptr;
	for (auto& prItem : mSlot)
	{
		pItem = prItem.second;
		if (pItem->nItemID == nItemID && (bProtected || !pItem->IsProtectedItem()))
		{
			if (pItem->nType == GW_ItemSlotBase::EQUIP || pItem->nType == GW_ItemSlotBase::CASH)
				nResult += (pUser->GetCharacterData()->GetTradingCount(pItem->nType, pItem->nPOS) == 0 ? 1 : 0);
			else
				nResult += (std::max(0, ((GW_ItemSlotBundle*)pItem)->nNumber - pUser->GetCharacterData()->GetTradingCount(pItem->nType, pItem->nPOS)));
		}
	}

	return nResult;
}
