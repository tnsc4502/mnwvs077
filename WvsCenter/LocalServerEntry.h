#pragma once
#include "LocalServer.h"
class LocalServerEntry
{
private:
	std::shared_ptr<SocketBase> m_pLocalSocket;
	int m_nMaxUserCount, m_nTotalUserCount, m_nExternalIP;
	short m_nExternalPort;

public:
	LocalServerEntry();
	~LocalServerEntry();

	void SetExternalIP(int dwIPv4);
	void SetExternalPort(short port);

	int GetExternalIP() const;
	short GetExternalPort() const;

	void SetLocalSocket(std::shared_ptr<SocketBase> &pSocket);
	std::shared_ptr<SocketBase> &GetLocalSocket();
};

