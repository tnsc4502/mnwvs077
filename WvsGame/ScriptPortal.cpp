#include "ScriptPortal.h"
#include "Portal.h"
#include "Script.h"
#include "ScriptField.h"
#include "..\WvsLib\Script\luaconf.h"
#include "..\WvsLib\Script\lvm.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

ScriptPortal::ScriptPortal()
{
}

ScriptPortal::~ScriptPortal()
{
}

void ScriptPortal::SetPortal(Portal * pPortal)
{
}

ScriptPortal * ScriptPortal::GetSelf(lua_State * L)
{
	auto pSys = ((Script*)L->selfPtr);
	if (pSys->GetPortal() == nullptr)
		return nullptr;

	if (pSys->m_pUniqueScriptBinding == nullptr)
		pSys->m_pUniqueScriptBinding = AllocObj(ScriptPortal);
	((ScriptPortal*)pSys->m_pUniqueScriptBinding)->m_pPortal = pSys->GetPortal();
	return (ScriptPortal*)pSys->m_pUniqueScriptBinding;
}

void ScriptPortal::DestroySelf(lua_State * L, ScriptPortal * p)
{
	FreeObj(p);
}

void ScriptPortal::Register(lua_State * L)
{
	static luaL_Reg PMetatable[] = {
		{ "getPortalID", PortalGetPortalID },
		{ "getField", PortalGetField },
		{ NULL, NULL }
	};

	static luaL_Reg PTable[] = {
		{ NULL, NULL }
	};

	luaW_register<ScriptPortal>(L, "Portal",
		PTable,
		PMetatable,
		&(ScriptPortal::GetSelf),
		&(ScriptPortal::DestroySelf));
}

int ScriptPortal::PortalGetPortalID(lua_State * L)
{
	ScriptPortal* self = luaW_check<ScriptPortal>(L, 1);
	lua_pushinteger(L, self->m_pPortal->GetID());
	return 1;
}

int ScriptPortal::PortalGetField(lua_State * L)
{
	ScriptPortal* self = luaW_check<ScriptPortal>(L, 1);
	ScriptField* obj =
		(self->m_pPortal == nullptr ? nullptr :
		(self->m_pPortal->GetField() == nullptr ? nullptr : AllocObj(ScriptField)));
	if (obj)
		obj->SetField(self->m_pPortal->GetField());

	((Script*)L->selfPtr)->PushClassObject(obj);
	return 1;
}