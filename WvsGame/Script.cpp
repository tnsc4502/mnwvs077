#include "Script.h"
#include <memory>
#include "User.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "QWUser.h"
#include "..\WvsLib\Script\lvm.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

Script * Script::GetSelf(lua_State * L)
{
	long long ptr = luaL_checkinteger(L, 1);
	return (Script*)(ptr);
}

void Script::DestroySelf(lua_State * L, Script * p)
{
	//FreeObj(p);
}

void Script::Register(lua_State * L)
{
	luaL_Reg SysMetatable[] = {
		{ NULL, NULL }
	};

	luaL_Reg SysTable[] = {
		{ "random", ScriptSysRandom },
		{ NULL, NULL }
	};

	luaW_register<Script>(L, "System", 
		SysTable, 
		SysMetatable,
		&(Script::GetSelf),
		&(Script::DestroySelf));
}

void Script::Wait()
{
	lua_yield(L, 0);
}

void Script::Notify()
{
	Run();
}

Script::Script(const std::string & file, int nTemplateID, Field *pField, const std::vector<void(*)(lua_State*)>& aReg) :
	L(luaL_newstate())
{
	C = lua_newthread(L);
	m_fileName = file;
	m_nTemplateID = nTemplateID;
	m_pField = pField;
	L->selfPtr = this;
	Register(L);
	for (auto& f : aReg)
		f(L);
}

int Script::GetID() const
{
	return m_nTemplateID;
}

void Script::SetUser(User * pUser)
{
	m_pUser = pUser;
	if (pUser)
	{
		lua_pushinteger(L, pUser->GetUserID());
		lua_setglobal(L, "userID");
	}
}

User * Script::GetUser()
{
	return m_pUser;
}

Field * Script::GetField()
{
	return m_pField;
}

lua_State * Script::GetLuaState()
{
	return L;
}

lua_State * Script::GetLuaCoroutineState()
{
	return C;
}

void Script::PushInteger(const std::string & strVarName, int nVarValue)
{
	lua_pushinteger(L, nVarValue);
	lua_setglobal(L, strVarName.c_str());
}

void Script::PushNumber(const std::string & strVarName, double dVarValue)
{
	lua_pushnumber(L, dVarValue);
	lua_setglobal(L, strVarName.c_str());
}

void Script::PushString(const std::string & strVarName, const std::string & sVarValue)
{
	lua_pushstring(L, sVarValue.c_str());
	lua_setglobal(L, strVarName.c_str());
}

Script::NPCConversationState & Script::GetConverstaionState()
{
	return m_sState;
}

void Script::SetLastConversationInfo(const NPCConverstaionInfo & refInfo)
{
	m_sLastConversationInfo = refInfo;
}

Script::NPCConverstaionInfo & Script::GetLastConversationInfo()
{
	return m_sLastConversationInfo;
}

int Script::ScriptSysRandom(lua_State * L)
{
	auto liRand = Rand32::GetInstance()->Random();
	auto liMin = luaL_checkinteger(L, 1);
	auto liMax = luaL_checkinteger(L, 2);
	lua_pushinteger(L, (liMin + (liRand % (liMax - liMin))));
	return 1;
}

void Script::Run()
{
	//m_bResume = 1代表被yield的腳本繼續執行並且把對話結果壓入堆疊中
	auto nResult = lua_resume(L, C, m_sState.m_bResume ? 1 : 0);
	m_sState.m_bResume = false;
	if (nResult == LUA_OK || nResult != LUA_YIELD)
		Abort();
}

void Script::Run(const std::string & sFunc)
{
	lua_pcall(GetLuaState(), 0, 0, 0); //Initialize all functions
	lua_getglobal(GetLuaState(), sFunc.c_str());
	Run();
}

void Script::Abort()
{
	if(m_pUser)
		m_pUser->SetScript(nullptr);
	if (L) 
		lua_close(L);
	m_bDone = true;
}

bool Script::IsDone()
{
	return m_bDone;
}

bool Script::Init()
{
	if (luaL_loadfile(L, m_fileName.c_str())) 
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Error, Unable to open the specific script: %s.\n", m_fileName.c_str());
		return false;
	}
	return true;
}

void Script::OnPacket(InPacket * iPacket)
{
	m_pOnPacketInvoker(iPacket, this, L);
}

Script::~Script()
{
}
