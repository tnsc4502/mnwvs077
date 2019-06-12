#pragma once
#include "..\WvsLib\Net\WvsBase.h"
#include "..\WvsLib\Net\WorldInfo.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"

#include "Center.h"
#include <thread>
#include <string>
#include <map>

class User;
class OutPacket;

class WvsGame : public WvsBase
{
	ConfigLoader* m_pCfgLoader;

	std::recursive_mutex m_mUserLock;
	std::map<int, std::shared_ptr<User>> m_mUserMap;
	std::map<int, std::pair<unsigned int, int>> m_mMigratingUser;
	std::map<std::string, std::shared_ptr<User>> m_mUserNameMap;
	std::string m_sCenterIP;
	std::shared_ptr<Center> m_pCenterInstance;
	asio::io_service* m_pCenterServerService;
	std::thread* m_pCenterWorkThread;
	int m_nChannelID = 0, m_nCenterPort = 0;
	void WvsGame::CenterAliveMonitor();

public:
	WvsGame();
	~WvsGame();

	std::shared_ptr<Center>& GetCenter();
	std::recursive_mutex& GetUserLock();
	const std::map<int, std::shared_ptr<User>>& GetConnectedUser();
	int GetChannelID() const;

	void ConnectToCenter(int nCenterIdx);
	void WvsGame::InitializeCenter();
	void SetConfigLoader(ConfigLoader* pCfg);

	void OnUserMigrating(int nUserID, int nSocketID);
	void RemoveMigratingUser(int nUserID);
	void OnUserConnected(std::shared_ptr<User> &pUser);
	void OnNotifySocketDisconnected(SocketBase *pSocket);

	User* FindUser(int nUserID);
	User* FindUserByName(const std::string& strName);
	const std::pair<unsigned int, int>& GetMigratingUser(int nUserID);

	void ShutdownService();
};