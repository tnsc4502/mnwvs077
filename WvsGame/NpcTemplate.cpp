#include "NpcTemplate.h"
#include "User.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

std::map<int, NpcTemplate*> NpcTemplate::m_mNpcTemplates;

NpcTemplate::NpcTemplate()
{
}


NpcTemplate::~NpcTemplate()
{
}

void NpcTemplate::Load()
{
	auto& refWz = stWzResMan->GetWz(Wz::Npc);
	for (auto& refNpc : refWz)
	{
		RegisterNpc(atoi(refNpc.Name().c_str()), &refNpc);
	}
}

void NpcTemplate::RegisterNpc(int nNpcID, void * pProp)
{
	NpcTemplate *pTemplate = AllocObj(NpcTemplate);
	pTemplate->m_nTemplateID = nNpcID;
	pTemplate->m_aShopItem = GW_Shop::GetInstance()->GetShopItemList(nNpcID);
	m_mNpcTemplates[nNpcID] = pTemplate;
}

NpcTemplate* NpcTemplate::GetNpcTemplate(int dwTemplateID)
{
	auto findIter = m_mNpcTemplates.find(dwTemplateID);
	if (findIter == m_mNpcTemplates.end())
		return nullptr;
	return findIter->second;
}

bool NpcTemplate::HasShop() const
{
	return m_aShopItem != nullptr;
}

NpcTemplate * NpcTemplate::GetInstance()
{
	static NpcTemplate* pInstance = AllocObj(NpcTemplate);
	return pInstance;
}

std::vector<GW_Shop::ShopItem*>* NpcTemplate::GetShopItem()
{
	return m_aShopItem;
}

void NpcTemplate::EncodeShop(User * pUser, OutPacket * oPacket)
{
	oPacket->Encode4(m_nTemplateID);
	oPacket->Encode2((short)(!m_aShopItem ? 0 : m_aShopItem->size()));
	if (m_aShopItem)
	{
		for (auto& item : *m_aShopItem)
			EncodeShopItem(pUser, item, oPacket);
	}
}

void NpcTemplate::EncodeShopItem(User *pUser, GW_Shop::ShopItem * pItem, OutPacket * oPacket)
{
	oPacket->Encode4(pItem->nItemID);
	oPacket->Encode4(pItem->nPrice);

	if (!ItemInfo::IsRechargable(pItem->nItemID)) 
	{
		oPacket->Encode2(pItem->nQuantity);
		oPacket->Encode2(pItem->nMaxPerSlot);
	}
	else
	{
		auto pBundleItem = ItemInfo::GetInstance()->GetBundleItem(pItem->nItemID);
		double dPrice = 0;
		if (pBundleItem)
		{
			dPrice = pBundleItem->dSellUnitPrice;
			oPacket->EncodeBuffer((unsigned char*)&dPrice, 8);
		}
		oPacket->Encode2(
			SkillInfo::GetInstance()->GetBundleItemMaxPerSlot(
				pItem->nItemID,
				pUser->GetCharacterData()
			));
	}
}

