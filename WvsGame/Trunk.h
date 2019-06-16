#pragma once
#include <vector>
#include "..\WvsLib\Memory\ZMemory.h"

struct GW_ItemSlotBase;
class OutPacket;
class InPacket;
class User;

class Trunk
{
	friend class User;
	int m_nSlotCount = 4, m_nMoney = 0, m_nTrunkTemplateID = 0, m_nTrunkCost = 0;
	std::vector<ZSharedPtr<GW_ItemSlotBase>> m_aaItemSlot[6];
	ZSharedPtr<GW_ItemSlotBase> m_pTradingItem;

public:
	enum TrunkRequest
	{
		rq_Trunk_MoveTrunkToSlot = 0x04,
		rq_Trunk_MoveSlotToTrunk = 0x05,
		rq_Trunk_WithdrawMoney = 0x07,
		rq_Trunk_Close = 0x08,
		rq_Trunk_Load = 0x15
	};

	enum TrunkResult
	{
		res_Trunk_Load = 0x15,
		res_Trunk_MoveTrunkToSlot = 0x09,
		res_Trunk_MoveSlotToTrunk = 0x0C,
		res_Trunk_WithdrawMoney = 0x14,
	};

	Trunk();
	~Trunk();

	void Encode(long long int liFlag, OutPacket *oPacket);
	void Decode(InPacket *iPacket);
	static Trunk* Load(int nAccountID);

	//GAME
	void OnPacket(User *pUser, InPacket *iPacket);
	void OnMoveSlotToTrunkRequest(User *pUser, InPacket *iPacket);
	void OnMoveTrunkToSlotRequest(User *pUser, InPacket *iPacket);
	void OnWithdrawMoney(User *pUser, InPacket *iPacket);

	void OnLoadDone(User *pUser, InPacket *iPacket);
	void OnMoveSlotToTrunkDone(User *pUser, InPacket *iPacket);
	void OnMoveTrunkToSlotDone(User *pUser, InPacket *iPacket);
	void OnWithdrawMoneyDone(User *pUser, InPacket *iPacket);

	//CENTER
	void MoveSlotToTrunk(int nAccountID, InPacket *iPacket);
	void MoveTrunkToSlot(int nAccountID, InPacket *iPacket);
	void WithdrawMoney(int nAccountID, InPacket *iPacket);
};

