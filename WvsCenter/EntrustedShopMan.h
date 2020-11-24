#pragma once
#include "..\WvsLib\Memory\ZMemory.h"
#include <string>
#include <unordered_map>
#include <mutex>

struct GW_ItemSlotBase;
class LocalServer;
class InPacket;

class EntrustedShopMan
{
public:
	struct ShopItem
	{
		int nNumber, nSet, nPrice;
		ZSharedPtr<GW_ItemSlotBase> pItem;
	};

	struct ShopData
	{
		std::string sEmployer, sTitle;
		int nEmployerID = 0, 
			nFieldID = 0, 
			nChannelID = 0,
			nShopSN = 0;

		std::unordered_map<long long int, ShopItem> mItem;
		const ShopItem* GetShopItem(long long int liItemSN) const;
	};

private:
	EntrustedShopMan();
	~EntrustedShopMan();

	std::unordered_map<int, ShopData> m_mEmployer;
	std::recursive_mutex m_mtxLock;

public:
	enum EntrustedShopRequest
	{
		req_EShop_OpenCheck = 0x00,
		req_EShop_RegisterShop = 0x01,
		req_EShop_UnRegisterShop = 0x02,
		req_EShop_SaveItemRequest = 0x03,
		req_EShop_ItemNumberChanged = 0x04,
		req_EShop_LoadItemRequest = 0x05,
		req_EShop_UpdateItemListRequest = 0x06,
	};

	enum EntrustedShopCheckResult
	{
		res_EShop_OpenCheck_NonEmptyStoreBank = 0x04,
		res_EShop_OpenCheck_AlreadyOpened = 0x05,
		res_EShop_OpenCheck_Valid = 0x06,
		res_EShop_LoadItemResult = 0x07,
	};


	static EntrustedShopMan* GetInstance();
	int CheckEntrustedShopOpenPossible(int nCharacterID, long long int liCashItemSN);
	void CheckEntrustedShopOpenPossible(LocalServer *pSrv, int nCharacterID, long long int liCashItemSN);
	void CreateEntrustedShop(LocalServer *pSrv, int nCharacterID, int SlotCount, long long int liCashItemSN, InPacket *iPacket);
	void RemoveEntrustedShop(LocalServer *pSrv, int nCharacterID);
	void SaveItem(LocalServer *pSrv, int nCharacterID, InPacket *iPacket);
	void ItemNumberChanged(LocalServer *pSrv, int nCharacterID, InPacket *iPacket);
	void LoadItemRequest(LocalServer *pSrv, int nCharacterID);
	void UpdateItemListRequest(LocalServer *pSrv, int nCharacterID, InPacket *iPacket);
	const ShopData* GetShopData(int nCharacterID);
	std::recursive_mutex& GetLock();
};

