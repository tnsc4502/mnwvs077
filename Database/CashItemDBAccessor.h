#pragma once
class SocketBase;

class CashItemDBAccessor
{
public:
	static void PostBuyCashItemRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket);
	static void PostLoadLockerRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket);
	static void PostUpdateCashRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket);
	static void PostMoveSlotToLockerRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket);
	static void PostMoveLockerToSlotRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket);
	static void PostExpireCashItemRequest(SocketBase *pSrv, int uClientSocketSN, int nCharacterID, void *iPacket);
};

