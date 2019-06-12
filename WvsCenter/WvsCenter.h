#pragma once
#include "..\WvsLib\Net\WvsBase.h"
#include "LocalServerEntry.h"

#include <map>

class WvsCenter : public WvsBase
{
private:
	std::map<int, LocalServerEntry*> m_mChannel;
	LocalServerEntry* m_pShopEntry = nullptr, *m_pLoginEntry = nullptr;
	int nConnectedChannel = 0;

public:
	WvsCenter();
	~WvsCenter();
	void Init();
	int GetChannelCount();
	void OnNotifySocketDisconnected(SocketBase *pSocket);
	void NotifyWorldChanged();

	void RegisterChannel(int nChannelID, std::shared_ptr<SocketBase> &pServer, InPacket *iPacket);
	void RegisterCashShop(std::shared_ptr<SocketBase> &pServer, InPacket *iPacket);
	void RestoreConnectedUser(std::shared_ptr<SocketBase> &pServer, int nChannelID, InPacket *iPacket);
	void RegisterLoginServer(std::shared_ptr<SocketBase> &pServer);
	void SetShop(LocalServerEntry* pEntry);
	void SetLoginServer(LocalServerEntry* pEntry);

	LocalServerEntry* GetChannel(int nIdx);
	LocalServerEntry* GetShop();
	LocalServerEntry* GetLoginServer();
};

