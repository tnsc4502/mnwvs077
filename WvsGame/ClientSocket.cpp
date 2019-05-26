#include "ClientSocket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\WvsLib\Net\PacketFlags\GameSrvPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\LoginPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\MobPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\NPCPacketFlags.hpp"
#include "WvsGame.h"
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
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
	case LoginRecvPacketFlag::Client_ClientMigrateIn:
		OnMigrateIn(iPacket);
		break;
	default:
		if (pUser)
		{
			iPacket->RestorePacket();
			if (nType != UserRecvPacketFlag::User_OnFuncKeyMappedModified
				&& nType != NPCRecvPacketFlags::NPC_OnMoveRequest
				&& nType != MobRecvPacketFlag::Mob_OnMove) {
				//WvsLogger::LogRaw("[WvsGame][ClientSocket::OnPacket]«Ê¥]±µ¦¬¡G");
				//iPacket->Print();
			}
			pUser->OnPacket(iPacket);
		}
	}
}

void ClientSocket::OnMigrateIn(InPacket *iPacket)
{
	auto pCenter = WvsBase::GetInstance<WvsGame>()->GetCenter();
	int nCharacterID = iPacket->Decode4();
	OutPacket oPacket;
	oPacket.Encode2(GameSrvSendPacketFlag::RequestMigrateIn);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(nCharacterID);
	oPacket.Encode4(WvsBase::GetInstance<WvsGame>()->GetChannelID());
	pCenter->SendPacket(&oPacket);
}

void ClientSocket::SetUser(User *_pUser)
{
	pUser = _pUser;
}

User *ClientSocket::GetUser()
{
	return pUser;
}