#include "CenterApp.h"
#include <iostream>
#include <thread>

#include "LocalServer.h"
#include "WvsCenter.h"
#include "WvsWorld.h"

#include "..\Database\WvsUnified.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "..\WvsLib\Exception\WvsException.h"
#include "..\WvsLib\String\StringPool.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsCenter *centerServer = WvsBase::GetInstance<WvsCenter>();
	centerServer->CreateAcceptor(nPort);
	centerServer->BeginAccept<LocalServer>();
}

void CenterApp::InitializeService(int argc, char **argv)
{
	WvsException::RegisterUnhandledExceptionFilter("WvsCenter", nullptr);
	ConfigLoader* pConfigLoader = nullptr;
	if (argc > 1)
		pConfigLoader = ConfigLoader::Get(argv[1]);
	else
	{
		std::cout << "Please run this program with command line, and provide a path of config file." << std::endl;
		exit(0);
	}
	WzResMan::GetInstance()->Init(pConfigLoader->StrValue("GlobalConfig"));
	StringPool::Init(pConfigLoader->StrValue("GlobalConfig"));
	WvsUnified::InitDB(pConfigLoader);
	GW_ItemSlotBase::InitItemSN(pConfigLoader->IntValue("WorldID"));
	WvsWorld::GetInstance()->SetConfigLoader(pConfigLoader);
	WvsBase::GetInstance<WvsCenter>()->Init();
	WvsWorld::GetInstance()->InitializeWorld();

	// start the connection acceptor thread
	std::thread thread1(ConnectionAcceptorThread, (pConfigLoader->IntValue("Port")));

	// start the i/o work
	asio::io_service &io = WvsBase::GetInstance<WvsCenter>()->GetIOService();
	asio::io_service::work work(io);

	for (;;)
	{
		std::error_code ec;
		io.run(ec);
	}
}

void CenterApp::OnCommandPromptInput(std::string& sInput)
{
}

