
#include "..\Database\CharacterDBAccessor.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\WvsLogin\LoginPacketTypes.hpp"
#include "..\WvsCenter\CenterPacketTypes.hpp"
#include "..\WvsShop\ShopPacketTypes.hpp"
#include "..\WvsGame\FieldPacketTypes.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Common\ServerConstants.hpp"
#include "WvsCenter.h"
#include "WvsWorld.h"
#include "LocalServer.h"
#include "AuthEntry.h"
#include "UserTransferStatus.h"
#include "EntrustedShopMan.h"
#include "GuildBBSMan.h"

#include "..\WvsGame\ItemInfo.h"
#include "..\WvsGame\PartyMan.h"
#include "..\WvsGame\GuildMan.h"
#include "..\WvsGame\FriendMan.h"
#include "..\WvsGame\Trunk.h"
#include <cmath>

LocalServer::LocalServer(asio::io_service& serverService)
	: SocketBase(serverService, true)
{
}

LocalServer::~LocalServer()
{
	for (auto& nCharacterID : m_sUser)
		WvsWorld::GetInstance()->RemoveUser(
			nCharacterID, -1, -1, false
		);
}

void LocalServer::OnClosed()
{
}

void LocalServer::InsertConnectedUser(int nUserID)
{
	std::lock_guard<std::mutex> lock(m_mtxUserLock);
	InPacket iPacket((unsigned char*)&nUserID, 4);
	GuildMan::GetInstance()->LoadGuild(&iPacket, nullptr);
	m_sUser.insert(nUserID);
}

void LocalServer::RemoveConnectedUser(int nUserID)
{
	std::lock_guard<std::mutex> lock(m_mtxUserLock);
	m_sUser.erase(nUserID);
}

int ProcessLocalServerPacket(LocalServer* pSrv, InPacket *iPacket)
{
	int nType = ((short*)iPacket->GetPacket())[0];
	bool bExcpetionOccurred = false;
	__try
	{
		pSrv->ProcessPacket(iPacket);
	}
	__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
	{
		bExcpetionOccurred = true;
	}
	return bExcpetionOccurred ? nType : -1;
}

void LocalServer::OnPacket(InPacket *iPacket)
{
	WvsLogger::LogRaw("[WvsCenter][LocalServer::OnPacket]Packet received: ");
	iPacket->Print();
	int nResult = ProcessLocalServerPacket(this, iPacket);

	if (nResult != -1)
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Unhandled System-Level Excpetion Has Been Caught: Packet Type = %d\n", nResult);
}

void LocalServer::ProcessPacket(InPacket * iPacket)
{
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
		case CenterRequestPacketType::RegisterCenterRequest:
			OnRegisterCenterRequest(iPacket);
			break;
		case CenterRequestPacketType::RequestCharacterList:
			OnRequestCharacterList(iPacket);
			break;
		case CenterRequestPacketType::RequestCreateNewCharacter:
			OnRequestCreateNewCharacter(iPacket);
			break;
		case CenterRequestPacketType::RequestCheckDuplicatedID:
			OnRequestCheckDuplicatedID(iPacket);
			break;
		case CenterRequestPacketType::RequestGameServerInfo:
			OnRequestGameServerInfo(iPacket);
			break;
		case CenterRequestPacketType::RequestLoginAuth:
			OnRequestLoginAuth(iPacket);
			break;
		case CenterRequestPacketType::RequestMigrateIn:
			OnRequestMigrateIn(iPacket);
			break;
		case CenterRequestPacketType::RequestMigrateOut:
			OnRequestMigrateOut(iPacket);
			break;
		case CenterRequestPacketType::RequestTransferChannel:
			OnRequestTransferChannel(iPacket);
			break;
		case CenterRequestPacketType::RequestTransferShop:
			OnRequestMigrateCashShop(iPacket);
			break;
		case CenterRequestPacketType::PartyRequest:
			OnPartyRequest(iPacket);
			break;
		case CenterRequestPacketType::GuildRequest:
			OnGuildRequest(iPacket);
			break;
		case CenterRequestPacketType::GuildBBSRequest:
			OnGuildBBSRequest(iPacket);
			break;
		case CenterRequestPacketType::FriendRequest:
			OnFriendRequest(iPacket);
			break;
		case CenterRequestPacketType::GroupMessage:
			OnGroupMessage(iPacket);
			break;
		case CenterRequestPacketType::WhisperMessage:
			OnWhisperMessage(iPacket);
			break;
		case CenterRequestPacketType::TrunkRequest:
			OnTrunkRequest(iPacket);
			break;
		case CenterRequestPacketType::FlushCharacterData:
			CharacterDBAccessor::GetInstance()->OnCharacterSaveRequest(iPacket);
			break;
		case CenterRequestPacketType::EntrustedShopRequest:
			OnEntrustedShopRequest(iPacket);
			break;
		case CenterRequestPacketType::GameClientDisconnected:
			OnGameClientDisconnected(iPacket);
			break;
		case CenterResultPacketType::CheckMigrationStateResult:
			OnCheckMigrationStateAck(iPacket);
			break;
		case CenterRequestPacketType::BroadcastPacket:
			OnBroadcastPacket(iPacket);
			break;
		case CenterRequestPacketType::CheckGivePopularityRequest:
			OnCheckGivePopularity(iPacket);
			break;
		case CenterRequestPacketType::CashItemRequest:
			OnCashItemRequest(iPacket);
			break;
	}
}

void LocalServer::OnRegisterCenterRequest(InPacket *iPacket)
{
	auto nServerType = iPacket->Decode1();
	SetServerType(nServerType);
	const char* sInstanceName = (
		nServerType == ServerConstants::SRV_LOGIN ? "WvsLogin" : 
		(nServerType == ServerConstants::SRV_GAME ? "WvsGame" : "WvsShop")
	);

	WvsLogger::LogFormat("[WvsCenter][LocalServer::OnRegisterCenterRequest]A connection request is received, instance information: [%s][%d].\n", sInstanceName, nServerType);
	int nChannel = WvsWorld::CHANNELID_SHOP;

	if (nServerType == ServerConstants::SRV_GAME)
	{
		nChannel = iPacket->Decode1();
		WvsBase::GetInstance<WvsCenter>()->RegisterChannel(nChannel, shared_from_this(), iPacket);
		WvsBase::GetInstance<WvsCenter>()->NotifyWorldChanged();
	}

	OutPacket oPacket;
	oPacket.Encode2(CenterResultPacketType::RegisterCenterAck);
	oPacket.Encode1(1); //Success;

	if (nServerType == ServerConstants::SRV_GAME)
	{
		oPacket.Encode1(WvsWorld::GetInstance()->GetWorldInfo().nWorldID);
		for (int i = 1; i <= 5; ++i)
			oPacket.Encode8(GW_ItemSlotBase::GetInitItemSN(
			(GW_ItemSlotBase::GW_ItemSlotType)i,
				WvsWorld::GetInstance()->GetWorldInfo().nWorldID,
				nChannel + 1 //Channel 0 is reserved for Center
			));
	}

	if (nServerType == ServerConstants::SRV_LOGIN)
	{
		auto pWorld = WvsWorld::GetInstance();
		oPacket.Encode1(pWorld->GetWorldInfo().nWorldID);
		oPacket.Encode1(pWorld->GetWorldInfo().nEventType);
		oPacket.EncodeStr(pWorld->GetWorldInfo().strWorldDesc);
		oPacket.EncodeStr(pWorld->GetWorldInfo().strEventDesc);
		WvsBase::GetInstance<WvsCenter>()->RegisterLoginServer(shared_from_this());
		WvsBase::GetInstance<WvsCenter>()->NotifyWorldChanged();
	}
	
	if (nServerType == ServerConstants::SRV_SHOP)
		WvsBase::GetInstance<WvsCenter>()->RegisterCashShop(shared_from_this(), iPacket);

	SendPacket(&oPacket);
}

void LocalServer::OnRequestCharacterList(InPacket *iPacket)
{
	int nLoginSocketID = iPacket->Decode4();
	int nAccountID = iPacket->Decode4();
	int nChannelID = iPacket->Decode1();
	if (WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID) != nullptr) 
		CharacterDBAccessor::GetInstance()->PostLoadCharacterListRequest(this, nLoginSocketID, nAccountID, WvsWorld::GetInstance()->GetWorldInfo().nWorldID);
}

void LocalServer::OnRequestCreateNewCharacter(InPacket *iPacket)
{
	int nLoginSocketID = iPacket->Decode4();
	int nAccountID = iPacket->Decode4();

	std::string strName = iPacket->DecodeStr();
	//iPacket->Decode4();
	//iPacket->Decode4();

	int nJobType = 1;
	//int nSubJob = iPacket->Decode2();
	unsigned char nGender = 0;
	unsigned char nSkin = 0;
	int nAttr = 6, 
		nAttrRead = 0,
		nFaceID = 0,
		nHairID = 0;

	int aEquips[CharacterDBAccessor::eEQPData_POS_END] = { 0 };
	int aStats[CharacterDBAccessor::eStatData_POS_END] = { 0 };

	for (int i = 0; i < nAttr; ++i)
	{
		nAttrRead = iPacket->Decode4();
		if (ItemInfo::IsWeapon(nAttrRead))
			aEquips[CharacterDBAccessor::eEQPData_POS_WeaponEquip] = nAttrRead;
		else if (ItemInfo::IsPants(nAttrRead))
			aEquips[CharacterDBAccessor::eEQPData_POS_PantsEquip] = nAttrRead;
		else if (ItemInfo::IsCoat(nAttrRead))
			aEquips[CharacterDBAccessor::eEQPData_POS_CoatEquip] = nAttrRead;
		else if (ItemInfo::IsCap(nAttrRead))
			aEquips[CharacterDBAccessor::eEQPData_POS_CapEquip] = nAttrRead;
		else if (ItemInfo::IsCape(nAttrRead))
			aEquips[CharacterDBAccessor::eEQPData_POS_CapeEquip] = nAttrRead;
		else if (ItemInfo::IsFace(nAttrRead))
			nFaceID = nAttrRead;
		else if (ItemInfo::IsHair(nAttrRead))
			nHairID = nAttrRead;
		else if (ItemInfo::IsShield(nAttrRead))
			aEquips[CharacterDBAccessor::eEQPData_POS_ShieldEquip] = nAttrRead;
		else if (ItemInfo::IsShoes(nAttrRead))
			aEquips[CharacterDBAccessor::eEQPData_POS_ShoesEquip] = nAttrRead;
		else if (ItemInfo::IsLongcoat(nAttrRead))
			aEquips[CharacterDBAccessor::eEQPData_POS_CoatEquip] = nAttrRead;
	}
	CharacterDBAccessor::GetDefaultCharacterStat(aStats);
	int nTotal = 0;
	nTotal += (aStats[CharacterDBAccessor::eStatData_POS_Str] = std::max(0, std::min(11, (int)iPacket->Decode1())));
	nTotal += (aStats[CharacterDBAccessor::eStatData_POS_Luk] = std::max(0, std::min(11, (int)iPacket->Decode1())));
	nTotal += (aStats[CharacterDBAccessor::eStatData_POS_Dex] = std::max(0, std::min(11, (int)iPacket->Decode1())));
	nTotal += (aStats[CharacterDBAccessor::eStatData_POS_Int] = std::max(0, std::min(11, (int)iPacket->Decode1())));

	if (nTotal > 25)
		aStats[CharacterDBAccessor::eStatData_POS_Str] 
		= aStats[CharacterDBAccessor::eStatData_POS_Dex]
		= aStats[CharacterDBAccessor::eStatData_POS_Int] 
		= aStats[CharacterDBAccessor::eStatData_POS_Luk] = 4;

	CharacterDBAccessor::GetInstance()->PostCreateNewCharacterRequest(
		this, 
		nLoginSocketID, 
		nAccountID, 
		WvsWorld::GetInstance()->GetWorldInfo().nWorldID, 
		strName, 
		nGender, 
		nFaceID,
		nHairID,
		nSkin, 
		(const int*)aEquips, 
		(const int*)aStats);
}

void LocalServer::OnRequestCheckDuplicatedID(InPacket * iPacket)
{
	int nLoginSocketID = iPacket->Decode4();
	int nAccountID = iPacket->Decode4();
	std::string sCharacterName = iPacket->DecodeStr();

	CharacterDBAccessor::GetInstance()->PostCheckDuplicatedID(
		this,
		nLoginSocketID,
		nAccountID,
		sCharacterName
	);
}

void LocalServer::OnRequestGameServerInfo(InPacket *iPacket)
{
	int nLoginSocketID = iPacket->Decode4();
	int nAccountID = iPacket->Decode4();
	int nWorldID = iPacket->Decode4();
	int nChannelID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();

	OutPacket oPacket;
	oPacket.Encode2(CenterResultPacketType::GameServerInfoResponse);
	oPacket.Encode4(nLoginSocketID);

	if (nWorldID != WvsWorld::GetInstance()->GetWorldInfo().nWorldID ||
		WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID) == nullptr ||
		CharacterDBAccessor::GetInstance()->QueryCharacterAccountID(nCharacterID) != nAccountID)
	{
		WvsLogger::LogFormat(
			WvsLogger::LEVEL_ERROR, 
			"[WvsCenter][LocalServer::OnRequstGameServerInfo]Warning: A client is trying to connect to an inexistent channel server [WvsGame: %02d] or login to an inexistent account [AccountID = %d, CharacterID = %d].\n", 
			nChannelID,
			nAccountID,
			nCharacterID);

		oPacket.Encode1(0); //Failed
		SendPacket(&oPacket);
		return;
	}

	auto pAuthEntry = AllocObj(AuthEntry);
	pAuthEntry->nAccountID = nAccountID;
	pAuthEntry->nChannelID = nChannelID;
	pAuthEntry->nCharacterID = nCharacterID;
	oPacket.Encode1(1); //Auth Inserted.

	//Encode For Client
	oPacket.Encode2(0);
	oPacket.Encode4(WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID)->GetExternalIP());
	oPacket.Encode2(WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID)->GetExternalPort());
	oPacket.Encode4(nCharacterID);
	oPacket.Encode1(0);
	oPacket.Encode4(0);

	WvsWorld::GetInstance()->InsertAuthEntry(nCharacterID, nAccountID, pAuthEntry);
	WvsWorld::GetInstance()->ClearUserTransferStatus(nCharacterID);
	SendPacket(&oPacket);
}

void LocalServer::OnRequestLoginAuth(InPacket* iPacket)
{
	int nType = iPacket->Decode1();
	switch (nType)
	{
		case LoginAuthRequest::rq_LoginAuth_RefreshLoginState:
		{
			int nAccountID = iPacket->Decode4();

			OutPacket oPacket;
			oPacket.Encode2(CenterResultPacketType::LoginAuthResult);
			oPacket.Encode1(LoginAuthResult::res_LoginAuth_RefreshLoginState);
			oPacket.Encode4(nAccountID);
			oPacket.Encode1(WvsWorld::GetInstance()->RefreshLoginState(nAccountID));
			SendPacket(&oPacket);
			break;
		}
	}
}

void LocalServer::OnRequestMigrateIn(InPacket *iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	int nChannelID = iPacket->Decode4();

	InsertConnectedUser(nCharacterID);
	if (!WvsWorld::GetInstance()->IsUserTransfering(nCharacterID))
	{
		auto pAuthEntry = WvsWorld::GetInstance()->GetAuthEntry(nCharacterID);
		auto pMigratedInUser = WvsWorld::GetInstance()->GetUser(nCharacterID);

		if(!pAuthEntry)
		{
			WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "[WvsCenter][LocalServer::OnRequestMigrateIn]Warning: A client is trying to login to a character without authentications. [CharacterID = %d].\n",	nCharacterID);
			return;
		}

		if (pMigratedInUser)
		{
			WvsWorld::GetInstance()->RemoveAuthEntry(nCharacterID);
			WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "[WvsCenter][LocalServer::OnRequestMigrateIn]Warning: A client is trying to login to a character that has already logged into the game server. [CharacterID = %d].\n", nCharacterID);
			return;
		}

		auto pwUser = AllocObj(WvsWorld::WorldUser);
		pwUser->m_nCharacterID = nCharacterID;
		pwUser->m_bInShop = false;
		pwUser->m_bMigrated = true;
		pwUser->m_nChannelID = nChannelID;
		pwUser->m_nLocalSocketSN = nClientSocketID;
		pwUser->m_nAccountID = pAuthEntry->nAccountID;

		WvsWorld::GetInstance()->RemoveAuthEntry(nCharacterID);
		WvsWorld::GetInstance()->SetUser(
			nCharacterID,
			pwUser
		);
	}

	OutPacket oPacket;
	oPacket.Encode2(CenterResultPacketType::CenterMigrateInResult);
	oPacket.Encode4(nClientSocketID);
	oPacket.Encode4(nCharacterID);
	CharacterDBAccessor::GetInstance()->PostCharacterDataRequest(this, nClientSocketID, nCharacterID, &oPacket); // for WvsGame
	auto pUserTransferStatus = WvsWorld::GetInstance()->GetUserTransferStatus(nCharacterID);

	WvsLogger::LogFormat("OnRequestMigrateIn, has transfer status ? %d\n", (pUserTransferStatus != nullptr));
	if (pUserTransferStatus == nullptr)
		oPacket.Encode1(0);
	else
	{
		oPacket.Encode1(1);
		pUserTransferStatus->Encode(&oPacket);
	}
	this->SendPacket(&oPacket);

	WvsWorld::GetInstance()->UserMigrateIn(nCharacterID, nChannelID);
}

void LocalServer::OnRequestMigrateOut(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	int nChannelID = iPacket->Decode4();
	RemoveConnectedUser(nCharacterID);

	CharacterDBAccessor::GetInstance()->OnCharacterSaveRequest(iPacket);
	char nGameEndType = iPacket->Decode1();
	WvsLogger::LogFormat("OnRequestMigrateOut code = %d\n", (int)nGameEndType);

	if (nGameEndType == 1) //Transfer to another game server or to the shop.
	{
		UserTransferStatus* pStatus = AllocObj( UserTransferStatus );
		pStatus->Decode(iPacket);
		WvsWorld::GetInstance()->SetUserTransferStatus(nCharacterID, pStatus);
		WvsWorld::GetInstance()->SetUserTransfering(nCharacterID, true);
	}
	else if (nGameEndType == 0) //Migrate out from the game server.
	{
		WvsWorld::GetInstance()->ClearUserTransferStatus(nCharacterID);
		WvsWorld::GetInstance()->RemoveUser(nCharacterID, nChannelID, nClientSocketID, false);
	}
	// nGameEndType = 2 : From shop to game server
}

void LocalServer::OnRequestTransferChannel(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	int nChannelID = iPacket->Decode1();
	auto pEntry = WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID);
	auto pUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
	OutPacket oPacket;
	oPacket.Encode2(CenterResultPacketType::TransferChannelResult);
	oPacket.Encode4(nClientSocketID);
	oPacket.Encode1((pEntry != nullptr ? 1 : 0)); //bSuccess
	if (pEntry != nullptr)
	{
		oPacket.Encode1(1);
		oPacket.Encode4(pEntry->GetExternalIP());
		oPacket.Encode2(pEntry->GetExternalPort());
		oPacket.Encode4(0);
	}
	this->SendPacket(&oPacket);

	if(pUser)
		pUser->m_nChannelID = nChannelID;
}

void LocalServer::OnRequestMigrateCashShop(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	auto pUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
	OutPacket oPacket;
	oPacket.Encode2(CenterResultPacketType::MigrateCashShopResult);
	oPacket.Encode4(nClientSocketID);
	auto pEntry = WvsBase::GetInstance<WvsCenter>()->GetShop();
	if (WvsBase::GetInstance<WvsCenter>()->GetShop() == nullptr)
		oPacket.Encode1(0); //bSuccess
	else
	{
		oPacket.Encode1(1);
		oPacket.Encode1(1);
		oPacket.Encode4(pEntry->GetExternalIP());
		oPacket.Encode2(pEntry->GetExternalPort());
		oPacket.Encode4(0);
	}
	this->SendPacket(&oPacket);

	pUser->m_nChannelID = WvsWorld::CHANNELID_SHOP;
	pUser->m_bInShop = true;

	WvsWorld::GetInstance()->UserMigrateIn(nCharacterID, WvsWorld::CHANNELID_SHOP);
}

void LocalServer::OnGameClientDisconnected(InPacket *iPacket)
{
	iPacket->Decode4();
	WvsWorld::GetInstance()->RemoveAuthEntry(iPacket->Decode4());
}

void LocalServer::OnCheckMigrationStateAck(InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(WvsWorld::GetInstance()->GetLock());
	int nCharacterID = iPacket->Decode4();
	int nChannelID = iPacket->Decode4();
	bool bMigratedIn = iPacket->Decode1() ? 1 : 0;

	auto pwUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
	if (pwUser && pwUser->m_nChannelID != nChannelID) //Already transfered to other channels.
		WvsWorld::GetInstance()->SendMigrationStateCheck(pwUser);
	else if (pwUser && !bMigratedIn)
	{
		WvsWorld::GetInstance()->RemoveUser(nCharacterID, 0, 0, 0);
		WvsWorld::GetInstance()->RemoveAuthEntry(nCharacterID);
	}
}

void LocalServer::OnBroadcastPacket(InPacket *iPacket)
{
	int nGameSrvCount = iPacket->Decode1();
	OutPacket oPacket;
	oPacket.Encode2(CenterResultPacketType::RemoteBroadcasting);
	oPacket.Encode4(-1); //nUserID --> -1 = broadcast to all.
	oPacket.EncodeBuffer(
		iPacket->GetPacket() + iPacket->GetReadCount() + nGameSrvCount,
		iPacket->GetPacketSize() - iPacket->GetReadCount() - nGameSrvCount
	); //remember to substract nGameSrvCount ( = bytes for channel ids).

	LocalServerEntry *pChannel = nullptr;
	//Broadcast to all.
	if (nGameSrvCount == 0)
		for (int i = 0; i < ServerConstants::kMaxChannelCount; ++i) 
		{
			pChannel = WvsBase::GetInstance<WvsCenter>()->GetChannel(i);
			if (pChannel)
				pChannel->GetLocalSocket()->SendPacket(&oPacket);
		}
	else //Broadcast to specified game server(s).
		for (int i = 0; i < nGameSrvCount; ++i)
		{
			pChannel = WvsBase::GetInstance<WvsCenter>()->GetChannel(iPacket->Decode1());
			if(pChannel)
				pChannel->GetLocalSocket()->SendPacket(&oPacket);
		}
}

void LocalServer::OnCheckGivePopularity(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	int nTargetID = iPacket->Decode4();

	OutPacket oPacket;
	oPacket.Encode2(CenterResultPacketType::CheckGivePopularityResult);
	oPacket.Encode4(nCharacterID);

	if(!WvsWorld::GetInstance()->CheckEventAvailabilityForUser("GivePop", nCharacterID))
		oPacket.Encode1(3);
	else
	{
		oPacket.Encode1(0);
		WvsWorld::GetInstance()->InsertNextAvailableEventTimeForUser("GivePop", nCharacterID, GameDateTime::GetDateExpireFromPeriod(1));
	}
	oPacket.Encode4(nTargetID);
	oPacket.Encode1(1);

	SendPacket(&oPacket);
}

void LocalServer::OnCashItemRequest(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	int nRequest = iPacket->Decode2();
	switch (nRequest)
	{
		case CenterCashItemRequestType::eBuyCashItemRequest:
			CharacterDBAccessor::GetInstance()->PostBuyCashItemRequest(this, nClientSocketID, nCharacterID, iPacket);
			break;
		case CenterCashItemRequestType::eLoadCashItemLockerRequest:
			CharacterDBAccessor::GetInstance()->PostLoadLockerRequest(this, nClientSocketID, nCharacterID, iPacket);
			break;
		case CenterCashItemRequestType::eMoveCashItemLtoSRequest:
			CharacterDBAccessor::GetInstance()->PostMoveLockerToSlotRequest(this, nClientSocketID, nCharacterID, iPacket);
			break;
		case CenterCashItemRequestType::eMoveCashItemStoLRequest:
			CharacterDBAccessor::GetInstance()->PostMoveSlotToLockerRequest(this, nClientSocketID, nCharacterID, iPacket);
			break;
		case CenterCashItemRequestType::eExpireCashItemRequest:
			CharacterDBAccessor::GetInstance()->PostExpireCashItemRequest(this, nClientSocketID, nCharacterID, iPacket);
			break;
		case CenterCashItemRequestType::eGetMaplePointRequest:
			CharacterDBAccessor::GetInstance()->PostUpdateCashRequest(this, nClientSocketID, nCharacterID, iPacket);
			break;
	}
}

void LocalServer::OnPartyRequest(InPacket * iPacket)
{
	int nRequest = iPacket->Decode1();
	OutPacket oPacket;
	switch (nRequest)
	{
		case PartyMan::PartyRequest::rq_Party_Create:
			PartyMan::GetInstance()->CreateNewParty(iPacket, &oPacket);
			break;
		case PartyMan::PartyRequest::rq_Party_Load:
			PartyMan::GetInstance()->LoadParty(iPacket, &oPacket);
			break;
		case PartyMan::PartyRequest::rq_Party_Join:
			PartyMan::GetInstance()->JoinParty(iPacket, &oPacket);
			break;
		case PartyMan::PartyRequest::rq_Party_Withdraw_Kick:
			PartyMan::GetInstance()->WithdrawParty(iPacket, &oPacket);
			break;
		case PartyMan::PartyRequest::rq_Party_ChangeBoss:
			PartyMan::GetInstance()->ChangePartyBoss(iPacket, &oPacket);
			break;
		case PartyMan::PartyRequest::rq_Guild_LevelOrJobChanged:
			PartyMan::GetInstance()->ChangeLevelOrJob(iPacket, &oPacket);
			break;
	}

	if (oPacket.GetPacketSize() != 0)
		SendPacket(&oPacket);
}

void LocalServer::OnGuildRequest(InPacket * iPacket)
{
	int nRequest = iPacket->Decode1();
	OutPacket oPacket;
	switch (nRequest)
	{
		case GuildMan::GuildRequest::rq_Guild_Load:
			GuildMan::GetInstance()->LoadGuild(iPacket, &oPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_Create:
			GuildMan::GetInstance()->CreateNewGuild(iPacket, &oPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_Join:
			GuildMan::GetInstance()->JoinGuild(iPacket, &oPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_Withdraw:
		case GuildMan::GuildRequest::rq_Guild_Withdraw_Kick:
			GuildMan::GetInstance()->WithdrawGuild(iPacket, &oPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_SetGradeName:
			GuildMan::GetInstance()->SetGradeName(iPacket, &oPacket);
			break; 
		case GuildMan::GuildRequest::rq_Guild_SetNotice:
			GuildMan::GetInstance()->SetNotice(iPacket, &oPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_SetMark:
			GuildMan::GetInstance()->SetMark(iPacket, &oPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_SetMemberGrade:
			GuildMan::GetInstance()->SetMemberGrade(iPacket, &oPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_IncMaxMemberNum:
			GuildMan::GetInstance()->IncMaxMemberNum(iPacket, &oPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_IncPoint:
			GuildMan::GetInstance()->IncPoint(iPacket, &oPacket);
			break;
		case GuildMan::GuildRequest::rq_Guild_LevelOrJobChanged:
			GuildMan::GetInstance()->ChangeJobOrLevel(iPacket, &oPacket);
			break;
		
	}

	if (oPacket.GetPacketSize() != 0)
		SendPacket(&oPacket);
}

void LocalServer::OnGuildBBSRequest(InPacket *iPacket)
{
	int nGuildID = iPacket->Decode4();
	auto pGuild = GuildMan::GetInstance()->GetGuild(nGuildID);
	if (!pGuild)
		return;
	
	GuildBBSMan::GetInstance()->OnGuildBBSRequest(pGuild, iPacket);
}

void LocalServer::OnFriendRequest(InPacket *iPacket)
{
	int nRequest = iPacket->Decode1();
	OutPacket oPacket;
	switch (nRequest)
	{
		case FriendMan::FriendRequest::rq_Friend_Set:
			FriendMan::GetInstance()->SetFriend(iPacket, &oPacket);
			break;
		case FriendMan::FriendRequest::rq_Friend_Load:
			FriendMan::GetInstance()->LoadFriend(iPacket, &oPacket);
			break;
		case FriendMan::FriendRequest::rq_Friend_Accept:
			FriendMan::GetInstance()->AcceptFriend(iPacket, &oPacket);
			break;
		case FriendMan::FriendRequest::rq_Friend_Delete:
			FriendMan::GetInstance()->DeleteFriend(iPacket, &oPacket);
			break;
	}

	if (oPacket.GetPacketSize() != 0)
		SendPacket(&oPacket);
}

void LocalServer::OnGroupMessage(InPacket * iPacket)
{
	int nUserID = iPacket->Decode4();
	int nType = iPacket->Decode1();

	int nReceiverCount = iPacket->Decode1();
	std::vector<int> aReceiverID;
	for (int i = 0; i < nReceiverCount; ++i)
		aReceiverID.push_back(iPacket->Decode4());

	WvsWorld::WorldUser *pwUser = nullptr;
	bool bSend = false;
	for (auto& nID : aReceiverID)
	{
		bSend = false;
		switch (nType)
		{
			case 0: //Friend Group Message
			{
				auto pEntry = FriendMan::GetInstance()->GetFriendEntry(nUserID);
				auto pEntry_Friend = FriendMan::GetInstance()->GetFriendEntry(nID);
				bSend = (pEntry &&
					pEntry_Friend &&
					pEntry->FindIndex(nID) >= 0 &&
					pEntry_Friend->FindIndex(nUserID) >= 0);
				break;
			}
			case 1: //Party Group Message
			{
				auto pParty = PartyMan::GetInstance()->GetPartyByCharID(nUserID);
				bSend = (pParty && PartyMan::GetInstance()->FindUser(nID, pParty) >= 0);
				break;
			}
			case 2: //Guild Group Message
			{
				auto pGuild = GuildMan::GetInstance()->GetGuildByCharID(nUserID);
				bSend = (pGuild && GuildMan::GetInstance()->FindUser(nID, pGuild) >= 0);
				break;
			}
		}
		if (bSend && (pwUser = WvsWorld::GetInstance()->GetUser(nID)))
		{
			OutPacket oPacket;
			oPacket.Encode2(CenterResultPacketType::RemoteBroadcasting);
			oPacket.Encode4(nID);
			oPacket.EncodeBuffer(
				iPacket->GetPacket() + iPacket->GetReadCount(),
				iPacket->GetPacketSize() - iPacket->GetReadCount()
			);

			pwUser->SendPacket(&oPacket);
		}
	}
}

void LocalServer::OnWhisperMessage(InPacket * iPacket)
{
	int nUserID = iPacket->Decode4();
	std::string strUserName = iPacket->DecodeStr();
	int nType = iPacket->Decode1();
	std::string strTargetName = iPacket->DecodeStr();

	//Query the character id of the specified name.
	int nTargetID = CharacterDBAccessor::GetInstance()->QueryCharacterIDByName(strTargetName);

	//Check again the existence of sender.
	std::lock_guard<std::recursive_mutex> lock(WvsWorld::GetInstance()->GetLock());
	auto pwSender = WvsWorld::GetInstance()->GetUser(nUserID);
	if (!pwSender)
		return;

	auto pwUser = (nTargetID == -1 ? nullptr : WvsWorld::GetInstance()->GetUser(nTargetID));
	OutPacket oReply;
	oReply.Encode2(CenterResultPacketType::RemoteBroadcasting);
	if (!pwUser || pwUser->m_nChannelID == WvsWorld::CHANNELID_SHOP)
	{
		oReply.Encode4(nUserID);
		oReply.Encode2(FieldSendPacketType::Field_OnWhisper);
		oReply.Encode1(!pwUser ? 
			WhisperResult::e_Whisper_Res_Message_Ack:
			WhisperResult::e_Whisper_Res_QuerySuccess);
		oReply.EncodeStr(strTargetName);
		oReply.Encode1(
			!pwUser ? 
			WhisperResult::e_Whisper_QR_NotMigratedIn: 
			WhisperResult::e_Whisper_QR_InShop);

		oReply.Encode4(-1);
		pwSender->SendPacket(&oReply);
		return;
	}

	if (nType == WhisperResult::e_Whisper_Type_QueryLocation) //Require location info.
	{
		oReply.Encode4(nUserID);
		oReply.Encode2(FieldSendPacketType::Field_OnWhisper);
		oReply.Encode1(WhisperResult::e_Whisper_Res_QuerySuccess);
		oReply.EncodeStr(strTargetName);
		oReply.Encode1(WhisperResult::e_Whisper_QR_ChannelID);
		oReply.Encode4(pwUser->m_nChannelID);
		pwSender->SendPacket(&oReply);
	}
	else if (nType == WhisperResult::e_Whisper_Type_SendMessage) //Whisper msg.
	{
		//Reply
		oReply.Encode4(nUserID);
		oReply.Encode2(FieldSendPacketType::Field_OnWhisper);
		oReply.Encode1(WhisperResult::e_Whisper_Res_Message_Ack);
		oReply.EncodeStr(strTargetName);
		oReply.Encode1(1); //Success
		pwSender->SendPacket(&oReply);

		//Send to target
		OutPacket oWhisper;
		oWhisper.Encode2(CenterResultPacketType::RemoteBroadcasting);
		oWhisper.Encode4(nTargetID);
		oWhisper.Encode2(FieldSendPacketType::Field_OnWhisper);
		oWhisper.Encode1(WhisperResult::e_Whisper_Res_Message_Send);
		oWhisper.EncodeStr(strUserName);
		oWhisper.Encode2(pwUser->m_nChannelID); //Success
		oWhisper.EncodeStr(iPacket->DecodeStr());
		pwUser->SendPacket(&oWhisper);
	}
}

void LocalServer::OnTrunkRequest(InPacket * iPacket)
{
	int nRequest = iPacket->Decode1();
	int nAccountID = iPacket->Decode4();
	auto pTrunk = Trunk::Load(nAccountID);
	switch (nRequest)
	{
		case Trunk::TrunkRequest::rq_Trunk_Load:
		{
			int nCharacterID = iPacket->Decode4();
			OutPacket oPacket;
			oPacket.Encode2(CenterResultPacketType::TrunkResult);
			oPacket.Encode4(nCharacterID);
			oPacket.Encode1(Trunk::TrunkResult::res_Trunk_Load);
			pTrunk->Encode(0xFFFFFFFF, &oPacket);
			auto pwUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
			if (pwUser)
				pwUser->SendPacket(&oPacket);
			break;
		}
		case Trunk::TrunkRequest::rq_Trunk_MoveSlotToTrunk:
			pTrunk->MoveSlotToTrunk(nAccountID, iPacket);
			break;
		case Trunk::TrunkRequest::rq_Trunk_MoveTrunkToSlot:
			pTrunk->MoveTrunkToSlot(nAccountID, iPacket);
			break;
		case Trunk::TrunkRequest::rq_Trunk_WithdrawMoney:
			pTrunk->WithdrawMoney(nAccountID, iPacket);
			break;
		
	}
	FreeObj(pTrunk);
}

void LocalServer::OnEntrustedShopRequest(InPacket *iPacket)
{
	int nType = iPacket->Decode1();
	switch (nType)
	{
		case EntrustedShopMan::EntrustedShopRequest::req_EShop_OpenCheck:
			EntrustedShopMan::GetInstance()->CheckEntrustedShopOpenPossible(
				this, iPacket->Decode4(), 0
			);
			break;
		case EntrustedShopMan::EntrustedShopRequest::req_EShop_RegisterShop:
			EntrustedShopMan::GetInstance()->CreateEntrustedShop(
				this, iPacket->Decode4(), 0, 0
			);
			break;
		case EntrustedShopMan::EntrustedShopRequest::req_EShop_UnRegisterShop:
			EntrustedShopMan::GetInstance()->RemoveEntrustedShop(
				this, iPacket->Decode4()
			);
			break;
		case EntrustedShopMan::EntrustedShopRequest::req_EShop_SaveItemRequest:
			EntrustedShopMan::GetInstance()->SaveItem(
				this, iPacket->Decode4(), iPacket
			);
			break;
		case EntrustedShopMan::EntrustedShopRequest::req_EShop_ItemNumberChanged:
			EntrustedShopMan::GetInstance()->ItemNumberChanged(
				this, iPacket->Decode4(), iPacket
			);
			break;
		case EntrustedShopMan::EntrustedShopRequest::req_EShop_LoadItemRequest:
			EntrustedShopMan::GetInstance()->LoadItemRequest(
				this, iPacket->Decode4()
			);
			break;
	}
}
