#include "ScriptField.h"
#include "ScriptPacket.h"
#include "Script.h"
#include "Field.h"
#include "FieldMan.h"
#include "LifePool.h"
#include "Mob.h"
#include "MobTemplate.h"
#include "StaticFoothold.h"
#include "WvsPhysicalSpace2D.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"

ScriptField::ScriptField()
{
}


ScriptField::~ScriptField()
{
}

void ScriptField::SetField(Field * pField)
{
	m_pField = pField;
}

ScriptField * ScriptField::GetSelf(lua_State * L)
{
	int nFieldID = (int)luaL_checkinteger(L, 1);
	auto pField = FieldMan::GetInstance()->GetField(nFieldID);
	if (!pField)
		return nullptr;
	ScriptField *p = AllocObj(ScriptField);
	p->m_pField = pField;
	return p;
}

void ScriptField::DestroySelf(lua_State * L, ScriptField * p)
{
	FreeObj(p);
}

void ScriptField::Register(lua_State * L)
{
	luaL_Reg InvMetatable[] = {
		{ "getID", FieldGetID },
		{ "summonMob", FieldSummonMob },
		{ "summonNpc", FieldSummonNpc },
		{ "broadcast", FieldBroadcastPacket },
		{ NULL, NULL }
	};

	luaL_Reg InvTable[] = {
		{ NULL, NULL }
	};

	luaW_register<ScriptField>(L, "Field",
		InvTable,
		InvMetatable,
		&(ScriptField::GetSelf),
		&(ScriptField::DestroySelf));
}

int ScriptField::FieldGetID(lua_State * L)
{
	ScriptField* self = luaW_check<ScriptField>(L, 1);
	lua_pushinteger(L, self->m_pField->GetFieldID());
	return 1;
}

int ScriptField::FieldSummonMob(lua_State * L)
{
	ScriptField* self = luaW_check<ScriptField>(L, 1);
	int nTemplateID = (int)luaL_checkinteger(L, 2);
	int nPosX = (int)luaL_checkinteger(L, 3);
	int nPosY = (int)luaL_checkinteger(L, 4);
	int nFhX = 0, nFhY = 0;
	Mob mob;
	mob.SetPosX(nPosX);
	mob.SetPosY(nPosY);
	mob.SetTemplateID(nTemplateID);
	mob.SetMobTemplate(MobTemplate::GetMobTemplate(mob.GetTemplateID()));
	self->m_pField->GetLifePool()->CreateMob(
		mob,
		nPosX,
		nPosY,
		0,
		0,
		-2,
		0,
		0,
		0,
		nullptr
	);
	return 1;
}

int ScriptField::FieldSummonNpc(lua_State * L)
{
	ScriptField* self = luaW_check<ScriptField>(L, 1);
	return 1;
}

int ScriptField::FieldBroadcastPacket(lua_State * L)
{
	ScriptField* self = luaW_check<ScriptField>(L, 1);
	ScriptPacket* packet = luaW_check<ScriptPacket>(L, 2);
	self->m_pField->BroadcastPacket(packet->GetPacket());
	return 1;
}
