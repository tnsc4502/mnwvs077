#include "Center.h"
#include <functional>
#include <thread>

#include "..\Database\GW_ItemSlotBase.h"

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLogin\LoginPacketTypes.hpp"
#include "..\WvsCenter\CenterPacketTypes.hpp"
#include "..\WvsGame\UserPacketTypes.hpp"
#include "..\WvsGame\FieldPacketTypes.hpp"

#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Common\ServerConstants.hpp"
#include "..\WvsLib\String\StringPool.h"

#include "WvsGame.h"
#include "User.h"
#include "QWUser.h"
#include "FieldMan.h"
#include "PartyMan.h"
#include "GuildMan.h"
#include "FriendMan.h"
#include "StoreBank.h"

Center::Center(asio::io_service& serverService)
	: SocketBase(serverService, true)
{
}

Center::~Center()
{
}

void Center::SetCenterIndex(int nIdx)
{
	m_nCenterIndex = nIdx;
}

void Center::SetWorldID(int nWorldID)
{
	m_nWorldID = nWorldID;
}

int Center::GetWorldID() const
{
	return m_nWorldID;
}

void Center::OnNotifyCenterDisconnected(SocketBase *pSocket)
{
	WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsGame]Disconnected from WvsCenter (closed by remote server).\n");
}

void Center::OnConnected()
{
	WvsLogger::LogRaw("[WvsGame][Center::OnConnect]Successfully connected to center server.\n");

	//向Center Server發送Hand Shake封包
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::RegisterCenterRequest);

	//WvsGame的ServerType為SRV_GAME
	oPacket.Encode1(ServerConstants::ServerType::SRV_GAME);

	//[+07-07] Game的頻道ID
	oPacket.Encode1((int)WvsBase::GetInstance<WvsGame>()->GetChannelID());

	//Encode IP
	auto ip = WvsBase::GetInstance<WvsGame>()->GetExternalIP();
	for (int i = 0; i < 4; ++i)
		oPacket.Encode1((unsigned char)ip[i]);

	//Encode Port
	oPacket.Encode2(WvsBase::GetInstance<WvsGame>()->GetExternalPort());

	//Encode Existing Users.
	std::lock_guard<std::recursive_mutex> lock(WvsBase::GetInstance<WvsGame>()->GetUserLock());
	auto& mConnectedUser = WvsBase::GetInstance<WvsGame>()->GetConnectedUser();
	oPacket.Encode4((int)mConnectedUser.size());
	for (auto& prUser : mConnectedUser) 
	{
		oPacket.Encode4(prUser.first);
		oPacket.Encode4(prUser.second->GetAccountID());
	}

	SendPacket(&oPacket);
	OnWaitingPacket();
}

void Center::OnPacket(InPacket *iPacket)
{
	WvsLogger::LogRaw("[Center::OnPacket]");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
		case CenterResultPacketType::RegisterCenterAck:
		{
			auto result = iPacket->Decode1();
			if (!result)
			{
				WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, GET_STRING(GameSrv_System_Center_Connection_Rejected));
				exit(0);
			}
			int nWorldID = iPacket->Decode1();
			WvsBase::GetInstance<WvsGame>()->GetCenter()->SetWorldID(nWorldID);
			GW_ItemSlotBase::ms_nWorldID = nWorldID;
			GW_ItemSlotBase::ms_nChannelID = 
				WvsBase::GetInstance<WvsGame>()->GetChannelID() + 1; //Channel 0 is reserved for Center

			char aBuffer[128];
			sprintf_s(aBuffer, "Game Server[WvsGame][TWMS][% 03d][WorldID:% 02d][ChannelID:% 02d]",
				ServerConstants::kGameVersion,
				nWorldID,
				WvsBase::GetInstance<WvsGame>()->GetChannelID());
			SetWindowTextA(GetActiveWindow(), aBuffer);

			for (int i = 1; i <= 5; ++i)
				GW_ItemSlotBase::SetInitSN(i, iPacket->Decode8());
			WvsLogger::LogRaw("[Center][RegisterCenterAck]The connection between local server(WvsCenter) has been authenciated by remote server.\n");
			break;
		}
		case CenterResultPacketType::CenterMigrateInResult:
			OnCenterMigrateInResult(iPacket);
			break;
		case CenterResultPacketType::MigrateCashShopResult:
		case CenterResultPacketType::TransferChannelResult:
			OnTransferChannelResult(iPacket);
			break;
		case CenterResultPacketType::PartyResult:
			PartyMan::GetInstance()->OnPacket(iPacket);
			break;
		case CenterResultPacketType::GuildResult:
			GuildMan::GetInstance()->OnPacket(iPacket);
			break;
		case CenterResultPacketType::FriendResult:
			FriendMan::GetInstance()->OnPacket(iPacket);
			break;
		case CenterResultPacketType::RemoteBroadcasting:
			OnRemoteBroadcasting(iPacket);
			break;
		case CenterResultPacketType::TrunkResult:
			OnTrunkResult(iPacket);
			break;
		case CenterResultPacketType::EntrustedShopResult:
			OnEntrustedShopResult(iPacket);
			break;
		case CenterRequestPacketType::CheckMigrationState:
			OnCheckMigrationState(iPacket);
			break;
		case CenterResultPacketType::GuildBBSResult:
			OnGuildBBSResult(iPacket);
			break;
		case CenterResultPacketType::CheckGivePopularityResult:
			OnCheckGivePopularityResult(iPacket);
			break;
		case CenterResultPacketType::CashItemResult:
			OnCenterCashItemResult(iPacket);
			break;
	}
}

void Center::OnClosed()
{
}

void Center::OnConnectFailed()
{
	WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[Center][RegisterCenterAck]Center has rejected the connection request, WvsGame server may not work properly.\n");
	OnDisconnect();
}

void Center::OnCenterMigrateInResult(InPacket *iPacket)
{
	unsigned int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	bool bValid = iPacket->Decode1() ? 1 : 0;
	auto pSocket = WvsBase::GetInstance<WvsGame>()->GetSocket(nClientSocketID);
	if (!bValid || !pSocket)
	{
		if(pSocket)
			pSocket->GetSocket().close();
		WvsBase::GetInstance<WvsGame>()->RemoveMigratingUser(nCharacterID);
		return;
	}

	ZSharedPtr<User> pUser = MakeShared<User>((ClientSocket*)pSocket, iPacket);
	WvsBase::GetInstance<WvsGame>()->OnUserConnected(pUser);
}

void Center::OnTransferChannelResult(InPacket * iPacket)
{
	unsigned int nClientSocketID = iPacket->Decode4();
	auto pSocket = WvsBase::GetInstance<WvsGame>()->GetSocket(nClientSocketID);
	OutPacket oPacket;
	bool bSuccess = iPacket->Decode1() == 1 ? true : false;
	if (bSuccess)
	{
		oPacket.Encode2(UserSendPacketType::UserLocal_OnTransferChannel);
		
		// 7 = Header(2) + nClientSocketID(4) + bSuccess(1)
		oPacket.EncodeBuffer(iPacket->GetPacket() + 7, iPacket->GetPacketSize() - 7);
	}
	else
	{
		oPacket.Encode2(FieldSendPacketType::Field_OnTransferChannelReqIgnored);
		oPacket.Encode1(1);
	}
	pSocket->SendPacket(&oPacket);
}

void Center::OnRemoteBroadcasting(InPacket *iPacket)
{
	int nUserID = iPacket->Decode4();

	OutPacket oPacket;
	oPacket.EncodeBuffer(
		iPacket->GetPacket() + iPacket->GetReadCount(),
		iPacket->GetPacketSize() - iPacket->GetReadCount()
	);
	if (nUserID == -1)
		User::Broadcast(&oPacket);
	else
	{
		auto pUser = User::FindUser(nUserID);
		if (pUser)
			pUser->SendPacket(&oPacket);
	}
}

void Center::OnTrunkResult(InPacket *iPacket)
{
	auto pUser = User::FindUser(iPacket->Decode4());
	if (pUser)
		pUser->OnTrunkResult(iPacket);
}

void Center::OnEntrustedShopResult(InPacket *iPacket)
{
	auto pUser = User::FindUser(iPacket->Decode4());
	if (pUser)
		pUser->GetStoreBank()->OnPacket(iPacket);
}

void Center::OnCheckMigrationState(InPacket *iPacket)
{
	int nCharacterID = iPacket->Decode4();
	OutPacket oPacket;
	oPacket.Encode2(CenterResultPacketType::CheckMigrationStateResult);
	oPacket.Encode4(nCharacterID);
	oPacket.Encode4(WvsBase::GetInstance<WvsGame>()->GetChannelID());
	auto pUser = User::FindUser(nCharacterID);
	if (pUser)
		oPacket.Encode1(1); //already migrated in.
	else
	{
		auto& prMigrating = WvsBase::GetInstance<WvsGame>()->GetMigratingUser(nCharacterID);
		if (prMigrating.first == 0)
			oPacket.Encode1(0); //Not yet migrated in.
		else
		{
			int tMigrateTime = prMigrating.second;

			//Check migrating time out.
			if (GameDateTime::GetTime() - tMigrateTime > 60 * 1000)
			{
				auto pSocket = WvsBase::GetInstance<WvsGame>()->GetSocket(prMigrating.first);
				if (pSocket)
					pSocket->GetSocket().close();
				WvsBase::GetInstance<WvsGame>()->RemoveMigratingUser(nCharacterID);
				oPacket.Encode1(0);
			}
			else
				oPacket.Encode1(1); //Migration request had been sent, but haven't received the character data.
		}
	}
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

void Center::OnGuildBBSResult(InPacket *iPacket)
{
	int nCharacterID = iPacket->Decode4();
	auto pUser = User::FindUser(nCharacterID);
	if (pUser)
	{
		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketType::UserLocal_OnGuildBBSResult);
		oPacket.EncodeBuffer(
			iPacket->GetPacket() + iPacket->GetReadCount(),
			iPacket->GetPacketSize() - iPacket->GetReadCount()
		);
		pUser->SendPacket(&oPacket);
	}
}

void Center::OnCheckGivePopularityResult(InPacket * iPacket)
{
	int nUserID = iPacket->Decode4(), nFailedReason = 0, nInc = 0;
	ZSharedPtr<User> pUser = User::FindUser(nUserID), pTarget = nullptr;
	if (pUser)
	{
		nFailedReason = iPacket->Decode1();
		if (!nFailedReason)
		{
			pTarget = User::FindUser(iPacket->Decode4());
			nInc = iPacket->Decode1();
			if (pTarget)
				pTarget->SendCharacterStat(
					false,
					QWUser::IncPOP(pTarget, nInc != 0 ? 1 : -1, true)
				);
			else
				nFailedReason = User::GivePopluarityMessage::eUserDoesNotExist;
		}

		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketType::UserLocal_OnGivePopularityResult);
		oPacket.Encode1(nFailedReason);
		if (!nFailedReason)
		{
			oPacket.EncodeStr(pTarget->GetName());
			oPacket.Encode1((char)nInc);
			oPacket.Encode4((int)QWUser::GetPOP(pTarget));

			OutPacket oRemote;
			oRemote.Encode2(UserSendPacketType::UserLocal_OnGivePopularityResult);
			oRemote.Encode1(User::GivePopluarityMessage::eUserPOPIsIncreased);
			oRemote.EncodeStr(pUser->GetName());
			oRemote.Encode1((char)nInc);
			pTarget->SendPacket(&oRemote);
		}
		pUser->SendPacket(&oPacket);
	}
}

void Center::OnCenterCashItemResult(InPacket * iPacket)
{
	int nUserID = iPacket->Decode4();
	auto pUser = User::FindUser(nUserID);
	if (pUser)
		pUser->OnCenterCashItemResult(iPacket);
}

