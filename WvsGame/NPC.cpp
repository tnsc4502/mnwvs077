#include "Npc.h"
#include "NpcTemplate.h"
#include "User.h"
#include "ExchangeElement.h"
#include "BackupItem.h"
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "QWUInventory.h"
#include "QWUser.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsGame\UserPacketTypes.hpp"
#include "..\WvsGame\NpcPacketTypes.hpp"
#include "..\WvsLib\String\StringPool.h"

void Npc::OnShopPurchaseItem(User * pUser, InPacket * iPacket)
{
	int nPOS = iPacket->Decode2();
	int nItemID = iPacket->Decode4();
	int nCount = iPacket->Decode2();
	if (ItemInfo::IsRechargable(nItemID) || ItemInfo::GetItemSlotType(nItemID) == GW_ItemSlotBase::EQUIP)
		nCount = 1;

	auto& aItemList = m_pTemplate->GetShopItem();
	if (nPOS >= 0 && nPOS < aItemList.size())
	{
		auto pItem = aItemList[nPOS];
		if (nCount > 0 && pItem && pItem->nPrice)
		{
			int nPrice = pItem->nPrice * (ItemInfo::IsRechargable(nItemID) ? 1 : nCount);
			std::vector<ExchangeElement> aExchange;
			if(ItemInfo::IsRechargable(pItem->nItemID))
				nCount = SkillInfo::GetInstance()->GetBundleItemMaxPerSlot(
					pItem->nItemID,
					pUser->GetCharacterData()
				);
			else
				nCount *= pItem->nQuantity;
			if (pItem->nTokenPrice > 0)
			{
				aExchange.push_back({});
				aExchange.back().m_nItemID = pItem->nTokenItemID;
				aExchange.back().m_nCount = -(pItem->nTokenPrice);
			}
			aExchange.push_back({});
			aExchange.back().m_nItemID = pItem->nItemID;
			aExchange.back().m_nCount = nCount;

			auto nResult = QWUInventory::Exchange(pUser, -nPrice, aExchange);
			switch (nResult)
			{
				case InventoryManipulator::Exchange_InsufficientMeso:
					pUser->SendNoticeMessage(GET_STRING(GameSrv_User_Insufficient_Money));
					break;
				case InventoryManipulator::Exchange_InsufficientSlotCount:
					pUser->SendNoticeMessage(GET_STRING(GameSrv_User_Insufficient_SlotCount));
					break;
				case InventoryManipulator::Exchange_InsufficientItemCount:
					pUser->SendNoticeMessage(GET_STRING(GameSrv_User_Insufficient_ItemCount));
					break;
			}
			if (nResult)
			{
				pUser->SendCharacterStat(true, 0);
				return;
			}
			OutPacket oPacket;
			MakeShopResult(pUser, pItem, &oPacket, 8, 0);
			pUser->SendPacket(&oPacket);
		}
	}
}

void Npc::OnShopSellItem(User * pUser, InPacket * iPacket)
{
	int nPOS = iPacket->Decode2();
	int nItemID = iPacket->Decode4();
	int nCount = iPacket->Decode2();
	int nTI = ItemInfo::GetItemSlotType(nItemID);
	int nPrice = 0;
	if (nTI > 0 && nTI <= 5 && nCount > 0)
	{
		auto pItem = pUser->GetCharacterData()->GetItem(
			nTI,
			nPOS
		);
		if (pItem == nullptr || pItem->IsProtectedItem())
		{
			pUser->SendNoticeMessage(GET_STRING(GameSrv_Shop_Invalid_Item));
			pUser->SendCharacterStat(true, 0);
			return;
		}
		if (nTI == 1) 
		{
			nCount = 1;
			auto pEquipItem = ItemInfo::GetInstance()->GetEquipItem(nItemID);
			if (pEquipItem)
				nPrice = nCount * pEquipItem->nSellPrice;
		}
		else
		{
			auto pBundle = (GW_ItemSlotBundle*)pItem;
			auto pBundleItem = ItemInfo::GetInstance()->GetBundleItem(nItemID);
			nCount = (nCount > pBundle->nNumber ? pBundle->nNumber : nCount);
			if (pBundleItem && ItemInfo::IsRechargable(nItemID))
				nPrice = (int)std::ceil(nCount * pBundleItem->dSellUnitPrice);
			else if (pBundleItem)
					nPrice = nCount * pBundleItem->nSellPrice;
		}
		if (nCount > 0)
		{
			std::vector<ExchangeElement> aExchange;
			aExchange.push_back({});
			aExchange.back().m_pItem = pItem;
			aExchange.back().m_nCount = -nCount;
			auto nResult = QWUInventory::Exchange(pUser, nPrice, aExchange);

			OutPacket oPacket;
			MakeShopResult(pUser, pItem, &oPacket, 0, -1);
			pUser->SendPacket(&oPacket);
		}
	}
}

void Npc::OnShopRechargeItem(User * pUser, InPacket * iPacket)
{
	int nPOS = iPacket->Decode2();
	auto pItem = (GW_ItemSlotBundle*)pUser->GetCharacterData()->GetItem(GW_ItemSlotBase::CONSUME, nPOS);
	if (!pItem || !ItemInfo::GetInstance()->IsRechargable(pItem->nItemID))
	{
		pUser->SendNoticeMessage(GET_STRING(Common_Unknown_Error));
		pUser->SendCharacterStat(true, 0);
		return;
	}
	auto nMaxPerSlot = SkillInfo::GetInstance()->GetBundleItemMaxPerSlot(pItem->nItemID, pUser->GetCharacterData());
	auto& apItemList = m_pTemplate->GetShopItem();
	int nCount = nMaxPerSlot - pItem->nNumber;
	if (nCount <= 0)
	{
		pUser->SendNoticeMessage(GET_STRING(Common_Unknown_Error));
		pUser->SendCharacterStat(true, 0);
		return;
	}
	double dUnitPrice = 0.0;
	for(auto& pShopItem : apItemList)
		if (pShopItem->nItemID == pItem->nItemID)
		{
			auto pBundleItem = ItemInfo::GetInstance()->GetBundleItem(pItem->nItemID);
			dUnitPrice = pBundleItem->dSellUnitPrice;
			break;
		}
	int nCost = (int)(dUnitPrice * nCount);
	if (dUnitPrice && QWUser::GetMoney(pUser) >= nCost)
	{
		std::vector<BackupItem> aBackup;
		std::vector<ExchangeElement> aExchange;
		std::vector<InventoryManipulator::ChangeLog> aChangeLog;
		if (!QWUInventory::Exchange(pUser, -nCost, aExchange, nullptr, nullptr, aBackup))
		{
			if (QWUInventory::RawRechargeItem(pUser, nPOS, &aChangeLog))
			{
				QWUInventory::SendInventoryOperation(pUser, true, aChangeLog);
				pUser->SendNoticeMessage(GET_STRING(GameSrv_Shop_Item_Recharged));
				pUser->SendCharacterStat(true, 0);

				OutPacket oPacket;
				MakeShopResult(pUser, pItem, &oPacket, 8, 0);
				pUser->SendPacket(&oPacket);
				return;
			}
			QWUInventory::Exchange(pUser, nCost, aExchange, nullptr, nullptr, aBackup);
		}
	}
	pUser->SendNoticeMessage(GET_STRING(Common_Unknown_Error));
	pUser->SendCharacterStat(true, 0);
	return;
}

void Npc::MakeShopResult(User *pUser, void* pItem_, OutPacket * oPacket, int nAction, int nIdx)
{
	auto pItem = (NpcTemplate::ShopItem*)pItem_;
	oPacket->Encode2((short)NPCSendPacketTypes::NPC_OnNpcShopRequest);
	oPacket->Encode1(nAction);
	if (nAction == 8)
	{
		oPacket->Encode4(0);
		oPacket->Encode4(GetTemplateID());
		oPacket->Encode4(0); //nStarCoin
		oPacket->Encode1(0);
		oPacket->Encode4(0); //nShopVerNo

		oPacket->Encode1(0); //Rank stuffs
		oPacket->Encode4(0);
		oPacket->Encode2(1);
		NpcTemplate::EncodeShopItem(
			pUser, pItem, oPacket
		);
	}
	else
	{
		oPacket->Encode1(nIdx >= 0 ? 1 : 0);
		if (nIdx >= 0)
			oPacket->Encode4(nIdx);
		else
		{
			oPacket->Encode1(0);
			oPacket->Encode4(0);
		}
		oPacket->Encode4(0);
	}
}

Npc::Npc()
{
}

Npc::~Npc()
{
}

void Npc::SetTemplate(NpcTemplate * pTemplate)
{
	m_pTemplate = pTemplate;
}

NpcTemplate * Npc::GetTemplate()
{
	return m_pTemplate;
}

void Npc::OnShopRequest(User * pUser, InPacket * iPacket)
{
	char nAction = iPacket->Decode1();
	switch (nAction)
	{
		case 0:
			OnShopPurchaseItem(pUser, iPacket);
			break;
		case 1:
			OnShopSellItem(pUser, iPacket);
			break;
		case 2:
			OnShopRechargeItem(pUser, iPacket);
			break;
		default:
			pUser->SetTradingNpc(nullptr);
			break;
	}
}

void Npc::OnUpdateLimitedInfo(User * pUser, InPacket * iPacket)
{
	int nRemained = iPacket->RemainedCount();
	OutPacket oPacket;
	oPacket.Encode2(NPCSendPacketTypes::NPC_OnMove);
	oPacket.Encode4(GetFieldObjectID());
	if (nRemained == 2)
	{
		oPacket.Encode2(iPacket->Decode2());
	}
	else
		oPacket.EncodeBuffer(iPacket->GetPacket() + iPacket->GetReadCount(), nRemained - 5);
	pUser->SendPacket(&oPacket);
}

void Npc::SendChangeControllerPacket(User * pUser)
{
	OutPacket oPacket;
	oPacket.Encode2((short)NPCSendPacketTypes::NPC_OnNpcChangeController);
	oPacket.Encode1(1);
	oPacket.Encode4(GetFieldObjectID());
	oPacket.Encode4(GetTemplateID());
	EncodeInitData(&oPacket);
	pUser->SendPacket(&oPacket);
}

void Npc::MakeEnterFieldPacket(OutPacket *oPacket)
{
	oPacket->Encode2((short)NPCSendPacketTypes::NPC_OnMakeEnterFieldPacket); //CNpcPool::OnUserEnterField
	oPacket->Encode4(GetFieldObjectID());
	oPacket->Encode4(GetTemplateID());
	EncodeInitData(oPacket);
}

void Npc::MakeLeaveFieldPacket(OutPacket * oPacket)
{
}

void Npc::EncodeInitData(OutPacket *oPacket)
{
	oPacket->Encode2(GetPosX());
	oPacket->Encode2(GetCy());
	oPacket->Encode1(GetF() == 1 ? 0 : 1);
	oPacket->Encode2(GetFh());
	oPacket->Encode2(GetRx0());
	oPacket->Encode2(GetRx1());
	oPacket->Encode1(!IsHidden());
}