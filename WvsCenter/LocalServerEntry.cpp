#include "LocalServerEntry.h"

LocalServerEntry::LocalServerEntry()
	: m_pLocalSocket(nullptr)
{
}

LocalServerEntry::~LocalServerEntry()
{
}

void LocalServerEntry::SetExternalIP(int dwIPv4)
{
	m_nExternalIP = dwIPv4;
}

void LocalServerEntry::SetExternalPort(short nPort)
{
	m_nExternalPort = nPort;
}

int LocalServerEntry::GetExternalIP() const
{
	return m_nExternalIP;
}

short LocalServerEntry::GetExternalPort() const
{
	return m_nExternalPort;
}

void LocalServerEntry::SetLocalSocket(std::shared_ptr<SocketBase>& pSocket)
{
	m_pLocalSocket = pSocket;
}

std::shared_ptr<SocketBase>& LocalServerEntry::GetLocalSocket()
{
	return m_pLocalSocket;
}
