#pragma once
#include <string>

namespace ServerConstants
{
	enum ServerType
	{
		SRV_LOGIN,
		SRV_CENTER,
		SRV_GAME,
		SRV_SHOP,
		SRV_CLAIM
	};

	enum class GameLocale { TW = 6 };

	constexpr int kMaxChannelCount = 30;
	constexpr int kGameVersion = 77;
	constexpr int kGameLocale = (int)GameLocale::TW;

	const std::string strGameSubVersion = "";
	const int kMaxNumberOfCenters = 30;
}