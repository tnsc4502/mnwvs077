#include "ClientSocket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\WvsCenter\CenterPacketTypes.hpp"
#include "..\WvsLogin\LoginPacketTypes.hpp"
#include "..\WvsGame\UserPacketTypes.hpp"
#include "..\WvsGame\MobPacketTypes.hpp"
#include "..\WvsGame\NpcPacketTypes.hpp"
#include "WvsGame.h"
#include "User.h"
#include "Script.h"

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

int ProcessUserPacket(User *pUser, InPacket *iPacket)
{
	int nType = ((short*)iPacket->GetPacket())[0];
	bool bExcpetionOccurred = false;
	__try
	{
		pUser->OnPacket(iPacket);
	}
	__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
	{
		bExcpetionOccurred = true;
		switch (nType)
		{
			case UserRecvPacketType::User_OnSelectNpc:
			case UserRecvPacketType::User_OnScriptMessageAnswer:
				if(pUser->GetScript())
					pUser->GetScript()->Abort( );
				//pUser->SetScript(nullptr);
				break;
			case UserRecvPacketType::User_OnMiniRoomRequest:
				pUser->SetMiniRoom(nullptr);
				break;
			case UserRecvPacketType::User_OnStoreBankRequest:
				pUser->SetStoreBank(nullptr);
				break;
			case UserRecvPacketType::User_OnTrunkRequest:
				pUser->SetTrunk(nullptr);
				break;
			case UserRecvPacketType::User_OnShopRequest:
				pUser->SetTradingNpc(0);
				break;
		}
	}
	return bExcpetionOccurred ? nType : -1;
}

void ClientSocket::OnPacket(InPacket *iPacket)
{
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
			if (nType != UserRecvPacketType::User_OnFuncKeyMappedModified
				&& nType != NPCRecvPacketTypes::NPC_OnMoveRequest
				&& nType != UserRecvPacketType::User_OnUserMoveRequest
				&& nType != MobRecvPacketType::Mob_OnMove) {
				WvsLogger::LogRaw("[WvsGame][ClientSocket::OnPacket]Received Packet: ");
				iPacket->Print();
			}
			nType = ProcessUserPacket(m_pUser, iPacket);
			if(nType != -1)
				WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Unhandled System-Level Excpetion Has Been Caught: UserID = %d, Packet Type = %d\n", m_pUser->GetUserID(), nType);
		}
	}
}

void ClientSocket::OnMigrateIn(InPacket *iPacket)
{
	auto pCenter = WvsBase::GetInstance<WvsGame>()->GetCenter();
	m_nCharacterID = iPacket->Decode4();
	WvsBase::GetInstance<WvsGame>()->OnUserMigrating(m_nCharacterID, GetSocketID());
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::RequestMigrateIn);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(m_nCharacterID);
	oPacket.Encode4(WvsBase::GetInstance<WvsGame>()->GetChannelID());
	pCenter->SendPacket(&oPacket);
}

void ClientSocket::OnSocketDisconnected()
{
	auto pCenter = WvsBase::GetInstance<WvsGame>()->GetCenter();
	WvsBase::GetInstance<WvsGame>()->RemoveMigratingUser(m_nCharacterID);
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::GameClientDisconnected);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(m_nCharacterID);
	pCenter->SendPacket(&oPacket);
}

void ClientSocket::SetUser(User *pUser)
{
	m_pUser = pUser;
}

User *ClientSocket::GetUser()
{
	return m_pUser;
}