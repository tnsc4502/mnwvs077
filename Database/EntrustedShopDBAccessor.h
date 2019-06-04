#pragma once
#include <vector>

struct GW_ItemSlotBase;

class EntrustedShopDBAccessor
{
public:
	static void MoveItemToShop(int nCharacterID, int nTI, long long int liItemSN);
	static void RestoreItemFromShop(int nCharacterID, int nTI, long long int liItemSN, bool bRemoveFromItemSlot);
	static void LoadEntrustedShopItem(std::vector<GW_ItemSlotBase*>& aItemRet, int nCharacterID);
	static long long int QueryEntrustedShopMoney(int nCharacterID);
	static void UpdateEntrustedShopMoney(int nCharacterID, int nMoney);
};

