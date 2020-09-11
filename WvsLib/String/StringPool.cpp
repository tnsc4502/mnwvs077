#include "StringPool.h"
#include "..\Common\ConfigLoader.hpp"
#include "..\Exception\WvsException.h"
#include "..\Logger\WvsLogger.h"
#include "StringUtility.h"
#include <fstream>

StringPool::StringLocale StringPool::ms_strLocale = StringPool::StringLocale::CHT_TW;
//StringPool::C_STR StringPool::ms_asStringPool[(int)StringLocale::Tag_StringLocale_End][(int)StringTag::Tag_String_End];
std::unordered_map<std::string, std::string> StringPool::ms_mStringMap;

/*void StringPool::InitializeCommonStrPool()
{
	Construct<StringTag::Common_No_CommandLine_Provided>(
		"請以命令列執行程式，並且指定設定檔的路徑。", 
		"Please run this program with command line, and provide the path of config file.\n");
}

void StringPool::InitializeLoginSrvStrPool()
{
	Construct<StringTag::Login_Invalid_Socket_Status>("錯誤的Socket狀態。", "Invalid Socket Status.");
}

void StringPool::InitializeGameSrvStrPool()
{
	Construct<StringTag::GameSrv_ItemInfo_Init_ItemName>(
		"[ItemInfo::Initialize<IterateItemString>]開始載入所有物品名稱....\n", 
		"[ItemInfo::Initialize<IterateItemString>]On iterating all item names....\n");

	Construct<StringTag::GameSrv_ItemInfo_Init_ItemName_Done>(
		"[ItemInfo::Initialize<IterateItemString>]物品名稱載入完成....\n",
		"[ItemInfo::Initialize<IterateItemString>]Item names are completely loaded....\n");

	Construct<StringTag::GameSrv_ItemInfo_Init_Equip>(
		"[ItemInfo::Initialize<IterateEquipItem>]開始載入所有裝備....\n",
		"[ItemInfo::Initialize<IterateEquipItem>]On iterating all equip items....\n");

	Construct<StringTag::GameSrv_ItemInfo_Init_Equip_Done>(
		"[ItemInfo::Initialize<IterateEquipItem>]裝備載入完成....\n",
		"[ItemInfo::Initialize<IterateEquipItem>]Equip items are completely loaded....\n");

	Construct<StringTag::GameSrv_ItemInfo_Init_Bundle>(
		"[ItemInfo::Initialize<IterateBundleItem>]開始載入所有物品....\n",
		"[ItemInfo::Initialize<IterateBundleItem>]On iterating all bundle items....\n");

	Construct<StringTag::GameSrv_ItemInfo_Init_Bundle_Done>(
		"[ItemInfo::Initialize<IterateBundleItem>]物品載入完成....\n",
		"[ItemInfo::Initialize<IterateBundleItem>]Bundle items are completely loaded....\n");
}

void StringPool::InitializeCenterSrvStrPool()
{
}

void StringPool::InitializeDBStrPool()
{
}

void StringPool::InitializeLibStrPool()
{
	Construct<StringTag::Lib_WvsBase_Acceptor_Created>(
		"[WvsBase::CreateAcceptor]成功於Port %d建立Wvs伺服器程序。\n",
		"[WvsBase::CreateAcceptor]WvsApp server instance is successfully initialized and listening on port %d.\n"
		);
	Construct<StringTag::Lib_WvsBase_Socket_Disconnected>(
		"[WvsBase::OnSocketDisconnected]移除遠端連線Socket實體，Socket ID = %u\n",
		"[WvsBase::OnSocketDisconnected]Socket has been disconnected from server [Socket ID = %u].\n"
		);
}*/

void StringPool::Init()
{
	//Initialize locale setting.
	auto pCfg = ConfigLoader::Get("GlobalSetting.txt");
	std::string strLocale = pCfg->StrValue("Locale");
	if (strLocale == "EN")
		ms_strLocale = StringLocale::ENG;
	else if(strLocale == "CHT_TW")
		ms_strLocale = StringLocale::CHT_TW;
	else
		ms_strLocale = StringLocale::ENG;

	//Initialize string pools
	/*InitializeCommonStrPool();
	InitializeLoginSrvStrPool();
	InitializeGameSrvStrPool();
	InitializeCenterSrvStrPool();
	InitializeDBStrPool();
	InitializeLibStrPool();*/

	std::string strFileName = "./DataSrv/Locale/Locale_" + strLocale;
	std::fstream localeFile(strFileName, std::ios::in);
	std::string strLine;
	std::vector<std::string> aStrSplit;
	int nSubStrStart, nLine = 0;
	while (std::getline(localeFile, strLine))
	{
		++nLine;
		if (strLine == "")
			continue;
		try 
		{
			nSubStrStart = 0;
			aStrSplit.clear();
			StringUtility::Split(strLine, aStrSplit, "=");

			//Skip comments (start with "\\" or without value assignment)
			if (aStrSplit.size() != 2 || aStrSplit[0].find("\\\\") != std::string::npos)
				continue;

			//Fetch real "key" (skip blanks)
			auto& keyStr = aStrSplit[0];
			while (keyStr.back() == ' ') keyStr.pop_back();
			for (auto c : keyStr) 
			{
				if (c == ' ') ++nSubStrStart;
				else break;
			}
			auto& key = ms_mStringMap[keyStr.substr(nSubStrStart, (int)keyStr.size() - nSubStrStart)];

			//Fetch real "value" (skip non " chars)
			nSubStrStart = 0;
			auto& valueStr = aStrSplit[1];
			while (valueStr.back() != '\"')	valueStr.pop_back();
			valueStr.pop_back(); //Skip "
			for (auto c : valueStr)
			{
				if (c != '\"') ++nSubStrStart;
				else break;
			}
			++nSubStrStart; //Skip "
			StringUtility::Replace(valueStr, "\\n", "\n");
			key = valueStr.substr(nSubStrStart, (int)valueStr.size() - nSubStrStart);
		}
		catch (std::exception& ex)
		{
			WvsException::FatalError("Fatal error occurred during StringPool initialization. Error Message :%s at line: %d\n", ex.what(), nLine);
		}
	}
}

StringPool::C_STR StringPool::GetString(const char* pKey, const char *pFile, int nLine)
{
	auto findIter = ms_mStringMap.find(pKey);
	if (findIter == ms_mStringMap.end()) 
	{
		if(pFile && nLine >= 0)
			WvsLogger::LogFormat(WvsLogger::LEVEL_WARNING, "Undefined string [%s] referenced in file: [%s] at line [%d].\n", pKey, pFile, nLine);
		else
			WvsLogger::LogFormat(WvsLogger::LEVEL_WARNING, "Undefined string [%s] referenced.\n", pKey);
		return "";
	}
	return findIter->second.c_str();
}

/*StringPool::C_STR StringPool::GetString(StringTag STR_TAG)
{
	return ms_asStringPool[(int)ms_strLocale][(int)STR_TAG];
}
*/