#include "PersonalShop.h"
#include "QWUser.h"
#include "User.h"
#include "ItemInfo.h"
#include "BackupItem.h"
#include "ExchangeElement.h"
#include "QWUInventory.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\FieldPacketFlags.hpp"

PersonalShop::PersonalShop()
	: MiniRoomBase(4)
{
	m_nSlotCount = 10;
	m_aItem.clear();
	m_bOpened = false;
}

PersonalShop::~PersonalShop()
{
	for (auto& psItem : m_aItem)
		psItem.pItem->Release();
}

void PersonalShop::OnPutItem(User *pUser, InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> uLock(pUser->GetLock());
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);

	int nTI = iPacket->Decode1();
	int nPOS = iPacket->Decode2();
	int nNumber = iPacket->Decode2();
	int nSet = iPacket->Decode2();
	int nPrice = iPacket->Decode4();
	int nPOS2 = 0;

	int nIdx = FindUserSlot(pUser);
	if (!m_nCurUsers || nIdx == -1 || m_bOpened || nSet == 0 || (int)m_aItem.size() == m_nSlotCount)
	{
		pUser->SendCharacterStat(true, 0);
		return;
	}

	auto pItem = pUser->GetCharacterData()->GetItem(nTI, nPOS);
	if (!pItem ||
		(ItemInfo::GetInstance()->IsTreatSingly(pItem->nItemID, 0) && (nSet != 1 || nNumber != 1)) ||
		ItemInfo::GetInstance()->IsTradeBlockItem(pItem->nItemID) || 
		ItemInfo::GetInstance()->IsQuestItem(pItem->nItemID) ||
		(nTI != GW_ItemSlotBase::EQUIP && ((GW_ItemSlotBundle*)pItem)->nNumber < nNumber))
	{
		pUser->SendCharacterStat(true, 0);
		return;
	}

	Item item;
	item.nNumber = nNumber;
	item.nTI = nTI;
	item.nPrice = nPrice;
	item.nSet = nSet;
	item.pItem = pItem->MakeClone();

	if (item.pItem->nType == GW_ItemSlotBase::EQUIP || 
		ItemInfo::IsRechargable(pItem->nItemID) ||
		((GW_ItemSlotBundle*)item.pItem)->nNumber == nNumber * nSet)
		item.pItem->liItemSN = pItem->liItemSN;
	else
		((GW_ItemSlotBundle*)item.pItem)->nNumber = nNumber * nSet;

	MoveItemToShop(pItem, pUser, nTI, nPOS, nNumber * nSet, &nPOS2);
	item.nPOS = nPOS2;
	m_aItem.push_back(item);
	BroadcastItemList();
}

void PersonalShop::OnBuyItem(User *pUser, InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> uLock(pUser->GetLock());
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);

	int nItemIdx = iPacket->Decode1();
	int nNumber = iPacket->Decode2();
	int nIdx = FindUserSlot(pUser);
	if (!m_bOpened || 
		nIdx == -1 ||
		nItemIdx >= (int)m_aItem.size() || 
		nNumber > m_aItem[nItemIdx].nNumber)
	{
		pUser->SendCharacterStat(true, 0);
		return;
	}
	if (m_aItem[nItemIdx].nPrice * nNumber > QWUser::GetMoney(pUser))
	{
		pUser->SendNoticeMessage("楓幣不足，請稍後再試。");
		pUser->SendCharacterStat(true, 0);
		return;
	}
	DoTransaction(pUser, nIdx, &(m_aItem[nItemIdx]), nNumber);
	pUser->SendCharacterStat(true, 0);
}

void PersonalShop::OnMoveItemToInventory(User *pUser, InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> uLock(pUser->GetLock());
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	int nItemIdx = iPacket->Decode2();

	if (nItemIdx >= (int)m_aItem.size())
	{
		pUser->SendCharacterStat(true, 0);
		return;
	}

	if (!RestoreItemFromShop(pUser, &m_aItem[nItemIdx]))
	{
		pUser->SendCharacterStat(true, 0);
		return;
	}
	m_aItem.erase(m_aItem.begin() + nItemIdx);
	//m_aItem.pop_back();
	BroadcastItemList();
	pUser->SendCharacterStat(true, 0);
}

GW_ItemSlotBase * PersonalShop::MoveItemToShop(GW_ItemSlotBase *pItem, User *pUser, int nTI, int nPOS, int nNumber, int* nPOS2)
{
	*nPOS2 = nPOS;
	QWUInventory::MoveItemToTemp(pUser, nullptr, nTI, nPOS, nNumber);
	return pItem;
}

bool PersonalShop::RestoreItemFromShop(User *pUser, PersonalShop::Item* psItem)
{
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	auto pItem = psItem->pItem;
	if (pItem->nType == GW_ItemSlotBase::EQUIP ||
		((GW_ItemSlotBundle*)pItem)->nNumber == psItem->nNumber * psItem->nSet)
		InventoryManipulator::InsertChangeLog(
			aChangeLog, InventoryManipulator::ChangeType::Change_AddToSlot, psItem->nTI, psItem->nPOS, pItem, 0, 0
		);
	else
	{
		int nTradingCount = pUser->GetCharacterData()->GetTradingCount(psItem->nTI, psItem->nPOS);
		InventoryManipulator::InsertChangeLog(
			aChangeLog, InventoryManipulator::ChangeType::Change_QuantityChanged, psItem->nTI, psItem->nPOS, nullptr, 0, nTradingCount - (psItem->nNumber * psItem->nSet)
		);
	}
	QWUInventory::SendInventoryOperation(pUser, true, aChangeLog);
	pItem->Release();
	return true;
}

void PersonalShop::DoTransaction(User *pUser, int nSlot, Item *psItem, int nNumber)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	std::lock_guard<std::recursive_mutex> buyerLock(pUser->GetLock());
	std::lock_guard<std::recursive_mutex> ownerLock(m_apUser[0]->GetLock());

	std::vector<BackupItem> aBackup[2];
	std::vector<ExchangeElement> elem[2];
	std::vector<InventoryManipulator::ChangeLog> aLogAdd[2];

	//For owner
	elem[0].push_back({});
	elem[0][0].m_nCount = -nNumber * psItem->nSet;
	elem[0][0].m_pItem = m_apUser[0]->GetCharacterData()->GetItem(psItem->nTI, psItem->nPOS);

	//For buyer
	elem[1].push_back({});
	elem[1][0].m_nCount = nNumber * psItem->nSet;
	elem[1][0].m_pItem = psItem->pItem->MakeClone();

	int nMoneyCost = psItem->nPrice * psItem->nNumber * -1, 
		nMoneyBackup = (int)QWUser::GetMoney(m_apUser[0]);
	int nExchangeRes = QWUInventory::Exchange(m_apUser[0], nMoneyCost * -1, elem[0], nullptr, nullptr, aBackup[0], true, false);
	if (!nExchangeRes)
		nExchangeRes = QWUInventory::Exchange(m_apUser[nSlot], nMoneyCost, elem[1], &aLogAdd[1], nullptr, aBackup[1]);

	if (nExchangeRes)
	{
		if (elem[1][0].m_pItem && elem[1][0].m_nCount >= 0)
			elem[1][0].m_pItem->Release(); //Release all cloned items which are not properly added.

		m_apUser[0]->SendCharacterStat(true, QWUser::SetMoney(m_apUser[0], nMoneyBackup));
		InventoryManipulator::RestoreBackupItem(m_apUser[0]->GetCharacterData(), aBackup[0]);
		//Reset item, the original item has been released during restoring.
		psItem->pItem = aBackup[0][0].m_pItem;
		pUser->SendNoticeMessage("購買失敗，請確認背包欄位是否足夠。");
	}
	else //Exchanging success, release all backup items. 
	{
		for (auto &bkItem : aBackup[0])
			if (bkItem.m_pItem)
				bkItem.m_pItem->Release();
		psItem->nNumber -= nNumber;
		pUser->SendNoticeMessage("購買成功。");
	}
	BroadcastItemList();
}

void PersonalShop::OnPacket(User *pUser, int nType, InPacket *iPacket)
{
	switch (nType)
	{
		case PersonalShopRequest::rq_PShop_PutItem:
			OnPutItem(pUser, iPacket);
			break;
		case PersonalShopRequest::rq_PShop_BuyItem:
			OnBuyItem(pUser, iPacket);
			break;
		case PersonalShopRequest::rq_PShop_MoveItemToInventory:
			OnMoveItemToInventory(pUser, iPacket);
	}
}

void PersonalShop::OnLeave(User *pUser, int nLeaveType)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);
	int nIdx = FindUserSlot(pUser);
						//Owner condition
	if (nIdx >= 0 && (nIdx == 0 || m_nCurUsers == 1)) 
	{
		QWUInventory::RestoreFromTemp(pUser, pUser->GetCharacterData()->mItemTrading);
		pUser->SetMiniRoomBalloon(false);

		m_bOpened = IsEntrusted() && IsEmployer(pUser);
		if(m_nCurUsers > 1)
			CloseRequest(pUser, 0, PersonalShopMessgae::e_Message_ShopClosed);
	}
}

void PersonalShop::EncodeEnterResult(User *pUser, OutPacket *oPacket)
{
	oPacket->EncodeStr(m_sTitle);
	oPacket->Encode1(m_nSlotCount);
	EncodeItemList(oPacket);
}

void PersonalShop::EncodeItemList(OutPacket *oPacket)
{
	oPacket->Encode1((char)m_aItem.size());
	for (auto& item : m_aItem)
	{
		oPacket->Encode2(item.nNumber);
		oPacket->Encode2(item.nSet);
		oPacket->Encode4(item.nPrice);
		item.pItem->RawEncode(oPacket);
	}
}

void PersonalShop::Encode(OutPacket * oPacket)
{
}

void PersonalShop::Release()
{
	FreeObj(this);
}

void PersonalShop::BroadcastItemList()
{
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
	oPacket.Encode1(PersonalShopMessgae::e_Message_ShopItemUpdated);
	EncodeItemList(&oPacket);
	Broadcast(&oPacket, nullptr);
}
