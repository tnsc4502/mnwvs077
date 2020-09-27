#include "LoginSocket.h"
#include "WvsLogin.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\Database\LoginDBAccessor.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\LoginPacketFlags.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Common\WvsLoginConstants.hpp"

LoginSocket::LoginSocket(asio::io_service& serverService)
	: SocketBase(serverService)
{
	m_pLoginEntry.reset(AllocObj(LoginEntry), [](LoginEntry* p) { FreeObj(p); });
	m_pLoginEntry->Initialize();
}

LoginSocket::~LoginSocket()
{
}

void LoginSocket::CheckLoginState(int nState)
{
	if (m_pLoginEntry->nLoginState != nState)
	{
		GetSocket().close();
		throw std::runtime_error("Incorrect login state.");
	}
}

void LoginSocket::OnPacket(InPacket *iPacket)
{
	/*WvsLogger::LogRaw("[WvsLogin][LoginSocket::OnPacket]Received Packet: ");
	iPacket->Print();*/
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
	case LoginRecvPacketFlag::Client_ClientSelectGenderAnd2ndPassword:
		OnSelectGenderAnd2ndPassword(iPacket);
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
	if (m_pLoginEntry->nLoginState != LoginState::LS_Connection_Established)
		return;

	m_pLoginEntry->Initialize();
	auto sID = iPacket->DecodeStr();
	auto sPasswd = iPacket->DecodeStr();
	int nCheckResult = LoginDBAccessor::CheckPassword(
		sID,
		sPasswd,
		0,
		7,
		m_pLoginEntry->nAccountID,
		m_pLoginEntry->nGender
	);

	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_CheckPasswordResponse);
	std::shared_ptr<LoginEntry> pEntry = nullptr;

	if (nCheckResult == LoginResult::res_PasswdCheck_Success)
	{
		if ((pEntry = WvsBase::GetInstance<WvsLogin>()->GetLoginEntryByAccountID(m_pLoginEntry->nAccountID)))
		{
			nCheckResult = LoginResult::res_LoginStatus_Account_AlreadyLoggedIn;
			if (!WvsBase::GetInstance<WvsLogin>()->GetSocket(pEntry->uLoginSocketSN))
			{
				if (pEntry->nWorldID == -1)
				{
					nCheckResult = LoginResult::res_PasswdCheck_Success;
					WvsBase::GetInstance<WvsLogin>()->RemoveLoginEntryByAccountID(m_pLoginEntry->nAccountID);
					goto LOGIN_SUCCESS;
				}
				//To ensure the consistency of specific login state between WvsLogin and WvsCenter.
				OutPacket oPacket;
				oPacket.Encode2(LoginSendPacketFlag::Center_RequestLoginAuth);
				oPacket.Encode1(LoginAuthRequest::rq_LoginAuth_RefreshLoginState);
				oPacket.Encode4(m_pLoginEntry->nAccountID);
				WvsBase::GetInstance<WvsLogin>()->GetCenter(pEntry->nWorldID)->SendPacket(&oPacket);
			}
			goto REPORT_FAILED_REASON;
		}
		else if (WvsBase::GetInstance<WvsLogin>()->GetLoginEntryByLoginSocketSN(GetSocketID()))
		{
			nCheckResult = LoginResult::res_LoginStatus_Socket_AlreadyLoggedIn;
			goto REPORT_FAILED_REASON;
		}

	LOGIN_SUCCESS:
		oPacket.Encode1(nCheckResult);
		m_pLoginEntry->uLoginSocketSN = GetSocketID();
		m_pLoginEntry->strAccountName = sID;
		m_pLoginEntry->nLoginState = LoginState::LS_PasswdCheck_Authenticated;
		WvsBase::GetInstance<WvsLogin>()->RegisterLoginEntry(m_pLoginEntry);
		if (m_pLoginEntry->nGender == -1)
		{
			oPacket.Reset();
			oPacket.Encode2(LoginSendPacketFlag::Client_RequestSelectGenderAnd2ndPassword);
			oPacket.EncodeStr(m_pLoginEntry->strAccountName);
			m_pLoginEntry->nLoginState = LoginState::LS_SelectGenderAnd2ndPassword;
			SendPacket(&oPacket);
			return;
		}

		EncodeLoginEntry(&oPacket);
		SendPacket(&oPacket);
		SendWorldInformation();
	}

REPORT_FAILED_REASON:
	if (nCheckResult)
	{
		oPacket.Encode1(nCheckResult);
		m_pLoginEntry->Initialize();
		SendPacket(&oPacket);
	}
}

void LoginSocket::OnSelectGenderAnd2ndPassword(InPacket *iPacket)
{
	CheckLoginState(LoginState::LS_SelectGenderAnd2ndPassword);

	std::string sAccountName = iPacket->DecodeStr();
	std::string s2ndPasswd = iPacket->DecodeStr();
	int nGender = iPacket->Decode1();
	if (m_pLoginEntry->strAccountName != sAccountName ||
		m_pLoginEntry != WvsBase::GetInstance<WvsLogin>()->GetLoginEntryByLoginSocketSN(GetSocketID()) ||
		(nGender < 0 || nGender > 1))
	{
		GetSocket().close();
		return;
	}

	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Client_SelectGenderAnd2ndPasswordResult);
	if (s2ndPasswd != "" && s2ndPasswd.size() >= 5 && s2ndPasswd.size() < 16)
	{
		LoginDBAccessor::UpdateGenderAnd2ndPassword(
			m_pLoginEntry->nAccountID,
			nGender,
			s2ndPasswd
		);

		m_pLoginEntry->nGender = nGender;
		m_pLoginEntry->nLoginState = LoginState::LS_PasswdCheck_Authenticated;
		oPacket.Encode1(1);
		SendPacket(&oPacket);

		oPacket.Reset();
		oPacket.Encode2(LoginSendPacketFlag::Client_CheckPasswordResponse);
		oPacket.Encode1(LoginResult::res_PasswdCheck_Success);
		EncodeLoginEntry(&oPacket);
		SendPacket(&oPacket);
		SendWorldInformation();
	}
	else 
	{
		oPacket.Encode1(0);
		m_pLoginEntry->nLoginState = LoginState::LS_Connection_Established;
		WvsBase::GetInstance<WvsLogin>()->RemoveLoginEntryByLoginSocketID(GetSocketID());
		m_pLoginEntry->Initialize();
		SendPacket(&oPacket);
	}
}

void LoginSocket::EncodeLoginEntry(OutPacket *oPacket)
{
	oPacket->Encode4(m_pLoginEntry->nAccountID); //Account ID
	oPacket->Encode1(m_pLoginEntry->nGender); //Gender
	oPacket->Encode1(0); //GM
	oPacket->EncodeStr(m_pLoginEntry->strAccountName);
	oPacket->Encode4(0);
	oPacket->Encode8(0);
	oPacket->Encode8(0);
	oPacket->Encode8(0);
}

void LoginSocket::OnCheckWorldStatusRequst(InPacket *iPacket)
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
	CheckLoginState(LoginState::LS_PasswdCheck_Authenticated);
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
		m_pLoginEntry->nLoginState = LoginState::LS_Stage_SelectedWorld;
		OutPacket oPacket;
		oPacket.Encode2(LoginSendPacketFlag::Center_RequestCharacterList);
		oPacket.Encode4(GetSocketID());
		oPacket.Encode4(m_pLoginEntry->nAccountID);
		oPacket.Encode1(nChannelIndex);
		pCenter->SendPacket(&oPacket);
		m_pLoginEntry->nChannelID = nChannelIndex;
		m_pLoginEntry->nWorldID = nWorldIndex;
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
	//CheckLoginState(LoginState::LS_Stage_SelectedWorld);
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Center_RequestCheckDuplicatedID);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(m_pLoginEntry->nAccountID);
	oPacket.EncodeStr(iPacket->DecodeStr());
	m_pLoginEntry->nLoginState = LoginState::LS_Stage_CheckDuplicatedID;
	WvsBase::GetInstance<WvsLogin>()->GetCenter(m_pLoginEntry->nWorldID)->SendPacket(&oPacket);
}

void LoginSocket::OnClientCreateNewCharacter(InPacket *iPacket)
{
	CheckLoginState(LoginState::LS_Stage_CheckDuplicatedID);
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Center_RequestCreateNewCharacter);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(m_pLoginEntry->nAccountID);
	oPacket.EncodeBuffer(iPacket->GetPacket() + 2, iPacket->GetPacketSize() - 2); //SKIP OPCODE
	WvsBase::GetInstance<WvsLogin>()->GetCenter(m_pLoginEntry->nWorldID)->SendPacket(&oPacket);
}

void LoginSocket::OnClientSelectCharacter(InPacket *iPacket)
{
	CheckLoginState(LoginState::LS_Stage_SelectedWorld);
	m_pLoginEntry->nLoginState = LoginState::LS_Stage_SelectedCharacter;
	OutPacket oPacket;
	oPacket.Encode2(LoginSendPacketFlag::Center_RequestGameServerInfo);
	oPacket.Encode4(GetSocketID());
	oPacket.Encode4(m_pLoginEntry->nAccountID);
	oPacket.Encode4(m_pLoginEntry->nWorldID);
	oPacket.Encode4(m_pLoginEntry->nChannelID);
	oPacket.EncodeBuffer(iPacket->GetPacket() + 2, iPacket->GetPacketSize() - 2);
	WvsBase::GetInstance<WvsLogin>()->GetCenter(m_pLoginEntry->nWorldID)->SendPacket(&oPacket);
}