
#include "..\Database\CharacterDBAccessor.h"
#include "LocalServer.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

#include "..\WvsLib\Net\PacketFlags\LoginPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\ShopPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\GameSrvPacketFlags.hpp"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Common\ServerConstants.hpp"
#include "WvsCenter.h"
#include "WvsWorld.h"
#include "UserTransferStatus.h"
#include "..\WvsGame\ItemInfo.h"
#include "..\WvsGame\PartyMan.h"
#include "..\WvsGame\GuildMan.h"

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

void LocalServer::OnPacket(InPacket *iPacket)
{
	WvsLogger::LogRaw("[WvsCenter][LocalServer::OnPacket]封包接收：");
	iPacket->Print();
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
		case LoginSendPacketFlag::Center_RegisterCenterRequest:
			OnRegisterCenterRequest(iPacket);
			break;
		case LoginSendPacketFlag::Center_RequestCharacterList:
			OnRequestCharacterList(iPacket);
			break;
		case LoginSendPacketFlag::Center_RequestCreateNewCharacter:
			OnRequestCreateNewCharacter(iPacket);
			break;
		case LoginSendPacketFlag::Center_RequestGameServerInfo:
			OnRequestGameServerInfo(iPacket);
			break;
		case GameSrvSendPacketFlag::RequestMigrateIn:
			OnRequestMigrateIn(iPacket);
			break;
		case ShopInternalPacketFlag::RequestMigrateOut:
		case GameSrvSendPacketFlag::RequestMigrateOut:
			OnRequestMigrateOut(iPacket);
			break;
		case ShopInternalPacketFlag::RequestTransferToGame:
		case GameSrvSendPacketFlag::RequestTransferChannel:
			OnRequestTransferChannel(iPacket);
			break;
		case GameSrvSendPacketFlag::RequestTransferShop:
			OnRequestMigrateCashShop(iPacket);
			break;
		case ShopInternalPacketFlag::RequestBuyCashItem:
			OnRequestBuyCashItem(iPacket);
			break;
		case ShopInternalPacketFlag::RequestLoadLocker:
			OnRequestLoadLocker(iPacket);
			break;
		case ShopInternalPacketFlag::RequestUpdateCash:
			OnReuqestUpdateCash(iPacket);
			break;
		case ShopInternalPacketFlag::RequestMoveLToS:
			OnReuqestMoveLockerToSlot(iPacket);
			break;
		case ShopInternalPacketFlag::RequestMoveSToL:
			OnReuqestMoveSlotToLocker(iPacket);
			break;
		case GameSrvSendPacketFlag::PartyRequest:
			OnPartyRequest(iPacket);
			break;
		case GameSrvSendPacketFlag::GuildRequest:
			OnGuildRequest(iPacket);
			break;
	}
}

void LocalServer::OnRegisterCenterRequest(InPacket *iPacket)
{
	auto serverType = iPacket->Decode1();
	SetServerType(serverType);
	const char* pInstanceName = (serverType == ServerConstants::SRV_LOGIN ? "WvsLogin" : (serverType == ServerConstants::SRV_GAME ? "WvsGame" : "WvsShop"));
	WvsLogger::LogFormat("[WvsCenter][LocalServer::OnRegisterCenterRequest]收到新的[%s][%d]連線請求。\n", pInstanceName, serverType);

	if (serverType == ServerConstants::SRV_GAME)
	{
		WvsBase::GetInstance<WvsCenter>()->RegisterChannel(shared_from_this(), iPacket);
		WvsBase::GetInstance<WvsCenter>()->NotifyWorldChanged();
	}

	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::RegisterCenterAck);
	oPacket.Encode1(1); //Success;
	if (serverType == ServerConstants::SRV_LOGIN)
	{
		auto pWorld = WvsWorld::GetInstance();
		oPacket.Encode1(pWorld->GetWorldInfo().nWorldID);
		oPacket.Encode1(pWorld->GetWorldInfo().nEventType);
		oPacket.EncodeStr(pWorld->GetWorldInfo().strWorldDesc);
		oPacket.EncodeStr(pWorld->GetWorldInfo().strEventDesc);
		WvsBase::GetInstance<WvsCenter>()->NotifyWorldChanged();
	}
	
	if (serverType == ServerConstants::SRV_SHOP)
		WvsBase::GetInstance<WvsCenter>()->RegisterCashShop(shared_from_this(), iPacket);

	SendPacket(&oPacket);
}

void LocalServer::OnRequestCharacterList(InPacket *iPacket)
{
	int nLoginSocketID = iPacket->Decode4();
	int nAccountID = iPacket->Decode4();
	int nChannelID = iPacket->Decode1();
	if (WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID) != nullptr) 
	{
		auto aID = CharacterDBAccessor::GetInstance()->PostLoadCharacterListRequest(this, nLoginSocketID, nAccountID, WvsWorld::GetInstance()->GetWorldInfo().nWorldID);
		/*for (auto nID : aID)
		{
			auto pUser = WvsWorld::GetInstance()->GetUser(nID);
			if (!pUser) 
			{
				pUser = AllocObj(WvsWorld::WorldUser);
				pUser->m_nCharacterID = nID;
				pUser->m_nAccountID = nAccountID;
				WvsWorld::GetInstance()->SetUser(nID, pUser);
			}
			pUser->m_bLoggedIn = true;
			pUser->m_nChannelID = nChannelID;
			pUser->m_nLocalSrvIdx = 0; //
			pUser->m_nLocalSocketSN = nLoginSocketID;
		}*/
	}
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

	int aEquips[CharacterDBAccessor::EQP_ID_FLAG_END] = { 0 };
	int aStats[CharacterDBAccessor::STAT_FLAG_END] = { 0 };

	for (int i = 0; i < nAttr; ++i)
	{
		nAttrRead = iPacket->Decode4();
		if (ItemInfo::IsWeapon(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_WeaponEquip] = nAttrRead;
		else if (ItemInfo::IsPants(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_PantsEquip] = nAttrRead;
		else if (ItemInfo::IsCoat(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_CoatEquip] = nAttrRead;
		else if (ItemInfo::IsCap(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_CapEquip] = nAttrRead;
		else if (ItemInfo::IsCape(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_CapeEquip] = nAttrRead;
		else if (ItemInfo::IsFace(nAttrRead))
			nFaceID = nAttrRead;
		else if (ItemInfo::IsHair(nAttrRead))
			nHairID = nAttrRead;
		else if (ItemInfo::IsShield(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_ShieldEquip] = nAttrRead;
		else if (ItemInfo::IsShoes(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_ShoesEquip] = nAttrRead;
		else if (ItemInfo::IsLongcoat(nAttrRead))
			aEquips[CharacterDBAccessor::EQP_ID_CoatEquip] = nAttrRead;
	}

	CharacterDBAccessor::GetDefaultCharacterStat(aStats);
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

void LocalServer::OnRequestGameServerInfo(InPacket *iPacket)
{
	int nLoginSocketID = iPacket->Decode4();
	int nAccountID = iPacket->Decode4();
	int nWorldID = iPacket->Decode4();
	if (nWorldID != WvsWorld::GetInstance()->GetWorldInfo().nWorldID)
	{
		WvsLogger::LogRaw(WvsLogger::LEVEL_ERROR, "[WvsCenter][LocalServer::OnRequstGameServerInfo]異常：客戶端嘗試連線至不存在的頻道伺服器[WvsGame]。\n");
		return;
	}

	int nChannelID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	
	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::GameServerInfoResponse);
	oPacket.Encode4(nLoginSocketID);
	oPacket.Encode2(0);
	oPacket.Encode4(WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID)->GetExternalIP());
	oPacket.Encode2(WvsBase::GetInstance<WvsCenter>()->GetChannel(nChannelID)->GetExternalPort());

	oPacket.Encode4(nCharacterID);
	oPacket.Encode1(0);
	oPacket.Encode4(0);

	WvsWorld::GetInstance()->ClearUserTransferStatus(nCharacterID);
	SendPacket(&oPacket);
}

void LocalServer::OnRequestMigrateIn(InPacket *iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	int nChannelID = iPacket->Decode4();
	/*auto pUser = WvsWorld::GetInstance()->GetUser(nCharacterID);
	if (!pUser ||
		pUser->m_bLoggedIn == false ||
		pUser->m_bMigrating ||
		pUser->m_bMigrated ||
		pUser->m_nChannelID != nChannelID ||
		pUser->m_bInShop)
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Migration Status Suspected User ID = %d\n", nCharacterID);
	}*/

	m_sUser.insert(nCharacterID);
	auto pwUser = AllocObj(WvsWorld::WorldUser);
	pwUser->m_bInShop = false;
	pwUser->m_bMigrated = true;
	pwUser->m_nChannelID = nChannelID;
	pwUser->m_nLocalSocketSN = nClientSocketID;

	WvsWorld::GetInstance()->SetUser(
		nCharacterID,
		pwUser
	);
	OutPacket oPacket;
	oPacket.Encode2(CenterSendPacketFlag::CenterMigrateInResult);
	oPacket.Encode4(nClientSocketID);
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
	int nChannelID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();

	CharacterDBAccessor::GetInstance()->OnCharacterSaveRequest(iPacket);
	char nGameEndType = iPacket->Decode1();
	WvsLogger::LogFormat("OnRequestMigrateOut code = %d\n", (int)nGameEndType);
	m_sUser.erase(nCharacterID);

	if (nGameEndType == 1) //Transfer to another game server or to the shop.
	{
		UserTransferStatus* pStatus = AllocObj( UserTransferStatus );
		pStatus->Decode(iPacket);
		WvsWorld::GetInstance()->SetUserTransferStatus(nCharacterID, pStatus);
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
	oPacket.Encode2(CenterSendPacketFlag::TransferChannelResult);
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
	oPacket.Encode2(CenterSendPacketFlag::MigrateCashShopResult);
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

void LocalServer::OnRequestBuyCashItem(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	CharacterDBAccessor::GetInstance()->PostBuyCashItemRequest(this, nClientSocketID, nCharacterID, iPacket);
}

void LocalServer::OnRequestLoadLocker(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	CharacterDBAccessor::GetInstance()->PostLoadLockerRequest(this, nClientSocketID, nCharacterID, iPacket);
}

void LocalServer::OnReuqestUpdateCash(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	CharacterDBAccessor::GetInstance()->PostUpdateCashRequest(this, nClientSocketID, nCharacterID, iPacket);
}

void LocalServer::OnReuqestMoveLockerToSlot(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	CharacterDBAccessor::GetInstance()->PostMoveLockerToSlotRequest(this, nClientSocketID, nCharacterID, iPacket);
}

void LocalServer::OnReuqestMoveSlotToLocker(InPacket * iPacket)
{
	int nClientSocketID = iPacket->Decode4();
	int nCharacterID = iPacket->Decode4();
	CharacterDBAccessor::GetInstance()->PostMoveSlotToLockerRequest(this, nClientSocketID, nCharacterID, iPacket);
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
	}

	if (oPacket.GetPacketSize() != 0)
		SendPacket(&oPacket);
}
