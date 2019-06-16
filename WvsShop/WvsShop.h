#pragma once
#include "Center.h"
#include "..\WvsLib\Net\WvsBase.h"
#include "..\WvsLib\Memory\ZMemory.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"

class User;

class WvsShop : public WvsBase
{
	ConfigLoader* m_pCfgLoader;

	int m_nExternalPort = 0;
	int m_aExternalIP[4];

	std::shared_ptr<Center> m_pCenterInstance;
	asio::io_service* m_pCenterServerService;
	std::thread* m_pCenterWorkThread;

	std::recursive_mutex m_mUserLock;
	std::map<int, ZSharedPtr<User>> m_mUserMap;
	std::map<int, std::pair<unsigned int, unsigned int>> m_mMigratingUser;

	void ConnectToCenter();
	void CenterAliveMonitor();

public:
	WvsShop();
	~WvsShop();
	std::recursive_mutex& GetUserLock();
	std::map<int, ZSharedPtr<User>>& GetConnectedUser();
	std::shared_ptr<Center>& GetCenter();

	void SetConfigLoader(ConfigLoader* pCfg);
	void InitializeCenter(); 

	ZSharedPtr<User> FindUser(int nUserID);
	const std::pair<unsigned int, unsigned int>& GetMigratingUser(int nUserID);
	void OnUserMigrating(int nUserID, int nSocketID);
	void RemoveMigratingUser(int nUserID);
	void OnUserConnected(ZSharedPtr<User> &pUser);
	void OnNotifySocketDisconnected(SocketBase *pSocket);
	void ShutdownService();
};

