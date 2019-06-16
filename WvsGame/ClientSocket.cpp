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
			case UserRecvPacketFlag::User_OnSelectNpc:
			case UserRecvPacketFlag::User_OnScriptMessageAnswer:
				if(pUser->GetScript())
					pUser->GetScript()->Abort( );
				//pUser->SetScript(nullptr);
				break;
			case UserRecvPacketFlag::User_OnMiniRoomRequest:
				pUser->SetMiniRoom(nullptr);
				break;
			case UserRecvPacketFlag::User_OnStoreBankRequest:
				pUser->SetStoreBank(nullptr);
				break;
			case UserRecvPacketFlag::User_OnTrunkRequest:
				pUser->SetTrunk(nullptr);
				break;
			case UserRecvPacketFlag::User_OnShopRequest:
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
				WvsLogger::LogRaw("[WvsGame][ClientSocket::OnPacket]«Ê¥]±µ¦¬¡G");
				iPacket->Print();
			}
			nType = ProcessUserPacket(pUser, iPacket);
			if(nType != -1)
				WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Unhandled System-Level Excpetion Has Been Caught: UserID = %d, Packet Type = %d\n", pUser->GetUserID(), nType);
		}
	}
}

void ClientSocket::OnMigrateIn(InPacket *iPacket)
{
	auto pCenter = WvsBase::GetInstance<WvsGame>()->GetCenter();
	m_nCharacterID = iPacket->Decode4();
	WvsBase::GetInstance<WvsGame>()->OnUserMigrating(m_nCharacterID, GetSocketID());
	OutPacket oPacket;
	oPacket.Encode2(GameSrvSendPacketFlag::RequestMigrateIn);
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
	oPacket.Encode2(GameSrvSendPacketFlag::GameClientDisconnected);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(m_nCharacterID);
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