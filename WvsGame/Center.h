#pragma once
#include "..\WvsLib\Net\SocketBase.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"

class Center :
	public SocketBase
{
private:
	int m_nCenterIndex, m_nWorldID;
	void OnConnected();

public:
	Center(asio::io_service& serverService);
	~Center();

	void OnClosed();
	void OnConnectFailed();
	void SetCenterIndex(int idx);
	void SetWorldID(int nWorldID);
	int GetWorldID() const;

	void OnPacket(InPacket *iPacket);
	void OnCenterMigrateInResult(InPacket *iPacket);
	void OnTransferChannelResult(InPacket *iPacket);
	void OnRemoteBroadcasting(InPacket *iPacket);
	void OnTrunkResult(InPacket *iPacket);
	void OnEntrustedShopResult(InPacket *iPacket);

	static void OnNotifyCenterDisconnected(SocketBase *pSocket);
};

