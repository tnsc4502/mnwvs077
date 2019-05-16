#include "TradingRoom.h"
#include "QWUInventory.h"
#include "ExchangeElement.h"
#include "User.h"
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
		GW_ItemSlotBase* pItemCopyed = nullptr;
		QWUInventory::MoveItemToTemp(pUser, &pItemCopyed, nTI, nPOS, nNumber);

		for (int i = 0; i < 2; ++i)
		{
			OutPacket oPacket;
			oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
			oPacket.Encode1(TradingRequest::rq_Trading_PutItem);
			oPacket.Encode1(i != nIdx);
			oPacket.Encode1(nTradingPOS);
			pItemCopyed->RawEncode(&oPacket);

			m_apUser[i]->SendPacket(&oPacket);
		}
		FreeObj(pItemCopyed);
	}
}

void TradingRoom::OnPutMoney(User *pUser, InPacket *iPacket)
{
}

void TradingRoom::OnTrade(User *pUser, InPacket *iPacket)
{
	int nIdx = FindUserSlot(pUser);
	if (nIdx == -1 || nIdx > 1)
		return;
	std::lock_guard<std::recursive_mutex> lock(m_mtxMiniRoomLock);

	if (m_nCurUsers && !m_abLock[nIdx])
	{
		m_abLock[nIdx] = true;

		auto pUserNotify = m_apUser[1 - nIdx];
		OutPacket oPacket;
		oPacket.Encode2(FieldSendPacketFlag::Field_MiniRoomRequest);
		oPacket.Encode1(15);
		pUserNotify->SendPacket(&oPacket);

		if (m_abLock[0] && m_abLock[1])
		{
			std::lock_guard<std::recursive_mutex> lock0(m_apUser[0]->GetLock());
			std::lock_guard<std::recursive_mutex> lock1(m_apUser[1]->GetLock());
			DoTrade();
		}
	}
}

void TradingRoom::DoTrade()
{
	std::map<int, int> maBackupItemTrading[2][6];
	int anMoneyTrading[2];
	for (int i = 0; i < 2; ++i)
	{
		anMoneyTrading[i] = m_apUser[i]->GetCharacterData()->nMoneyTrading;
		for(int nTI = 1; nTI <= 5; ++nTI)
			maBackupItemTrading[i][nTI] = std::move(m_apUser[i]->GetCharacterData()->mItemTrading[nTI]);
		m_apUser[i]->GetCharacterData()->ClearTradeInfo();
	}

	std::vector<ExchangeElement> aEx1, aEx2;

	for (int i = 0; i < 2; ++i)
		for (int nTI = 1; nTI <= 5; ++nTI)
			for (auto& prItem : maBackupItemTrading[i][nTI])
			{
				auto pItem = m_apUser[i]->GetCharacterData()->GetItem(nTI, prItem.first);
				if (pItem)
				{
					if (pItem->nType == GW_ItemSlotBase::EQUIP ||
						((GW_ItemSlotBundle*)pItem)->nNumber == prItem.second)
					{

					}
				}
			}
}

void TradingRoom::Encode(OutPacket *oPacket)
{
}

void TradingRoom::Release()
{
	FreeObj(this);
}
