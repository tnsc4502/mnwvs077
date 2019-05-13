#include "WvsWorld.h"
#include "UserTransferStatus.h"
#include "..\WvsGame\PartyMan.h"
#include "..\WvsGame\GuildMan.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

WvsWorld::WvsWorld()
{
}


WvsWorld::~WvsWorld()
{
}

void WvsWorld::SetConfigLoader(ConfigLoader * pCfg)
{
	m_pCfgLoader = pCfg;
}

void WvsWorld::InitializeWorld()
{
	m_WorldInfo.nEventType = m_pCfgLoader->IntValue("EventType");
	m_WorldInfo.nWorldID = m_pCfgLoader->IntValue("WorldID");
	m_WorldInfo.strEventDesc = m_pCfgLoader->StrValue("EventDesc");
	m_WorldInfo.strWorldDesc = m_pCfgLoader->StrValue("WorldDesc");
}

void WvsWorld::SetUserTransferStatus(int nUserID, UserTransferStatus* pStatus)
{
	auto deleter = [](UserTransferStatus *p) { FreeObj(p); };
	m_mUserTransferStatus[nUserID].reset(pStatus, deleter);
}

const UserTransferStatus* WvsWorld::GetUserTransferStatus(int nUserID) const
{
	auto findIter = m_mUserTransferStatus.find(nUserID);
	if (findIter == m_mUserTransferStatus.end())
		return nullptr;
	return (findIter->second.get());
}

void WvsWorld::ClearUserTransferStatus(int nUserID)
{
	m_mUserTransferStatus.erase(nUserID);
}

void WvsWorld::UserMigrateIn(int nCharacterID, int nChannelID)
{
	auto pwUser = GetUser(nCharacterID);
	if (pwUser)
	{
		pwUser->m_bMigrated = true;
		pwUser->m_nChannelID = nChannelID;

		PartyMan::GetInstance()->NotifyMigrateIn(nCharacterID, nChannelID);		
		GuildMan::GetInstance()->NotifyLoginOrLogout(nCharacterID, true);
	}
}

void WvsWorld::RemoveUser(int nUserID, int nIdx, int nLocalSocketSN, bool bMigrate)
{
	auto pwUser = GetUser(nUserID);

	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	if (pwUser)
	{
		WvsLogger::LogFormat("WvsWorld::RemoveUser[pwUser = [nIdx = %d], [nLocalSocketSN = %d]], Received: [nIdx = %d], [nLocalSocketSN = %d]\n",
			pwUser->m_nChannelID, pwUser->m_nLocalSocketSN, nIdx, nLocalSocketSN);
		PartyMan::GetInstance()->NotifyMigrateIn(
			nUserID,
			WvsWorld::CHANNELID_NOT_MIGRATED_IN
		);
		GuildMan::GetInstance()->NotifyLoginOrLogout(
			nUserID,
			false
		);
		m_mUser.erase(nUserID);
		FreeObj(pwUser);
	}
}

void WvsWorld::SetUser(int nUserID, WorldUser * pWorldUser)
{
	auto pUser = GetUser(nUserID);

	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	m_mUser[nUserID] = pWorldUser;
	if (pUser)
		FreeObj(pUser);
}

WvsWorld::WorldUser * WvsWorld::GetUser(int nUserID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);

	auto findIter = m_mUser.find(nUserID);
	return findIter == m_mUser.end() ? nullptr : findIter->second;
}

const WorldInfo & WvsWorld::GetWorldInfo() const
{
	return m_WorldInfo;
}

WvsWorld * WvsWorld::GetInstance()
{
	static WvsWorld* pInstance = new WvsWorld;
	return pInstance;
}
