#include "Center.h"
#include <functional>
#include <thread>

#include "..\Database\GW_ItemSlotBase.h"

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\LoginPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\GameSrvPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\FieldPacketFlags.hpp"

#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Common\ServerConstants.hpp"

#include "WvsGame.h"
#include "User.h"
#include "FieldMan.h"
#include "PartyMan.h"
#include "GuildMan.h"
#include "FriendMan.h"

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
	WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsLogin][Center]與Center Server中斷連線。\n");
}

void Center::OnConnected()
{
	WvsLogger::LogRaw("[WvsGame][Center::OnConnect]Center Server 認證完成，與世界伺服器連線成功建立。\n");

	//向Center Server發送Hand Shake封包
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Center_RegisterCenterRequest);

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
	std::lock_guard<std::mutex> lock(WvsBase::GetInstance<WvsGame>()->GetUserLock());
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
		case CenterSendPacketFlag::RegisterCenterAck:
		{
			auto result = iPacket->Decode1();
			if (!result)
			{
				WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[Warning]The Center Server Didn't Accept This Socket. Program Will Terminated.\n");
				exit(0);
			}
			int nWorldID = iPacket->Decode1();
			WvsBase::GetInstance<WvsGame>()->GetCenter()->SetWorldID(nWorldID);
			GW_ItemSlotBase::ms_nWorldID = nWorldID;
			GW_ItemSlotBase::ms_nChannelID = 
				WvsBase::GetInstance<WvsGame>()->GetChannelID() + 1; //Channel 0 is reserved for Center

			char aBuffer[128];
			sprintf_s(aBuffer, "MapleStory Server [WvsGame][TWMS][%03d][WorldID : %02d][ChannelID : %02d]",
				ServerConstants::kGameVersion,
				nWorldID,
				WvsBase::GetInstance<WvsGame>()->GetChannelID());
			SetConsoleTitleA(aBuffer);

			for (int i = 1; i <= 5; ++i)
				GW_ItemSlotBase::SetInitSN(i, iPacket->Decode8());
			WvsLogger::LogRaw("Center Server Authenciated Ok. The Connection Between Local Server Has Builded.\n");
			break;
		}
		case CenterSendPacketFlag::CenterMigrateInResult:
			OnCenterMigrateInResult(iPacket);
			break;
		case CenterSendPacketFlag::MigrateCashShopResult:
		case CenterSendPacketFlag::TransferChannelResult:
			OnTransferChannelResult(iPacket);
			break;
		case CenterSendPacketFlag::PartyResult:
			PartyMan::GetInstance()->OnPacket(iPacket);
			break;
		case CenterSendPacketFlag::GuildResult:
			GuildMan::GetInstance()->OnPacket(iPacket);
			break;
		case CenterSendPacketFlag::FriendResult:
			FriendMan::GetInstance()->OnPacket(iPacket);
			break;
		case CenterSendPacketFlag::RemoteBroadcasting:
			OnRemoteBroadcasting(iPacket);
			break;
		case CenterSendPacketFlag::TrunkResult:
			OnTrunkResult(iPacket);
			break;
	}
}

void Center::OnClosed()
{
}

void Center::OnConnectFailed()
{
	WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsGame][Center::OnConnect]Center Server拒絕當前LocalServer連接，程式即將終止。\n");
	OnDisconnect();
}

void Center::OnCenterMigrateInResult(InPacket *iPacket)
{
	unsigned int nClientSocketID = iPacket->Decode4();
	auto pSocket = WvsBase::GetInstance<WvsGame>()->GetSocket(nClientSocketID);

	auto deleter = [](User* p) { FreeObj(p); };
	std::shared_ptr <User> pUser{ 
		AllocObjCtor(User)((ClientSocket*)pSocket, iPacket),
		deleter
	};
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
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnTransferChannel);
		
		// 7 = Header(2) + nClientSocketID(4) + bSuccess(1)
		oPacket.EncodeBuffer(iPacket->GetPacket() + 7, iPacket->GetPacketSize() - 7);
	}
	else
	{
		oPacket.Encode2(FieldSendPacketFlag::Field_OnTransferChannelReqIgnored);
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
	{
		auto& mConnectedUser = WvsBase::GetInstance<WvsGame>()->GetConnectedUser();
		for (auto& prUser : mConnectedUser)
			prUser.second->SendPacket(&oPacket);
	}
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

