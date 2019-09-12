#include "LocalServerEntry.h"

LocalServerEntry::LocalServerEntry()
	: pLocalSocket(nullptr)
{
}

LocalServerEntry::~LocalServerEntry()
{
}

void LocalServerEntry::SetExternalIP(int dwIPv4)
{
	nExternalIP = dwIPv4;
}

void LocalServerEntry::SetExternalPort(short nPort)
{
	nExternalPort = nPort;
}

int LocalServerEntry::GetExternalIP() const
{
	return nExternalIP;
}

short LocalServerEntry::GetExternalPort() const
{
	return nExternalPort;
}
