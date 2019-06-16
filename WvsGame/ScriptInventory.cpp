#include "ScriptInventory.h"
#include "QWUInventory.h"
#include "Script.h"
#include "User.h"
#include "BackupItem.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterSlotCount.h"

#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

ScriptInventory::ScriptInventory()
{
}


ScriptInventory::~ScriptInventory()
{
}

void ScriptInventory::SetUser(User * pUser)
{
	m_pUser = pUser;
}

ScriptInventory* ScriptInventory::GetSelf(lua_State* L)
{
	int nUserID = (int)luaL_checkinteger(L, 1);
	auto pUser = User::FindUser(nUserID);
	if (!pUser)
		return nullptr;
	ScriptInventory *p = AllocObj(ScriptInventory);
	p->m_pUser = pUser;
	return p;
}

void ScriptInventory::DestroySelf(lua_State * L, ScriptInventory * p)
{
	FreeObj(p);
}

void ScriptInventory::Register(lua_State * L)
{
	static luaL_Reg InvMetatable[] = {
		{ "exchange", InventoryExchange },
		{ "itemCount", InventoryItemCount },
		{ "incSlotCount", InventoryIncSlotCount },
		{ "slotCount", InventoryGetSlotCount },
		{ "holdCount", InventoryGetHoldCount },
		{ "freeCount", InventoryGetFreeCount },
		{ NULL, NULL }
	};

	static luaL_Reg InvTable[] = {
		{ NULL, NULL }
	};

	luaW_register<ScriptInventory>(L, "Inventory", 
		InvTable, 
		InvMetatable, 
		&(ScriptInventory::GetSelf),
		&(ScriptInventory::DestroySelf));
}

int ScriptInventory::InventoryExchange(lua_State * L)
{
	ScriptInventory* self = luaW_check<ScriptInventory>(L, 1);
	int nMoney = (int)luaL_checkinteger(L, 2), nItemID, nCount;
	int nArg = lua_gettop(L);
	std::vector<ExchangeElement> aExchange;

	for (int i = 3; i <= nArg - 1;)
	{
		nItemID = (int)lua_tointeger(L, i);
		nCount = (int)lua_tointeger(L, i + 1);
		aExchange.push_back({});
		aExchange.back().m_nItemID = nItemID;
		aExchange.back().m_nCount = nCount;

		i += 2;
	}
	int nResult = QWUInventory::Exchange(self->m_pUser, nMoney, aExchange);

	/*
	0 = Success
	1 = Meso Insufficient
	2 = No Available Slot
	3 = Insufficient Item In The Slot
	*/


	lua_pushinteger(L, nResult);
	return 1;
}

int ScriptInventory::InventoryItemCount(lua_State * L)
{
	ScriptInventory* self = luaW_check<ScriptInventory>(L, 1);
	int nItemID = (int)luaL_checkinteger(L, 2);
	int nCount = self->m_pUser->GetCharacterData()->GetItemCount(nItemID / 1000000, nItemID);
	lua_pushinteger(L, nCount);
	return 1;
}

int ScriptInventory::InventoryIncSlotCount(lua_State * L)
{
	ScriptInventory* self = luaW_check<ScriptInventory>(L, 1);
	int nTI = (int)luaL_checkinteger(L, 2);
	int nCount = (int)luaL_checkinteger(L, 3);
	std::lock_guard<std::recursive_mutex> lock(self->m_pUser->GetLock());
	if(nTI < 0
		|| nTI > 5
		|| self->m_pUser->GetCharacterData()->mSlotCount->aSlotCount[nTI] + nCount > 80)
		lua_pushinteger(L, 0);
	else 
	{
		self->m_pUser->GetCharacterData()->mSlotCount->aSlotCount[nTI] += nCount;
		lua_pushinteger(L, 1);
		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnInventoryGrow);
		oPacket.Encode1(nTI);
		oPacket.Encode1(self->m_pUser->GetCharacterData()->mSlotCount->aSlotCount[nTI]);
		self->m_pUser->SendPacket(&oPacket);
	}
	return 1;
}

int ScriptInventory::InventoryGetSlotCount(lua_State * L)
{
	ScriptInventory* self = luaW_check<ScriptInventory>(L, 1);
	int nTI = (int)luaL_checkinteger(L, 2);
	lua_pushinteger(L, QWUInventory::GetSlotCount(self->m_pUser, nTI));
	return 1;
}

int ScriptInventory::InventoryGetHoldCount(lua_State * L)
{
	ScriptInventory* self = luaW_check<ScriptInventory>(L, 1);
	int nTI = (int)luaL_checkinteger(L, 2);
	lua_pushinteger(L, QWUInventory::GetHoldCount(self->m_pUser, nTI));
	return 1;
}

int ScriptInventory::InventoryGetFreeCount(lua_State * L)
{
	ScriptInventory* self = luaW_check<ScriptInventory>(L, 1);
	int nTI = (int)luaL_checkinteger(L, 2);
	lua_pushinteger(L, QWUInventory::GetFreeCount(self->m_pUser, nTI));
	return 1;
}
