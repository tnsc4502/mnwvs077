#include "StoreBank.h"
#include "User.h"
#include "QWUser.h"
#include "WvsGame.h"
#include "QWUInventory.h"
#include "ExchangeElement.h"
#include "BackupItem.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsCenter\CenterPacketTypes.hpp"
#include "..\WvsGame\FieldPacketTypes.hpp"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\WvsCenter\EntrustedShopMan.h"

StoreBank::StoreBank(User *pUser)
{
	m_pUser = pUser;
}

StoreBank::~StoreBank()
{
}

void StoreBank::OnPacket(InPacket *iPacket)
{
	int nType = iPacket->Decode1();
	switch (nType)
	{
		case EntrustedShopMan::EntrustedShopCheckResult::res_EShop_LoadItemResult:
			OnItemLoadDone(iPacket);
			break;
		case StoreBankRequest::rq_OnOpenStoreBank:
			OnOpenStoreBank();
			break;
		case StoreBankRequest::rq_OnCloseStoreBank:
			m_pUser->SetStoreBank(nullptr);
			break;
		case StoreBankRequest::rq_MoveItemToInventory:
			MoveItemToInventory(iPacket);
			break;
	}
}

void StoreBank::OnSelectStoreBankNPC()
{
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketType::Field_StoreBankRequest);
	oPacket.Encode1(StoreBankResult::res_OnSelectStoreBankNPC);
	oPacket.Encode1(0);
	m_pUser->SendPacket(&oPacket);
}

void StoreBank::OnOpenStoreBank()
{
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::EntrustedShopRequest);
	oPacket.Encode4(m_pUser->GetSocketID());
	oPacket.Encode4(m_pUser->GetUserID());
	oPacket.Encode1(EntrustedShopMan::EntrustedShopRequest::req_EShop_LoadItemRequest);

	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

void StoreBank::OnItemLoadDone(InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_pUser->GetLock());
	int nCount = iPacket->Decode1(), nTI = 0;
	if (nCount < 0)
	{
		m_pUser->SendNoticeMessage("請關閉商店後再試一次。");
		m_pUser->SetStoreBank(nullptr);
		m_pUser->SendCharacterStat(true, 0);
		return;
	}
	else if (nCount == 0)
	{
		m_pUser->SendNoticeMessage("沒有任何寄放中的物品。");
		m_pUser->SetStoreBank(nullptr);
		m_pUser->SendCharacterStat(true, 0);
		return;
	}
	else
	{
		m_liShopMoney = iPacket->Decode8();
		OutPacket oPacket;
		oPacket.Encode2(FieldSendPacketType::Field_StoreBankRequest);
		oPacket.Encode1(StoreBankResult::res_OnItemLoadDone);
		oPacket.Encode4(m_nStoreBankTemplateID);
		oPacket.Encode1(nCount); //nSlotCount
		oPacket.Encode8(2 | 4);
		oPacket.Encode4((int)m_liShopMoney); //nMoney
		oPacket.Encode1(nCount);

		ZSharedPtr<GW_ItemSlotBase> pItem;
		for (int i = 0; i < nCount; ++i)
		{
			nTI = iPacket->Decode1();
			iPacket->Decode1(); //nPOS
			pItem.reset(GW_ItemSlotBase::CreateItem(
				nTI == GW_ItemSlotBase::EQUIP ?
				GW_ItemSlotBase::GW_ItemSlotEquip_Type :
				GW_ItemSlotBase::GW_ItemSlotBundle_Type
			));
			pItem->Decode(iPacket, true);
			pItem->nType = (GW_ItemSlotBase::GW_ItemSlotType)nTI;
			m_aItem.push_back(pItem);
			pItem->RawEncode(&oPacket);
		}
		m_pUser->SendPacket(&oPacket);
	}
}

void StoreBank::MoveItemToInventory(InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_pUser->GetLock());
	std::vector<BackupItem> aBackup;
	std::vector<ExchangeElement> aExchange;
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;

	ZSharedPtr<GW_ItemSlotBase> pClone;
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::EntrustedShopRequest);
	oPacket.Encode4(m_pUser->GetSocketID());
	oPacket.Encode4(m_pUser->GetUserID());
	oPacket.Encode1(EntrustedShopMan::EntrustedShopRequest::req_EShop_ItemNumberChanged);
	oPacket.Encode8(0);
	oPacket.Encode1((char)m_aItem.size());

	for (auto pItem : m_aItem)
	{
		//pClone = pItem;
		//pClone->liItemSN = pItem->liItemSN;
		aExchange.push_back({});
		aExchange.back().m_pItem = pItem;
		aExchange.back().m_nCount =
			pItem->nType == GW_ItemSlotBase::EQUIP ?
			1 : ((GW_ItemSlotBundle*)pItem)->nNumber;

		//Send to center
		oPacket.Encode1(pItem->nType);
		oPacket.Encode2(0);
		pItem->Encode(&oPacket, true);
	}

	if (QWUInventory::Exchange(
		m_pUser,
		(int)m_liShopMoney,
		aExchange,
		&aChangeLog,
		nullptr,
		aBackup))
	{
		m_pUser->SendNoticeMessage("背包欄位空間不足。");
		m_pUser->SendCharacterStat(true, 0);
		return;
	}
	else
	{
		m_liShopMoney = 0;
		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);

		OutPacket oCloseBank;
		oCloseBank.Encode2(FieldSendPacketType::Field_StoreBankMessage);
		oCloseBank.Encode1(StoreBankResult::res_OnItemRestoredToSlot);
		m_pUser->SendPacket(&oCloseBank);
		m_pUser->FlushCharacterData();
		m_pUser->SetStoreBank(nullptr);
	}
}

void StoreBank::SetStoreBankTemplateID(int nTemplateID)
{
	m_nStoreBankTemplateID = nTemplateID;
}
