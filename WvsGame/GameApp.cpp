#include "GameApp.h"
#include <iostream>
#include <thread>
#include <functional>

#include "QuestMan.h"
#include "ClientSocket.h"
#include "WvsGame.h"
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "FieldMan.h"
#include "TimerThread.h"
#include "NpcTemplate.h"
#include "ReactorTemplate.h"
#include "ContinentMan.h"
#include "Reward.h"
#include "CalcDamage.h"
#include "ScriptMan.h"
#include "PetTemplate.h"

#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Exception\WvsException.h"
#include "..\WvsLib\String\StringPool.h"
#include "..\Database\GW_ItemSlotEquip.h"

BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
	switch (CEvent)
	{
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			//WvsBase::GetInstance<WvsGame>()->ShutdownService();
			break;
	}
	return TRUE;
}

void ConnectionAcceptorThread(short nPort)
{
	WvsGame *gameServer = WvsBase::GetInstance<WvsGame>();
	gameServer->CreateAcceptor(nPort);
	gameServer->BeginAccept<ClientSocket>();
}

void UnhandledExcpetionHandler()
{
	WvsBase::GetInstance<WvsGame>()->ShutdownService();
}

void GameApp::InitializeService(int argc, char** argv)
{
	ConfigLoader* pCfgLoader = nullptr;
	if (argc > 1)
		pCfgLoader = ConfigLoader::Get(argv[1]);
	else
	{
		WvsLogger::LogRaw("Please run this program with command line, and provide the path of the config file.\n");
		exit(0);
	}
	
	auto tInitStart = std::chrono::high_resolution_clock::now();
	WzResMan::GetInstance()->Init(pCfgLoader->StrValue("GlobalConfig"));
	StringPool::Init(pCfgLoader->StrValue("GlobalConfig"));
	WvsException::RegisterUnhandledExceptionFilter("WvsGame", UnhandledExcpetionHandler);
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);
	TimerThread::RegisterTimerPool(50, 1000);
	QuestMan::GetInstance()->Initialize();
	ItemInfo::GetInstance()->Initialize();
	Reward::LoadReward();
	ReactorTemplate::Load();
	NpcTemplate::GetInstance()->Load();
	FieldMan::GetInstance()->LoadAreaCode();
	FieldMan::GetInstance()->LoadFieldSet();
	ContinentMan::GetInstance()->Init();
	CalcDamage::LoadStandardPDD();
	PetTemplate::Load();

	if (pCfgLoader->IntValue("PreRegisterAllField"))
		FieldMan::GetInstance()->RegisterAllField();

	SkillInfo::GetInstance()->LoadMobSkill();
	SkillInfo::GetInstance()->LoadMCSkill();
	SkillInfo::GetInstance()->LoadMCGuardian();
	SkillInfo::GetInstance()->IterateSkillInfo();

	WvsBase::GetInstance<WvsGame>()->Init();
	WvsBase::GetInstance<WvsGame>()->SetExternalIP(pCfgLoader->StrValue("ExternalIP"));
	WvsBase::GetInstance<WvsGame>()->SetExternalPort(pCfgLoader->IntValue("Port"));
	// start the connection acceptor thread

	std::thread thread1(ConnectionAcceptorThread, pCfgLoader->IntValue("Port"));
	WvsBase::GetInstance<WvsGame>()->SetConfigLoader(pCfgLoader);
	WvsBase::GetInstance<WvsGame>()->InitializeCenter();
	ScriptMan::GetInstance()->RegisterScriptFuncReflector();

	auto tInitEnd = std::chrono::high_resolution_clock::now();
	WvsLogger::LogFormat("Game server has successfully initialized in %lld us.\n", std::chrono::duration_cast<std::chrono::microseconds>(tInitEnd - tInitStart).count());

	// start the i/o work
	asio::io_service &io = WvsBase::GetInstance<WvsGame>()->GetIOService();
	asio::io_service::work work(io);

	for (;;)
	{
		std::error_code ec;
		io.run(ec);
	}
}
