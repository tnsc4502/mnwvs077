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
#include "..\WvsLib\Logger\WvsLogger.h"

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
	if (m_mEmployer.find(nCharacterID) == m_mEmployer.end())
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

void EntrustedShopMan::CreateEntrustedShop(LocalServer * pSrv, int nCharacterID, int SlotCount, long long int liCashItemSN, InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	ShopData shop;
	shop.sEmployer = iPacket->DecodeStr();
	shop.sTitle = iPacket->DecodeStr();
	shop.nFieldID = iPacket->Decode4();
	shop.nChannelID = iPacket->Decode4();
	shop.nShopSN = iPacket->Decode4();
	m_mEmployer.insert({ nCharacterID, shop });
}

void EntrustedShopMan::RemoveEntrustedShop(LocalServer * pSrv, int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	m_mEmployer.erase(nCharacterID);
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
	if (m_mEmployer.find(nCharacterID) != m_mEmployer.end())
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

void EntrustedShopMan::UpdateItemListRequest(LocalServer * pSrv, int nCharacterID, InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	auto findIter = m_mEmployer.find(nCharacterID);
	if (findIter == m_mEmployer.end())
		return;

	auto& shop = findIter->second;
	shop.mItem.clear();

	int nCount = iPacket->Decode1();
	long long int liItemSN = 0;
	for (int i = 0; i < nCount; ++i)
	{
		ShopItem item;
		liItemSN = iPacket->Decode8();
		item.nNumber = iPacket->Decode2();
		item.nSet = iPacket->Decode2();
		item.nPrice = iPacket->Decode4();
		if (iPacket->Decode1() == GW_ItemSlotBase::EQUIP) 
		{
			item.pItem.reset(GW_ItemSlotBase::CreateItem(iPacket->Decode1()));
			item.pItem->RawDecode(iPacket);
		}
		shop.mItem.insert({ liItemSN, item });
	}
}

const EntrustedShopMan::ShopData * EntrustedShopMan::GetShopData(int nCharacterID)
{
	auto findIter = m_mEmployer.find(nCharacterID);
	if (findIter == m_mEmployer.end())
		return nullptr;

	return &(findIter->second);
}

std::recursive_mutex & EntrustedShopMan::GetLock()
{
	return m_mtxLock;
}

const EntrustedShopMan::ShopItem * EntrustedShopMan::ShopData::GetShopItem(long long int liItemSN) const
{
	auto findIter = mItem.find(liItemSN);
	if (findIter == mItem.end())
		return nullptr;

	return &(findIter->second);
}
