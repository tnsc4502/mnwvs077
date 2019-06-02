#pragma once
#include "..\WvsLib\Net\SocketBase.h"
#include "LoginEntry.h"

class LoginSocket :
	public SocketBase
{
private:
	LoginEntry m_loginEntry;
	void OnClosed();

public:
	LoginSocket(asio::io_service& serverService);
	~LoginSocket();

	void OnPacket(InPacket *iPacket);
	void OnClientRequestStart();
	void OnLoginBackgroundRequest();
	void OnCheckPasswordRequst(InPacket *iPacket);
	void OnCheckWorldStatusRequst(InPacket *iPacket);
	void SendWorldInformation();
	void OnClientSelectWorld(InPacket *iPacket);
	void OnClientSecondPasswdCheck();
	void OnClientCheckDuplicatedID(InPacket *iPacket);
	void OnClientCreateNewCharacter(InPacket *iPacket);
	void OnClientSelectCharacter(InPacket *iPacket);
};

