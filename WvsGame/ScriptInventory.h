#pragma once
#include "..\WvsLib\Script\lua.h"

class User;

class ScriptInventory
{
	User *m_pUser;

public:
	ScriptInventory();
	~ScriptInventory();
	void SetUser(User *pUser);

	static ScriptInventory* GetSelf(lua_State* L);
	static void DestroySelf(lua_State* L, ScriptInventory* p);
	static void Register(lua_State* L);
	static int InventoryExchange(lua_State* L);
	static int InventoryItemCount(lua_State* L);
	static int InventoryIncSlotCount(lua_State* L);
	static int InventoryGetSlotCount(lua_State* L);
	static int InventoryGetHoldCount(lua_State* L);
	static int InventoryGetFreeCount(lua_State* L);
	static int InventoryRemoveEquippedItem(lua_State* L);
	static int InventoryGetThisItemInHisParty(lua_State* L);
};

