#include "PartyMan.h"
#include "User.h"
#include "Center.h"
#include "WvsGame.h"
#include "QWUser.h"
#include "Field.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\PacketFlags\GamePacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsCenter\WvsWorld.h"

#ifdef _WVSCENTER
#include "..\WvsCenter\WvsCenter.h"
#endif

#define ENCODE_MEMBER_INFO(pUser)\
	oPacket.Encode4(pUser->GetUserID());\
	oPacket.Encode4(pUser->GetChannelID());\
	oPacket.EncodeStr(pUser->GetName());\
	oPacket.Encode2(QWUser::GetJob(pUser));\
	oPacket.Encode2(QWUser::GetLevel(pUser));\
	oPacket.Encode4(pUser->GetField()->GetFieldID());

int PartyMan::FindUser(int nUserID, PartyData * pParty)
{
	if (pParty)
	{
		for (int i = 0; i < MAX_PARTY_MEMBER_COUNT; ++i)
			if (pParty->party.anCharacterID[i] == nUserID)
				return i;
	}
	return -1;
}

PartyMan::PartyMan()
{
	m_atiPartyIDCounter = 1;
}

PartyMan::~PartyMan()
{
}

#ifdef _WVSGAME
void PartyMan::OnPartyRequest(User *pUser, InPacket * iPacket)
{
	int nRequest = iPacket->Decode1();
	switch (nRequest)
	{
		case PartyRequest::rq_Party_Create:
		{
			OutPacket oPacket;
			oPacket.Encode2(GameSendPacketFlag::PartyRequest);
			oPacket.Encode1(PartyRequest::rq_Party_Create);
			ENCODE_MEMBER_INFO(pUser);
			WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
			break;
		}
		case PartyRequest::rq_Party_Invite:
		{
			OutPacket oPacket;
			int nPartyID = GetPartyIDByCharID(pUser->GetUserID());
			if (nPartyID > 0 && IsPartyBoss(nPartyID, pUser->GetUserID()))
			{
				std::string strToInvite = iPacket->DecodeStr();
				auto pToInvite = User::FindUserByName(strToInvite);
				if (pToInvite)
				{
					if (GetPartyIDByCharID(pToInvite->GetUserID()) > 0)
					{
						oPacket.Encode2(UserSendPacketFlag::UserLocal_OnPartyResult);
						oPacket.Encode1(PartyResult::res_Party_Invite_Failed_AlreadyInParty);
						pUser->SendPacket(&oPacket);
					}
					else
					{
						oPacket.Encode2(UserSendPacketFlag::UserLocal_OnPartyResult);
						oPacket.Encode1(PartyRequest::rq_Party_Invite);
						oPacket.Encode4(pUser->GetUserID());
						oPacket.EncodeStr(pUser->GetName());
						oPacket.Encode4(QWUser::GetLevel(pUser));
						oPacket.Encode4(QWUser::GetJob(pUser));
						oPacket.Encode1(0);

						pToInvite->SendPacket(&oPacket);
						pToInvite->AddPartyInvitedCharacterID(pUser->GetUserID());
					}
				}
			}
			break;
		}
		case PartyRequest::rq_Party_Join:
		{
			OutPacket oPacket;
			int nPartyBossID = iPacket->Decode4();
			auto pBoss = User::FindUser(nPartyBossID);
			if (!pBoss || GetPartyIDByCharID(pUser->GetUserID()) > 0)
			{
				oPacket.Encode2(UserSendPacketFlag::UserLocal_OnPartyResult);
				oPacket.Encode1(PartyResult::res_Party_Invite_Failed_UnableToProcess);
				pUser->SendPacket(&oPacket);
				return;
			}
			int nPartyID = GetPartyIDByCharID(nPartyBossID);
			if (nPartyID > 0 && 
				IsPartyBoss(nPartyID, nPartyBossID) &&
				pUser->IsPartyInvitedCharacterID(nPartyBossID))
			{
				oPacket.Encode2(GameSendPacketFlag::PartyRequest);
				oPacket.Encode1(PartyRequest::rq_Party_Join);
				oPacket.Encode4(nPartyID);
				oPacket.Encode4(nPartyBossID);
				ENCODE_MEMBER_INFO(pUser);
				WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
				pUser->RemovePartyInvitedCharacterID(nPartyBossID);
			}
			break;
		}
	}
}

void PartyMan::OnPacket(InPacket *iPacket)
{
	int nResult = iPacket->Decode1();
	switch (nResult)
	{
		case PartyResult::res_Party_Create:
			OnCreateNewPartyDone(iPacket);
			break;
		case PartyResult::res_Party_Load:
			OnLoadPartyDone(iPacket);
			break;
		case PartyResult::res_Party_Join:
			OnJoinPartyDone(iPacket);
			break;
		case PartyResult::res_Party_MigrateIn:
			OnUserMigration(iPacket);
			break;
	}
}

void PartyMan::OnCreateNewPartyDone(InPacket *iPacket)
{
	int nCharacterID = iPacket->Decode4();
	int nPartyID = iPacket->Decode4();

	std::lock_guard<std::recursive_mutex> lock(m_mtxPartyLock);
	if (nPartyID != -1)
	{
		auto pUser = User::FindUser(nCharacterID);
		if (pUser)
		{
			if (GetPartyByCharID(nCharacterID) == nullptr)
			{
				PartyData *pParty = AllocObj(PartyData);
				for (int i = 0; i < MAX_PARTY_MEMBER_COUNT; ++i)
					pParty->party.Initialize(i);

				pParty->nPartyID = nPartyID;
				pParty->party.nPartyBossCharacterID = nCharacterID;
				pParty->party.anChannelID[0] = pUser->GetChannelID();
				pParty->party.anJob[0] = QWUser::GetJob(pUser);
				pParty->party.anLevel[0] = QWUser::GetLevel(pUser);
				pParty->party.asCharacterName[0] = pUser->GetName();
				pParty->party.anFieldID[0] = pUser->GetField()->GetFieldID();
				pParty->party.anCharacterID[0] = nCharacterID;

				m_mParty[nPartyID] = pParty;
				m_mCharacterIDToPartyID[nCharacterID] = nPartyID;

				OutPacket oPacket;
				oPacket.Encode2(UserSendPacketFlag::UserLocal_OnPartyResult);
				oPacket.Encode1(PartyResult::res_Party_Create);
				oPacket.Encode4(nPartyID);
				oPacket.Encode4(999999999);
				oPacket.Encode4(999999999);
				oPacket.Encode8(0);

				pUser->SendPacket(&oPacket);
			}
		}
	}
}

void PartyMan::OnLoadPartyDone(InPacket * iPacket)
{
	int nPartyID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();

	std::lock_guard<std::recursive_mutex> lock(m_mtxPartyLock);
	auto pParty = GetParty(nPartyID);
	auto pUser = User::FindUser(nCharacterID);
	auto iterUserInParty = m_mCharacterIDToPartyID.find(nCharacterID);
	if (iterUserInParty != m_mCharacterIDToPartyID.end())
	{
		NotifyTransferField(nCharacterID, pUser->GetField()->GetFieldID());
		return;
	}
	else if (!pUser)
		return;
	
	pParty = AllocObj(PartyData);
	for (int i = 0; i < MAX_PARTY_MEMBER_COUNT; ++i)
		pParty->party.Initialize(i);

	pParty->nPartyID = nPartyID;
	pParty->Decode(iPacket);
	for (int i = 0; i < MAX_PARTY_MEMBER_COUNT; ++i)
		if (pParty->party.anChannelID[i] != WvsBase::GetInstance<WvsGame>()->GetChannelID())
			pParty->party.anFieldID[i] = 999999999;

	m_mParty[nPartyID] = pParty;
	m_mCharacterIDToPartyID[nCharacterID] = nPartyID;
	NotifyTransferField(nCharacterID, pUser->GetField()->GetFieldID());
}

void PartyMan::OnJoinPartyDone(InPacket * iPacket)
{
	int nCharacterID = iPacket->Decode4();
	char nIdx = iPacket->Decode1();
	auto pUser = User::FindUser(nCharacterID);
	if (!pUser)
		return;

	if (nIdx == -1)
	{
	JOIN_FAILED:
		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnPartyResult);
		oPacket.Encode1(PartyResult::res_Party_Invite_Failed_UnableToProcess);
		pUser->SendPacket(&oPacket);
		return;
	}

	int nPartyID = iPacket->Decode4();
	auto pParty = GetParty(nPartyID);
	if (!pParty)
		goto JOIN_FAILED;

	std::lock_guard<std::recursive_mutex> lock(m_mtxPartyLock);
	m_mCharacterIDToPartyID[nCharacterID] = nPartyID;
	pParty->party.anChannelID[nIdx] = pUser->GetChannelID();
	pParty->party.anCharacterID[nIdx] = pUser->GetUserID();
	pParty->party.asCharacterName[nIdx] = pUser->GetName();
	pParty->party.anLevel[nIdx] = QWUser::GetLevel(pUser);
	pParty->party.anJob[nIdx] = QWUser::GetJob(pUser);
	pParty->party.anFieldID[nIdx] = pUser->GetField()->GetFieldID();

	//NotifyTransferField(pUser->GetUserID(), pUser->GetField()->GetFieldID());

	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnPartyResult);
	oPacket.Encode1(PartyResult::res_Party_Join);
	oPacket.Encode4(nPartyID);
	oPacket.EncodeStr(pUser->GetName());
	pParty->Encode(&oPacket);
	Broadcast(&oPacket, pParty->party.anCharacterID, 0);
}

void PartyMan::OnLeave(User * pUser, bool bMigrate)
{
	if (!bMigrate)
	{
		int nCharacterID = pUser->GetUserID();
		auto pParty = GetPartyByCharID(nCharacterID);

		std::lock_guard<std::recursive_mutex> lock(m_mtxPartyLock);
		if (pParty)
		{
			int nIdx = FindUser(nCharacterID, pParty);
			if (nIdx != -1)
			{
				if (pParty->party.anCharacterID[nIdx] == pParty->party.nPartyBossCharacterID)
				{
					for (int i = 0; i < MAX_PARTY_MEMBER_COUNT; ++i)
						if (i == nIdx)
							continue;
						else if (pParty->party.anChannelID[i] >= 0)
						{
							//New Party Boss
						}
				}
			}
			m_mCharacterIDToPartyID.erase(nCharacterID);
			NotifyTransferField(nCharacterID, 999999999);
		}
	}
}

void PartyMan::OnUserMigration(InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxPartyLock);
	int nPartyID = iPacket->Decode4();
	auto pParty = GetParty(nPartyID);
	if (pParty)
	{
		int nIdx = iPacket->Decode1();
		int nChannel = iPacket->Decode4();
		pParty->party.anChannelID[nIdx] = nChannel;
		if (nChannel != WvsBase::GetInstance<WvsGame>()->GetChannelID())
			pParty->party.anFieldID[nIdx] = 999999999;

		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketFlag::UserLocal_OnPartyResult);
		oPacket.Encode1(PartyResult::res_Party_Update);
		oPacket.Encode4(pParty->nPartyID);
		pParty->Encode(&oPacket);
		Broadcast(&oPacket, pParty->party.anCharacterID, 0);
	}
}

void PartyMan::NotifyTransferField(int nCharacterID, int nFieldID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxPartyLock);
	auto pParty = GetPartyByCharID(nCharacterID);
	if (pParty)
	{
		int nIdx = FindUser(nCharacterID, pParty);
		if (nIdx >= 0)
		{
			pParty->party.anFieldID[nIdx] = nFieldID;
			if (nFieldID == 999999999)
			{
				pParty->party.aTownPortal[nIdx].m_TownPortalFieldID = 999999999;
				pParty->party.aTownPortal[nIdx].m_TownPortalReturnFieldID = 999999999;
				pParty->party.aTownPortal[nIdx].m_nFieldPortalX = -1;
				pParty->party.aTownPortal[nIdx].m_nFieldPortalY = -1;
			}
			OutPacket oPacket;
			oPacket.Encode2(UserSendPacketFlag::UserLocal_OnPartyResult);
			oPacket.Encode1(PartyResult::res_Party_Update);
			oPacket.Encode4(pParty->nPartyID);
			pParty->Encode(&oPacket);
			Broadcast(&oPacket, pParty->party.anCharacterID, 0);
		}
	}
}

void PartyMan::Broadcast(OutPacket *oPacket, int * anCharacterID, int nPlusOne)
{
	oPacket->GetSharedPacket()->ToggleBroadcasting();
	if (nPlusOne != 0)
	{
		auto pUser = User::FindUser(nPlusOne);
		if (pUser)
			pUser->SendPacket(oPacket);
	}

	for (int i = 0; i < MAX_PARTY_MEMBER_COUNT; ++i)
		if (anCharacterID[i] > 0)
		{
			auto pUser = User::FindUser(anCharacterID[i]);
			if (pUser)
				pUser->SendPacket(oPacket);
		}
}

#endif

PartyMan * PartyMan::GetInstance()
{
	static PartyMan *pInstance = new PartyMan();

	return pInstance;
}

int PartyMan::GetPartyIDByCharID(int nUserID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxPartyLock);
	auto findIter = m_mCharacterIDToPartyID.find(nUserID);
	if (findIter == m_mCharacterIDToPartyID.end())
		return -1;

	return findIter->second;
}

PartyMan::PartyData *PartyMan::GetPartyByCharID(int nUserID)
{
	int nPartyID = GetPartyIDByCharID(nUserID);
	if (nPartyID != -1)
		return GetParty(nPartyID);

	return nullptr;
}

PartyMan::PartyData *PartyMan::GetParty(int nPartyID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxPartyLock);

	auto findIter = m_mParty.find(nPartyID);
	if (findIter == m_mParty.end())
		return nullptr;
	return findIter->second;
}

bool PartyMan::IsPartyBoss(int nPartyID, int nUserID)
{
	auto pParty = GetPartyByCharID(nUserID);
	return pParty->party.nPartyBossCharacterID == nUserID;
}

#ifdef _WVSCENTER
void PartyMan::OnCreateNewPartyRequest(InPacket * iPacket, OutPacket *oPacket)
{
	int nCharacterID = iPacket->Decode4();
	int nUserChannelID = iPacket->Decode4();
	std::string strUserName = iPacket->DecodeStr();
	int nUserJob = iPacket->Decode2();
	int nUserLevel = iPacket->Decode2();
	int nUserFieldID = iPacket->Decode4();

	auto pUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
	int nPartyCreateResult = -1;

	std::lock_guard<std::recursive_mutex> lock(m_mtxPartyLock);
	if (pUser)
	{
		if (GetPartyByCharID(nCharacterID) == nullptr)
		{
			PartyData *pParty = AllocObj(PartyData);
			for (int i = 0; i < MAX_PARTY_MEMBER_COUNT; ++i)
				pParty->party.Initialize(i);

			pParty->nPartyID = ++m_atiPartyIDCounter;
			pParty->party.nPartyBossCharacterID = nCharacterID;
			pParty->party.anChannelID[0] = nUserChannelID;
			pParty->party.anJob[0] = nUserJob;
			pParty->party.anLevel[0] = nUserLevel;
			pParty->party.asCharacterName[0] = strUserName;
			//pParty->party.anFieldID[0] = nUserFieldID;
			pParty->party.anCharacterID[0] = nCharacterID;

			nPartyCreateResult = pParty->nPartyID;
			m_mParty[nPartyCreateResult] = pParty;
			m_mCharacterIDToPartyID[nCharacterID] = nPartyCreateResult;
		}
	}
	oPacket->Encode2(CenterSendPacketFlag::PartyResult);
	oPacket->Encode1(PartyResult::res_Party_Create);
	oPacket->Encode4(nCharacterID);
	oPacket->Encode4(nPartyCreateResult);
}

void PartyMan::OnLoadPartyRequest(InPacket * iPacket, OutPacket *oPacket)
{
	int nCharacterID = iPacket->Decode4();
	auto pParty = GetPartyByCharID(nCharacterID);
	oPacket->Encode2(CenterSendPacketFlag::PartyResult);
	oPacket->Encode1(PartyResult::res_Party_Load);

	std::lock_guard<std::recursive_mutex> lock(m_mtxPartyLock);
	if (pParty)
	{
		oPacket->Encode4(pParty->nPartyID);
		oPacket->Encode4(nCharacterID);
		pParty->Encode(oPacket);
	}
	else
		oPacket->Encode4(-1);
}

void PartyMan::OnJoinPartyRequest(InPacket * iPacket, OutPacket *oPacket)
{
	int nPartyID = iPacket->Decode4();
	int nBossID = iPacket->Decode4();
	int nToInvite = iPacket->Decode4();
	char nAvailableIndex = -1;

	oPacket->Encode2(CenterSendPacketFlag::PartyResult);
	oPacket->Encode1(PartyResult::res_Party_Join);
	auto pParty = GetPartyByCharID(nBossID);

	if (pParty &&
		pParty->nPartyID == nPartyID &&
		GetPartyIDByCharID(nToInvite) == -1)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxPartyLock);
		for (int i = 0; i < MAX_PARTY_MEMBER_COUNT; ++i)
			if (pParty->party.anCharacterID[i] == 0)
			{
				nAvailableIndex = i;
				break;
			}
		if (nAvailableIndex != -1)
		{
			pParty->party.anCharacterID[nAvailableIndex] = nToInvite;
			pParty->party.anChannelID[nAvailableIndex] = iPacket->Decode4();
			pParty->party.asCharacterName[nAvailableIndex] = iPacket->DecodeStr();
			pParty->party.anJob[nAvailableIndex] = iPacket->Decode2();
			pParty->party.anLevel[nAvailableIndex] = iPacket->Decode2();
			pParty->party.anFieldID[nAvailableIndex] = iPacket->Decode4();

			m_mCharacterIDToPartyID[nToInvite] = nPartyID;
		}
	}
	oPacket->Encode4(nToInvite);
	oPacket->Encode1(nAvailableIndex);
	if (nAvailableIndex != -1)
		oPacket->Encode4(nPartyID);
}

void PartyMan::NotifyMigrateIn(int nCharacterID, int nChannelID)
{
	auto pParty = GetPartyByCharID(nCharacterID);
	std::lock_guard<std::recursive_mutex> lock(m_mtxPartyLock);
	if (pParty)
	{
		int nIdx = FindUser(nCharacterID, pParty);
		pParty->party.anChannelID[nIdx] = nChannelID;

		OutPacket oPacket;
		oPacket.Encode2(CenterSendPacketFlag::PartyResult);
		oPacket.Encode1(res_Party_MigrateIn);
		oPacket.Encode4(pParty->nPartyID);
		oPacket.Encode1((char)nIdx);
		oPacket.Encode4(nChannelID);

		SendPacket(&oPacket, pParty);
	}
}

void PartyMan::SendPacket(OutPacket *oPacket, PartyData *pParty)
{
	oPacket->GetSharedPacket()->ToggleBroadcasting();

	for (int i = 0; i < MAX_PARTY_MEMBER_COUNT; ++i)
	{
		if (pParty->party.anChannelID[i] >= 0)
		{
			auto pChannel = WvsBase::GetInstance<WvsCenter>()->GetChannel(
				pParty->party.anChannelID[i]
			);
			if(pChannel)
				pChannel->GetLocalSocket()->SendPacket(oPacket);
		}
	}
}

#endif

void PartyMan::PartyMember::Initialize(int nIdx)
{
	if (nIdx >= 0 && nIdx < PartyMan::MAX_PARTY_MEMBER_COUNT) 
	{
		anChannelID[nIdx] = WvsWorld::CHANNELID_NOT_MIGRATED_IN;
		anLevel[nIdx] = 0;
		anFieldID[nIdx] = 0;
		asCharacterName[nIdx] = "";
		anJob[nIdx] = 0;
		anCharacterID[nIdx] = 0;
	}
}

void PartyMan::PartyData::Encode(OutPacket *oPacket)
{
	typedef unsigned char* BuffType;
	oPacket->EncodeBuffer((BuffType)(party.anCharacterID), sizeof(int) * MAX_PARTY_MEMBER_COUNT);
	for (int i = 0; i < MAX_PARTY_MEMBER_COUNT; ++i)
		oPacket->EncodeBuffer((BuffType)party.asCharacterName[i].c_str(), 13);
	oPacket->EncodeBuffer((BuffType)(party.anJob), sizeof(int) * MAX_PARTY_MEMBER_COUNT);
	oPacket->EncodeBuffer((BuffType)(party.anLevel), sizeof(int) * MAX_PARTY_MEMBER_COUNT);
	oPacket->EncodeBuffer((BuffType)(party.anChannelID), sizeof(int) * MAX_PARTY_MEMBER_COUNT);
	oPacket->Encode4(party.nPartyBossCharacterID);
	oPacket->EncodeBuffer((BuffType)(party.anFieldID), sizeof(int) * MAX_PARTY_MEMBER_COUNT);
	oPacket->EncodeBuffer((BuffType)(party.aTownPortal), sizeof(PartyTownPortal) * MAX_PARTY_MEMBER_COUNT);
}

void PartyMan::PartyData::Decode(InPacket * iPacket)
{
	typedef unsigned char* BuffType;
	iPacket->DecodeBuffer((BuffType)(party.anCharacterID), sizeof(int) * MAX_PARTY_MEMBER_COUNT);

	char aBuffName[14]{ 0 };
	for (int i = 0; i < MAX_PARTY_MEMBER_COUNT; ++i)
	{
		iPacket->DecodeBuffer((unsigned char*)aBuffName, 13);
		party.asCharacterName[i] = aBuffName;
	}
	iPacket->DecodeBuffer((BuffType)(party.anJob), sizeof(int) * MAX_PARTY_MEMBER_COUNT);
	iPacket->DecodeBuffer((BuffType)(party.anLevel), sizeof(int) * MAX_PARTY_MEMBER_COUNT);
	iPacket->DecodeBuffer((BuffType)(party.anChannelID), sizeof(int) * MAX_PARTY_MEMBER_COUNT);
	party.nPartyBossCharacterID = iPacket->Decode4();
	for (int i = 0; i < MAX_PARTY_MEMBER_COUNT; ++i)
	{
		WvsLogger::LogFormat("Decode Party [IDX: %d][ID: %d][Channel: %d][Name: %s]\n", i, party.anCharacterID[i], party.anChannelID[i], party.asCharacterName[i].c_str());
		//iPacket->DecodeBuffer((unsigned char*)aBuffName, 13);
		//party.asCharacterName[i] = aBuffName;
	}
	iPacket->DecodeBuffer((BuffType)(party.anFieldID), sizeof(int) * MAX_PARTY_MEMBER_COUNT);
	iPacket->DecodeBuffer((BuffType)(party.aTownPortal), sizeof(PartyTownPortal) * MAX_PARTY_MEMBER_COUNT);
}
