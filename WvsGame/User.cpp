#include "User.h"
#include "ClientSocket.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterLevel.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_FuncKeyMapped.h"
#include "..\Database\GW_SkillRecord.h"
#include "..\Database\GW_Avatar.hpp"
#include "..\Database\GW_ItemSlotPet.h"

#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\PacketFlags\GameSrvPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\ShopPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\SummonedPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\NPCPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\FieldPacketFlags.hpp"
#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Random\Rand32.h"

#include "FieldMan.h"
#include "Portal.h"
#include "PortalMap.h"
#include "WvsGame.h"
#include "Field.h"
#include "FieldSet.h"
#include "QWUser.h"
#include "QWUInventory.h"
#include "InventoryManipulator.h"
#include "BasicStat.h"
#include "SecondaryStat.h"
#include "AttackInfo.h"
#include "NpcTemplate.h"
#include "LifePool.h"
#include "ItemInfo.h"
#include "SkillInfo.h"
#include "SkillLevelData.h"
#include "SkillEntry.h"
#include "USkill.h"
#include "CommandManager.h"
#include "QuestMan.h"
#include "QuestAct.h"
#include "ActItem.h"
#include "ExchangeElement.h"
#include "QWUQuestRecord.h"
#include "ScriptMan.h"
#include "Script.h"
#include "Pet.h"
#include "Summoned.h"
#include "SummonedPool.h"
#include "PartyMan.h"
#include "GuildMan.h"
#include "FriendMan.h"
#include "UMiniRoom.h"
#include "BackupItem.h"
#include "MiniRoomBase.h"

User::User(ClientSocket *_pSocket, InPacket *iPacket)
	: m_pSocket(_pSocket),
	m_pCharacterData(AllocObj(GA_Character)),
	  m_pBasicStat(AllocObj(BasicStat)),
	  m_pSecondaryStat(AllocObj(SecondaryStat))
{
	_pSocket->SetUser(this);
	m_pCharacterData->nAccountID = iPacket->Decode4();
	m_pCharacterData->DecodeCharacterData(iPacket, true);
	m_pFuncKeyMapped = AllocObjCtor(GW_FuncKeyMapped)(m_pCharacterData->nCharacterID);
	m_pFuncKeyMapped->Decode(iPacket);

	//Internal Stats Are Encoded Outside PostCharacterDataRequest
	m_pSecondaryStat->DecodeInternal(this, iPacket);
	UpdateAvatar();

	m_pField = (FieldMan::GetInstance()->GetField(m_pCharacterData->nFieldID));
	if (m_pField && m_pField->GetForcedReturn() != 999999999) 
	{
		auto pField = FieldMan::GetInstance()->GetField(m_pField->GetForcedReturn());
		if (pField)
		{
			auto pPortal = m_pField->GetPortalMap()->GetRandStartPoint();
			m_pCharacterData->nFieldID = pField->GetFieldID();
			m_pField = pField;
			if (pPortal)
			{
				SetPosX(pPortal->GetX());
				SetPosY(pPortal->GetY());
			}
		}
	}
		
}

User::~User()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserlock);

	auto bindT = std::bind(&User::Update, this);
	m_pUpdateTimer->Abort();

	if (m_pMiniRoom) 
		m_pMiniRoom->OnPacketBase(this, MiniRoomBase::MiniRoomRequest::rq_MiniRoom_LeaveBase, nullptr);

	if (GetFieldSet())
		GetFieldSet()->OnLeaveFieldSet(GetUserID());

	OutPacket oPacket;
	oPacket.Encode2(GameSrvSendPacketFlag::RequestMigrateOut);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(GetUserID());
	oPacket.Encode4(WvsBase::GetInstance<WvsGame>()->GetChannelID());
	m_pCharacterData->EncodeCharacterData(&oPacket, true);
	m_pFuncKeyMapped->Encode(&oPacket, true);
	if (m_nTransferStatus == TransferStatus::eOnTransferShop || m_nTransferStatus == TransferStatus::eOnTransferChannel) 
	{
		oPacket.Encode1(1); //bGameEnd
		m_pSecondaryStat->EncodeInternal(this, &oPacket);
	}
	else
		oPacket.Encode1(0); //bGameEnd, Dont decode and save the secondarystat info.
	WvsGame::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);

	//m_pField->OnLeave(this);
	RemoveSummoned(0, 0, -1);
	LeaveField();
	PartyMan::GetInstance()->OnLeave(this, false);
	GuildMan::GetInstance()->OnLeave(this);
	FriendMan::GetInstance()->OnLeave(this);

	try 
	{
		if (GetScript()) 
		{
			auto pScript = GetScript();
			pScript->Abort();
			FreeObj(pScript);
		}
	}
	catch (...) {}

	FreeObj(m_pCharacterData);
	FreeObj(m_pBasicStat);
	FreeObj(m_pSecondaryStat);
	FreeObj(m_pFuncKeyMapped);

	m_pUpdateTimer->Pause();
	FreeObj(m_pUpdateTimer);

	int nMaxPetIndex = GetMaxPetIndex();
	for (int i = 0; i < nMaxPetIndex; ++i)
		if (m_apPet[i])
			FreeObj(m_apPet[i]);
}

int User::GetUserID() const
{
	return m_pCharacterData->nCharacterID;
}

int User::GetChannelID() const
{
	return WvsBase::GetInstance<WvsGame>()->GetChannelID();
}

int User::GetAccountID() const
{
	return m_pCharacterData->nAccountID;
}

const std::string & User::GetName() const
{
	return m_pCharacterData->strName;
}

void User::SendPacket(OutPacket *oPacket)
{
	m_pSocket->SendPacket(oPacket);
}

GA_Character * User::GetCharacterData()
{
	return m_pCharacterData;
}

Field * User::GetField()
{
	return m_pField;
}

FieldSet *User::GetFieldSet()
{
	if (GetField())
		return GetField()->GetFieldSet();
	return nullptr;
}

void User::MakeEnterFieldPacket(OutPacket *oPacket)
{
	oPacket->Encode2((short)UserSendPacketFlag::UserRemote_OnMakeEnterFieldPacket);
	oPacket->Encode4(m_pCharacterData->nCharacterID);
	oPacket->EncodeStr(m_pCharacterData->strName);

	//==========Guild Info=========
	//oPacket->Encode8(0);
	oPacket->EncodeStr(m_sGuildName);
	oPacket->Encode2(m_nMarkBg);
	oPacket->Encode1(m_nMarkBgColor);
	oPacket->Encode2(m_nMark);
	oPacket->Encode1(m_nMarkColor);

	//SecondaryStat::EncodeForRemote
	m_pSecondaryStat->EncodeForRemote(oPacket, TemporaryStat::TS_Flag::GetDefault());

	oPacket->Encode2((short)m_pCharacterData->mStat->nJob);
	m_pCharacterData->EncodeAvatarLook(oPacket);

	oPacket->Encode4(
		m_pCharacterData->GetItemCount(
			GW_ItemSlotBase::CASH,
			5110000));

	//ItemEffect
	oPacket->Encode4(0);

	//PortableChair
	oPacket->Encode4(0);

	oPacket->Encode2(GetPosX());
	oPacket->Encode2(GetPosY());
	oPacket->Encode1(GetMoveAction());
	oPacket->Encode2(GetFh());

	//PetInfo
	oPacket->Encode1(0);

	//Mount::Info
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);

	//MiniRoom
	EncodeMiniRoomBalloon(oPacket, m_pMiniRoom == nullptr ? false : m_pMiniRoom->IsOpened());

	//ChatBallon
	oPacket->Encode1(0);

	EncodeCoupleInfo(oPacket);
	EncodeFriendshipInfo(oPacket);
	EncodeMarriageInfo(oPacket);

	//Berserk
	oPacket->Encode1(0);

	/*oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->EncodeStr("");
	oPacket->Encode4(0);
	oPacket->Encode1(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);
	oPacket->Encode4(0);*/
}

void User::MakeLeaveFieldPacket(OutPacket * oPacket)
{
	oPacket->Encode2(UserSendPacketFlag::UserRemote_OnMakeLeaveFieldPacket);
	oPacket->Encode4(GetUserID());
}

void User::TryParsingDamageData(AttackInfo * pInfo, InPacket * iPacket)
{
	int nDamageMobCount = pInfo->GetDamagedMobCount();
	int nDamagedCountPerMob = pInfo->GetDamageCountPerMob();
	for (int i = 0; i < nDamageMobCount; ++i)
	{
		int nObjectID = iPacket->Decode4();
		auto& ref = pInfo->m_mDmgInfo[nObjectID];
		iPacket->Decode1();
		iPacket->Decode1();
		iPacket->Decode1();
		iPacket->Decode1();
		iPacket->Decode1();
		iPacket->Decode4();
		iPacket->Decode1();
		iPacket->Decode2();
		iPacket->Decode2();

		for (int j = 0; j < nDamagedCountPerMob; ++j)
		{
			long long int nDmg = iPacket->Decode4();
			//printf("Monster %d Damage : %d\n", nObjectID, (int)nDmg);
			ref.push_back(nDmg);
		}
	}

	pInfo->m_nX = iPacket->Decode2();
	pInfo->m_nY = iPacket->Decode2();
}

AttackInfo * User::TryParsingMeleeAttack(AttackInfo* pInfo, int nType, InPacket * iPacket)
{
	pInfo->m_nType = nType;
	pInfo->m_bFieldKey = iPacket->Decode1();
	pInfo->m_bAttackInfoFlag = iPacket->Decode1();
	int nSkillID = pInfo->m_nSkillID = iPacket->Decode4();
	pInfo->m_nSLV = SkillInfo::GetInstance()->GetSkillLevel(
		m_pCharacterData,
		nSkillID,
		nullptr,
		0,
		0,
		0,
		0
	);

	if (WvsGameConstants::IsRushBombSkill(nSkillID))
		pInfo->m_pGrenade = iPacket->Decode4();

	iPacket->Decode1();
	pInfo->m_nDisplay = iPacket->Decode1();
	pInfo->m_nAttackActionType = iPacket->Decode1();
	pInfo->m_nAttackSpeed = iPacket->Decode1();

	pInfo->m_tLastAttackTime = iPacket->Decode4();
	TryParsingDamageData(pInfo, iPacket);
	return pInfo;
}

AttackInfo * User::TryParsingMagicAttack(AttackInfo* pInfo, int nType, InPacket * iPacket)
{
	pInfo->m_nType = nType;
	pInfo->m_bFieldKey = iPacket->Decode1();
	pInfo->m_bAttackInfoFlag = iPacket->Decode1();
	int nSkillID = pInfo->m_nSkillID = iPacket->Decode4();
	pInfo->m_nSLV = SkillInfo::GetInstance()->GetSkillLevel(
		m_pCharacterData,
		nSkillID,
		nullptr,
		0,
		0,
		0,
		0
	);


	if (WvsGameConstants::IsKeyDownSkill(nSkillID))
		pInfo->m_tKeyDown = iPacket->Decode4();

	iPacket->Decode1();
	pInfo->m_nDisplay = iPacket->Decode1();
	pInfo->m_nAttackActionType = iPacket->Decode1();
	pInfo->m_nAttackSpeed = iPacket->Decode1();
	pInfo->m_tLastAttackTime = iPacket->Decode4();

	TryParsingDamageData(pInfo, iPacket);
	return pInfo;
}

AttackInfo * User::TryParsingShootAttack(AttackInfo* pInfo, int nType, InPacket * iPacket)
{
	pInfo->m_nType = nType;
	pInfo->m_bFieldKey = iPacket->Decode1();
	pInfo->m_bAttackInfoFlag = iPacket->Decode1();
	int nSkillID = pInfo->m_nSkillID = iPacket->Decode4();

	pInfo->m_nSLV = SkillInfo::GetInstance()->GetSkillLevel(
		m_pCharacterData,
		nSkillID,
		nullptr,
		0,
		0,
		0,
		0
	);

	if (WvsGameConstants::IsKeyDownSkill(nSkillID))
		pInfo->m_tKeyDown = iPacket->Decode4();

	pInfo->m_nOption = iPacket->Decode1();
	pInfo->m_nDisplay = iPacket->Decode1();
	pInfo->m_nAttackActionType = iPacket->Decode1();
	pInfo->m_nAttackSpeed = iPacket->Decode1();
	pInfo->m_tLastAttackTime = iPacket->Decode4();
	pInfo->m_nSlot = iPacket->Decode2();
	int nDecCount = 0;
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	if (!QWUInventory::RawWasteItem(this, pInfo->m_nSlot, 1, aChangeLog)
		&& !QWUInventory::RawRemoveItem(this, GW_ItemSlotBase::CONSUME, pInfo->m_nSlot, 1, &aChangeLog, nDecCount, nullptr))
	{
		SendNoticeMessage("Invalid Attack.");
	}
	else
		QWUInventory::SendInventoryOperation(this, false, aChangeLog);

	pInfo->m_nCsStar = iPacket->Decode2();
	pInfo->m_nShootRange = iPacket->Decode1();

	TryParsingDamageData(pInfo, iPacket);
	return pInfo;
}

AttackInfo * User::TryParsingBodyAttack(AttackInfo* pInfo, int nType, InPacket * iPacket)
{
	return TryParsingMeleeAttack(pInfo, nType, iPacket);
}

void User::OnPacket(InPacket *iPacket)
{
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
		case UserRecvPacketFlag::User_OnStatChangeItemUseRequest:
			OnStatChangeItemUseRequest(iPacket, false);
			break;
		case UserRecvPacketFlag::User_OnStatChangeItemCancelRequest:
			OnStatChangeItemCancelRequest(iPacket);
			break;
		case UserRecvPacketFlag::User_OnUserChat:
			OnChat(iPacket);
			break;
		case UserRecvPacketFlag::User_OnUserTransferFieldRequest:
			OnTransferFieldRequest(iPacket);
			break;
		case UserRecvPacketFlag::User_OnUserTransferChannelRequest:
			OnTransferChannelRequest(iPacket);
			break;
		case UserRecvPacketFlag::User_OnUserMigrateToCashShopRequest:
			OnMigrateToCashShopRequest(iPacket);
			break;
		case UserRecvPacketFlag::User_OnUserMoveRequest:
			m_pField->OnUserMove(this, iPacket);
			break;
		case UserRecvPacketFlag::User_OnUserChangeSlotRequest:
			QWUInventory::OnChangeSlotPositionRequest(this, iPacket);
			break;
		case UserRecvPacketFlag::User_OnUserSkillUpRequest:
			USkill::OnSkillUpRequest(this, iPacket);
			break;
		case UserRecvPacketFlag::User_OnUserSkillUseRequest:
			USkill::OnSkillUseRequest(this, iPacket);
			break;
		case UserRecvPacketFlag::User_OnUserSkillCancelRequest:
			USkill::OnSkillCancelRequest(this, iPacket);
			break;
		case UserRecvPacketFlag::User_OnUserAttack_MeleeAttack:
		case UserRecvPacketFlag::User_OnUserAttack_ShootAttack:
		case UserRecvPacketFlag::User_OnUserAttack_MagicAttack:
		case UserRecvPacketFlag::User_OnUserAttack_BodyAttack:
			OnAttack(nType, iPacket);
			break;
		case UserRecvPacketFlag::User_OnSelectNpc:
			OnSelectNpc(iPacket);
			break;
		case UserRecvPacketFlag::User_OnScriptMessageAnswer:
			OnScriptMessageAnswer(iPacket);
			break;
		case UserRecvPacketFlag::User_OnQuestRequest:
			OnQuestRequest(iPacket);
			break;
		case UserRecvPacketFlag::User_OnFuncKeyMappedModified:
			OnFuncKeyMappedModified(iPacket);
			break;
		case UserRecvPacketFlag::User_OnShopRequest:
			if (m_pTradingNpc)
				m_pTradingNpc->OnShopRequest(this, iPacket);
			break;
		case UserRecvPacketFlag::User_OnItemUpgradeRequest:
			QWUInventory::OnUpgradeItemRequest(this, iPacket);
			break;
		case UserRecvPacketFlag::User_OnActivatePetRequest:
			OnActivatePetRequest(iPacket);
			break;
		case UserRecvPacketFlag::User_OnPartyRequest:
			PartyMan::GetInstance()->OnPartyRequest(this, iPacket);
			break;
		case UserRecvPacketFlag::User_OnPartyRequestRejected:
			PartyMan::GetInstance()->OnPartyRequestRejected(this, iPacket);
			break;
		case UserRecvPacketFlag::User_OnGuildRequest:
			GuildMan::GetInstance()->OnGuildRequest(this, iPacket);
			break;
		case UserRecvPacketFlag::User_OnFriendRequest:
			FriendMan::GetInstance()->OnFriendRequest(this, iPacket);
			break;
		case UserRecvPacketFlag::User_OnSendGroupMessage:
			OnSendGroupMessage(iPacket);
			break;
		case UserRecvPacketFlag::User_OnSendWhisperMessage:
			OnWhisper(iPacket);
			break;
		case UserRecvPacketFlag::User_OnMiniRoomRequest:
			UMiniRoom::OnMiniRoom(this, iPacket);
			break;
		case UserRecvPacketFlag::User_OnEntrustedShopRequest:
		{
			OutPacket oPacket;
			oPacket.Encode2(UserSendPacketFlag::UserLocal_OnEntrustedShopCheckResult);
			oPacket.Encode1(6);
			SendPacket(&oPacket);
			break;
		}
		default:
			iPacket->RestorePacket();

			//Pet Packet
			if (nType >= UserRecvPacketFlag::User_OnPetMove
				&& nType <= UserRecvPacketFlag::User_OnPetActionSpeak)
				OnPetPacket(iPacket);
			//Summoned Packet
			else if (nType >= FlagMin(SummonedRecvPacketFlag)
				&& nType <= FlagMax(SummonedRecvPacketFlag))
				OnSummonedPacket(iPacket);
			//Field Packet
			else if (m_pField)
				m_pField->OnPacket(this, iPacket);
	}
	ValidateStat();
	SendCharacterStat(false, 0);
}

void User::OnTransferFieldRequest(InPacket * iPacket)
{
	if (!m_pField)
		m_pSocket->GetSocket().close();
	iPacket->Decode1(); //ms_RTTI_CField ?
	int dwFieldReturn = iPacket->Decode4();
	std::string sPortalName = iPacket->DecodeStr();
	if (sPortalName.size() > 0)
	{
		iPacket->Decode2(); //not sure
		iPacket->Decode2(); //not sure
	}
	/*
	if(m_character.characterStat.nHP == 0)
	{
		m_basicStat->SetFrom(m_character, m_aRealEquip, m_aRealEqup2, 0, 0, 0);
		m_secondaryStat->Clear();
		....
	}
	*/
	TryTransferField(dwFieldReturn, sPortalName);
}

bool User::TryTransferField(int nFieldID, const std::string& sPortalName)
{
	std::lock_guard<std::recursive_mutex> user_lock(m_mtxUserlock);
	SetTransferStatus(TransferStatus::eOnTransferField);
	Portal* pPortal = m_pField->GetPortalMap()->FindPortal(sPortalName);
	Field *pTargetField = FieldMan::GetInstance()->GetField(
		nFieldID == -1 ?
		pPortal->GetTargetMap() :
		nFieldID
	);
	if (pTargetField != nullptr)
	{
		Portal* pTargetPortal = pPortal == nullptr ?
			pTargetField->GetPortalMap()->GetRandStartPoint() :
			pTargetField->GetPortalMap()->FindPortal(pPortal->GetTargetPortalName());

		LeaveField();

		if (GetFieldSet() && GetFieldSet() != pTargetField->GetFieldSet())
			GetFieldSet()->OnLeaveFieldSet(GetUserID());

		m_pField = pTargetField;
		PostTransferField(m_pField->GetFieldID(), pTargetPortal, false);
		m_pField->OnEnter(this);
		m_pCharacterData->nFieldID = m_pField->GetFieldID();
		SetTransferStatus(TransferStatus::eOnTransferNone);

		int nMaxPetIndex = GetMaxPetIndex();
		for (int i = 0; i < nMaxPetIndex; ++i)
			if (m_apPet[i] != nullptr)
				m_apPet[i]->OnEnterField(m_pField);

		ReregisterSummoned();
		return true;
	}
	return false;
}

void User::OnTransferChannelRequest(InPacket * iPacket)
{
	int nChannelID = iPacket->Decode1();

	if (nChannelID == WvsBase::GetInstance<WvsGame>()->GetChannelID())
	{
		//SendTransferChannelIgnored
	}
	//Check if the server is connected.
	//Check if the user can attach additional process.

	SetTransferStatus(TransferStatus::eOnTransferChannel);
	OutPacket oPacket;
	oPacket.Encode2(GameSrvSendPacketFlag::RequestTransferChannel);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(GetUserID());
	oPacket.Encode1(nChannelID);
	WvsGame::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

void User::OnMigrateToCashShopRequest(InPacket * iPacket)
{
	//Check if the server is connected.
	//Check if the user can attach additional process.
	SetTransferStatus(TransferStatus::eOnTransferShop);
	OutPacket oPacket;
	oPacket.Encode2(GameSrvSendPacketFlag::RequestTransferShop);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(GetUserID());
	WvsGame::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

void User::OnChat(InPacket *iPacket)
{
	//iPacket->Decode4(); //TIME TICK
	std::string strMsg = iPacket->DecodeStr();
	CommandManager::GetInstance()->Process(this, strMsg);

	bool bBalloon = iPacket->Decode1() != 0;

	OutPacket oPacketForBroadcasting, oPacketForSelf;

	EncodeChatMessage(&oPacketForBroadcasting, strMsg, false, bBalloon);
	EncodeChatMessage(&oPacketForSelf		 , strMsg, false, true);

	m_pField->SplitSendPacket(&oPacketForBroadcasting, nullptr);
	SendPacket(&oPacketForSelf);
}

void User::EncodeChatMessage(OutPacket *oPacket, const std::string strMsg, bool bAdmin, bool bBallon)
{
	oPacket->Encode2(UserSendPacketFlag::UserCommon_OnChat);
	oPacket->Encode4(GetUserID());
	oPacket->Encode1((char)bAdmin);
	oPacket->EncodeStr(strMsg);
	oPacket->Encode1(bBallon);
	oPacket->Encode1(0);
	oPacket->Encode1(-1);
}

void User::PostTransferField(int dwFieldID, Portal * pPortal, int bForce)
{
	OutPacket oPacket;
	oPacket.Encode2((short)GameSrvSendPacketFlag::Client_SetFieldStage); //Set Stage
	oPacket.Encode4(WvsBase::GetInstance<WvsGame>()->GetChannelID()); //nChannel

	oPacket.Encode1(1); //bCharacterData?
	oPacket.Encode1(0); // Change Stage(1) or Transfer Field(0)
	oPacket.Encode2(0);

	oPacket.Encode4(dwFieldID);
	oPacket.Encode1(pPortal == nullptr ? 0x80 : pPortal->GetID());
	oPacket.Encode2(m_pCharacterData->mStat->nHP); //HP
	oPacket.Encode1(0);
	oPacket.Encode8(GameDateTime::GetCurrentDate());
	SendPacket(&oPacket);
}

void User::SetMovePosition(int x, int y, char bMoveAction, short nFSN)
{
	SetPosX(x);
	SetPosY(y);
	SetMoveAction(bMoveAction);
	SetFh(nFSN);
}

void User::UpdateAvatar()
{
	std::lock_guard<std::mutex> lock(m_pCharacterData->GetCharacterDataLock());
	m_pCharacterData->mAvatarData->nHair = m_pCharacterData->mStat->nHair;
	m_pCharacterData->mAvatarData->nFace = m_pCharacterData->mStat->nFace;
	m_pCharacterData->mAvatarData->nSkin = m_pCharacterData->mStat->nSkin;

	m_pCharacterData->mAvatarData->mEquip.clear();
	m_pCharacterData->mAvatarData->mUnseenEquip.clear();

	GW_ItemSlotEquip* pEquip = nullptr;
	short nPOS = 0;
	for(auto pItemSlot : m_pCharacterData->mItemSlot[GW_ItemSlotBase::EQUIP])
		if (pItemSlot.second->nPOS < 0)
		{
			pEquip = (GW_ItemSlotEquip*)pItemSlot.second;
			nPOS = pEquip->nPOS * -1;
			if (nPOS < 100 || nPOS == 111)
				GetAvatar()->mEquip.insert({ pEquip->nPOS, pEquip->nItemID });
			else if (nPOS > 100)
			{
				auto iter = 
					GetAvatar()->mEquip.find(nPOS);

				if (iter != GetAvatar()->mEquip.end())
					GetAvatar()->mUnseenEquip.insert({ pEquip->nPOS, iter->second });

				GetAvatar()->mEquip.insert({ pEquip->nPOS + 100, pEquip->nItemID });
			}
			else
				GetAvatar()->mUnseenEquip.insert({ pEquip->nPOS, pEquip->nItemID });
		}
}

GW_Avatar * User::GetAvatar()
{
	return m_pCharacterData->mAvatarData;
}

void User::OnAvatarModified()
{
	UpdateAvatar();
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserRemote_OnAvatarModified);
	oPacket.Encode4(GetUserID());
	int dwAvatarModFlag = 1;
	oPacket.Encode1(dwAvatarModFlag); //m_dwAvatarModFlag
	if (dwAvatarModFlag & 1)
		this->m_pCharacterData->EncodeAvatarLook(&oPacket);
	if (dwAvatarModFlag & 2)
		oPacket.Encode1(0); //secondayStat.nSpeed
	if (dwAvatarModFlag & 4)
		oPacket.Encode1(0); //nChoco

	for (int i = 0; i < 2; ++i)
	{
		oPacket.Encode4(0);
		oPacket.Encode1((char)0xFF);
	}

	EncodeCoupleInfo(&oPacket);
	EncodeFriendshipInfo(&oPacket);
	EncodeMarriageInfo(&oPacket);

	oPacket.Encode4(0);
	oPacket.Encode4(0);
	oPacket.Encode4(0);

	m_pField->BroadcastPacket(&oPacket);
}

void User::EncodeAvatar(OutPacket *oPacket)
{
	UpdateAvatar();
	m_pCharacterData->EncodeAvatarLook(oPacket);
}

void User::EncodeCharacterData(OutPacket * oPacket)
{
	m_pCharacterData->EncodeCharacterData(oPacket, false);
}

void User::EncodeCoupleInfo(OutPacket * oPacket)
{
	oPacket->Encode1(0);
	for (int i = 0; i < 0; ++i)
	{
		//oPacket->Encode4(1);
		oPacket->Encode8(0); //liSN
		oPacket->Encode8(0); //liPairSN
		oPacket->Encode4(0); //nItemID
	}
}

void User::EncodeFriendshipInfo(OutPacket * oPacket)
{
	oPacket->Encode1(0);
	for (int i = 0; i < 0; ++i)
	{
		//oPacket->Encode4(1);
		oPacket->Encode8(0); //liSN
		oPacket->Encode8(0); //liPairSN
		oPacket->Encode4(0); //nItemID
	}
}

void User::EncodeMarriageInfo(OutPacket * oPacket)
{
	oPacket->Encode1(0);
	for (int i = 0; i < 0; ++i)
	{
		oPacket->Encode4(GetUserID()); //
		oPacket->Encode4(0); //nPairID
		oPacket->Encode4(0); //nItemID
	}
}

void User::ValidateStat(bool bCalledByConstructor)
{
	m_pBasicStat->SetFrom(m_pCharacterData, m_pSecondaryStat->nMaxHP, m_pSecondaryStat->nMaxMP, m_pSecondaryStat->nBasicStatUp);
	m_pSecondaryStat->SetFrom(m_pCharacterData, m_pBasicStat);
	long long int liFlag = 0;
	if (m_pCharacterData->mStat->nHP > m_pBasicStat->nMHP)
		liFlag |= QWUser::IncHP(this, 0, false);
	if (m_pCharacterData->mStat->nMP > m_pBasicStat->nMMP)
		liFlag |= QWUser::IncMP(this, 0, false);
	if (!bCalledByConstructor)
	{
		if (liFlag)
			SendCharacterStat(false, liFlag);
	}
}

void User::SendCharacterStat(bool bOnExclRequest, long long int liFlag)
{
	if (liFlag & BasicStat::BasicStatFlag::BS_Level)
	{
		GuildMan::GetInstance()->PostChangeLevelOrJob(this, m_pCharacterData->mLevel->nLevel, true);
		PartyMan::GetInstance()->PostChangeLevelOrJob(this, m_pCharacterData->mLevel->nLevel, true);
	}
	if(liFlag & BasicStat::BasicStatFlag::BS_Job)
	{
		GuildMan::GetInstance()->PostChangeLevelOrJob(this, m_pCharacterData->mStat->nJob, false);
		PartyMan::GetInstance()->PostChangeLevelOrJob(this, m_pCharacterData->mStat->nJob, false);
	}

	if (liFlag & BasicStat::BasicStatFlag::BS_HP)
		PostHPToPartyMembers();

	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnStatChanged);
	oPacket.Encode1((char)bOnExclRequest);
	oPacket.Encode1(0);
	oPacket.Encode4((int)liFlag);
	if (liFlag & BasicStat::BasicStatFlag::BS_Skin)
		oPacket.Encode1(m_pCharacterData->mAvatarData->nSkin);
	if (liFlag & BasicStat::BasicStatFlag::BS_Face)
		oPacket.Encode4(m_pCharacterData->mAvatarData->nFace);
	if (liFlag & BasicStat::BasicStatFlag::BS_Hair)
		oPacket.Encode4(m_pCharacterData->mAvatarData->nHair);
	if (liFlag & BasicStat::BasicStatFlag::BS_Level)
		oPacket.Encode1(m_pCharacterData->mLevel->nLevel);
	if (liFlag & BasicStat::BasicStatFlag::BS_Job)
		oPacket.Encode2(m_pCharacterData->mStat->nJob);
	if (liFlag & BasicStat::BasicStatFlag::BS_STR)
		oPacket.Encode2(m_pCharacterData->mStat->nStr);
	if (liFlag & BasicStat::BasicStatFlag::BS_DEX)
		oPacket.Encode2(m_pCharacterData->mStat->nDex);
	if (liFlag & BasicStat::BasicStatFlag::BS_INT)
		oPacket.Encode2(m_pCharacterData->mStat->nInt);
	if (liFlag & BasicStat::BasicStatFlag::BS_LUK)
		oPacket.Encode2(m_pCharacterData->mStat->nLuk);
	if (liFlag & BasicStat::BasicStatFlag::BS_HP)
		oPacket.Encode2(m_pCharacterData->mStat->nHP);
	if (liFlag & BasicStat::BasicStatFlag::BS_MaxHP)
		oPacket.Encode2(m_pCharacterData->mStat->nMaxHP);
	if (liFlag & BasicStat::BasicStatFlag::BS_MP)
		oPacket.Encode2(m_pCharacterData->mStat->nMP);
	if (liFlag & BasicStat::BasicStatFlag::BS_MaxMP)
		oPacket.Encode2(m_pCharacterData->mStat->nMaxMP);

	if (liFlag & BasicStat::BasicStatFlag::BS_AP)
		oPacket.Encode2(m_pCharacterData->mStat->nAP);

	//not done yet.
	if (liFlag & BasicStat::BasicStatFlag::BS_SP)
		oPacket.Encode2(m_pCharacterData->mStat->aSP[0]);

	if (liFlag & BasicStat::BasicStatFlag::BS_EXP)
		oPacket.Encode4((int)m_pCharacterData->mStat->nExp);
	if (liFlag & BasicStat::BasicStatFlag::BS_POP)
		oPacket.Encode2(m_pCharacterData->mStat->nPOP);
	if (liFlag & BasicStat::BasicStatFlag::BS_Meso)
		oPacket.Encode4((int)m_pCharacterData->mMoney->nMoney - m_pCharacterData->nMoneyTrading);

	if (liFlag & BasicStat::BasicStatFlag::BS_Pet)
	{
		oPacket.Encode8(0);
		oPacket.Encode8(0);
		oPacket.Encode8(0);
	}
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);

	SendPacket(&oPacket);
}

void User::SendTemporaryStatReset(TemporaryStat::TS_Flag& flag)
{
	if (flag.IsEmpty())
		return;
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnTemporaryStatReset);
	flag.Encode(&oPacket);
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	SendPacket(&oPacket);
}

void User::SendTemporaryStatSet(TemporaryStat::TS_Flag& flag, int tDelay)
{
	if (flag.IsEmpty())
		return;
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnTemporaryStatSet);
	m_pSecondaryStat->EncodeForLocal(&oPacket, flag);
	oPacket.Encode2(0);
	oPacket.Encode1(0);
	SendPacket(&oPacket);

	OutPacket oForRemote;
	oPacket.Encode2(UserSendPacketFlag::UserRemote_OnSetTemporaryStat);
	oPacket.Encode4(GetUserID());
	m_pSecondaryStat->EncodeForRemote(&oPacket, flag);
	GetField()->SplitSendPacket(&oPacket, nullptr);
}

void User::OnAttack(int nType, InPacket * iPacket)
{
	AttackInfo attackInfo, *pResult = nullptr;

	switch (nType)
	{
		case UserRecvPacketFlag::User_OnUserAttack_MeleeAttack:
			pResult = (TryParsingMeleeAttack(&attackInfo, nType, iPacket));
			break;
		case UserRecvPacketFlag::User_OnUserAttack_ShootAttack:
			pResult = (TryParsingShootAttack(&attackInfo, nType, iPacket));
			break;
		case UserRecvPacketFlag::User_OnUserAttack_MagicAttack:
			pResult = (TryParsingMagicAttack(&attackInfo, nType, iPacket));
			break;
		case UserRecvPacketFlag::User_OnUserAttack_BodyAttack:
			pResult = (TryParsingBodyAttack(&attackInfo, nType, iPacket));
			break;
	}
	if (pResult)
	{
		m_pField->GetLifePool()->OnUserAttack(
			this,
			SkillInfo::GetInstance()->GetSkillByID(pResult->m_nSkillID),
			pResult
		);
	}
}

void User::OnLevelUp()
{
}

SecondaryStat * User::GetSecondaryStat()
{
	return this->m_pSecondaryStat;
}

BasicStat * User::GetBasicStat()
{
	return this->m_pBasicStat;
}

std::recursive_mutex & User::GetLock()
{
	return m_mtxUserlock;
}

void User::Update()
{
	int tCur = GameDateTime::GetTime();
	m_pSecondaryStat->ResetByTime(this, GameDateTime::GetTime());
}

void User::ResetTemporaryStat(int tCur, int nReasonID)
{
	if (nReasonID == 0)
	{
		
	}
}

void User::SendUseSkillEffect(int nSkillID, int nSLV)
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserRemote_OnEffect);
	oPacket.Encode4(GetUserID());
	oPacket.Encode1(Effect::eEffect_OnUseSkill);
	oPacket.Encode4(nSkillID);
	oPacket.Encode1((char)nSLV);

	if (nSkillID == 1320006)
		oPacket.Encode1(1);
	else if (nSkillID == 1121001 || nSkillID == 1221001 || nSkillID == 1321001)
		oPacket.Encode1(1);

	GetField()->SplitSendPacket(&oPacket, this);
}

void User::SendLevelUpEffect()
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnEffect);
	oPacket.Encode4(GetUserID());
	oPacket.Encode1(Effect::eEffect_LevelUp);
	GetField()->SplitSendPacket(&oPacket, this);
}

void User::SendChangeJobEffect()
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnEffect);
	oPacket.Encode4(GetUserID());
	oPacket.Encode1(Effect::eEffect_ChangeJobEffect);
	GetField()->SplitSendPacket(&oPacket, this);
}

void User::OnStatChangeItemUseRequest(InPacket * iPacket, bool bByPet)
{
	int tTick = iPacket->Decode4();
	short nTI = iPacket->Decode2();
	int nItemID = iPacket->Decode4();

	auto pItem = m_pCharacterData->GetItem(2, nTI);
	auto pItemInfo = ItemInfo::GetInstance()->GetStateChangeItem(nItemID);
	if (pItem == nullptr || pItem->nItemID != nItemID || pItemInfo == nullptr)
	{
		SendCharacterStat(true, 0);
		return;
	}
	int nDecRet = 0;
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	bool bRemoveResult = QWUInventory::RawRemoveItem(
		this,
		2, // = Consume
		nTI,
		1,
		&aChangeLog,
		nDecRet,
		nullptr);
	if(!bRemoveResult || nDecRet != 1)
	{
		SendCharacterStat(true, 0);
		return;
	}
	QWUInventory::SendInventoryOperation(this, false, aChangeLog);
	auto tsFlag = pItemInfo->Apply(this, GameDateTime::GetTime(), false);
	SendTemporaryStatReset(tsFlag);
	SendTemporaryStatSet(tsFlag, 0);
}

void User::OnStatChangeItemCancelRequest(InPacket * iPacket)
{
	int nItemID = -iPacket->Decode4();
	auto pItemInfo = ItemInfo::GetInstance()->GetStateChangeItem(nItemID);
	if (pItemInfo) 
	{
		auto tsFlag = pItemInfo->Apply(this, 0, false, true);
		SendTemporaryStatReset(tsFlag);
	}
}

void User::OnMigrateOut()
{
	//LeaveField();
	m_pSocket->GetSocket().close();
}

bool User::CanAttachAdditionalProcess()
{
	return m_pSocket &&
		m_nTransferStatus == TransferStatus::eOnTransferNone &&
		m_pCharacterData->mStat->nHP &&
		!m_pScript &&
		!m_pTradingNpc;
}

void User::SetTransferStatus(TransferStatus e)
{
	m_nTransferStatus = e;
}

User::TransferStatus User::GetTransferStatus() const
{
	return m_nTransferStatus;
}

User* User::FindUser(int nUserID)
{
	return WvsGame::GetInstance<WvsGame>()->FindUser(nUserID);
}

User* User::FindUserByName(const std::string & strName)
{
	return WvsGame::GetInstance<WvsGame>()->FindUserByName(strName);
}

void User::SendDropPickUpResultPacket(bool bPickedUp, bool bIsMoney, int nItemID, int nCount, bool bOnExcelRequest)
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnMessage);
	oPacket.Encode1((char)Message::eDropPickUpMessage);
	if (bPickedUp)
	{
		oPacket.Encode1(bIsMoney == true ? 1 : 0);
		if (bIsMoney)
		{
			oPacket.Encode1(0);
			oPacket.Encode8(nCount);
			oPacket.Encode2(0);
		}
		else
		{
			oPacket.Encode4(nItemID);
			oPacket.Encode4(nCount);
		}
	}
	else
	{
		oPacket.Encode1((char)0xFF);
		oPacket.Encode4(0);
		oPacket.Encode4(0);
	}
	SendPacket(&oPacket);
}

void User::SendDropPickUpFailPacket(bool bOnExcelRequest)
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnMessage);
	oPacket.Encode1((char)Message::eDropPickUpMessage);
	oPacket.Encode1((char)0xFE);
	oPacket.Encode4(0);
	oPacket.Encode4(0);
	SendPacket(&oPacket);
}

Script * User::GetScript()
{
	return m_pScript;
}

void User::SetScript(Script * pScript)
{
	m_pScript = pScript;
}

void User::OnSelectNpc(InPacket * iPacket)
{
	int nLifeNpcID = iPacket->Decode4();
	auto pNpc = m_pField->GetLifePool()->GetNpc(nLifeNpcID);
	auto pTemplate = NpcTemplate::GetInstance()->GetNpcTemplate(pNpc->GetTemplateID());
	if (pTemplate && pTemplate->HasShop())
	{
		OutPacket oPacket;
		oPacket.Encode2((int)NPCSendPacketFlags::NPC_OnNpcShopItemList);
		pTemplate->EncodeShop(this, &oPacket);
		m_pTradingNpc = pNpc;
		SendPacket(&oPacket);
		return;
	}
	if (pNpc != nullptr && GetScript() == nullptr)
	{
		auto pScript = ScriptMan::GetInstance()->GetScript(
			"./DataSrv/Script/Npc/" + std::to_string(pNpc->GetTemplateID()) + ".lua", 
			pNpc->GetTemplateID(),
			pNpc->GetField()
		);
		if (pScript == nullptr) 
		{
			SendChatMessage(4, "Npc : " + std::to_string(pNpc->GetTemplateID()) + " has no script.");
			return;
		}
		pScript->SetUser(this);
		SetScript(pScript);
		
		pScript->Run();
		//std::thread* t = new std::thread(&Script::Run, pScript);
		//pScript->SetThread(t);
		//t->detach();
	}
}

void User::OnScriptMessageAnswer(InPacket * iPacket)
{
	if (GetScript() != nullptr)
		m_pScript->OnPacket(iPacket);
}

void User::SetTradingNpc(Npc * pNpc)
{
	m_pTradingNpc = pNpc;
}

Npc * User::GetTradingNpc()
{
	return m_pTradingNpc;
}

void User::OnQuestRequest(InPacket * iPacket)
{
	char nAction = iPacket->Decode1();
	int nQuestID = iPacket->Decode2(), nNpcID; 
	NpcTemplate* pNpcTemplate = nullptr;
	Npc* pNpc = nullptr;
	if (nAction != 0 && nAction != 3)
	{
		nNpcID = iPacket->Decode4();
		pNpc = m_pField->GetLifePool()->GetNpc(nNpcID);
		pNpcTemplate = NpcTemplate::GetInstance()->GetNpcTemplate(nNpcID);
		
		//if (pNpcTemplate == nullptr) // Invalid NPC Template
		//	return;

		/*if (!QuestMan::GetInstance()->IsAutoStartQuest(nQuestID))
		{
			pNpc = m_pField->GetLifePool()->GetNpc(nNpcID);
			if (!pNpc) // the npc not existed in this field
				return;
			auto rangeX = pNpc->GetPosX() - this->GetPosX();
			auto rangeY = pNpc->GetPosY() - this->GetPosY();

			if (abs(rangeX) > 1920 || abs(rangeY) > 1080)
			{
				WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "玩家%s疑似使用不當方式進行任務。任務 ID = %d, Npc ID = %d\n",
					this->m_pCharacterData->strName.c_str(),
					nQuestID,
					nNpcID);
				return;
			}
		}*/
	}

	WvsLogger::LogFormat("OnQuestRequest npc id = %d, quest action = %d, pNpc == nullptr ? %d\n", 
		nNpcID, 
		(int)nAction,
		(int)(pNpc == nullptr));
	switch (nAction)
	{
		case 0:
			OnLostQuestItem(iPacket, nQuestID);
			break;
		case 1:
			OnAcceptQuest(iPacket, nQuestID, nNpcID, pNpc);
			break;
		case 2:
			OnCompleteQuest(iPacket, nQuestID, nNpcID, pNpc, QuestMan::GetInstance()->IsAutoCompleteQuest(nQuestID));
			break;
		case 3:
			OnResignQuest(iPacket, nQuestID);
			break;
		case 4:
		case 5:
			{
				auto pScript = ScriptMan::GetInstance()->GetScript(
					"./DataSrv/Script/Quest/" + std::to_string(nQuestID) + ".lua",
					nNpcID,
					m_pField
				);
				if (pScript == nullptr)
				{
					SendChatMessage(0, "Quest : " + std::to_string(nQuestID) + " has no script.");
					return;
				}
				pScript->SetUser(this);
				SetScript(pScript);
				pScript->PushInteger("questID", nQuestID);

				if(nAction == 4)
					pScript->Run("start");
				else
					pScript->Run("complete");
			}
			break;
	}
}

void User::OnAcceptQuest(InPacket * iPacket, int nQuestID, int dwTemplateID, Npc * pNpc)
{
	if (!QuestMan::GetInstance()->CheckStartDemand(nQuestID, this))
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "[OnAcceptQuest]無法通過任務需求檢測，玩家名稱: %s, 任務ID: %d\n",
			m_pCharacterData->strName.c_str(),
			nQuestID);
		return;
	}
	WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "任務檢測成功。\n");
	TryQuestStartAct(nQuestID, dwTemplateID, pNpc);
}

void User::OnCompleteQuest(InPacket * iPacket, int nQuestID, int dwTemplateID, Npc * pNpc, bool bIsAutoComplete)
{
	if (!QuestMan::GetInstance()->CheckCompleteDemand(nQuestID, this))
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "[OnCompleteQuest]無法通過任務需求檢測，玩家名稱: %s, 任務ID: %d\n",
			m_pCharacterData->strName.c_str(),
			nQuestID);
		return;
	}
	WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "任務檢測成功。\n");
	TryQuestCompleteAct(nQuestID, pNpc);
}

void User::OnResignQuest(InPacket * iPacket, int nQuestID)
{
	QWUQuestRecord::Remove(this, nQuestID, QWUQuestRecord::GetState(this, nQuestID) == 2);
}

void User::OnLostQuestItem(InPacket * iPacket, int nQuestID)
{
	int tTick = iPacket->Decode4();
	int nItemID = iPacket->Decode4();

	auto pStartAct = QuestMan::GetInstance()->GetStartAct(nQuestID);
	auto& aActItem = pStartAct->aActItem;
	if (nItemID <= 0)
		return;
	for (auto& actItem : aActItem)
	{
		int nCount = actItem->nCount - GetCharacterData()->GetItemCount(
			actItem->nItemID / 1000000, actItem->nItemID);
		if (nCount > 0)
		{
			std::vector<InventoryManipulator::ChangeLog> aChangeLog;
			std::vector<ExchangeElement> aExchange;
			std::vector<BackupItem> aBackup;
			ExchangeElement exchange;
			exchange.m_nItemID = actItem->nItemID;
			exchange.m_nCount = nCount;
			QWUInventory::Exchange(
				this,
				0,
				aExchange,
				&aChangeLog,
				&aChangeLog,
				aBackup
			);
		}
	}
}

void User::TryQuestStartAct(int nQuestID, int nNpcID, Npc * pNpc)
{
	auto pStartAct = QuestMan::GetInstance()->GetStartAct(nQuestID);
	if (!pStartAct)
		return;
	TryExchange(pStartAct->aActItem);
	QWUQuestRecord::Set(this, nQuestID, pStartAct->sInfo);

	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnQuestResult);
	oPacket.Encode1(8);
	oPacket.Encode2(nQuestID);
	oPacket.Encode4(nNpcID);
	oPacket.Encode4(0);
	SendPacket(&oPacket);
}

void User::TryQuestCompleteAct(int nQuestID, Npc * pNpc)
{
	auto pCompleteAct = QuestMan::GetInstance()->GetCompleteAct(nQuestID);
	if (!pCompleteAct)
		return;
	TryExchange(pCompleteAct->aActItem);

	if (pCompleteAct->nEXP >= 0)
		QWUser::IncEXP(this, pCompleteAct->nEXP, false);
	QWUser::IncMoney(this, pCompleteAct->nMoney, false);

	QWUQuestRecord::SetComplete(this, nQuestID);
	SendQuestEndEffect();
}

void User::TryExchange(const std::vector<ActItem*>& aActItem)
{
	std::vector<std::pair<int, int>> randItem;
	int nRandWeight = 0, nSelectedItemID = 0;
	for (auto& pItem : aActItem)
	{
		if (pItem->nProp > 0 && AllowToGetQuestItem(pItem)) 
		{
			randItem.push_back({ pItem->nProp, pItem->nItemID });
			nRandWeight += pItem->nProp;
		}
	}
	if (randItem.size() > 0) 
	{
		int randPos = Rand32::GetInstance()->Random() % nRandWeight;
		for(auto& rndItem : randItem)
			if ((randPos -= rndItem.first) <= 0)
			{
				nSelectedItemID = rndItem.second;
				break;
			}
	}
	for (auto& pItem : aActItem)
	{
		if (!AllowToGetQuestItem(pItem))
			continue;
		int nItemID = pItem->nItemID;
		if (pItem->nProp != 0 && nItemID != nSelectedItemID)
			continue;
		int nCount = pItem->nCount;
		if (nCount < 0)
			QWUInventory::RawRemoveItemByID(this, nItemID, nCount);
		else
			QWUInventory::RawAddItemByID(this, nItemID, nCount);
	}
}

bool User::AllowToGetQuestItem(const ActItem * pActionItem)
{
	if (pActionItem->nGender != 2 && pActionItem->nGender >= 0 && pActionItem->nGender != this->m_pBasicStat->nGender)
		return false;
	auto lmdCheckJob = [&](int encoded, int job) 
	{
		if ((encoded & 0x1) != 0) 
		{
			if ((0) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x2) != 0) 
		{
			if ((100) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x4) != 0) 
		{
			if ((200) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x8) != 0) 
		{
			if ((300) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x10) != 0) 
		{
			if ((400) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x20) != 0) 
		{
			if ((500) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x400) != 0) 
		{
			if ((1000) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x800) != 0) 
		{
			if ((1100) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x1000) != 0)
		{
			if ((1200) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x2000) != 0) 
		{
			if ((1300) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x4000) != 0) 
		{
			if ((1400) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x8000) != 0)
		{
			if ((1500) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x20000) != 0) 
		{
			if ((2001) / 100 == job / 100)
				return true;
			if ((2200) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x100000) != 0) 
		{
			if ((2000) / 100 == job / 100)
				return true;
			if ((2001) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x200000) != 0) 
		{
			if ((2100) / 100 == job / 100)
				return true;
		}
		if ((encoded & 0x400000) != 0) 
		{
			if ((2001) / 100 == job / 100)
				return true;
			if ((2200) / 100 == job / 100)
				return true;
		}

		if ((encoded & 0x40000000) != 0)
		{
			if ((3000) / 100 == job / 100)
				return true;
			if ((3200) / 100 == job / 100)
				return true;
			if ((3300) / 100 == job / 100)
				return true;
			if ((3500) / 100 == job / 100)
				return true;
		}
		return false;
	};
	auto lmdCheckJobEx = [&](int encoded, int job) 
	{
		if ((encoded & 0x1) != 0) 
		{
			if (((200) / 100) % 10 == (job / 100) % 10)
				return true;
		}
		if ((encoded & 0x2) != 0) 
		{
			if (((300) / 100) % 10 == (job / 100) % 10)
				return true;
		}
		if ((encoded & 0x4) != 0) 
		{
			if (((400) / 100) % 10 == (job / 100) % 10)
				return true;
		}
		if ((encoded & 0x8) != 0) 
		{
			if (((500) / 100) % 10 == (job / 100) % 10)
				return true;
		}
		return false;
	};
	if (pActionItem->nJob > 0) 
	{
		if (!lmdCheckJob(pActionItem->nJob, m_pBasicStat->nJob)
			&& !lmdCheckJobEx(pActionItem->nJobEx, m_pBasicStat->nJob))
			return false;
	}
	return true;
}

void User::SendQuestResult(int nResult, int nQuestID, int dwTemplateID)
{
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketFlag::UserLocal_OnMessage);
	oPacket.Encode1(nResult);
	oPacket.Encode2(nQuestID);
	oPacket.Encode4(dwTemplateID);
	oPacket.Encode4(0);
	oPacket.Encode1(0);
	SendPacket(&oPacket);
}

void User::SendQuestEndEffect()
{
	OutPacket oPacketLocal;
	oPacketLocal.Encode2(UserSendPacketFlag::UserLocal_OnEffect);
	oPacketLocal.Encode1(Effect::eEffect_QuestCompleteEffect);
	SendPacket(&oPacketLocal);

	OutPacket oPacketRemote;
	oPacketRemote.Encode2(UserSendPacketFlag::UserLocal_OnEffect);
	oPacketRemote.Encode4(GetUserID());
	oPacketRemote.Encode1(Effect::eEffect_QuestCompleteEffect);
	GetField()->SplitSendPacket(&oPacketRemote, this);
}

void User::SendChatMessage(int nType, const std::string & sMsg)
{
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketFlag::UserLocal_OnBroadcastMsg);
	oPacket.Encode1((char)nType);
	if (nType == 4)
		oPacket.Encode1(1);
	oPacket.EncodeStr(sMsg);
	SendPacket(&oPacket);
}

void User::SendNoticeMessage(const std::string & sMsg)
{
	SendChatMessage(1, sMsg);
}

void User::SendFuncKeyMapped()
{
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketFlag::User_OnFuncKeyMapped);
	m_pFuncKeyMapped->Encode(&oPacket, false);
	SendPacket(&oPacket);
}

void User::OnFuncKeyMappedModified(InPacket * iPacket)
{
	int nType = iPacket->Decode4();
	if (nType == 0)
	{
		int nToModify = iPacket->Decode4(), nKey = 0;
		for (int i = 0; i < nToModify; ++i)
		{
			nKey = iPacket->Decode4();
			if (nKey != -1 && nKey < GW_FuncKeyMapped::TOTAL_KEY_NUM)
			{
				auto& ref = m_pFuncKeyMapped->m_mKeyMapped[nKey];
				ref.nType = iPacket->Decode1();
				ref.nValue = iPacket->Decode4();
				ref.bModified = true;
			}
		}
	}
	else
	{
		OutPacket oPacket;
		int nItemID = iPacket->Decode4();
		if (m_pCharacterData->GetItemCount(GW_ItemSlotBase::CONSUME, nItemID) == 0)
			nItemID = 0;

		oPacket.Encode2((short)UserSendPacketFlag::User_OnFuncKeyMapped + std::min(nType, 2));
		oPacket.Encode4(nItemID);
		SendPacket(&oPacket);
	}
	//SendFuncKeyMapped();
}

void User::OnPetPacket(InPacket * iPacket)
{
	int nIndex = iPacket->Decode4();
	if (nIndex < 0 || nIndex >= GetMaxPetIndex())
		return;
	auto pPet = m_apPet[nIndex];
	if (pPet != nullptr)
		pPet->OnPacket(iPacket);
}

void User::ActivatePet(int nPos, int nRemoveReaseon, bool bOnInitialize)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserlock);
	int nAvailableIdx = -1;
	int nMaxIndex = GetMaxPetIndex();
	GW_ItemSlotPet *pPetSlot = nullptr;
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	for (int i = 0; i < nMaxIndex; ++i)
		if (m_apPet[i] && m_apPet[i]->m_pPetSlot->nPOS == nPos)
		{
			pPetSlot = m_apPet[i]->m_pPetSlot;
			pPetSlot->nActiveState = 0;
			m_apPet[i]->OnLeaveField();
			FreeObj(m_apPet[i]);
			m_apPet[i] = nullptr;
			nAvailableIdx = -1;
			break;
		}
		else if (nAvailableIdx == -1 && m_apPet[i] == nullptr)
			nAvailableIdx = i;
	if (nAvailableIdx >= 0 && nPos > 0)
	{
		auto pItem = GetCharacterData()->GetItem(GW_ItemSlotBase::CASH, nPos);
		if (!pItem || pItem->bIsPet == false)
			return;
		pPetSlot = (GW_ItemSlotPet*)pItem;
		if (nAvailableIdx != -1)
		{
			m_apPet[nAvailableIdx] = AllocObjCtor(Pet)(pPetSlot);
			pPetSlot->nActiveState = 1;
			m_apPet[nAvailableIdx]->SetIndex(nAvailableIdx);
			m_apPet[nAvailableIdx]->Init(this);
			m_apPet[nAvailableIdx]->OnEnterField(m_pField);
		}
		else
			pPetSlot->nActiveState = 0;
	}
	if (pPetSlot)
	{
		InventoryManipulator::InsertChangeLog(
			aChangeLog,
			InventoryManipulator::Change_RemoveFromSlot,
			GW_ItemSlotBase::CASH, nPos, pPetSlot, 0, 0
		);
		InventoryManipulator::InsertChangeLog(
			aChangeLog,
			InventoryManipulator::Change_AddToSlot,
			GW_ItemSlotBase::CASH, nPos, pPetSlot->MakeClone(), 0, 0
		);
		QWUInventory::SendInventoryOperation(this, true, aChangeLog);
	}
	SendCharacterStat(true, 0);
}

int User::GetMaxPetIndex()
{
	//Should check how many pets the user can spawn
	return MAX_PET_INDEX;
}

void User::OnActivatePetRequest(InPacket * iPacket)
{
	iPacket->Decode4();
	int nPos = iPacket->Decode1();
	bool bLead = iPacket->Decode1() == 1;
	ActivatePet(nPos, 0, 0);
}

void User::OnSummonedPacket(InPacket * iPacket)
{
	int nFieldObjID = iPacket->Decode4();
	auto pSummoned = m_pField->GetSummonedPool()->GetSummoned(nFieldObjID);
	if (pSummoned)
		pSummoned->OnPacket(iPacket);
}

void User::ReregisterSummoned()
{
	for (auto& pSummoned : m_lSummoned)
		m_pField->GetSummonedPool()->CreateSummoned(this, pSummoned, m_ptPos);
}

void User::CreateSummoned(const SkillEntry * pSkill, int nSLV, const FieldPoint & pt, bool bMigrate)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserlock);
	if (!m_pField)
	{
		m_aMigrateSummoned.push_back(pSkill->GetSkillID());
		return;
	}
	auto pSummoned = m_pField->GetSummonedPool()->CreateSummoned(
		this, pSkill->GetSkillID(), nSLV, m_ptPos
	);
	if (pSummoned)
		m_lSummoned.push_back(pSummoned);
}

//nForceRemoveSkillID = -1 means that remove all summoneds.
void User::RemoveSummoned(int nSkillID, int nLeaveType, int nForceRemoveSkillID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserlock);
	if (nForceRemoveSkillID == -1 || nForceRemoveSkillID != 0)
	{
		int nSummoned = (int)m_lSummoned.size();
		for (int i = 0; i < nSummoned; ++i)
		{
			if (nForceRemoveSkillID == -1 || 
				m_lSummoned[i]->GetSkillID() == nForceRemoveSkillID)
			{
				m_lSummoned.erase(m_lSummoned.begin() + i);
				m_pField->GetSummonedPool()->RemoveSummoned(
					GetUserID(),
					nForceRemoveSkillID,
					0
				);
				break;
			}
		}
	}
	if (m_pField)
		m_pField->GetSummonedPool()->RemoveSummoned(GetUserID(), nSkillID, nLeaveType);
}

void User::AddPartyInvitedCharacterID(int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserlock);
	m_snPartyInvitedCharacterID.insert(nCharacterID);
}

bool User::IsPartyInvitedCharacterID(int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserlock);

	return m_snPartyInvitedCharacterID.find(nCharacterID) 
		!= m_snPartyInvitedCharacterID.end();
}

void User::RemovePartyInvitedCharacterID(int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserlock);
	m_snPartyInvitedCharacterID.erase(nCharacterID);
}

void User::ClearPartyInvitedCharacterID()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserlock);
	m_snPartyInvitedCharacterID.clear();
}

void User::SetPartyID(int nPartyID)
{
	m_nPartyID = nPartyID;
}

int User::GetPartyID() const
{
	return m_nPartyID;
}

void User::PostHPToPartyMembers()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserlock);

	if (GetPartyID() == -1)
		return;
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserRemote_OnReceiveHP);
	oPacket.Encode4(GetUserID());
	oPacket.Encode4((int)QWUser::GetHP(this));
	oPacket.Encode4((int)m_pCharacterData->mStat->nMaxHP);
	oPacket.GetSharedPacket()->ToggleBroadcasting();

	auto pParty = PartyMan::GetInstance()->GetPartyByCharID(GetUserID());
	User *pUser = nullptr;
	if (pParty)
	{
		int anCharacterID[PartyMan::MAX_PARTY_MEMBER_COUNT]{ 0 };
		PartyMan::GetInstance()->GetSnapshot(pParty->nPartyID, anCharacterID);

		for (auto& nID : anCharacterID)
		{
			pUser = User::FindUser(nID);
			if (pUser && pUser != this && pUser->GetField() == GetField()) 
				pUser->SendPacket(&oPacket);
		}
	}
}

void User::AddGuildInvitedCharacterID(int nCharacterID)
{
	m_snGuildInvitedCharacterID.insert(nCharacterID);
}

const std::set<int>& User::GetGuildInvitedCharacterID() const
{
	return m_snGuildInvitedCharacterID;
}

void User::RemoveGuildInvitedCharacterID(int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserlock);
	m_snGuildInvitedCharacterID.erase(nCharacterID);
}

void User::ClearGuildInvitedCharacterID()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserlock);
	m_snGuildInvitedCharacterID.clear();
}

const std::string& User::GetGuildName() const
{
	return m_sGuildName;
}

void User::SetGuildName(const std::string & strName)
{
	m_sGuildName = strName;
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserRemote_OnGuildNameChanged);
	oPacket.Encode4(GetUserID());
	oPacket.EncodeStr(m_sGuildName);

	GetField()->SplitSendPacket(
		&oPacket, nullptr
	);
}

void User::SetGuildMark(int nMarkBg, int nMarkBgColor, int nMark, int nMarkColor)
{
	m_nMarkBg = nMarkBg;
	m_nMarkBgColor = nMarkBgColor;
	m_nMark = nMark;
	m_nMarkColor = nMarkColor;

	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserRemote_OnGuildMarkChanged);
	oPacket.Encode4(GetUserID());
	oPacket.Encode2(m_nMarkBg);
	oPacket.Encode1(m_nMarkBgColor);
	oPacket.Encode2(m_nMark);
	oPacket.Encode1(m_nMarkColor);

	GetField()->SplitSendPacket(
		&oPacket, nullptr
	);
}

void User::OnSendGroupMessage(InPacket * iPacket)
{
	int nType = (int)(unsigned char)iPacket->Decode1();

	if (nType == 1 && GetPartyID() == -1)
		return;
	else if (nType == 2 && GetGuildName() == "")
		return;
	else if (nType > 2)
		return;

	int nReceiverNum = (int)iPacket->Decode1();

	//Send from remote server.
	OutPacket oPacket;
	oPacket.Encode2(GameSrvSendPacketFlag::GroupMessage);
	oPacket.Encode4(GetUserID());
	oPacket.Encode1((char)nType);
	oPacket.Encode1(nReceiverNum);
	for (int i = 0; i < nReceiverNum; ++i)
		oPacket.Encode4(iPacket->Decode4());

	oPacket.Encode2(FieldSendPacketFlag::Field_OnGroupMessage);
	oPacket.Encode1(nType);
	oPacket.EncodeStr(GetName());
	oPacket.EncodeStr(iPacket->DecodeStr());

	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

void User::OnWhisper(InPacket * iPacket)
{
	int nType = iPacket->Decode1();
	if (nType == WhisperResult::e_Whisper_Type_QueryLocation || 
		nType == WhisperResult::e_Whisper_Type_SendMessage) //5 = Require location info. 6 = Whipser
	{
		std::string strWhisperName = iPacket->DecodeStr();
		std::string strWhisperMsg = "";
		if (nType == WhisperResult::e_Whisper_Type_SendMessage && ((strWhisperMsg = iPacket->DecodeStr()).size() > 90))
			return;

		auto pUser = User::FindUserByName(strWhisperName);
		//User exists in local server.
		if(pUser)
		{
			OutPacket oPacketReply;
			oPacketReply.Encode2(FieldSendPacketFlag::Field_OnWhisper);
			if (nType == WhisperResult::e_Whisper_Type_QueryLocation)
			{
				//Reply to sender.
				oPacketReply.Encode1(WhisperResult::e_Whisper_Res_QuerySuccess);
				oPacketReply.EncodeStr(strWhisperName);
				oPacketReply.Encode1(WhisperResult::e_Whisper_QR_FieldID);
				oPacketReply.Encode4(pUser->GetField()->GetFieldID());
				oPacketReply.Encode4(0);
				oPacketReply.Encode4(0);
				SendPacket(&oPacketReply);
			}
			else
			{
				//Reply to sender.
				oPacketReply.Encode1(WhisperResult::e_Whisper_Res_Message_Ack);
				oPacketReply.EncodeStr(strWhisperName);
				oPacketReply.Encode1(1);
				SendPacket(&oPacketReply);

				//Send to the targeted user.
				OutPacket oPacketSend;
				oPacketSend.Encode2(FieldSendPacketFlag::Field_OnWhisper);
				oPacketSend.Encode1(WhisperResult::e_Whisper_Res_Message_Send);
				oPacketSend.EncodeStr(GetName());
				oPacketSend.Encode2(GetChannelID());
				oPacketSend.EncodeStr(strWhisperMsg);
				pUser->SendPacket(&oPacketSend);
			}
		}
		//Search target from Center server.
		else
		{
			OutPacket oPacket;
			oPacket.Encode2(GameSrvSendPacketFlag::WhisperMessage);
			oPacket.Encode4(GetUserID());
			oPacket.EncodeStr(GetName());
			oPacket.Encode1(nType);
			oPacket.EncodeStr(strWhisperName);

			if (nType == WhisperResult::e_Whisper_Type_SendMessage) //Whisper
				oPacket.EncodeStr(strWhisperMsg);

			WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
		}
	}
}

MiniRoomBase *User::GetMiniRoom()
{
	return m_pMiniRoom;
}

void User::SetMiniRoom(MiniRoomBase *pMiniRoom)
{
	m_pMiniRoom = pMiniRoom;
}

void User::SetMiniRoomBalloon(bool bOpen)
{
	if (GetField() && m_pMiniRoom)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxUserlock);
		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketFlag::UserCommon_OnMiniRoomBalloon);
		oPacket.Encode4(GetUserID());
		EncodeMiniRoomBalloon(&oPacket, bOpen);
		GetField()->SplitSendPacket(&oPacket, nullptr);
	}
}

void User::EncodeMiniRoomBalloon(OutPacket *oPacket, bool bOpen)
{
	oPacket->Encode1(bOpen ? m_pMiniRoom->GetType() : 0);
	if (bOpen)
	{
		oPacket->Encode4(m_pMiniRoom->GetMiniRoomSN());
		oPacket->EncodeStr(m_pMiniRoom->GetTitle());
		oPacket->Encode1(m_pMiniRoom->IsPrivate());
		oPacket->Encode1(m_pMiniRoom->GetMiniRoomSpec());
		oPacket->Encode1(m_pMiniRoom->GetCurUsers());
		oPacket->Encode1(m_pMiniRoom->GetMaxUsers());
		oPacket->Encode1(m_pMiniRoom->IsGameOn());
	}
}

bool User::HasOpenedEntrustedShop() const
{
	return m_bHasOpenedEntrustedShop;
}

void User::SendQuestRecordMessage(int nKey, int nState, const std::string & sStringRecord)
{
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketFlag::UserLocal_OnMessage);
	oPacket.Encode1((char)Message::eQuestRecordMessage);
	oPacket.Encode2(nKey);
	oPacket.Encode1(nState);
	switch (nState)
	{
		case 0:
			oPacket.Encode1(0);
			break;
		case 1:
			oPacket.EncodeStr(sStringRecord);
			break;
		case 2:
			oPacket.Encode8(GameDateTime::GetTime());
			break;
	}
	SendPacket(&oPacket);
}

void User::LeaveField()
{
	int nMaxPetIndex = GetMaxPetIndex();
	for (int i = 0; i < nMaxPetIndex; ++i)
		if (m_apPet[i])
			m_apPet[i]->OnLeaveField();

	for (auto& pSummoned : m_lSummoned)
		RemoveSummoned(pSummoned->GetSkillID(), Summoned::eLeave_TransferField, 0);

	m_pField->OnLeave(this);
}

void User::OnMigrateIn()
{
	SendCharacterStat(false, 0);
	SendFuncKeyMapped();
	m_pField->OnEnter(this);
	auto bindT = std::bind(&User::Update, this);
	auto pUpdateTimer = AsyncScheduler::CreateTask(bindT, 2000, true);
	m_pUpdateTimer = pUpdateTimer;
	pUpdateTimer->Start();
	SetTransferStatus(TransferStatus::eOnTransferNone);
	QWUQuestRecord::ValidMobCountRecord(this);

	for (auto& pCashItem : m_pCharacterData->mItemSlot[GW_ItemSlotBase::CASH])
	{
		if (pCashItem.second->bIsPet &&
			((GW_ItemSlotPet*)(pCashItem.second))->nActiveState == 1)
				ActivatePet(pCashItem.second->nPOS, 0, true);
	}

	for (auto& nSummonedSkill : m_aMigrateSummoned)
	{
		auto pSkillRecord = m_pCharacterData->GetSkill(nSummonedSkill);
		CreateSummoned(
			SkillInfo::GetInstance()->GetSkillByID(nSummonedSkill),
			pSkillRecord->nSLV,
			m_ptPos,
			true);
	}
	m_aMigrateSummoned.clear();

	OutPacket oPacket;
	oPacket.Encode2(GameSrvSendPacketFlag::PartyRequest);
	oPacket.Encode1(PartyMan::PartyRequest::rq_Party_Load);
	oPacket.Encode4(GetUserID());
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);

	OutPacket oPacketForGuildLoading;
	oPacketForGuildLoading.Encode2(GameSrvSendPacketFlag::GuildRequest);
	oPacketForGuildLoading.Encode1(GuildMan::GuildRequest::rq_Guild_Load);
	oPacketForGuildLoading.Encode4(GetUserID());
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacketForGuildLoading);

	OutPacket oPacketForFriendLoading;
	oPacketForFriendLoading.Encode2(GameSrvSendPacketFlag::FriendRequest);
	oPacketForFriendLoading.Encode1(FriendMan::FriendRequest::rq_Friend_Load);
	oPacketForFriendLoading.Encode4(GetUserID());
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacketForFriendLoading);
}
