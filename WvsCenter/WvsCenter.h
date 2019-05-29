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

	void RegisterChannel(int nChannelID, std::shared_ptr<SocketBase> &pServer, InPacket *iPacket);
	void RegisterCashShop(std::shared_ptr<SocketBase> &pServer, InPacket *iPacket);
	void RestoreConnectedUser(int nChannelID, InPacket *iPacket);
	void RegisterLoginServer(std::shared_ptr<SocketBase> &pServer);

	LocalServerEntry* GetChannel(int nIdx);
	int GetChannelCount();
	void Init();
	void OnNotifySocketDisconnected(SocketBase *pSocket);
	void NotifyWorldChanged();

	LocalServerEntry* GetShop();
	void SetShop(LocalServerEntry* pEntry);

	LocalServerEntry* GetLoginServer();
	void SetLoginServer(LocalServerEntry* pEntry);
};

