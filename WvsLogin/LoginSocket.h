#pragma once
#include "..\WvsLib\Memory\ZMemory.h"
#include "..\WvsLib\Net\SocketBase.h"
#include "LoginEntry.h"

class LoginSocket :
	public SocketBase
{
private:
	ZSharedPtr<LoginEntry> m_pLoginEntry;
	void OnClosed();

public:
	LoginSocket(asio::io_service& serverService);
	~LoginSocket();

	void CheckLoginState(int nState);
	void OnPacket(InPacket *iPacket);
	void OnClientRequestStart();
	void OnLoginBackgroundRequest();
	void OnCheckPasswordRequst(InPacket *iPacket);
	void OnSelectGenderAnd2ndPassword(InPacket *iPacket);
	void EncodeLoginEntry(OutPacket *oPacket);
	void OnCheckWorldStatusRequst(InPacket *iPacket);
	void SendWorldInformation();
	void OnClientSelectWorld(InPacket *iPacket);
	void OnClientSecondPasswdCheck();
	void OnClientCheckDuplicatedID(InPacket *iPacket);
	void OnClientCreateNewCharacter(InPacket *iPacket);
	void OnClientSelectCharacter(InPacket *iPacket);
};

