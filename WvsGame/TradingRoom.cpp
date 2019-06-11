#include "TradingRoom.h"
#include "QWUInventory.h"
#include "ExchangeElement.h"
#include "BackupItem.h"
#include "User.h"
#include "QWUser.h"
#include "ItemInfo.h"
#include <algorithm>
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\FieldPacketFlags.hpp"

TradingRoom::TradingRoom()
	: MiniRoomBase(2)
{
	m_abLock[0] = m_abLock[1] = 0;
	memset(m_aItem, 0, sizeof(m_aItem));
}


TradingRoom::~TradingRoom()
{
}

int TradingRoom::GetLeaveType() const
{
	return TradingResult::res_Trading_Aborted;
}

void TradingRoom::OnPacket(User *pUser, int nType, InPacket *iPacket)
{
	switch (nType)
	{
		case TradingRequest::rq_Trading_PutItem:
			OnPutItem(pUser, iPacket);
			break;
		case TradingRequest::rq_Trading_PutMoney:
			OnPutMoney(pUser, iPacket);
			break;
		case TradingRequest::rq_Trading_DoTrade:
			OnTrade(pUser, iPacket);
			break;
	}
}

void TradingRoom::OnPutItem(User *pUser, InPacket *iPacket)
{
	int nTI = iPacket->Decode1();
	int nPOS = iPacket->Decode2();
	int nNumber = iPacket->Decode2();
	int nTradingPOS = iPacket->Decode1();

	int nIdx = FindUserSlot(pUser);
	if (nIdx == -1 || (nTradingPOS < 1 || nTradingPOS > 9))
	{
		pUser->SendCharacterStat(true, 0);
		return;
	}

	auto pItem = pUser->GetCharacterData()->GetItem(nTI, nPOS);
	if (pItem)
	{
		if (ItemInfo::GetInstance()->IsTradeBlockItem(pItem->nItemID)
			|| ItemInfo::GetInstance()->IsQuestItem(pItem->nItemID))
		{
			pUser->SendNoticeMessage("無法交易的物品。");
			pUser->SendCharacterStat(true, 0);
			return;
		}
		GW_ItemSlotBase* pItemCopyed = nullptr;
		QWUInventory::MoveItemToTemp(pUser, &pItemCopyed, nTI, nPOS, nNumber);

		for (int i = 0; i < m_nMaxUsers; ++i)
		{
			OutPacket oPacket;
			oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
			oPacket.Encode1(TradingRequest::rq_Trading_PutItem);
			oPacket.Encode1(i != nIdx);
			oPacket.Encode1(nTradingPOS);
			pItemCopyed->RawEncode(&oPacket);

			m_apUser[i]->SendPacket(&oPacket);
		}
		pItemCopyed->Release();
	}
}

void TradingRoom::OnPutMoney(User *pUser, InPacket *iPacket)
{
	int nIdx = FindUserSlot(pUser);
	std::lock_guard<std::recursive_mutex> lock0(m_apUser[nIdx]->GetLock());
	if (nIdx >= 0)
	{
		int nMoney = iPacket->Decode4();
		if (!QWUInventory::MoveMoneyToTemp(pUser, nMoney)) 
		{
			nMoney = 0;
			QWUInventory::MoveMoneyToTemp(pUser, pUser->GetCharacterData()->nMoneyTrading);
		}

		for (int i = 0; i < m_nCurUsers; ++i)
		{
			OutPacket oPacket;
			oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
			oPacket.Encode1(TradingRequest::rq_Trading_PutMoney);
			oPacket.Encode1(i != nIdx);
			oPacket.Encode4(nMoney);

			m_apUser[i]->SendPacket(&oPacket);
		}
	}
}

void TradingRoom::OnTrade(User *pUser, InPacket *iPacket)
{
	int nIdx = FindUserSlot(pUser);
	if (nIdx == -1 || nIdx > 1)
		return;
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);

	std::lock_guard<std::recursive_mutex> lock0(m_apUser[0]->GetLock());
	std::lock_guard<std::recursive_mutex> lock1(m_apUser[1]->GetLock());
	if (m_nCurUsers == m_nMaxUsers && !m_abLock[nIdx])
	{
		m_abLock[nIdx] = true;

		auto pUserNotify = m_apUser[1 - nIdx];
		OutPacket oPacket;
		oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
		oPacket.Encode1(TradingRequest::rq_Trading_DoTrade);
		pUserNotify->SendPacket(&oPacket);

		if (m_abLock[0] && m_abLock[1])
		{
			int nResult = DoTrade();
			OnLeave(pUser, nResult);
		}
	}
}

void TradingRoom::MakeExchageElements(std::vector<ExchangeElement> aEx[2], std::map<int, int> amTradingItems[2][6])
{
	ExchangeElement elem;
	bool bTreatSingly = false;

	for (int i = 0; i < 2; ++i)
		for (int nTI = 1; nTI <= 5; ++nTI)
			for (auto& prItem : amTradingItems[i][nTI])
			{
				auto pItem = m_apUser[i]->GetCharacterData()->GetItem(nTI, prItem.first);
				if (pItem)
				{
					bTreatSingly = pItem->nType == GW_ItemSlotBase::EQUIP ||
						((GW_ItemSlotBundle*)pItem)->nNumber == prItem.second;

					//i = 0, aEx0: Remove Item, i = 1, aEx0: Add Item
					elem.m_pItem = bTreatSingly ? (i == 0 ? pItem : pItem->MakeClone()) : nullptr;
					elem.m_nItemID = bTreatSingly ? 0 : pItem->nItemID;
					elem.m_nCount = prItem.second * (i == 0 ? -1 : 1);
					aEx[0].push_back(elem);
					//i = 0, aEx1: Add Item, i = 1, aEx1: Remove Item
					elem.m_pItem = bTreatSingly ? (i == 1 ? pItem : pItem->MakeClone()) : nullptr;
					elem.m_nCount *= -1;
					aEx[1].push_back(elem);
				}
			}

	//Sorting by item count so that deletions are executed first to maintain correct item nPOSs.
	std::sort(aEx[0].begin(), aEx[0].end(), [](const ExchangeElement& e1, const ExchangeElement&e2) {
		return e1.m_nCount < e2.m_nCount; });
	std::sort(aEx[1].begin(), aEx[1].end(), [](const ExchangeElement& e1, const ExchangeElement&e2) {
		return e1.m_nCount < e2.m_nCount; });
}

int TradingRoom::DoTrade()
{
	std::map<int, int> amBackupItemTrading[2][6];
	int anMoneyTrading[2], anBackupMoneyTrading[2];
	for (int i = 0; i < 2; ++i)
	{
		anMoneyTrading[i] = m_apUser[i]->GetCharacterData()->nMoneyTrading;
		anBackupMoneyTrading[i] = (int)QWUser::GetMoney(m_apUser[i]);
		for(int nTI = 1; nTI <= 5; ++nTI)
			amBackupItemTrading[i][nTI] = std::move(m_apUser[i]->GetCharacterData()->mItemTrading[nTI]);
		m_apUser[i]->GetCharacterData()->ClearTradeInfo();
	}

	std::vector<InventoryManipulator::ChangeLog> aChangLog[2];
	std::vector<ExchangeElement> aEx[2];
	std::vector<BackupItem> aBackup[2];

	MakeExchageElements(aEx, amBackupItemTrading);
	int nTradingAmount = (-anMoneyTrading[0] + anMoneyTrading[1]);
	int nExchangeRes = QWUInventory::Exchange(m_apUser[0], nTradingAmount, aEx[0], &aChangLog[0], nullptr, aBackup[0], false, false);
	if (!nExchangeRes)
		nExchangeRes = QWUInventory::Exchange(m_apUser[1], nTradingAmount * -1, aEx[1], &aChangLog[1], nullptr, aBackup[1], false);

	//Exchanging with 2nd player failed then we should restore all items of 1st player.
	if (nExchangeRes) 
	{
		for (auto& elem : aEx[1])
			if (elem.m_pItem && elem.m_nCount >= 0)
				elem.m_pItem->Release(); //Release all cloned items which are not properly added.

		m_apUser[0]->SendCharacterStat(true, QWUser::SetMoney(m_apUser[0], anBackupMoneyTrading[0]));
		InventoryManipulator::RestoreBackupItem(m_apUser[0]->GetCharacterData(), aBackup[0]);
		QWUInventory::RestoreFromTemp(m_apUser[0], amBackupItemTrading[0]);
		QWUInventory::RestoreFromTemp(m_apUser[1], amBackupItemTrading[1]);
		return TradingResult::res_Trading_Exchange_Failed;
	}
	else //Exchanging success, release all backup items.
	{
		InventoryManipulator::ReleaseBackupItem(aBackup[0]);
		QWUInventory::SendInventoryOperation(m_apUser[0], true, aChangLog[0]);
		QWUInventory::SendInventoryOperation(m_apUser[1], true, aChangLog[1]);
	}

	return TradingResult::res_Trading_Exchange_Success;
}

void TradingRoom::OnLeave(User *pUser, int nLeaveType)
{
	int nIdx = FindUserSlot(pUser);
	std::lock_guard<std::recursive_mutex > lock(pUser->GetLock());
	if (nIdx >= 0)
	{
		pUser->GetCharacterData()->nMoneyTrading = 0;
		pUser->SendCharacterStat(true, QWUser::IncMoney(pUser, 0, false));
		QWUInventory::RestoreFromTemp(pUser, pUser->GetCharacterData()->mItemTrading);
	}
	CloseRequest(pUser, nLeaveType, nLeaveType);
}

void TradingRoom::Encode(OutPacket *oPacket)
{
}

void TradingRoom::Release()
{
	FreeObj(this);
}
