#include "ScriptFieldSet.h"
#include "Script.h"
#include "FieldMan.h"
#include "FieldSet.h"
#include "User.h"
#include "ScriptUser.h"
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
	luaL_Reg FieldSetMetatable[] = {
		{ "enter", FieldSetEnter },
		{ "setVar", FieldSetSetVar },
		{ "getVar", FieldSetGetVar },
		{ "transferAll", FieldSetTransferAll },
		{ "getUserCount", FieldSetGetUserCount },
		{ "incExpAll", FieldSetIncExpAll },
		{ NULL, NULL }
	};

	luaL_Reg FieldSetTable[] = {
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
