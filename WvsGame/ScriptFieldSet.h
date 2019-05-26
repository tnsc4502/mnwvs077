#pragma once
#include "..\WvsLib\Script\lua.h"

class Script;
class FieldSet;

class ScriptFieldSet
{
	FieldSet *m_pFieldSet;
public:
	ScriptFieldSet();
	~ScriptFieldSet();
	void SetFieldSet(FieldSet *pFieldSet);

	static ScriptFieldSet* GetSelf(lua_State* L);
	static void DestroySelf(lua_State* L, ScriptFieldSet* p);
	static void Register(lua_State* L);
	static int FieldSetEnter(lua_State* L);
	static int FieldSetGetVar(lua_State* L);
	static int FieldSetSetVar(lua_State* L);
	static int FieldSetTransferAll(lua_State* L);
	static int FieldSetGetUserCount(lua_State* L);
	static int FieldSetIncExpAll(lua_State* L);
};

