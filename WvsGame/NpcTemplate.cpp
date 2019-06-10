#include "NpcTemplate.h"
#include "User.h"
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Wz\ImgAccessor.h"
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

void NpcTemplate::LoadShop()
{
#undef max
	static WZ::ImgAccessor img("./DataSrv/NpcShop");
	std::string sNpcTemplateName = std::to_string(m_nTemplateID);
	while (sNpcTemplateName.size() < 7)
		sNpcTemplateName = "0" + sNpcTemplateName;

	auto& node = img[sNpcTemplateName];
	int nItemID = 0;
	ShopItem* pItem = nullptr;
	for (auto& item : node)
	{
		nItemID = item["item"];
		if (!ItemInfo::GetInstance()->GetBundleItem(nItemID) &&
			!ItemInfo::GetInstance()->GetEquipItem(nItemID))
			continue;

		pItem = AllocObj(ShopItem);
		pItem->nItemID = nItemID;
		pItem->nPrice = (int)item["price"];
		pItem->nPeriod = (int)item["period"];
		pItem->nQuantity = std::max(1, (int)item["quantity"]);
		pItem->dUnitPrice = (double)item["unitPrice"];
		pItem->nStockMax = item["stock"];
		pItem->nStock = 0;
		pItem->nLastFullStock = 0;

		m_aShopItem.push_back(pItem);
	}
}

void NpcTemplate::RegisterNpc(int nNpcID, void *pProp)
{
	auto data = *((WZ::Node*)pProp);
	NpcTemplate *pTemplate = AllocObj(NpcTemplate);
	pTemplate->m_nTemplateID = nNpcID;
	//pTemplate->m_aShopItem = GetInstance()->GetShopItemList(nNpcID);
	pTemplate->m_nTrunkPut = data["info"]["trunkPut"];
	pTemplate->m_sScriptName = (std::string)data["info"]["script"]["0"]["script"];
	pTemplate->LoadShop();
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
	return m_aShopItem.size() != 0;
}

int NpcTemplate::GetTrunkCost() const
{
	return m_nTrunkPut;
}

NpcTemplate * NpcTemplate::GetInstance()
{
	static NpcTemplate* pInstance = AllocObj(NpcTemplate);
	return pInstance;
}

std::vector<NpcTemplate::ShopItem*>& NpcTemplate::GetShopItem()
{
	return m_aShopItem;
}

void NpcTemplate::EncodeShop(User * pUser, OutPacket * oPacket)
{
	oPacket->Encode4(m_nTemplateID);
	oPacket->Encode2((short)(m_aShopItem.size()));
	if (m_aShopItem.size())
	{
		for (auto& item : m_aShopItem)
			EncodeShopItem(pUser, item, oPacket);
	}
}

void NpcTemplate::EncodeShopItem(User *pUser, NpcTemplate::ShopItem* pItem, OutPacket* oPacket)
{
	int nMaxPerSlot = 0;
	auto pBundleItem = ItemInfo::GetInstance()->GetBundleItem(pItem->nItemID);

	oPacket->Encode4(pItem->nItemID);
	oPacket->Encode4(pItem->nPrice);

	if (!ItemInfo::IsRechargable(pItem->nItemID)) 
	{
		oPacket->Encode2(pItem->nQuantity);
		nMaxPerSlot = (pBundleItem ? pBundleItem->nMaxPerSlot : 1);
	}
	else
	{
		if (pBundleItem)
			oPacket->EncodeBuffer((unsigned char*)&pBundleItem->dSellUnitPrice, 8);
		nMaxPerSlot = SkillInfo::GetInstance()->GetBundleItemMaxPerSlot(
				pItem->nItemID,
				pUser->GetCharacterData()
			);
	}
	oPacket->Encode2(nMaxPerSlot);
}

const std::string& NpcTemplate::GetScriptName() const
{
	return m_sScriptName;
}

