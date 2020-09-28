#pragma once
#include <string>
#include "..\WvsLib\Common\ServerConstants.hpp"

struct WorldConnectionInfo
{
	std::string strServerIP;
	short nServerPort;
};

struct WorldInfo
{
	int m_aChannelStatus[ServerConstants::kMaxChannelCount];
	int nWorldID, nEventType, nGameCount;
	std::string strWorldDesc, strEventDesc;
};