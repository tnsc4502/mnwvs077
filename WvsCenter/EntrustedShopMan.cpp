#include "EntrustedShopMan.h"
#include "LocalServer.h"
#include "WvsWorld.h"
#include "..\Database\EntrustedShopDBAccessor.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\WvsGame\ItemInfo.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsGame\UserPacketTypes.hpp"
#include "..\WvsCenter\CenterPacketTypes.hpp"
#include "..\WvsLib\Memory\ZMemory.h"

EntrustedShopMan::EntrustedShopMan()
{
}

EntrustedShopMan::~EntrustedShopMan()
{
}

EntrustedShopMan* EntrustedShopMan::GetInstance()
{
	static EntrustedShopMan *pInstance = new EntrustedShopMan;

	return pInstance;
}

int EntrustedShopMan::CheckEntrustedShopOpenPossible(int nCharacterID, long long int liCashItemSN)
{
	if (m_sEmployer.find(nCharacterID) == m_sEmployer.end())
	{
		std::vector<ZUniquePtr<GW_ItemSlotBase>> aItem;
		EntrustedShopDBAccessor::LoadEntrustedShopItem(
			aItem, nCharacterID
		);
		if (aItem.size() > 0 || EntrustedShopDBAccessor::QueryEntrustedShopMoney(nCharacterID))
			return EntrustedShopCheckResult::res_EShop_OpenCheck_NonEmptyStoreBank;
		else
			return EntrustedShopCheckResult::res_EShop_OpenCheck_Valid;
	}
	else
		return EntrustedShopCheckResult::res_EShop_OpenCheck_AlreadyOpened;
}

void EntrustedShopMan::CheckEntrustedShopOpenPossible(LocalServer* pSrv, int nCharacterID, long long int liCashItemSN)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	auto pwUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
	if (pwUser)
	{
		OutPacket oPacket;
		oPacket.Encode2(CenterResultPacketType::RemoteBroadcasting);
		oPacket.Encode4(nCharacterID);
		oPacket.Encode2(UserSendPacketType::UserLocal_OnEntrustedShopCheckResult);
		oPacket.Encode1(CheckEntrustedShopOpenPossible(nCharacterID, liCashItemSN));
		pwUser->SendPacket(&oPacket);
	}
}

void EntrustedShopMan::CreateEntrustedShop(LocalServer * pSrv, int nCharacterID, int SlotCount, long long int liCashItemSN)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	m_sEmployer.insert(nCharacterID);
}

void EntrustedShopMan::RemoveEntrustedShop(LocalServer * pSrv, int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	m_sEmployer.erase(nCharacterID);
}

void EntrustedShopMan::SaveItem(LocalServer *pSrv, int nCharacterID, InPacket *iPacket)
{
	int nCount = iPacket->Decode1(), nTI = 0;
	for (int i = 0; i < nCount; ++i)
	{
		nTI = iPacket->Decode1();
		if (iPacket->Decode1())
		{
			ZUniquePtr<GW_ItemSlotBase> pItem = GW_ItemSlotBase::CreateItem(
				nTI == GW_ItemSlotBase::EQUIP ?
				GW_ItemSlotBase::GW_ItemSlotEquip_Type :
				GW_ItemSlotBase::GW_ItemSlotBundle_Type
			);
			iPacket->Decode1(); //POS
			pItem->nType = (GW_ItemSlotBase::GW_ItemSlotType)nTI;
			pItem->Decode(iPacket, true);
			pItem->nPOS = GW_ItemSlotBase::LOCK_POS;
			pItem->Save(nCharacterID);
		}
		EntrustedShopDBAccessor::MoveItemToShop(
			nCharacterID,
			nTI,
			iPacket->Decode8()
		);
	}
}

void EntrustedShopMan::ItemNumberChanged(LocalServer *pSrv, int nCharacterID, InPacket *iPacket)
{
	EntrustedShopDBAccessor::UpdateEntrustedShopMoney(nCharacterID, (int)iPacket->Decode8());
	int nCount = iPacket->Decode1(), nTI = 0, nNumber = 0;
	for (int i = 0; i < nCount; ++i)
	{
		nTI = iPacket->Decode1();
		nNumber = iPacket->Decode2();
		ZUniquePtr<GW_ItemSlotBase> pItem = GW_ItemSlotBase::CreateItem(
			nTI == GW_ItemSlotBase::EQUIP ?
			GW_ItemSlotBase::GW_ItemSlotEquip_Type :
			GW_ItemSlotBase::GW_ItemSlotBundle_Type
		);
		iPacket->Decode1(); //POS
		pItem->nType = (GW_ItemSlotBase::GW_ItemSlotType)nTI;
		pItem->Decode(iPacket, true);
		pItem->nPOS = GW_ItemSlotBase::LOCK_POS;
		if (nNumber)
			pItem->Save(nCharacterID);
		else
			EntrustedShopDBAccessor::RestoreItemFromShop(
				nCharacterID,
				nTI,
				pItem->liItemSN,
				!(pItem->nType == GW_ItemSlotBase::EQUIP || ItemInfo::IsRechargable(pItem->nItemID)));
	}
}

void EntrustedShopMan::LoadItemRequest(LocalServer *pSrv, int nCharacterID)
{
	OutPacket oPacket;
	oPacket.Encode2(CenterResultPacketType::EntrustedShopResult);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode1(EntrustedShopCheckResult::res_EShop_LoadItemResult);

	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	if (m_sEmployer.find(nCharacterID) != m_sEmployer.end())
		oPacket.Encode1(-1);
	else
	{
		std::vector<ZUniquePtr<GW_ItemSlotBase>> aItem;
		EntrustedShopDBAccessor::LoadEntrustedShopItem(
			aItem, nCharacterID
		);
		oPacket.Encode1((char)aItem.size());
		oPacket.Encode8(EntrustedShopDBAccessor::QueryEntrustedShopMoney(nCharacterID));
		for (auto& pItem : aItem) 
		{
			oPacket.Encode1(pItem->nType);
			pItem->Encode(&oPacket, true);
		}
	}
	pSrv->SendPacket(&oPacket);
}
