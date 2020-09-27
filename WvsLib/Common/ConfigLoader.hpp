#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "..\String\StringUtility.h"

class ConfigLoader
{
private:
	static const char ms_cCfgDelimiter = '=';
	std::map<std::string, std::string> m_mSettings;

	ConfigLoader() {};
	void ParseConfig(const std::string& sCfgFileName);
	//static ConfigLoader* GetInstance();

public:

	static ConfigLoader* Get(const std::string& sCfgFileName);
	void LoadConfig(const std::string& sCfgFileName);
	std::string StrValue(const std::string& sKey, const std::string& sDef = "");
	int IntValue(const std::string& sKey, const int nDef = 0);
	double DoubleValue(const std::string& sKe, const double dDef = 0.0);

	template<typename T>
	inline std::vector<T> GetArray(const std::string& sKey)
	{
		std::vector<T> aRet;
		auto findResult = m_mSettings.find(sKey);
		if (findResult != m_mSettings.end())
			StringUtility::Split(findResult->second, aRet, ",");
		return aRet;
	}
};
