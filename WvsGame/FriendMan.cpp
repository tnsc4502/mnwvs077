#include "FriendMan.h"
#include "WvsGame.h"
#include "User.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\GameSrvPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\Database\GW_Friend.h"

#ifdef _WVSCENTER
#include "..\Database\CharacterDBAccessor.h"
#include "..\WvsCenter\WvsWorld.h"
#include "..\WvsCenter\WvsCenter.h"
#endif

FriendMan::FriendMan()
{
}

FriendMan::~FriendMan()
{
}

FriendMan *FriendMan::GetInstance()
{
	static FriendMan* pInstance = new FriendMan;
	return pInstance;
}

#ifdef _WVSGAME
void FriendMan::OnPacket(InPacket * iPacket)
{
	int nResult = iPacket->Decode1();
	switch (nResult)
	{
		case FriendMan::FriendResult::res_Friend_Invite:
			OnInviteRequest(iPacket);
			break;
		case FriendMan::FriendResult::res_Friend_Set:
			OnSetFriendDone(iPacket);
			break;
		case FriendMan::FriendResult::res_Friend_Load:
			OnLoadFriendDone(iPacket);
			break;
		case FriendMan::FriendResult::res_Friend_Notify:
			OnNotify(iPacket);
			break;
	}
}

void FriendMan::OnInviteRequest(InPacket * iPacket)
{
	int nCharacterID = iPacket->Decode4();
	int nRequestFrom = iPacket->Decode4();
	std::string strNameFrom = iPacket->DecodeStr();

	auto pUser = User::FindUser(nCharacterID);
	if (pUser)
	{
		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnFriendResult);
		oPacket.Encode1(FriendResult::res_Friend_Invite);
		oPacket.Encode4(nRequestFrom);
		oPacket.EncodeStr(strNameFrom);

		//GW_Friend::Encode
		oPacket.Encode4(nRequestFrom);
		strNameFrom.reserve(15);
		oPacket.EncodeBuffer((unsigned char*)strNameFrom.data(), 13);
		oPacket.Encode1(1);
		oPacket.Encode4(1);
		oPacket.Encode1(0);
		pUser->SendPacket(&oPacket);
	}
}

void FriendMan::OnSetFriendDone(InPacket * iPacket)
{
	int nCharacterID = iPacket->Decode4();
	int nFriendID = iPacket->Decode4();

	auto pUser = User::FindUser(nCharacterID);
	FriendEntry *pFriendEntry = nullptr;
	if (nFriendID >= 0 && 
		 pUser &&
		(pFriendEntry = GetFriendEntry(nCharacterID)))
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxFriendLock);

		auto pFriendRecord = AllocObj(GW_Friend);
		pFriendRecord->Decode(iPacket);
		pFriendEntry->aFriend.push_back(pFriendRecord);
		pFriendEntry->aInShop.push_back(0);

		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnFriendResult);
		oPacket.Encode1(FriendResult::res_Friend_Load);
		pFriendEntry->Encode(&oPacket);
		pUser->SendPacket(&oPacket);
	}
}

void FriendMan::OnLoadFriendDone(InPacket *iPacket)
{
	int nCharacterID = iPacket->Decode4();
	auto pUser = User::FindUser(nCharacterID);
	if (pUser)
	{
		auto pEntry = GetFriendEntry(nCharacterID);
		if (!pEntry) 
		{
			pEntry = AllocObj(FriendEntry);
			m_mFriend[nCharacterID] = pEntry;
		}
		pEntry->Decode(iPacket);
		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnFriendResult);
		oPacket.Encode1(FriendResult::res_Friend_Load);
		pEntry->Encode(&oPacket);
		pUser->SendPacket(&oPacket);
	}
}

void FriendMan::OnNotify(InPacket * iPacket)
{
	int nCharacterID = iPacket->Decode4();
	auto pUser = User::FindUser(nCharacterID);
	auto pEntry = GetFriendEntry(nCharacterID);

	if (pUser && pEntry)
	{
		int nFriendID = iPacket->Decode4();
		bool bShop = iPacket->Decode1() == 1;
		int nChannelID = iPacket->Decode4();
		int nIdx = pEntry->FindIndex(nFriendID);
		if (nIdx >= 0)
		{
			pEntry->aFriend[nIdx]->nChannelID = nChannelID;
			pEntry->aInShop[nIdx] = (bShop ? 1 : 0);
			OutPacket oPacket;
			oPacket.Encode2(UserSendPacketFlag::UserLocal_OnFriendResult);
			oPacket.Encode1(FriendResult::res_Friend_Notify);
			oPacket.Encode4(nFriendID);
			oPacket.Encode1(bShop ? 1 : 0);
			oPacket.Encode4(nChannelID);

			pUser->SendPacket(&oPacket);
		}
	}
}

void FriendMan::OnFriendRequest(User *pUser, InPacket *iPacket)
{
	int nRequest = iPacket->Decode1();
	switch (nRequest)
	{
		case FriendMan::FriendRequest::rq_Friend_Set:
			OnSetFriendRequest(pUser, iPacket);
			break;
		case FriendMan::FriendRequest::rq_Friend_Accept:
			OnAcceptFriendRequest(pUser, iPacket);
			break;
		case FriendMan::FriendRequest::rq_Friend_Delete:
			OnDeleteFriendRequest(pUser, iPacket);
			break;
	}
}

void FriendMan::OnDeleteFriendRequest(User * pUser, InPacket * iPacket)
{
	int nFriendID = iPacket->Decode4();
	auto pEntry = GetFriendEntry(pUser->GetUserID());

	//Rejection and deletion share the same request code, no checking here.
	OutPacket oPacket;
	oPacket.Encode2(GameSrvSendPacketFlag::FriendRequest);
	oPacket.Encode1(FriendRequest::rq_Friend_Delete);
	oPacket.Encode4(pUser->GetUserID());
	oPacket.Encode4(nFriendID);

	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

void FriendMan::OnSetFriendRequest(User *pUser, InPacket *iPacket)
{
	auto pFriendEntry = GetFriendEntry(pUser->GetUserID());
	std::string sFriendName = iPacket->DecodeStr();
	if (pFriendEntry)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxFriendLock);
		for (auto& pFriendRecord : pFriendEntry->aFriend)
			if (pFriendRecord->sFriendName == sFriendName)
				return;
	}
	OutPacket oPacket;
	oPacket.Encode2(GameSrvSendPacketFlag::FriendRequest);
	oPacket.Encode1(FriendMan::FriendRequest::rq_Friend_Set);
	oPacket.Encode4(pUser->GetUserID());
	oPacket.EncodeStr(pUser->GetName());
	oPacket.EncodeStr(sFriendName);

	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

void FriendMan::OnLeave(User * pUser)
{
	RemoveFriendEntry(pUser->GetUserID());
}

void FriendMan::OnAcceptFriendRequest(User *pUser, InPacket *iPacket)
{
	int nFriendID = iPacket->Decode4();
	auto pEntry = GetFriendEntry(pUser->GetUserID());
	if (pEntry && pEntry->FindIndex(nFriendID) == -1)
	{
		OutPacket oPacket;
		oPacket.Encode2(GameSrvSendPacketFlag::FriendRequest);
		oPacket.Encode1(FriendRequest::rq_Friend_Accept);
		oPacket.Encode4(pUser->GetUserID());
		oPacket.Encode4(nFriendID);

		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	}
}
#endif

FriendMan::FriendEntry *FriendMan::GetFriendEntry(int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFriendLock);

	auto findIter = m_mFriend.find(nCharacterID);
	return findIter != m_mFriend.end() ? findIter->second : nullptr;
}

void FriendMan::InsertFriendInvitationRequest(int nCharacterID, int nRequestFrom, const std::string& strNameFrom)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFriendLock);
	m_mFriendRequest[nCharacterID].push_back({ nRequestFrom, strNameFrom });
}

const std::vector<std::pair<int, std::string>>& FriendMan::GetFriendInvitationRequest(int nCharacterID)
{
	static std::vector<std::pair<int, std::string>> aEmpty(0);

	std::lock_guard<std::recursive_mutex> lock(m_mtxFriendLock);
	auto findIter = m_mFriendRequest.find(nCharacterID);
	return findIter == m_mFriendRequest.end() ? aEmpty : findIter->second;
}

const std::pair<int, std::string>& FriendMan::GetInvitationRequest(int nCharacterID, int nFriendID)
{
	static std::pair<int, std::string> pEmpty{ -1, "" };

	auto &aInvitation = GetFriendInvitationRequest(nCharacterID);
	std::lock_guard<std::recursive_mutex> lock(m_mtxFriendLock);

	int nCount = (int)aInvitation.size();
	for (const auto& pr : aInvitation)
		if (pr.first == nFriendID)
			return pr;
	return pEmpty;
}

void FriendMan::RemoveFriendInvitationRequest(int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFriendLock);
	m_mFriendRequest.clear();
}

void FriendMan::RemoveFriendInvitationRequest(int nCharacterID, int nFriendID)
{
	static std::vector<std::pair<int, std::string>> aEmpty(0);

	std::lock_guard<std::recursive_mutex> lock(m_mtxFriendLock);
	auto findIter = m_mFriendRequest.find(nCharacterID);
	if (findIter == m_mFriendRequest.end())
		return;

	int nCount = (int)findIter->second.size();
	for (int i = 0; i < nCount; ++i)
		if (findIter->second[i].first == nFriendID) 
		{
			findIter->second.erase(findIter->second.begin() + i);
			if (findIter->second.size() == 0)
				m_mFriendRequest.erase(findIter);
			return;
		}
}

#ifdef _WVSCENTER

void FriendMan::Notify(int nCharacterID, int nFriendID, int nChannelID, bool bShop)
{
	WvsWorld::WorldUser *pwUser = nullptr;
	auto pEntry = GetFriendEntry(nFriendID);
	if ((pwUser = WvsWorld::GetInstance()->GetUser(nFriendID)) &&
		pEntry) //Both are friends.
	{
		//Update nChannelID in friend's entries.
		int nIdx = pEntry->FindIndex(nCharacterID);
		auto pSrv = WvsBase::GetInstance<WvsCenter>()->GetChannel(pwUser->m_nChannelID);
		if (nIdx != -1)
		{
			std::lock_guard<std::recursive_mutex> lock(pEntry->mtxEntryLock);
			pEntry->aFriend[nIdx]->nChannelID = nChannelID;
			pEntry->aInShop[nIdx] = (bShop ? 1 : 0);
		}

		//Send notification packet.
		if (nIdx != -1 && pSrv)
		{
			OutPacket oPacket;
			oPacket.Encode2(CenterSendPacketFlag::FriendResult);
			oPacket.Encode1(FriendResult::res_Friend_Notify);
			oPacket.Encode4(nFriendID);
			oPacket.Encode4(nCharacterID);
			oPacket.Encode1(bShop ? 1 : 0);
			oPacket.Encode4(nChannelID);
			pSrv->GetLocalSocket()->SendPacket(&oPacket);
		}
	}
}

void FriendMan::NotifyLogin(int nCharacterID, int nChannelID, bool bShop)
{
	auto pEntry = GetFriendEntry(nCharacterID);
	if (!pEntry)
		pEntry = LoadFriendEntry(nCharacterID);
	if (pEntry)
	{
		std::lock_guard<std::recursive_mutex> lock(pEntry->mtxEntryLock);
		WvsWorld::WorldUser *pwUser = nullptr;
		LocalServerEntry *pSrv = nullptr;
		int nIdx = -1;

		for (auto pFriend : pEntry->aFriend)
			Notify(nCharacterID, pFriend->nFriendID, nChannelID, bShop);

		if (nChannelID >= 0)
		{
			auto& aInvitation = GetFriendInvitationRequest(nCharacterID);
			for (auto& pr : aInvitation)
				NotifyFriendRequest(nCharacterID, pr.first, pr.second);
		}
	}
}

void FriendMan::NotifyLogout(int nCharacterID)
{
	NotifyLogin(nCharacterID, WvsWorld::CHANNELID_NOT_MIGRATED_IN, false);
	RemoveFriendEntry(nCharacterID);
	RemoveFriendInvitationRequest(nCharacterID);
}

void FriendMan::NotifyFriendRequest(int nCharacterID, int nRequestFrom, const std::string& strNameFrom)
{
	auto pwUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
	LocalServerEntry* pSrv = nullptr;
	if (pwUser &&
		(pSrv = WvsBase::GetInstance<WvsCenter>()->GetChannel(pwUser->m_nChannelID)))
	{
		OutPacket oPacket;
		oPacket.Encode2(CenterSendPacketFlag::FriendResult);
		oPacket.Encode1(FriendResult::res_Friend_Invite);
		oPacket.Encode4(nCharacterID);
		oPacket.Encode4(nRequestFrom);
		oPacket.EncodeStr(strNameFrom);
		pSrv->GetLocalSocket()->SendPacket(&oPacket);
	}
}

void FriendMan::SetFriend(InPacket *iPacket, OutPacket *oPacket)
{
	int nCharacterID = iPacket->Decode4();
	std::string strCharName = iPacket->DecodeStr();
	auto pFriendEntry = GetFriendEntry(nCharacterID);
	std::string strFriendName = iPacket->DecodeStr();

	oPacket->Encode2(CenterSendPacketFlag::FriendResult);
	oPacket->Encode1(FriendResult::res_Friend_Set);
	oPacket->Encode4(nCharacterID);

	std::lock_guard<std::recursive_mutex> lock(m_mtxFriendLock);
	if (pFriendEntry)
	{
		for (auto& pFriendRecord : pFriendEntry->aFriend)
			if (pFriendRecord->sFriendName == strFriendName)
			{
				oPacket->Encode4(FriendStatus::s_AlreadyInFriendList); //Already in friend list.
				return;
			}
	}
	int nFriendID = CharacterDBAccessor::GetInstance()->QueryCharacterIDByName(strFriendName);
	if (nFriendID != -1)
	{
		if ((int)pFriendEntry->aFriend.size() == pFriendEntry->nFriendMax)
		{
			oPacket->Encode4(FriendStatus::s_FriendListIsFull);
			return;
		}
		auto pwUser = WvsWorld::GetInstance()->GetUser(nFriendID);
		if (pwUser)
			NotifyFriendRequest(nFriendID, nCharacterID, strCharName);

		InsertFriendInvitationRequest(nFriendID, nCharacterID, strCharName);
		oPacket->Encode4(nFriendID);
		auto pFriendRecord = AllocObj(GW_Friend);
		pFriendRecord->nCharacterID = nCharacterID;
		pFriendRecord->sFriendName = strFriendName;
		pFriendRecord->nFriendID = nFriendID;
		pFriendRecord->nChannelID = WvsWorld::CHANNELID_NOT_MIGRATED_IN;
		pFriendRecord->nFlag = 0;
		pFriendRecord->Save();
		pFriendEntry->aFriend.push_back(pFriendRecord);
		pFriendEntry->aInShop.push_back(0);

		pFriendRecord->Encode(oPacket);
	}
	else
		oPacket->Encode4(FriendStatus::s_FriendNotExists);
}

void FriendMan::LoadFriend(InPacket * iPacket, OutPacket * oPacket)
{
	int nCharacterID = iPacket->Decode4();
	auto pEntry = GetFriendEntry(nCharacterID);
	if (pEntry)
		pEntry = LoadFriendEntry(nCharacterID);
	oPacket->Encode2(CenterSendPacketFlag::FriendResult);
	oPacket->Encode1(FriendResult::res_Friend_Load);
	oPacket->Encode4(nCharacterID);
	pEntry->Encode(oPacket);
}

void FriendMan::AcceptFriend(InPacket * iPacket, OutPacket * oPacket)
{
	int nCharacterID = iPacket->Decode4();
	int nFriendID = iPacket->Decode4();

	auto prInvitation = GetInvitationRequest(nCharacterID, nFriendID);
	auto pEntry = GetFriendEntry(nCharacterID);

	oPacket->Encode2(CenterSendPacketFlag::FriendResult);
	oPacket->Encode1(FriendResult::res_Friend_Set);
	oPacket->Encode4(nCharacterID);
	if (pEntry &&
		pEntry->FindIndex(nFriendID) == -1 &&
		prInvitation.first != -1)
	{
		auto pwUser = WvsWorld::GetInstance()->GetUser(nFriendID);
		auto pFriendRecord = AllocObj(GW_Friend);
		pFriendRecord->nCharacterID = nCharacterID;
		pFriendRecord->sFriendName = prInvitation.second;
		pFriendRecord->nFriendID = prInvitation.first;
		pFriendRecord->nChannelID =
			pwUser == nullptr ?
			WvsWorld::CHANNELID_NOT_MIGRATED_IN : pwUser->m_nChannelID;

		pFriendRecord->nFlag = 0;
		pFriendRecord->Save();
		pEntry->aFriend.push_back(pFriendRecord);
		pEntry->aInShop.push_back(pFriendRecord->nChannelID == WvsWorld::CHANNELID_SHOP);

		oPacket->Encode4(nFriendID);
		pFriendRecord->Encode(oPacket);
		RemoveFriendInvitationRequest(nCharacterID, nFriendID);

		pwUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
		if(pwUser)
			Notify(
				nCharacterID,
				nFriendID,
				pwUser->m_nChannelID, 
				pwUser->m_nChannelID == WvsWorld::CHANNELID_SHOP);
	}
	else
		oPacket->Encode4(FriendStatus::s_FriendNotExists);
}

void FriendMan::DeleteFriend(InPacket * iPacket, OutPacket * oPacket)
{
	int nCharacterID = iPacket->Decode4();
	int nFriendID = iPacket->Decode4();
	int nIdx = -1;

	RemoveFriendInvitationRequest(nCharacterID, nFriendID);
	auto pEntry = GetFriendEntry(nCharacterID);

	if (pEntry && RemoveFriend(pEntry, nFriendID))
	{
		oPacket->Encode2(CenterSendPacketFlag::FriendResult);
		oPacket->Encode1(FriendResult::res_Friend_Load);
		oPacket->Encode4(nCharacterID);
		pEntry->Encode(oPacket);
		Notify(nCharacterID, nFriendID, WvsWorld::CHANNELID_NOT_MIGRATED_IN, false);
	}
}

FriendMan::FriendEntry* FriendMan::LoadFriendEntry(int nCharacterID)
{
	auto pFriendEntry = GetFriendEntry(nCharacterID);
	if (pFriendEntry)
		return pFriendEntry;

	FriendEntry *pEntry = AllocObj(FriendEntry);
	pEntry->aFriend = GW_Friend::LoadAll(nCharacterID);
	pEntry->aInShop.resize(pEntry->aFriend.size(), 0);
	pEntry->nFriendMax = CharacterDBAccessor::GetInstance()->QueryCharacterFriendMax(nCharacterID);

	std::lock_guard<std::recursive_mutex> lock(m_mtxFriendLock);
	std::lock_guard<std::recursive_mutex> entryLock(pEntry->mtxEntryLock);
	m_mFriend[nCharacterID] = pEntry;

	//Update channel id of friends.
	WvsWorld::WorldUser *pwUser = nullptr;
	GW_Friend *pFriend = nullptr;
	int nCount = (int)pEntry->aFriend.size();
	for (int i = 0; i < nCount; ++i)
	{
		pFriend = pEntry->aFriend[i];
		auto pEntry_Friend = GetFriendEntry(pFriend->nCharacterID);
		if ((pwUser = WvsWorld::GetInstance()->GetUser(pFriend->nFriendID)) &&
			pEntry_Friend &&
			pEntry_Friend->FindIndex(nCharacterID) >= 0)
		{
			pFriend->nChannelID = pwUser->m_nChannelID;
			pEntry->aInShop[i] = (pFriend->nChannelID == WvsWorld::CHANNELID_SHOP);
		}
	}
	return pEntry;
}
#endif

void FriendMan::RemoveFriendEntry(int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFriendLock);
	auto pFriendEntry = GetFriendEntry(nCharacterID);
	for (auto& pFriend : pFriendEntry->aFriend)
		FreeObj(pFriend);
	m_mFriend.erase(nCharacterID);
	FreeObj(pFriendEntry);
}

bool FriendMan::RemoveFriend(FriendEntry * pEntry, int nFriendID)
{
	std::lock_guard<std::recursive_mutex> lock(pEntry->mtxEntryLock);
	int nIdx = pEntry->FindIndex(nFriendID);
	if (nIdx == -1)
		return false;
#ifdef _WVSCENTER
	pEntry->aFriend[nIdx]->Delete();
#endif
	pEntry->aFriend.erase(pEntry->aFriend.begin() + nIdx);
	pEntry->aInShop.erase(pEntry->aInShop.begin() + nIdx);
	return true;
}

int FriendMan::FriendEntry::FindIndex(int nFriendID, int nFlag)
{
	std::lock_guard<std::recursive_mutex> lock(mtxEntryLock);

	int nFriendCount = (int)aFriend.size();
	for (int i = 0; i < nFriendCount; ++i)
		if (aFriend[i]->nFriendID == nFriendID &&
			(nFlag < 0 || aFriend[i]->nFlag == nFlag))
			return i;
	return -1;
}

void FriendMan::FriendEntry::Encode(OutPacket *oPacket)
{
	std::lock_guard<std::recursive_mutex> lock(mtxEntryLock);

	oPacket->Encode1((char)aFriend.size());
	for (auto& pFriend : aFriend)
		pFriend->Encode(oPacket);
	for (auto& bInShop : aInShop)
		oPacket->Encode4(bInShop);
}

void FriendMan::FriendEntry::Decode(InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(mtxEntryLock);
	int nCount = iPacket->Decode1();
	for (auto& pFriend : aFriend)
		FreeObj(pFriend);

	GW_Friend* pFriend = nullptr;

	aFriend.resize(nCount);
	for (int i = 0; i < nCount; ++i)
	{
		pFriend = AllocObj(GW_Friend);
		pFriend->Decode(iPacket);
		aFriend[i] = pFriend;
	}

	aInShop.resize(nCount);
	for (int i = 0; i < nCount; ++i)
		aInShop[i] = iPacket->Decode4();
}

