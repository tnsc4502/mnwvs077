#include "LoginSocket.h"
#include "WvsLogin.h"
#include "..\Database\LoginDBAccessor.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\LoginPacketFlags.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Common\WvsLoginConstants.hpp"

LoginSocket::LoginSocket(asio::io_service& serverService)
	: SocketBase(serverService)
{
	m_loginEntry.Initialize();
}

LoginSocket::~LoginSocket()
{
}

void LoginSocket::OnPacket(InPacket *iPacket)
{
	WvsLogger::LogRaw("[WvsLogin][LoginSocket::OnPacket]封包接收：");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();

	switch (nType)
	{
	case LoginRecvPacketFlag::Client_ClientRequestStart:
		//OnClientRequestStart();
		break;
	case LoginRecvPacketFlag::Client_ClientLoginBackgroundRequest:
		//OnLoginBackgroundRequest();
		break;
	case LoginRecvPacketFlag::Client_ClientCheckPasswordRequest:
		OnCheckPasswordRequst(iPacket);
		break;
	case LoginRecvPacketFlag::Client_ClientSelectWorld:
		OnClientSelectWorld(iPacket);
		break;
	case LoginRecvPacketFlag::Client_ClientWorldStatusCheck:
		OnCheckWorldStatusRequst(iPacket);
		break;
	case LoginRecvPacketFlag::Client_ClientSecondPasswordCheck:
		OnClientSecondPasswdCheck();
		break;
	case LoginRecvPacketFlag::Client_ClientCheckDuplicatedID:
		OnClientCheckDuplicatedID(iPacket);
		break;
	case LoginRecvPacketFlag::Client_ClientCreateNewCharactar:
		OnClientCreateNewCharacter(iPacket);
		break;
	case LoginRecvPacketFlag::Client_ClientSelectCharacter:
		OnClientSelectCharacter(iPacket);
		break;
	case LoginRecvPacketFlag::Client_ClientRequestServerList:
		SendWorldInformation();
		break;
	}
}

void LoginSocket::OnClosed()
{

}

void LoginSocket::OnClientRequestStart()
{
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_ClientStartResponse);
	oPacket.Encode4(0);
	SendPacket(&oPacket);
}

void LoginSocket::OnLoginBackgroundRequest()
{
	static std::string backgrounds[] = { "MapLogin", "MapLogin1", "MapLogin2" };
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_ClientLoginBackgroundResponse);
	oPacket.EncodeStr(backgrounds[rand() % (sizeof(backgrounds) / sizeof(backgrounds[0]))]);
	oPacket.Encode4(0);
	oPacket.Encode1(1);
	SendPacket(&oPacket);
}

void LoginSocket::OnCheckPasswordRequst(InPacket *iPacket)
{
	if (m_loginEntry.nLoginState != LoginState::LS_Connection_Established)
		return;

	m_loginEntry.Initialize();
	auto sID = iPacket->DecodeStr();
	auto sPasswd = iPacket->DecodeStr();
	int nCheckResult = LoginDBAccessor::CheckPassword(
		sID,
		sPasswd,
		0,
		7,
		m_loginEntry.nAccountID,
		m_loginEntry.nGender
	);

	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_CheckPasswordResponse);
	LoginEntry* pEntry = nullptr;

	if (nCheckResult == LoginResult::res_PasswdCheck_Success)
	{
		if ((pEntry = WvsBase::GetInstance<WvsLogin>()->GetLoginEntryByAccountID(m_loginEntry.nAccountID)))
		{
			nCheckResult = LoginResult::res_LoginStatus_Account_AlreadyLoggedIn;
			if (pEntry->nLoginState == LoginState::LS_Stage_MigratedIn ||
				pEntry->nLoginState == LoginState::LS_Stage_SelectCharacter)
			{
				//To ensure the consistency of specific login state between WvsLogin and WvsCenter.
				OutPacket oPacket;
				oPacket.Encode2(LoginSendPacketFlag::Center_RequestLoginAuth);
				oPacket.Encode1(LoginAuthRequest::rq_LoginAuth_RefreshLoginState);
				oPacket.Encode4(m_loginEntry.nAccountID);
				WvsBase::GetInstance<WvsLogin>()->GetCenter(pEntry->nWorldID)->SendPacket(&oPacket);
			}
			goto REPORT_FAILED_REASON;
		}
		else if (WvsBase::GetInstance<WvsLogin>()->GetLoginEntryByLoginSocketSN(GetSocketID()))
		{
			nCheckResult = LoginResult::res_LoginStatus_Socket_AlreadyLoggedIn;
			goto REPORT_FAILED_REASON;
		}

		m_loginEntry.uLoginSocketSN = GetSocketID();
		m_loginEntry.strAccountName = sID;
		m_loginEntry.nLoginState = LoginState::LS_PasswdCheck_Authenticated;
		WvsBase::GetInstance<WvsLogin>()->RegisterLoginEntry(&m_loginEntry);

		oPacket.Encode4(m_loginEntry.nAccountID); //Account ID
		oPacket.Encode1(m_loginEntry.nGender); //Gender
		oPacket.Encode1(0); //GM
		oPacket.EncodeStr(sID);
		oPacket.Encode4(0);
		oPacket.Encode8(0);
		oPacket.Encode8(0);
		oPacket.Encode8(0);
		SendPacket(&oPacket);
		SendWorldInformation();
	}

REPORT_FAILED_REASON:
	if (nCheckResult)
	{
		m_loginEntry.Initialize();
		oPacket.Encode1(nCheckResult);
		SendPacket(&oPacket);
	}
}

void LoginSocket::OnCheckWorldStatusRequst(InPacket * iPacket)
{
	auto& pCenter = WvsBase::GetInstance<WvsLogin>()->GetCenter(iPacket->Decode1());
	if (pCenter)
	{
		OutPacket oPacket;
		oPacket.Encode2((short)LoginSendPacketFlag::Client_WorldStatusResult);
		oPacket.Encode2(0); //Normal

		SendPacket(&oPacket);
	}
}

void LoginSocket::SendWorldInformation()
{
	int nCenterCount = WvsBase::GetInstance<WvsLogin>()->GetCenterCount();
	for (int i = 0; i < nCenterCount; ++i)
	{
		auto& pCenter = WvsBase::GetInstance<WvsLogin>()->GetCenter(i);
		if (pCenter && pCenter->CheckSocketStatus(SocketBase::SocketStatus::eConnected))
		{
			OutPacket oPacket;
			oPacket.Encode2(LoginSendPacketFlag::Client_WorldInformationResponse);
			oPacket.Encode1(pCenter->GetWorldInfo().nWorldID);
			oPacket.EncodeStr(pCenter->GetWorldInfo().strWorldDesc);
			oPacket.Encode1(pCenter->GetWorldInfo().nEventType);
			oPacket.EncodeStr(pCenter->GetWorldInfo().strEventDesc);
			oPacket.Encode2(0x64);
			oPacket.Encode2(0x64);
			int nMaxChannelCount = pCenter->GetWorldInfo().nGameCount;
			for (int i = 0; i < 30; ++i)
				if (pCenter->GetWorldInfo().m_aChannelStatus[i] == 1 && i + 1 > nMaxChannelCount)
					nMaxChannelCount = i + 1;
			oPacket.Encode1(nMaxChannelCount);
			for (int i = 1; i <= nMaxChannelCount; ++i)
			{
				oPacket.EncodeStr("Channel " + std::to_string(i));
				oPacket.Encode4(pCenter->GetWorldInfo().m_aChannelStatus[i - 1] == 0 ? 100000 : 1);
				oPacket.Encode1(pCenter->GetWorldInfo().nWorldID);
				oPacket.Encode2(i - 1);
			}
			oPacket.Encode2(0);
			SendPacket(&oPacket);
		}
	}
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_WorldInformationResponse);
	oPacket.Encode1((char)0xFF);
	SendPacket(&oPacket);
}

void LoginSocket::OnClientSelectWorld(InPacket *iPacket)
{
	bool bIsRelogin = iPacket->Decode1() == 1;
	int nWorldIndex = iPacket->Decode1();
	int nChannelIndex = iPacket->Decode1();

	//Check World
	if (nWorldIndex < 0 || nWorldIndex >= ServerConstants::kMaxNumberOfCenters)
		return;

	auto pCenter = WvsBase::GetInstance<WvsLogin>()->GetCenter(nWorldIndex);

	//Check Channel
	if (nChannelIndex < 0 ||
		nChannelIndex >= ServerConstants::kMaxChannelCount ||
		!(pCenter->GetWorldInfo().m_aChannelStatus[nChannelIndex]))
		return;

	if (pCenter->CheckSocketStatus(SocketBase::SocketStatus::eConnected))
	{
		m_loginEntry.nLoginState = LoginState::LS_Stage_SelectWorld;

		OutPacket oPacket;
		oPacket.Encode2(LoginSendPacketFlag::Center_RequestCharacterList);
		oPacket.Encode4(GetSocketID());
		oPacket.Encode4(m_loginEntry.nAccountID);
		oPacket.Encode1(nChannelIndex);
		pCenter->SendPacket(&oPacket);
		m_loginEntry.nChannelID = nChannelIndex;
		m_loginEntry.nWorldID = nWorldIndex;
	}
	else
		WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsLogin][LoginSocket::OnClientSelectWorld][錯誤]客戶端嘗試連線至不存在的Center Server。\n");
}

void LoginSocket::OnClientSecondPasswdCheck()
{
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_ClientSecondPasswordResult);
	oPacket.Encode1(0x03);
	oPacket.Encode1(0x00);
	SendPacket(&oPacket);
}

void LoginSocket::OnClientCheckDuplicatedID(InPacket *iPacket)
{
	std::string strName = iPacket->DecodeStr();

	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_ClientCheckDuplicatedIDResult);
	oPacket.EncodeStr(strName);
	oPacket.Encode1(0); //SUCCESS

	SendPacket(&oPacket);
}

void LoginSocket::OnClientCreateNewCharacter(InPacket *iPacket)
{
	m_loginEntry.nLoginState = LoginState::LS_Stage_CreateNewCharacter;

	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Center_RequestCreateNewCharacter);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(m_loginEntry.nAccountID);
	oPacket.EncodeBuffer(iPacket->GetPacket() + 2, iPacket->GetPacketSize() - 2); //SKIP OPCODE
	WvsBase::GetInstance<WvsLogin>()->GetCenter(m_loginEntry.nWorldID)->SendPacket(&oPacket);
}

void LoginSocket::OnClientSelectCharacter(InPacket *iPacket)
{
	m_loginEntry.nLoginState = LoginState::LS_Stage_SelectCharacter;

	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Center_RequestGameServerInfo);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(m_loginEntry.nAccountID);
	oPacket.Encode4(m_loginEntry.nWorldID);
	oPacket.Encode4(m_loginEntry.nChannelID);
	oPacket.EncodeBuffer(iPacket->GetPacket() + 2, iPacket->GetPacketSize() - 2);
	WvsBase::GetInstance<WvsLogin>()->GetCenter(m_loginEntry.nWorldID)->SendPacket(&oPacket);
}