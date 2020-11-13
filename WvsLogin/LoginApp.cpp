#include "LoginApp.h"

#include <iostream>
#include <thread>
#include "WvsLogin.h"
#include "LoginSocket.h"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\Database\WvsUnified.h"
#include "..\WvsLib\Exception\WvsException.h"

#include "..\WvsLogin\LoginPacketTypes.hpp"

void ConnectionAcceptorThread(short nPort)
{
	WvsLogin *pLoginServer = WvsBase::GetInstance<WvsLogin>();
	pLoginServer->CreateAcceptor(nPort);
	pLoginServer->BeginAccept<LoginSocket>();
}

void LoginApp::InitializeService(int argc, char** argv)
{
	WvsException::RegisterUnhandledExceptionFilter("WvsLogin", nullptr);
	WvsLogin *pLoginServer = WvsBase::GetInstance<WvsLogin>();
	ConfigLoader* pConfigLoader = nullptr;

	if (argc > 1)
		pConfigLoader = ConfigLoader::Get(argv[1]);
	else
	{
		WvsLogger::LogRaw("Please run this program with command line, and given the config file path.\n");
		exit(-1);
	}
	WvsUnified::InitDB(pConfigLoader);
	pLoginServer->SetConfigLoader(pConfigLoader);
	pLoginServer->Init();
	pLoginServer->InitializeCenter();
	std::thread initLoginServerThread(ConnectionAcceptorThread, pConfigLoader->IntValue("Port"));

	// start the i/o work

	asio::io_service &io = WvsBase::GetInstance<WvsLogin>()->GetIOService();
	asio::io_service::work work(io);
	for (;;)
	{
		std::error_code ec;
		io.run(ec);
	}
}

void LoginApp::OnCommandPromptInput(std::string& sInput)
{
}
