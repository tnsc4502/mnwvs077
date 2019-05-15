#include "TradingRoom.h"
#include "User.h"
#include "..\WvsLib\Net\InPacket.h"

TradingRoom::TradingRoom()
	: MiniRoomBase(2)
{
	m_abLock[0] = m_abLock[1] = 0;
	memset(m_aItem, 0, sizeof(m_aItem));
}


TradingRoom::~TradingRoom()
{
}

void TradingRoom::OnPacket(User *pUser, InPacket *iPacket)
{
}
