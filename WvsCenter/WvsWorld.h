#pragma once
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include "..\WvsLib\Net\WorldInfo.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"

class UserTransferStatus;
class User;
class OutPacket;
struct AuthEntry;

class WvsWorld
{
public:
	const static int 
		CHANNELID_NOT_MIGRATED_IN = -2,
		CHANNELID_SHOP = -1,
		MAX_CHANNEL_COUNT = 30;

	struct WorldUser
	{
		bool m_bMigrating = false,
			m_bTransfering = false,
			m_bLoggedIn = false,
			m_bMigrated = false,
			m_bInShop = false;

		int m_nCharacterID = 0, 
			m_nLocalSrvIdx = 0, 
			m_nChannelID = 0, 
			m_nLocalSocketSN = 0,
			m_nAccountID = 0;
		unsigned int m_tMigrateTime = 0;
		void SendPacket(OutPacket *oPacket);
	};

private:
	std::recursive_mutex m_mtxWorldLock;
	ConfigLoader* m_pCfgLoader;
	WorldInfo m_WorldInfo;

	std::map<int, WorldUser*> m_mUser;
	std::map<int, WorldUser*> m_mAccountToUser;
	std::map<int, AuthEntry*> m_mAuthEntry, m_mAccountIDToAuthEntry;
	std::map<int, std::shared_ptr<UserTransferStatus>> m_mUserTransferStatus;

public:
	WvsWorld();
	~WvsWorld();

	std::recursive_mutex& GetLock();
	void SetConfigLoader(ConfigLoader* pCfg);
	void InitializeWorld();
	const WorldInfo& GetWorldInfo() const;
	static WvsWorld* GetInstance();
	void SetUserTransferStatus(int nUserID, UserTransferStatus* pStatus);
	const UserTransferStatus* GetUserTransferStatus(int nUserID) const;
	void ClearUserTransferStatus(int nUserID);
	void UserMigrateIn(int nCharacterID, int nChannelID);
	void RemoveUser(int nUserID, int nIdx, int nLocalSocketSN, bool bMigrate);
	int RefreshLoginState(int nAccountID);
	void SendMigrationStateCheck(WorldUser *pwUser);
	void SetUser(int nUserID, WorldUser *pWorldUser);
	void SetUserTransfering(int nUserID, bool bTransfering);
	bool IsUserTransfering(int nUserID);
	WorldUser* GetUser(int nUserID);

	void InsertAuthEntry(int nAuthCharacterID, int nAuthAccountID, AuthEntry* pEntry);
	AuthEntry* GetAuthEntry(int nAuthCharacterID);
	AuthEntry* GetAuthEntryByAccountID(int nAuthAccountID);
	void RemoveAuthEntry(int nAuthCharacterID);
};

