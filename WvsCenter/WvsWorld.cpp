#include "WvsWorld.h"
#include "WvsCenter.h"
#include "AuthEntry.h"
#include "UserTransferStatus.h"
#include "..\WvsGame\PartyMan.h"
#include "..\WvsGame\GuildMan.h"
#include "..\WvsGame\FriendMan.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsCenter\CenterPacketTypes.hpp"
#include "..\WvsLogin\LoginPacketTypes.hpp"
#include "..\WvsLib\Task\AsyncScheduler.h"

WvsWorld::WvsWorld()
{
	m_pWorldTimer = AsyncScheduler::CreateTask(
		std::bind(&WvsWorld::Update, this), 30 * 1000, true
	);
	m_pWorldTimer->Start();
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
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	auto pwUser = GetUser(nUserID);
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

		OutPacket oPacket;
		oPacket.Encode2(CenterResultPacketType::LoginAuthResult);
		oPacket.Encode1(LoginAuthResult::res_LoginAuth_UnRegisterMigratinon);
		oPacket.Encode4(pwUser->m_nAccountID);

		//Remove User Entry
		m_mUser.erase(nUserID);
		m_mAccountToUser.erase(pwUser->m_nAccountID);

		//Remove Auth Entry
		RemoveAuthEntry(nUserID);
		FreeObj(pwUser);
		WvsBase::GetInstance<WvsCenter>()->GetLoginServer()->GetLocalSocket()->SendPacket(&oPacket);
	}
}

int WvsWorld::RefreshLoginState(int nAccountID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	if (GetAuthEntryByAccountID(nAccountID))
		return 1; //On auth.

	auto findIter = m_mAccountToUser.find(nAccountID);
	if (findIter == m_mAccountToUser.end())
		return 0; //Not migrated in.

	//Check again.
	auto pwUser = GetUser(findIter->second->m_nCharacterID);
	if (!pwUser)
	{
		m_mAccountToUser.erase(nAccountID);
		return 0;
	}
	if (GameDateTime::GetTime() - pwUser->m_tMigrateTime > 10 * 1000)
		SendMigrationStateCheck(pwUser);

	return 1; //Migrated in
}

void WvsWorld::SendMigrationStateCheck(WorldUser *pwUser)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	auto pSrvEntry = WvsBase::GetInstance<WvsCenter>()->GetChannel(pwUser->m_nChannelID);
	if (pSrvEntry)
	{
		OutPacket oPacket;
		oPacket.Encode2(CenterRequestPacketType::CheckMigrationState);
		oPacket.Encode4(pwUser->m_nCharacterID);
		pSrvEntry->GetLocalSocket()->SendPacket(&oPacket);
	}
}

void WvsWorld::SetUser(int nUserID, WorldUser* pWorldUser)
{
	auto pUser = GetUser(nUserID);
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	m_mUser[nUserID] = pWorldUser;
	m_mAccountToUser[pWorldUser->m_nAccountID] = pWorldUser;

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

bool WvsWorld::CheckEventAvailabilityForUser(const std::string& sEventName, int nUserID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	auto eventIter = m_mUserEventRecord.find(sEventName);
	if (eventIter == m_mUserEventRecord.end())
		return true;

	auto userIter = eventIter->second.find(nUserID);
	if (userIter == eventIter->second.end())
		return true;

	auto tDateTime = GameDateTime::GetCurrentDate();
	return tDateTime > userIter->second;
}

void WvsWorld::InsertNextAvailableEventTimeForUser(const std::string & sEventName, int nUserID, long long int nNextDateTime)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	m_mUserEventRecord[sEventName].insert({ nUserID, nNextDateTime });
}

void WvsWorld::ClearUserEventLog(const std::string & sEventName, bool bResetAll)
{
	unsigned int tCur = GameDateTime::GetTime();
	if (tCur < m_tNextTimeToClearEeventLog)
		return;

	m_tNextTimeToClearEeventLog = tCur + 10 * 10 * 1000; //Every 10 MINS.

	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);

	auto lmdRemoveAvailableUser = [&](std::map<int, long long int>& mUserRecord) {
		auto tDateTime = GameDateTime::GetCurrentDate();
		for (auto iterUser = mUserRecord.begin(); iterUser != mUserRecord.end();)
			if (tDateTime < iterUser->second) 
			{
				WvsLogger::LogFormat("User Event Log Timed Out! UserID = %d, NextAvailableTime = %lld\n", iterUser->first, iterUser->second);
				iterUser = mUserRecord.erase(iterUser);
			}
			else
				++iterUser;
	};

	if (sEventName == "")
	{
		if (bResetAll)
			m_mUserEventRecord.clear();
		else
			for (auto& eventMap : m_mUserEventRecord)
				lmdRemoveAvailableUser(eventMap.second);
	}
	else
	{
		auto iter = m_mUserEventRecord.find(sEventName);
		if (iter == m_mUserEventRecord.end())
			return;

		if (bResetAll)
			iter->second.clear();
		else
			lmdRemoveAvailableUser(iter->second);
	}
}

WvsWorld::WorldUser* WvsWorld::GetUser(int nUserID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);

	auto findIter = m_mUser.find(nUserID);
	return findIter == m_mUser.end() ? nullptr : findIter->second;
}

void WvsWorld::InsertAuthEntry(int nAuthCharacterID, int nAuthAccountID, AuthEntry* pEntry)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	m_mAuthEntry.insert({ nAuthCharacterID, pEntry });
	m_mAccountIDToAuthEntry.insert({ nAuthAccountID, pEntry });
}

AuthEntry* WvsWorld::GetAuthEntry(int nAuthCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	auto findIter = m_mAuthEntry.find(nAuthCharacterID);
	return findIter == m_mAuthEntry.end() ? nullptr : findIter->second;
}

AuthEntry* WvsWorld::GetAuthEntryByAccountID(int nAuthAccountID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	auto findIter = m_mAccountIDToAuthEntry.find(nAuthAccountID);
	return findIter == m_mAccountIDToAuthEntry.end() ? nullptr : findIter->second;
}

void WvsWorld::RemoveAuthEntry(int nAuthCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	auto pEntry = GetAuthEntry(nAuthCharacterID);
	if (pEntry)
	{
		m_mAccountIDToAuthEntry.erase(pEntry->nAccountID);
		m_mAuthEntry.erase(nAuthCharacterID);
		FreeObj(pEntry);
	}
}

void WvsWorld::EncodeAuthEntry(OutPacket * oPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);
	oPacket->Encode1(GetWorldInfo().nWorldID);
	oPacket->Encode4((int)m_mAuthEntry.size());
	for (auto& prEntry : m_mAuthEntry)
		oPacket->Encode4(prEntry.second->nAccountID);
}

void WvsWorld::Update()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxWorldLock);

	ClearUserEventLog("");
}

const WorldInfo& WvsWorld::GetWorldInfo() const
{
	return m_WorldInfo;
}

void WvsWorld::EncodeWorldInfo(OutPacket * oPacket)
{
	oPacket->Encode1(GetWorldInfo().nWorldID);
	oPacket->Encode1(GetWorldInfo().nEventType);
	oPacket->EncodeStr(GetWorldInfo().strWorldDesc);
	oPacket->EncodeStr(GetWorldInfo().strEventDesc);
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
