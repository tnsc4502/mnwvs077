#include "ScriptMan.h"
#include "ScriptNPC.h"
#include "ScriptInventory.h"
#include "ScriptFieldSet.h"
#include "ScriptUser.h"
#include "ScriptPacket.h"
#include "ScriptQuestRecord.h"
#include "ScriptField.h"
#include "ScriptPortal.h"
#include <functional>
#include <filesystem>
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

const std::string & ScriptMan::SearchScriptNameByFunc(const std::string& sType, const std::string & sFunc)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	static std::string sEmpty = "";
	auto& mTable = m_mFuncToFile[sType];
	auto findIter = mTable.find(sFunc);
	if (findIter == mTable.end())
		return sEmpty;

	return findIter->second;
}

#pragma warning(disable:4503) //Disable warning of exceeded name-length of m_mFuncToFile.
void ScriptMan::RegisterScriptFunc(const std::string& sType, const std::string & sScriptPath)
{
	auto &mFileTable = m_mFileToFunc[sType];
	auto &mFuncTable = m_mFuncToFile[sType];

	for (auto& prFunc : mFileTable[sScriptPath])
		mFuncTable.erase(prFunc);
	mFuncTable.erase(sScriptPath);

	auto pScript = CreateScript(sScriptPath, { 0, nullptr });
	if (!pScript)
		return;

	pScript->PushInteger("userID", 0);
	auto& aFunc = mFileTable[sScriptPath];
	auto L = pScript->GetLuaState();
	lua_pcall(L, 0, 0, 0);
	lua_pushglobaltable(L);
	//lua_getglobal(L, "_G");
	lua_pushnil(L);
	while (lua_next(L, -2) != 0)
	{
		if (lua_isfunction(L, lua_gettop(L)))
		{
			std::string sName = lua_tostring(L, -2);

			//Built-in scripts start with s_
			if (sName.substr(0, 2) == "s_")
			{
				aFunc.push_back(sName);
				mFuncTable[sName] = sScriptPath;
			}
		}
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	lua_close(L);

	/*
	Raw release.
	Prevent unknown memory leakage.
	*/
	pScript->~Script();
	WvsSingleObjectAllocator<char[sizeof(Script)]>::GetInstance()->Free(pScript);
	//FreeObj(pScript);
}

void ScriptMan::ScriptFileMonitor()
{
	static std::string sTable[] =
	{
		"Npc",
		"Portal",
		"Quest"
	};

	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);

	for (auto& sType : sTable)
	{
		for (auto &file : std::experimental::filesystem::recursive_directory_iterator("./DataSrv/Script/" + sType))
		{
			auto fTime = std::experimental::filesystem::last_write_time(file);
			if (fTime != m_mFileTime[file.path().string()])
			{
				m_mFileTime[file.path().string()] = fTime;
				RegisterScriptFunc(sType, file.path().string());
			}
		}
	}
}

ScriptMan *ScriptMan::GetInstance()
{
	static ScriptMan* pInstance = new ScriptMan;
	return pInstance;
}

void ScriptMan::RegisterScriptFuncReflector()
{
	static std::string sTable[] = 
	{
		"Npc",
		"Portal",
		"Quest"
	};

	for (auto sType : sTable)
	{
		for (auto &file : std::experimental::filesystem::recursive_directory_iterator("./DataSrv/Script/" + sType))
		{
			m_mFileTime[file.path().string()] = std::experimental::filesystem::last_write_time(file);
			RegisterScriptFunc(sType, file.path().string());
		}
	}
	auto pTimer = AsyncScheduler::CreateTask(std::bind(&ScriptMan::ScriptFileMonitor, this), 5000, true);
	pTimer->Start();
}

Script* ScriptMan::CreateScript(const std::string& sFile, const std::pair<int, Field*>& prParam)
{
	if (!std::experimental::filesystem::exists(sFile))
		return nullptr;

	static std::vector<void(*)(lua_State*)> aRegFunc = {
		&ScriptNPC::Register,
		&ScriptInventory::Register,
		&ScriptFieldSet::Register,
		&ScriptUser::Register,
		&ScriptQuestRecord::Register,
		&ScriptField::Register,
		&ScriptPacket::Register,
		&ScriptPortal::Register
	};

	auto pScript = AllocObjCtor(Script)(sFile, prParam.first, prParam.second, aRegFunc);
	if (pScript && pScript->Init())
	{
		pScript->m_pOnPacketInvoker = &(ScriptNPC::OnPacket);
		luaL_openlibs(pScript->L);
		return pScript;
	}
	else if (pScript)
	{
		pScript->Abort();
		FreeObj(pScript);
		pScript = nullptr;
	}

	return pScript;
}