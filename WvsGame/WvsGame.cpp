#include "WvsGame.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "ClientSocket.h"
#include "User.h"

WvsGame::WvsGame()
{

}

WvsGame::~WvsGame()
{

}

std::shared_ptr<Center>& WvsGame::GetCenter()
{
	return m_pCenterInstance;
}

std::recursive_mutex & WvsGame::GetUserLock()
{
	return m_mUserLock;
}

std::map<int, ZSharedPtr<User>>& WvsGame::GetConnectedUser()
{
	return m_mUserMap;
}

void WvsGame::ConnectToCenter(int nCenterIdx)
{
	m_pCenterInstance->SetSocketDisconnectedCallBack(std::bind(&Center::OnNotifyCenterDisconnected, m_pCenterInstance.get()));
	m_pCenterInstance->SetCenterIndex(nCenterIdx);
	m_pCenterInstance->Connect(
		m_sCenterIP,
		m_nCenterPort
	);
}

void WvsGame::CenterAliveMonitor()
{
	if (m_pCenterInstance->CheckSocketStatus(SocketBase::SocketStatus::eConnecting))
		return;
	WvsLogger::LogRaw(WvsLogger::LEVEL_WARNING, "=================定期檢查Center Server連線程序=================\n");
	if (m_pCenterInstance && m_pCenterInstance->CheckSocketStatus(SocketBase::SocketStatus::eClosed))
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Center Server %d 尚未連線，嘗試重新連線。\n", 0);
		m_pCenterWorkThread->detach();
		*m_pCenterWorkThread = std::thread(&WvsGame::ConnectToCenter, this, 0);
	}
}

void WvsGame::SetConfigLoader(ConfigLoader * pCfg)
{
	m_pCfgLoader = pCfg;
}

void WvsGame::OnUserMigrating(int nUserID, int nSocketID)
{
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	m_mMigratingUser.insert({ nUserID, { nSocketID, GameDateTime::GetTime()} });
}

void WvsGame::RemoveMigratingUser(int nUserID)
{
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	m_mMigratingUser.erase(nUserID);
}

void WvsGame::InitializeCenter()
{
	m_nChannelID = m_pCfgLoader->IntValue("ChannelID");
	m_sCenterIP = m_pCfgLoader->StrValue("Center0_IP");
	m_nCenterPort = m_pCfgLoader->IntValue("Center0_Port");
	m_pCenterServerService = new asio::io_service();
	m_pCenterInstance = std::make_shared<Center>(*m_pCenterServerService);
	m_pCenterWorkThread = new std::thread(&WvsGame::ConnectToCenter, this, 0);
	auto holderFunc = std::bind(&WvsGame::CenterAliveMonitor, this);
	auto aliveHolder = AsyncScheduler::CreateTask(holderFunc, 10 * 1000, true);
	aliveHolder->Start();


	std::thread tCenterWorkThread([&] {
		asio::io_service::work work(*m_pCenterServerService);
		std::error_code ec;
		m_pCenterServerService->run(ec);
	});
	tCenterWorkThread.detach();
}

void WvsGame::OnUserConnected(ZSharedPtr<User> &pUser)
{
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	m_mUserMap[pUser->GetUserID()] = pUser;
	m_mUserNameMap[pUser->GetName()] = pUser;
}

void WvsGame::OnNotifySocketDisconnected(SocketBase *pSocket)
{
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	auto pClient = (ClientSocket*)pSocket;
	pClient->OnSocketDisconnected();
	if (pClient->GetUser())
	{
		m_mUserMap.erase(pClient->GetUser()->GetUserID());
		m_mUserNameMap.erase(pClient->GetUser()->GetName());
		pClient->SetUser(nullptr);
	}
}

int WvsGame::GetChannelID() const
{
	return m_nChannelID;
}

ZSharedPtr<User> WvsGame::FindUser(int nUserID)
{
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	auto findIter = m_mUserMap.find(nUserID);
	if (findIter == m_mUserMap.end())
		return nullptr;
	return findIter->second;
}

ZSharedPtr<User> WvsGame::FindUserByName(const std::string & strName)
{
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	auto findIter = m_mUserNameMap.find(strName);
	if (findIter == m_mUserNameMap.end())
		return nullptr;
	return findIter->second;
}

const std::pair<unsigned int, unsigned int>& WvsGame::GetMigratingUser(int nUserID)
{
	static std::pair<unsigned int, unsigned int> prEmpty = { 0, 0 };
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	auto findIter = m_mMigratingUser.find(nUserID);
	return findIter == m_mMigratingUser.end() ? prEmpty : findIter->second;
}

void WvsGame::ShutdownService()
{
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	for (auto& prUser : m_mUserMap)
		prUser.second->OnMigrateOut();
}
