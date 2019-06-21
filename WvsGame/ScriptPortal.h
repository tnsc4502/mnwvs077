#pragma once
#include "..\WvsLib\Script\lua.h"

class Portal;

class ScriptPortal
{
	Portal *m_pPortal;
public:
	ScriptPortal();
	~ScriptPortal();

	void SetPortal(Portal *pPortal);
	static ScriptPortal* GetSelf(lua_State* L);
	static void DestroySelf(lua_State* L, ScriptPortal* p);
	static void Register(lua_State* L);
	static int PortalGetPortalID(lua_State* L);
	static int PortalGetField(lua_State* L);
};

