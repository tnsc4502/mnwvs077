#include "WvsWorld.h"
#include "WvsCenter.h"
#include "UserTransferStatus.h"
#include "..\WvsGame\PartyMan.h"
#include "..\WvsGame\GuildMan.h"
#include "..\WvsGame\FriendMan.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

#include "..\WvsLib\Net\OutPacket.h"

WvsWorld::WvsWorld()
{
}


WvsWorld::~WvsWorld()
{
}

std::recursive_mutex & WvsWorld::GetLock()
{
	return m_mtxWorldLock;
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
		FriendMan::GetInstance()->NotifyLogin(nCharacterID, nChannelID, (nChannelID == WvsWorld::CHANNELID_SHOP));
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
		FriendMan::GetInstance()->NotifyLogout(nUserID);
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

void WvsWorld::SetUserTransfering(int nUserID, bool bTransfering)
{
	auto pwUser = GetUser(nUserID);
	if (pwUser)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
		pwUser->m_bTransfering = bTransfering;
	}
}

bool WvsWorld::IsUserTransfering(int nUserID)
{
	auto pwUser = GetUser(nUserID);

	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	return pwUser && pwUser->m_bTransfering;
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

WvsWorld *WvsWorld::GetInstance()
{
	static WvsWorld* pInstance = new WvsWorld;
	return pInstance;
}

void WvsWorld::WorldUser::SendPacket(OutPacket *oPacket)
{
	auto pSrv = WvsBase::GetInstance<WvsCenter>()->GetChannel(m_nChannelID);
	if (pSrv)
		pSrv->GetLocalSocket()->SendPacket(oPacket);
}
