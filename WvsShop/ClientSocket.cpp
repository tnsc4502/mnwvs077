#include "ClientSocket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\WvsCenter\CenterPacketTypes.hpp"
#include "..\WvsLogin\LoginPacketTypes.hpp"

#include "WvsShop.h"
#include "User.h"

#include "..\WvsLib\Logger\WvsLogger.h"

ClientSocket::ClientSocket(asio::io_service& serverService)
	: SocketBase(serverService)
{
}

ClientSocket::~ClientSocket()
{
}

void ClientSocket::OnClosed()
{
}

void ClientSocket::OnPacket(InPacket *iPacket)
{
	iPacket->Print();
	//printf("\n");
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
		case LoginRecvPacketType::Client_ClientMigrateIn:
			OnMigrateIn(iPacket);
			break;
		default:
			if (m_pUser)
			{
				iPacket->RestorePacket();
				m_pUser->OnPacket(iPacket);
			}
	}
}

void ClientSocket::OnMigrateIn(InPacket *iPacket)
{
	m_nCharacterID = iPacket->Decode4();
	auto pCenter = WvsBase::GetInstance<WvsShop>()->GetCenter();
	WvsBase::GetInstance<WvsShop>()->OnUserMigrating(m_nCharacterID, GetSocketID());
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::RequestMigrateIn);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(m_nCharacterID);
	oPacket.Encode4(-1); //Shop
	pCenter->SendPacket(&oPacket);
}

void ClientSocket::OnSocketDisconnected()
{
	auto pCenter = WvsBase::GetInstance<WvsShop>()->GetCenter();
	WvsBase::GetInstance<WvsShop>()->RemoveMigratingUser(m_nCharacterID);
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::GameClientDisconnected);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(m_nCharacterID);
	pCenter->SendPacket(&oPacket);
}

void ClientSocket::SetUser(User* pUser)
{
	m_pUser = pUser;
}

User *ClientSocket::GetUser()
{
	return m_pUser;
}