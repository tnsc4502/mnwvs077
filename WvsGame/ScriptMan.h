#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <mutex>
#include <filesystem>

#include "Script.h"
#include "..\WvsLib\Memory\ZMemory.h"

#include "ScriptNPC.h"
#include "ScriptInventory.h"
#include "ScriptFieldSet.h"
#include "ScriptUser.h"
#include "ScriptPacket.h"
#include "ScriptQuestRecord.h"
#include "ScriptField.h"

class Field;

class ScriptMan
{
	std::recursive_mutex m_mtxLock;
	std::map<std::string, std::map<std::string, std::string>> m_mFuncToFile;
	std::map<std::string, std::map<std::string, std::vector<std::string>>> m_mFileToFunc;

	//This sh_t should be replaced after C++ 17 is officially released.
	std::map<std::string, std::experimental::filesystem::v1::file_time_type> m_mFileTime;

	void RegisterScriptFunc(const std::string& sType, const std::string& sScriptPath);
	void ScriptFileMonitor();
public:

	static ScriptMan* GetInstance();
	void RegisterScriptFuncReflector();
	const std::string& SearchScriptNameByFunc(const std::string& sType, const std::string& sFunc);

	//GetScript would cause memory leak... currently have no idea.
	//If "Init" return failed result then memory leak would occur again.
	Script* CreateScript(const std::string& sFile, const std::pair<int, Field*>& prParam);
};