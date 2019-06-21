#include "ScriptFieldSet.h"
#include "Script.h"
#include "FieldMan.h"
#include "FieldSet.h"
#include "User.h"
#include "ScriptUser.h"
#include "PartyMan.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

ScriptFieldSet::ScriptFieldSet()
{
}

ScriptFieldSet::~ScriptFieldSet()
{
}

void ScriptFieldSet::SetFieldSet(FieldSet * pFieldSet)
{
	m_pFieldSet = pFieldSet;
}

ScriptFieldSet * ScriptFieldSet::GetSelf(lua_State *L)
{
	auto pSys = ((Script*)L->selfPtr);	
	//Call from FieldSet script itself.
	if (pSys->m_pUniqueFieldSet != nullptr)
		return ((FieldSet*)(pSys->m_pUniqueFieldSet))->GetScriptFieldSet();

	//Call from Npc
	const char* sFieldSetName = luaL_checkstring(L, 1);
	auto pFieldSet = FieldMan::GetInstance()->GetFieldSet(sFieldSetName);
	if (pFieldSet == nullptr)
		return nullptr;
	
	ScriptFieldSet* p = AllocObj(ScriptFieldSet);
	p->m_pFieldSet = pFieldSet;
	return p;
}

void ScriptFieldSet::DestroySelf(lua_State *L, ScriptFieldSet *p)
{
	if(!((Script*)L->selfPtr)->m_pUniqueFieldSet)
		FreeObj(p);
}

void ScriptFieldSet::Register(lua_State * L)
{
	static luaL_Reg FieldSetMetatable[] = {
		{ "enter", FieldSetEnter },
		{ "join", FieldSetJoin },
		{ "setVar", FieldSetSetVar },
		{ "getVar", FieldSetGetVar },
		{ "transferAll", FieldSetTransferAll },
		{ "transferParty", FieldSetTransferParty },
		{ "getUserCount", FieldSetGetUserCount },
		{ "incExpAll", FieldSetIncExpAll },
		{ "resetTimeLimit", FieldSetResetTimeLimit },
		{ "setMCTeam", FieldSetSetMCTeam },
		{ "destroyClock", FieldSetDestroyClock },
		{ "broadcastMsg", FieldSetBroadcastMsg },
		{ "getReactorState", FieldGetReactorState },
		{ "setReactorState", FieldSetReactorState },
		{ NULL, NULL }
	};

	static luaL_Reg FieldSetTable[] = {
		{ NULL, NULL }
	};

	luaW_register<ScriptFieldSet>(L, "FieldSet",
		FieldSetTable, 
		FieldSetMetatable, 
		&(ScriptFieldSet::GetSelf),
		&(ScriptFieldSet::DestroySelf)
		);
}

int ScriptFieldSet::FieldSetEnter(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	int nUserID = (int)luaL_checkinteger(L, 2);
	lua_pushinteger(L, self->m_pFieldSet->Enter(nUserID, 0));
	return 1;
}

int ScriptFieldSet::FieldSetJoin(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	int nUserID = (int)luaL_checkinteger(L, 2);
	lua_pushinteger(L, self->m_pFieldSet->Enter(nUserID, 0, true));
	return 1;
}

int ScriptFieldSet::FieldSetGetVar(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	const char* sVarName = luaL_checkstring(L, 2);
	lua_pushstring(L, self->m_pFieldSet->GetVar(sVarName).c_str());
	return 1;
}

int ScriptFieldSet::FieldSetSetVar(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	const char* sVarName = luaL_checkstring(L, 2);
	const char* sVal = luaL_checkstring(L, 3);
	self->m_pFieldSet->SetVar(sVarName, sVal);
	return 1;
}

int ScriptFieldSet::FieldSetTransferAll(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	int nFieldID = (int)luaL_checkinteger(L, 2);
	const char* sVal = luaL_checkstring(L, 3);
	self->m_pFieldSet->TransferAll(nFieldID, sVal);
	return 1;
}

int ScriptFieldSet::FieldSetTransferParty(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	int nPartyID = (int)luaL_checkinteger(L, 2);
	int nFieldID = (int)luaL_checkinteger(L, 3);
	const char* sVal = luaL_checkstring(L, 4);
	self->m_pFieldSet->TransferParty(nPartyID, nFieldID, sVal);
	return 1;
}

int ScriptFieldSet::FieldSetGetUserCount(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	lua_pushinteger(L, self->m_pFieldSet->GetUserCount());
	return 1;
}

int ScriptFieldSet::FieldSetIncExpAll(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	int nInc = (int)luaL_checkinteger(L, 2);
	self->m_pFieldSet->IncExpAll(nInc);
	return 1;
}

int ScriptFieldSet::FieldSetResetTimeLimit(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	int nLimitSet = (int)luaL_checkinteger(L, 2);
	bool bResetTime = ((int)luaL_checkinteger(L, 3)) == 1;
	self->m_pFieldSet->ResetTimeLimit(nLimitSet, bResetTime);
	return 1;
}

int ScriptFieldSet::FieldSetSetMCTeam(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	int nPartyBoss = (int)luaL_checkinteger(L, 2);
	int nSet = (int)luaL_checkinteger(L, 3);
	auto pParty = PartyMan::GetInstance()->GetPartyByCharID(nPartyBoss);
	if (pParty)
	{
		int anCharacterID[6];
		PartyMan::GetInstance()->GetSnapshot(pParty->nPartyID, anCharacterID);
		for (int nID : anCharacterID)
		{
			auto pUser = User::FindUser(nID);
			if (pUser)
				pUser->SetMCarnivalTeam(nSet);
		}
	}
	return 1;
}

int ScriptFieldSet::FieldSetDestroyClock(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	self->m_pFieldSet->DestroyClock();
	return 1;
}

int ScriptFieldSet::FieldSetBroadcastMsg(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	int nType = (int)luaL_checkinteger(L, 2);
	std::string sMsg = luaL_checkstring(L, 3);
	int nNPCTemplateID = (int)(lua_gettop(L) > 3 ? luaL_checkinteger(L, 4) : 0);
	self->m_pFieldSet->BroadcastMsg(nType, sMsg, nNPCTemplateID);
	return 1;
}

int ScriptFieldSet::FieldGetReactorState(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	int nFieldIdx = (int)luaL_checkinteger(L, 2);
	std::string sName = luaL_checkstring(L, 3);
	lua_pushinteger(L, self->m_pFieldSet->GetReactorState(nFieldIdx, sName));
	return 1;
}

int ScriptFieldSet::FieldSetReactorState(lua_State * L)
{
	ScriptFieldSet* self = luaW_check<ScriptFieldSet>(L, 1);
	int nFieldIdx = (int)luaL_checkinteger(L, 2);
	std::string sName = luaL_checkstring(L, 3);
	int nState = (int)luaL_checkinteger(L, 4);
	int nOrder = (int)luaL_checkinteger(L, 5);
	self->m_pFieldSet->SetReactorState(
		nFieldIdx,
		sName,
		nState,
		nOrder
	);
	return 1;
}
