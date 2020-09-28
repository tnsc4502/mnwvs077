#include "ShopApp.h"
#include <iostream>
#include <thread>

#include "WvsShop.h"
#include "ClientSocket.h"
#include "ShopInfo.h"

#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Exception\WvsException.h"
#include "..\WvsGame\ItemInfo.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsShop *pShopServer = WvsBase::GetInstance<WvsShop>();
	pShopServer->CreateAcceptor(nPort);
	pShopServer->BeginAccept<ClientSocket>();
}

void ShopApp::InitializeService(int argc, char** argv)
{
	WvsException::RegisterUnhandledExceptionFilter("WvsShop", []() { WvsBase::GetInstance<WvsShop>()->ShutdownService(); });
	ConfigLoader* pCfgLoader = nullptr;
	if (argc > 1)
		pCfgLoader = ConfigLoader::Get(argv[1]);
	else
	{
		WvsLogger::LogRaw("Please run this program with command line, and given the config file path.\n");
		exit(-1);
	}

	WzResMan::GetInstance()->Init(pCfgLoader->StrValue("GlobalConfig"));
	WvsShop *pShopServer = WvsBase::GetInstance<WvsShop>();
	ItemInfo::GetInstance()->Initialize();
	ShopInfo::GetInstance()->Init();

	pShopServer->SetExternalIP(pCfgLoader->StrValue("ExternalIP"));
	pShopServer->SetExternalPort(pCfgLoader->IntValue("Port"));
	pShopServer->Init();
	std::thread pShopSrvThread(ConnectionAcceptorThread, pCfgLoader->IntValue("Port"));
	pShopServer->SetConfigLoader(pCfgLoader);
	pShopServer->InitializeCenter();

	// start the i/o work
	asio::io_service &io = WvsBase::GetInstance<WvsShop>()->GetIOService();
	asio::io_service::work work(io);
	for (;;)
	{
		std::error_code ec;
		io.run(ec);
	}
}

void ShopApp::OnCommandPromptInput(std::string & sInput)
{
}
