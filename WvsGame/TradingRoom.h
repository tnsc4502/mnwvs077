#pragma once
#include "MiniRoomBase.h"
#include <vector>

struct ExchangeElement;

class TradingRoom : public MiniRoomBase
{
	static const int MAX_TRADE_USER_NUM = 2;
	static const int MAX_TRADE_ITEM_COUNT = 9;

	enum TradingRequest
	{
		rq_Trading_PutItem = 0x0D,
		rq_Trading_PutMoney = 0x0E,
		rq_Trading_DoTrade = 0x0F
	};

	enum TradingResult
	{
		res_Trading_Aborted = 0x02,
		res_Trading_Exchange_Success = 0x07,
		res_Trading_Exchange_Failed = 0x08,
	};

	struct Item
	{

	};

	bool m_abLock[MAX_TRADE_USER_NUM];
	Item m_aItem[MAX_TRADE_USER_NUM][MAX_TRADE_ITEM_COUNT];

public:
	TradingRoom();
	~TradingRoom();

	int GetLeaveType() const;
	void OnPacket(User *pUser, int nType, InPacket *iPacket);
	void OnPutItem(User *pUser, InPacket *iPacket);
	void OnPutMoney(User *pUser, InPacket *iPacket);
	void OnTrade(User *pUser, InPacket *iPacket);
	void MakeExchageElements(std::vector<ExchangeElement> aEx[2], std::map<int, int> amTradingItems[2][6]);
	int DoTrade();
	void OnLeave(User *pUser, int nLeaveType);
	void Encode(OutPacket *oPacket);
	void Release();
};

