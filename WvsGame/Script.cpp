#include "Script.h"
#include <memory>
#include "User.h"
#include "QWUser.h"
#include <cmath>
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\WvsLib\Script\lvm.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
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
	static luaL_Reg SysMetatable[] = {
		{ NULL, NULL }
	};

	static luaL_Reg SysTable[] = {
		{ "random", ScriptSysRandom },
		{ "getTime", ScriptSysTime },
		{ "getDateTime", ScriptSysDateTime },
		{ "currentTime", ScriptSysCurrentTime },
		{ "compareTime", ScriptSysCompareTime },
		{ "dayOfWeek", ScriptSysDayOfWeek },
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

	PushInteger("templateID", nTemplateID);
}

int Script::GetID() const
{
	return m_nTemplateID;
}

void Script::SetUser(User * pUser)
{
	m_pUser = pUser;
	if (pUser)
		PushInteger("userID", pUser->GetUserID());
}

User * Script::GetUser()
{
	return m_pUser;
}

Field * Script::GetField()
{
	return m_pField;
}

void Script::SetFieldSet(FieldSet *pFieldSet)
{
	m_pUniqueFieldSet = pFieldSet;
}

void Script::SetPortal(Portal *pPortal)
{
	m_pPortal = pPortal;
}

Portal *Script::GetPortal()
{
	return m_pPortal;
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
	decltype(liRand) liMin = luaL_checkinteger(L, 1);
	decltype(liRand) liMax = luaL_checkinteger(L, 2);
	lua_pushinteger(L, (liMin + (liRand % (liMax - liMin))));
	return 1;
}

int Script::ScriptSysTime(lua_State * L)
{
	lua_pushinteger(L, GameDateTime::GetTime());
	return 1;
}

int Script::ScriptSysDateTime(lua_State * L)
{
	lua_pushinteger(L, GameDateTime::GetCurrentDate());
	return 1;
}

int Script::ScriptSysCurrentTime(lua_State * L)
{
	char sBuffer[32] = { 0 };

	//Use standard lib instead of _SYSTEMTIME
	time_t start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm tm;
	localtime_s(&tm, &start_time);
	sprintf_s(sBuffer, "%02d/%02d/%02d/%02d/%02d\n", tm.tm_year % 100, (tm.tm_mon + 1), tm.tm_mday, tm.tm_hour, tm.tm_min);

	lua_pushstring(L, sBuffer);
	return 1;
}

int Script::ScriptSysCompareTime(lua_State * L)
{
	std::string sTime1 = luaL_checkstring(L, 1);
	std::string sTime2 = luaL_checkstring(L, 2);

	tm tm1, tm2;
	memset(&tm1, 0, sizeof(tm1));
	memset(&tm2, 0, sizeof(tm2));
	sscanf_s(sTime1.data(), "%02d/%02d/%02d/%02d/%02d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min);
	tm1.tm_year += 100;
	--tm1.tm_mon;
	sscanf_s(sTime2.data(), "%02d/%02d/%02d/%02d/%02d", &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday, &tm2.tm_hour, &tm2.tm_min);
	tm2.tm_year += 100;
	--tm2.tm_mon;

	auto tp1 = std::chrono::system_clock::from_time_t(mktime(&tm1));
	auto tp2 = std::chrono::system_clock::from_time_t(mktime(&tm2));
	lua_pushinteger(L, std::chrono::duration_cast<std::chrono::seconds>(tp1 - tp2).count());
	return 1;
}

int Script::ScriptSysDayOfWeek(lua_State * L)
{
	time_t start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm tm;
	localtime_s(&tm, &start_time);
	lua_pushinteger(L, tm.tm_wday);
	return 1;
}

void Script::Run()
{
	//m_bResume = 1代表被yield的腳本繼續執行並且把對話結果壓入堆疊中
	auto nResult = lua_resume(L, C, m_sState.m_bResume ? 1 : 0);
	m_sState.m_bResume = false;
	if (nResult == LUA_OK || nResult != LUA_YIELD)
	{
		if (nResult)
			OnError();
		Abort();
	}
}

void Script::Run(const std::string & sFunc)
{
	lua_pcall(GetLuaState(), 0, 0, 0); //Initialize all functions
	lua_getglobal(GetLuaState(), sFunc.c_str());
	Run();
}

void Script::Abort()
{
	if (L)
		lua_close(L);
	if (m_pUser) 
	{
		m_pUser->SetScript(nullptr);
		return;
	}
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
		OnError();
		return false;
	}
	return true;
}

void Script::OnError()
{
	WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Script Error: %s\n", lua_tostring(L, -1));
	lua_pop(L, 1);
}

void Script::OnPacket(InPacket * iPacket)
{
	m_pOnPacketInvoker(iPacket, this, L);
}

bool Script::SafeInvocation(const std::string & sFunc, const std::vector<ScriptArg>& aArg)
{
	lua_getglobal(L, sFunc.c_str());
	if (lua_isfunction(L, lua_gettop(L)))
	{
		for (auto& arg : aArg)
		{
			switch (arg.nType)
			{
				case ScriptArg::e_INT:
					lua_pushinteger(L, arg.data.n);
					break;
				case ScriptArg::e_DOUBLE:
					lua_pushnumber(L, arg.data.d);
					break;
				case ScriptArg::e_STR:
					lua_pushstring(L, arg.data.c);
					break;
				case ScriptArg::e_BOOL:
					lua_pushboolean(L, arg.data.b ? 1 : 0);
					break;
			}
		}
		auto nResult = lua_resume(L, C, (int)aArg.size());
		if (nResult)
		{
			OnError();
			return false;
		}
		return true;
	}
	return false;
}

Script::~Script()
{
}
