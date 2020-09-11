#include "ConfigLoader.hpp"

void ConfigLoader::ParseConfig(const std::string & sCfgFileName)
{
	m_mSettings.clear();
	std::fstream cfgFile(sCfgFileName, std::ios::in);
	if (!cfgFile)
	{
		printf("Unable to open config file \"%s\"\n", sCfgFileName.c_str());
		return;
	}
	std::string strLine;
	int nDelimiterPos = 0, nLeftPOS = 0, nRightPOS = 0;
	bool bComment = false; //skip comment
	while (std::getline(cfgFile, strLine))
	{
		bComment = false;
		for (int i = 0; i < strLine.size(); ++i) 
		{
			if (strLine[i] == ' ')
				continue;
			if (strLine[i] == '#')
				bComment = true;
			break;
		}
		if (bComment)
			continue;
		nDelimiterPos = (int)strLine.find(ms_cCfgDelimiter, 0);
		if (nDelimiterPos < 0 || nDelimiterPos > strLine.size())
		{
			//printf("Error while parsing config file, please check that each line has the format : <KEY>,<VALUE>");
			continue;
		}
		nLeftPOS = nRightPOS = nDelimiterPos;
		++nRightPOS;
		while (nLeftPOS - 1 >= 0 && strLine[nRightPOS - 1] == ' ')--nLeftPOS;
		while (nRightPOS + 1 < strLine.size() && strLine[nRightPOS + 1] == ' ')++nRightPOS;
		auto sKey = strLine.substr(0, nLeftPOS);
		m_mSettings[sKey] = strLine.substr(nRightPOS, strLine.size() - nRightPOS + 1);
	}
}

ConfigLoader * ConfigLoader::Get(const std::string& sCfgFileName)
{
	static std::map<std::string, ConfigLoader*> s_mCfgLoader;

	auto findIter = s_mCfgLoader.find(sCfgFileName);
	if (findIter == s_mCfgLoader.end())
	{
		ConfigLoader *pCfg = new ConfigLoader();
		pCfg->LoadConfig(sCfgFileName);
		s_mCfgLoader[sCfgFileName] = pCfg;
		return pCfg;
	}
	return findIter->second;
}

void ConfigLoader::LoadConfig(const std::string& sCfgFileName)
{
	ParseConfig(sCfgFileName);
}

std::string ConfigLoader::StrValue(const std::string& sKey)
{
	auto findResult = m_mSettings.find(sKey);
	if (findResult == m_mSettings.end())
		return "";
	return findResult->second;
}

int ConfigLoader::IntValue(const std::string& sKey)
{
	auto findResult = m_mSettings.find(sKey);
	if (findResult == m_mSettings.end())
		return 0;
	return atoi(findResult->second.c_str());
}

double ConfigLoader::DoubleValue(const std::string& sKey)
{
	auto findResult = m_mSettings.find(sKey);
	if (findResult == m_mSettings.end())
		return 0;
	return atof(findResult->second.c_str());
}