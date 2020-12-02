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
	static int FieldGetUserCount(lua_State* L);
	static int FieldCountUserInArea(lua_State* L);
	static int FieldReset(lua_State* L);
	static int FieldEffectScreen(lua_State* L);
	static int FieldEffectSound(lua_State* L);
	static int FieldEffectObject(lua_State* L);
	static int FieldTransferAll(lua_State* L);
	static int FieldEnablePortal(lua_State* L);
	static int FieldGetContiState(lua_State* L);
	static int FieldGetContiBoardingTime(lua_State* L);
	static int FieldGetReactorState(lua_State* L);
	static int FieldIsItemInArea(lua_State* L);
};

