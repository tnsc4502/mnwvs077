#pragma once
#include <vector>
#include "MiniRoomBase.h"

struct GW_ItemSlotBase;

class PersonalShop : public MiniRoomBase
{
protected:
	struct Item
	{
		int nTI = 0, nPOS = 0, nSet = 0, nNumber = 0, nPrice = 0;
		GW_ItemSlotBase *pItem;
	};

	int m_nSlotCount = 10;
	std::vector<Item> m_aItem;

public:

	enum PersonalShopMessgae
	{
		e_Message_InvalidFieldID = 1,
		e_Message_ShopClosed = 3,
		e_Message_ShopItemUpdated = 21,
	};

	enum PersonalShopRequest
	{
		rq_PShop_PutItem = 18,
		rq_PShop_BuyItem = 19,
		rq_PShop_MoveItemToInventory = 23,
	};

	PersonalShop();
	~PersonalShop();

	void OnPutItem(User *pUser, InPacket *iPacket);
	void OnBuyItem(User *pUser, InPacket *iPacket);
	void OnMoveItemToInventory(User *pUser, InPacket *iPacket);
	virtual GW_ItemSlotBase* MoveItemToShop(GW_ItemSlotBase *pItem, User *pUser, int nTI, int nPOS, int nNumber, int *nPOS2);
	virtual bool RestoreItemFromShop(User *pUser, PersonalShop::Item* psItem);

	virtual void DoTransaction(User *pUser, int nSlot, Item* psItem, int nNumber);
	virtual void OnPacket(User *pUser, int nType, InPacket *iPacket);
	virtual void OnLeave(User *pUser, int nLeaveType);
	virtual void EncodeEnterResult(User *pUser, OutPacket *oPacket);
	virtual void EncodeItemList(OutPacket *oPacket);
	virtual void Encode(OutPacket *oPacket);
	virtual void Release();
	void BroadcastItemList();
};

