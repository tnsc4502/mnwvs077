#pragma once
#include <map>
#include <vector>
#include <string>

struct GW_ItemSlotBase;
class User;
class OutPacket;

class NpcTemplate
{
public:
	struct ShopItem
	{
		int nNpcID = 0,
			nItemID = 0,
			nPrice = 0,
			nPeriod = 0,
			nTokenItemID = 0,
			nTokenPrice = 0,
			nQuantity = 0,
			nMaxPerSlot = 0,
			nTabIndex = 0,
			nStock = 0,
			nStockMax = 0,
			nLastFullStock = 0,
			nLevelLimited = 0,
			nShowLevMin = 0,
			nShowLevMax = 0;

		double dUnitPrice = 0;
		long long int lilPeriod = 0, liSellStart = 0, liSellEnd = 0;
	};

private:
	int m_nTemplateID = 0, m_nTrunkPut = 0;
	static std::map<int, NpcTemplate*> m_mNpcTemplates;
	std::vector<ShopItem*> m_aShopItem;
	std::string m_sScriptName;

public:
	NpcTemplate();
	~NpcTemplate();

	void Load();
	void LoadShop();
	void RegisterNpc(int nNpcID, void *pProp);
	NpcTemplate* GetNpcTemplate(int dwTemplateID);
	bool HasShop() const;
	int GetTrunkCost() const;
	static NpcTemplate* GetInstance();
	std::vector<ShopItem*>& GetShopItem();
	void EncodeShop(User *pUser, OutPacket *oPacket);
	static void EncodeShopItem(User *pUser, ShopItem* pItem, OutPacket *oPacket);
	const std::string& GetScriptName() const;
};

