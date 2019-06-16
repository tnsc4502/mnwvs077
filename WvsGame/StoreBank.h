#pragma once
#include <mutex>
#include <vector>
#include "..\WvsLib\Memory\ZMemory.h"

class User;
class InPacket;
struct GW_ItemSlotBase;

class StoreBank
{
	std::recursive_mutex m_mtxLock;
	User *m_pUser;

	int m_nStoreBankTemplateID;
	long long int m_liShopMoney = 0;

	std::vector<ZSharedPtr<GW_ItemSlotBase>> m_aItem;

public:
	enum StoreBankRequest
	{
		rq_OnOpenStoreBank = 0x11,
		rq_MoveItemToInventory = 0x16,
		rq_OnCloseStoreBank = 0x18
	};

	enum StoreBankResult
	{
		res_OnItemRestoredToSlot = 0x1A,
		res_OnSelectStoreBankNPC = 0x1F,
		res_OnItemLoadDone = 0x20
	};
	StoreBank(User *pUser);
	~StoreBank();
	void OnPacket(InPacket *iPacket);
	void OnSelectStoreBankNPC();
	void OnOpenStoreBank();
	void OnItemLoadDone(InPacket *iPacket);
	void MoveItemToInventory(InPacket *iPacket);
	void SetStoreBankTemplateID(int nTemplateID);
};

