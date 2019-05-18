#pragma once
#include "..\WvsLib\Script\lua.h"

class Field;

class ScriptField
{
	Field *m_pField;
public:
	ScriptField();
	~ScriptField();

	void SetField(Field *pField);

	static ScriptField* GetSelf(lua_State* L);
	static void DestroySelf(lua_State* L, ScriptField* p);
	static void Register(lua_State* L);
	static int FieldGetID(lua_State* L);
	static int FieldSummonMob(lua_State* L);
	static int FieldSummonNpc(lua_State* L);
	static int FieldBroadcastPacket(lua_State* L);
};

