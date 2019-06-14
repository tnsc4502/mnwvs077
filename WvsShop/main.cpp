// WvsShop.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <thread>

#include "WvsShop.h"
#include "ClientSocket.h"
#include "ShopInfo.h"

#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "..\WvsLib\Common\WvsLoginConstants.hpp"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Exception\WvsException.h"
#include "..\WvsGame\ItemInfo.h"
#include "..\WvsLib\Memory\ZMemory.h"

void ConnectionAcceptorThread(short nPort)
{
	WvsShop *pShopServer = WvsBase::GetInstance<WvsShop>();
	pShopServer->CreateAcceptor(nPort);
	pShopServer->BeginAccept<ClientSocket>();
}

int main(int argc, char** argv)
{
	auto p = ZUniquePtr<int>(ZAllocateArray<int>(5));
	//ZReleaseArray(p);
	WvsException::RegisterUnhandledExceptionFilter("WvsShop", []() { WvsBase::GetInstance<WvsShop>()->ShutdownService(); });
	WvsShop *pShopServer = WvsBase::GetInstance<WvsShop>();
	ItemInfo::GetInstance()->Initialize();
	ShopInfo::GetInstance()->Init();

	ConfigLoader* pCfgLoader = nullptr;
	if (argc > 1)
		pCfgLoader = ConfigLoader::Get(argv[1]);
	else
	{
		WvsLogger::LogRaw("Please run this program with command line, and given the config file path.\n");
		return -1;
	}
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

