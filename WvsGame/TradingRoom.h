#pragma once
#include "MiniRoomBase.h"

class TradingRoom : public MiniRoomBase
{
	static const int MAX_TRADE_USER_NUM = 2;
	static const int MAX_TRADE_ITEM_COUNT = 9;

	struct Item
	{

	};

	bool m_abLock[MAX_TRADE_USER_NUM];
	Item m_aItem[MAX_TRADE_USER_NUM][MAX_TRADE_ITEM_COUNT];

public:
	TradingRoom();
	~TradingRoom();

	void OnPacket(User *pUser, InPacket *iPacket);
};

