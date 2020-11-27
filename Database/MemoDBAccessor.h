#pragma once

class SocketBase;

class MemoDBAccessor
{
public:
	static bool PostSendMemoRequest(SocketBase *pSrv, int nCharacterID, void *iPacket);
	static void PostLoadMemoRequest(SocketBase *pSrv, int nCharacterID);
	static void PostDeleteMemoRequest(SocketBase *pSrv, int nCharacterID, void *iPacket);
};

