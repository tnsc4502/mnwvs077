#pragma once
#include "..\WvsLib\Net\SocketBase.h"

class User;

class ClientSocket :
	public SocketBase
{
	void OnClosed();
	int m_nCharacterID = 0;
	User *m_pUser = nullptr;
public:
	ClientSocket(asio::io_service& serverService);
	~ClientSocket();

	void SetUser(User *pUser);
	User* GetUser();

	void OnPacket(InPacket* iPacket);
	void OnMigrateIn(InPacket* iPacket);
	void OnSocketDisconnected();
};

