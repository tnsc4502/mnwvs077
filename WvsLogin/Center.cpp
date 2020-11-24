#include "Center.h"
#include <functional>
#include <thread>

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLogin\LoginPacketTypes.hpp"
#include "..\WvsCenter\CenterPacketTypes.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Common\ServerConstants.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "WvsLogin.h"
#include "LoginEntry.h"

Center::Center(asio::io_service& serverService)
	: SocketBase(serverService, true)
{
}

Center::~Center()
{
}

void Center::SetCenterIndex(int idx)
{
	nCenterIndex = idx;
}

void Center::OnConnected()
{
	WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "[WvsLogin][Center::OnConnect]Successfully connected to center server.\n");

	//Encoding handshake packets for Center
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::RegisterCenterRequest);

	//WvsLogin => SVR_LOGIN
	oPacket.Encode1(ServerConstants::ServerType::SRV_LOGIN);

	SendPacket(&oPacket); 
	OnWaitingPacket();
}

void Center::OnPacket(InPacket *iPacket)
{
	WvsLogger::LogRaw("[WvsLogin][Center::OnPacket]Packet received: \n");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
		case CenterResultPacketType::RegisterCenterAck:
		{
			auto nResult = iPacket->Decode1();
			if (!nResult)
			{
				WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsLogin][RegisterCenterAck]Center rejected the connection request, WvsLogin server may not work properly.\n");
				exit(0);
			}
			WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "[WvsLogin][RegisterCenterAck]The connection between local server(WvsCenter) has been authenciated by remote server.\n");
			OnUpdateWorldInfo(iPacket);
			WvsBase::GetInstance<WvsLogin>()->RestoreLoginEntry(iPacket);
			break;
		}
		case CenterResultPacketType::CenterStatChanged:
			OnUpdateChannelInfo(iPacket);
			break;
		case CenterResultPacketType::CharacterListResponse:
			OnCharacterListResponse(iPacket);
			break;
		case CenterResultPacketType::GameServerInfoResponse:
			OnGameServerInfoResponse(iPacket);
			break;
		case CenterResultPacketType::CreateCharacterResult:
			OnCreateCharacterResult(iPacket);
			break;
		case CenterResultPacketType::LoginAuthResult:
			OnLoginAuthResult(iPacket);
			break;
		case CenterResultPacketType::CheckDuplicatedIDResult:
			OnCheckDuplicatedIDResult(iPacket);
			break;
	}
}

void Center::OnClosed()
{
}

void Center::OnUpdateChannelInfo(InPacket * iPacket)
{
	m_WorldInfo.nGameCount = iPacket->Decode2();
	memset(m_WorldInfo.m_aChannelStatus, 0, sizeof(int) * ServerConstants::kMaxChannelCount);
	for (int i = 0; i < m_WorldInfo.nGameCount; ++i)
		m_WorldInfo.m_aChannelStatus[(iPacket->Decode1())] = 1;
}

void Center::OnUpdateWorldInfo(InPacket *iPacket)
{
	m_WorldInfo.nWorldID = iPacket->Decode1();
	m_WorldInfo.nEventType = iPacket->Decode1();
	m_WorldInfo.strWorldDesc = iPacket->DecodeStr();
	m_WorldInfo.strEventDesc = iPacket->DecodeStr();
	WvsLogger::LogRaw(WvsLogger::LEVEL_INFO, "[WvsLogin][Center::OnUpdateWorld]World information is updated by remote notification.\n");
}

void Center::OnConnectFailed()
{
	WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsLogin][Center::OnConnect]Unable to connect to Center Server (Remote service unavailable).\n");
	OnDisconnect();
}

void Center::OnCharacterListResponse(InPacket *iPacket)
{
	unsigned int nLoginSocketID = iPacket->Decode4();
	auto pSocket = WvsBase::GetInstance<WvsLogin>()->GetSocket(nLoginSocketID);
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketType::Client_ClientSelectWorldResult);
	oPacket.Encode1(0);
	oPacket.Encode4(1000000);

	//WvsLogger::LogRaw("[WvsLogin][Center::OnCharacterListResponse]Packet of character list: ");
	//iPacket->Print();
	//WvsLogger::LogRaw("\n");

	oPacket.EncodeBuffer(iPacket->GetPacket() + 6, iPacket->GetPacketSize() - 6);

	oPacket.Encode2(0x03);
	oPacket.Encode4(6); //char slots
	oPacket.Encode4(8); //char slots
	oPacket.Encode4(8); //char slots
	oPacket.Encode8(0);
	pSocket->SendPacket(&oPacket);
}

void Center::OnGameServerInfoResponse(InPacket *iPacket)
{
	unsigned int nLoginSocketID = iPacket->Decode4();
	auto pEntry = WvsBase::GetInstance<WvsLogin>()->GetLoginEntryByLoginSocketSN(nLoginSocketID);
	auto pSocket = WvsBase::GetInstance<WvsLogin>()->GetSocket(nLoginSocketID);
	if (!pEntry || !pSocket)
		return;
	pEntry->nLoginState = LoginState::LS_Stage_MigratedIn;

	//Auth
	if (!iPacket->Decode1())
	{
		pEntry->nLoginState = LoginState::LS_Stage_SelectedWorld;
		pSocket->GetSocket().close();
		return;
	}
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketType::Client_ClientSelectCharacterResult);
	oPacket.EncodeBuffer(
		iPacket->GetPacket() + iPacket->GetReadCount(), 
		iPacket->GetPacketSize() - iPacket->GetReadCount());
	pSocket->SendPacket(&oPacket);
}

void Center::OnCreateCharacterResult(InPacket * iPacket)
{
	unsigned int nLoginSocketID = iPacket->Decode4();
	auto pSocket = WvsBase::GetInstance<WvsLogin>()->GetSocket(nLoginSocketID);
	auto pEntry = WvsBase::GetInstance<WvsLogin>()->GetLoginEntryByLoginSocketSN(nLoginSocketID);

	if (pEntry && pSocket)
	{
		OutPacket oPacket;
		oPacket.Encode2((short)LoginSendPacketType::Client_ClientCreateCharacterResult);
		oPacket.EncodeBuffer(iPacket->GetPacket() + 6, iPacket->GetPacketSize() - 6);
		pSocket->SendPacket(&oPacket);
		pEntry->nLoginState = LoginState::LS_Stage_SelectedWorld; //Reset
	}
}

void Center::OnLoginAuthResult(InPacket * iPacket)
{
	int nType = iPacket->Decode1();
	int nAccountID = iPacket->Decode4();
	switch (nType)
	{
		case LoginAuthResult::res_LoginAuth_RefreshLoginState:
		case LoginAuthResult::res_LoginAuth_UnRegisterMigratinon:
		{
			if (nType == LoginAuthResult::res_LoginAuth_UnRegisterMigratinon ||
				!iPacket->Decode1())
				WvsBase::GetInstance<WvsLogin>()->RemoveLoginEntryByAccountID(nAccountID);
		}
	}
}

void Center::OnCheckDuplicatedIDResult(InPacket *iPacket)
{
	unsigned int nLoginSocketID = iPacket->Decode4();
	auto pEntry = WvsBase::GetInstance<WvsLogin>()->GetLoginEntryByLoginSocketSN(nLoginSocketID);
	auto pSocket = WvsBase::GetInstance<WvsLogin>()->GetSocket(nLoginSocketID);
	if (pSocket &&
		pEntry &&
		pEntry->nAccountID == iPacket->Decode4() &&
		pEntry->nLoginState == LoginState::LS_Stage_CheckDuplicatedID)
	{
		OutPacket oPacket;
		oPacket.Encode2(LoginSendPacketType::Client_ClientCheckDuplicatedIDResult);
		oPacket.EncodeStr(iPacket->DecodeStr());
		oPacket.Encode1(iPacket->Decode1());
		pSocket->SendPacket(&oPacket);
	}
}

void Center::OnNotifyCenterDisconnected(SocketBase * pSocket)
{
	WvsLogger::LogRaw("[WvsLogin][Center::OnNotifyCenterDisconnected]Disconnected from WvsCenter (closed by remote server).\n");
}
