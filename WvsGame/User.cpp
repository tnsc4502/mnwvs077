#include "User.h"
#include "ClientSocket.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterLevel.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_FuncKeyMapped.h"
#include "..\Database\GW_SkillRecord.h"
#include "..\Database\GW_Avatar.hpp"
#include "..\Database\GW_ItemSlotBase.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_ItemSlotPet.h"

#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"

#include "..\WvsGame\UserPacketTypes.hpp"
#include "..\WvsShop\ShopPacketTypes.hpp"
#include "..\WvsGame\SummonedPacketTypes.hpp"
#include "..\WvsGame\NpcPacketTypes.hpp"
#include "..\WvsGame\FieldPacketTypes.hpp"
#include "..\WvsCenter\CenterPacketTypes.hpp"

#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\String\StringPool.h"
#include "..\WvsCenter\EntrustedShopMan.h"

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
#include "Trunk.h"
#include "MobSkillEntry.h"
#include "MobSkillLevelData.h"
#include "AffectedAreaPool.h"
#include "CalcDamage.h"
#include "MobStat.h"
#include "MobTemplate.h"
#include "QWUSkillRecord.h"
#include "StoreBank.h"
#include "Reward.h"
#include "DropPool.h"
#include "WvsPhysicalSpace2D.h"
#include "AdminSkills.h"
#include "UserCashItemImpl.h"

#define REGISTER_TS_BY_MOB(name, value) \
tsFlag |= GET_TS_FLAG(##name); \
auto *pRef = &m_pSecondaryStat->m_mSetByTS[TemporaryStat::TS_##name]; pRef->second.clear();\
m_pSecondaryStat->n##name##_ = bResetBySkill ? 0 : value;\
m_pSecondaryStat->r##name##_ = bResetBySkill ? 0 : nSkillID | (nSLV << 16);\
m_pSecondaryStat->t##name##_ = bResetBySkill ? 0 : nDuration;\
m_pSecondaryStat->nLv##name##_ =  bResetBySkill ? 0 : nSLV;\
m_pSecondaryStat->m_tsFlagSet ^= TemporaryStat::TS_##name;\
if(!bResetBySkill)\
{\
	m_pSecondaryStat->m_tsFlagSet |= TemporaryStat::TS_##name;\
	pRef->first = bForcedSetTime ? nForcedSetTime : (GameDateTime::GetTime() + nDuration);\
	pRef->second.push_back(&m_pSecondaryStat->n##name##_);\
	pRef->second.push_back(&m_pSecondaryStat->r##name##_);\
	pRef->second.push_back(&m_pSecondaryStat->t##name##_);\
	pRef->second.push_back(&m_pSecondaryStat->nLv##name##_);\
} else { m_pSecondaryStat->m_mSetByTS.erase(TemporaryStat::TS_##name); }

User::User(ClientSocket *pSocket, InPacket *iPacket)
	: 
	m_pSocket(pSocket),
	m_pCharacterData(AllocObj(GA_Character)),
	m_pBasicStat(AllocObj(BasicStat)),
	m_pSecondaryStat(AllocObj(SecondaryStat)),
	m_pCalcDamage(AllocObjCtor(CalcDamage)(this))
{
	pSocket->SetUser(this);
	m_pCharacterData->nAccountID = iPacket->Decode4();
	m_pCharacterData->DecodeCharacterData(iPacket, true);
	m_pFuncKeyMapped.reset(MakeUnique<GW_FuncKeyMapped>(m_pCharacterData->nCharacterID));
	m_pFuncKeyMapped->Decode(iPacket);

	m_pField = (FieldMan::GetInstance()->GetField(m_pCharacterData->nFieldID));
	if (m_pField && m_pField->GetForcedReturn() && m_pField->GetForcedReturn() != 999999999)
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
	SendSetFieldPacket(true);

	//Internal Stats Are Encoded Outside PostCharacterDataRequest
	DecodeInternal(iPacket);
	UpdateAvatar();
	OnMigrateIn();
}

void User::EncodeCharacterDataInternal(OutPacket *oPacket)
{
	m_pCharacterData->EncodeCharacterData(oPacket, true);
	m_pFuncKeyMapped->Encode(oPacket, true);
}

void User::FlushCharacterData()
{
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::FlushCharacterData);
	EncodeCharacterDataInternal(&oPacket);
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

User::~User()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	m_pUpdateTimer->Pause();

	if (m_pMiniRoom) 
		m_pMiniRoom->OnPacketBase(this, MiniRoomBase::MiniRoomRequest::rq_MiniRoom_LeaveBase, nullptr);

	if (GetFieldSet())
		GetFieldSet()->OnLeaveFieldSet(GetUserID());

	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::RequestMigrateOut);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(GetUserID());
	oPacket.Encode4(WvsBase::GetInstance<WvsGame>()->GetChannelID());
	EncodeCharacterDataInternal(&oPacket);
	if (m_nTransferStatus == TransferStatus::eOnTransferShop || m_nTransferStatus == TransferStatus::eOnTransferChannel) 
	{
		oPacket.Encode1(1); //bGameEnd
		EncodeInternal(&oPacket);
	}
	else
		oPacket.Encode1(0); //bGameEnd, Dont decode and save the secondarystat info.
	WvsGame::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	GetField()->GetSummonedPool()->RemoveSummoned(GetUserID(), -1, 0);
	LeaveField();
	PartyMan::GetInstance()->OnLeave(this, false);
	GuildMan::GetInstance()->OnLeave(this);
	FriendMan::GetInstance()->OnLeave(this);
	try 
	{
		if (GetScript()) 
			GetScript()->Abort();
	}
	catch (...) {}
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

ZUniquePtr<GA_Character>& User::GetCharacterData()
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
	oPacket->Encode2((short)UserSendPacketType::UserRemote_OnMakeEnterFieldPacket);
	oPacket->Encode4(m_pCharacterData->nCharacterID);
	oPacket->EncodeStr(m_pCharacterData->strName);
	oPacket->EncodeStr(m_sGuildName);
	oPacket->Encode2(m_nMarkBg);
	oPacket->Encode1(m_nMarkBgColor);
	oPacket->Encode2(m_nMark);
	oPacket->Encode1(m_nMarkColor);
	m_pSecondaryStat->EncodeForRemote(oPacket, m_pSecondaryStat->m_tsFlagSet);
	oPacket->Encode2((short)m_pCharacterData->mStat->nJob);
	m_pCharacterData->EncodeAvatarLook(oPacket);
	oPacket->Encode4(m_pCharacterData->GetItemCount(GW_ItemSlotBase::CASH, 5110000));
	oPacket->Encode4(m_pCharacterData->nActiveEffectItemID);
	oPacket->Encode4(m_nActivePortableChairID);
	oPacket->Encode2(GetPosX());
	oPacket->Encode2(GetPosY());
	oPacket->Encode1(GetMoveAction());
	oPacket->Encode2(GetFh());
	for (int i = 0; i < GetMaxPetIndex(); ++i)
		if (m_apPet[i])
			m_apPet[i]->EncodeInitData(oPacket);
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
	oPacket->Encode1(m_nTeamForMCarnival);

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
	oPacket->Encode2(UserSendPacketType::UserRemote_OnMakeLeaveFieldPacket);
	oPacket->Encode4(GetUserID());
}

void User::TryParsingDamageData(AttackInfo *pInfo, InPacket *iPacket, int nDamageMobCount, int nDamagedCountPerMob)
{
	//int nDamageMobCount = pInfo->GetDamagedMobCount();
	//int nDamagedCountPerMob = pInfo->GetDamageCountPerMob();
	int nAction = 0;
	for (int i = 0; i < nDamageMobCount; ++i)
	{
		int nObjectID = iPacket->Decode4();
		auto& ref = pInfo->m_mDmgInfo[nObjectID];
		pInfo->m_apDmgInfo.push_back(&ref);
		ref.nDamageCount = nDamagedCountPerMob;
		ref.nMobID = nObjectID;
		ref.nHitAction = iPacket->Decode1();
		nAction = iPacket->Decode1();
		ref.nForeAction = (nAction & 0x7F);
		ref.bLeft = (nAction >> 7) & 1;
		ref.nFrameIdx = iPacket->Decode1();
		iPacket->Decode1();
		ref.ptHit.x = iPacket->Decode2();
		ref.ptHit.y = iPacket->Decode2();
		ref.ptPosPrev.x = iPacket->Decode2();
		ref.ptPosPrev.y = iPacket->Decode2();

		if (pInfo->m_nSkillID == 4211006)
		{
			ref.nDamageCount = std::min(16, (int)iPacket->Decode1());
			for (int j = 0; j < ref.nDamageCount; ++j)
				ref.anDamageClient[j] = (int)iPacket->Decode4();
		}
		else
		{
			ref.m_tDelay = iPacket->Decode2();
			for (int j = 0; j < nDamagedCountPerMob; ++j)
				ref.anDamageClient[j] = (int)iPacket->Decode4();
		}
	}
	pInfo->m_nX = iPacket->Decode2();
	pInfo->m_nY = iPacket->Decode2();
	if (pInfo->m_nSkillID == 4211006)
	{
		int nDropCount = iPacket->Decode1();
		for (int i = 0; i < nDropCount; ++i) 
		{
			pInfo->anDropID.push_back(iPacket->Decode4());
			iPacket->Decode1();
		}
	}
}

AttackInfo* User::TryParsingAttackInfo(AttackInfo *pInfo, int nType, InPacket *iPacket)
{
	pInfo->m_nType = nType;
	pInfo->m_bFieldKey = iPacket->Decode1();
	pInfo->m_bDamageInfoFlag = iPacket->Decode1();
	pInfo->m_nDamagePerMob = pInfo->m_bDamageInfoFlag & 0xF;
	int nSkillID = pInfo->m_nSkillID = iPacket->Decode4();
	auto pSkillEntry = SkillInfo::GetInstance()->GetSkillByID(nSkillID);
	if (nSkillID && !pSkillEntry)
		return nullptr;

	pInfo->m_nSLV = !nSkillID ? 0 : std::min(
		pSkillEntry->GetMaxLevel(),
		SkillInfo::GetInstance()->GetSkillLevel(
			m_pCharacterData,
			nSkillID,
			nullptr
	));
	if (nSkillID && !pInfo->m_nSLV)
		return nullptr;

	if (nSkillID == 5201002)
		pInfo->m_pGrenade = iPacket->Decode4();
	else if (SkillInfo::IsKeyDownSkill(nSkillID))
		pInfo->m_tKeyDown = iPacket->Decode4();

	pInfo->m_nOption = iPacket->Decode1();
	pInfo->m_nDisplay = iPacket->Decode1();
	pInfo->m_nAction = pInfo->m_nDisplay & 0x7F;
	pInfo->m_nAttackActionType = iPacket->Decode1();
	int nAttackInfo = iPacket->Decode1();
	pInfo->m_nAttackSpeed = nAttackInfo & 0xF;
	pInfo->m_nPartyCount = nAttackInfo >> 4;
	pInfo->m_tLastAttackTime = iPacket->Decode4();

	if (nType == UserRecvPacketType::User_OnUserAttack_ShootAttack)
	{
		pInfo->m_nSlot = iPacket->Decode2();
		int nDecCount = 0;
		auto& pBullet = m_pCharacterData->GetItem(GW_ItemSlotBase::CONSUME, pInfo->m_nSlot);
		if (pBullet && (ItemInfo::IsRechargable(pBullet->nItemID) || pBullet->nItemID / 10000 == 206))
			pInfo->m_nBulletItemID = pBullet->nItemID;

		if (!GetSecondaryStat()->nSoulArrow_ && !GetSecondaryStat()->nAttract_)
		{
			auto pLevel = pSkillEntry ? pSkillEntry->GetLevelData(pInfo->m_nSLV) : nullptr;
			int nBulletConsume = pLevel ? pLevel->m_nBulletCount : 1;
			//Try Consume Item.
			std::vector<InventoryManipulator::ChangeLog> aChangeLog;
			if (!pBullet || (ItemInfo::IsRechargable(pBullet->nItemID) && !QWUInventory::RawWasteItem(this, pInfo->m_nSlot, std::max(1, nBulletConsume), aChangeLog))
				|| (!ItemInfo::IsRechargable(pBullet->nItemID) && !QWUInventory::RawRemoveItem(this, GW_ItemSlotBase::CONSUME, pInfo->m_nSlot, std::max(1, nBulletConsume), &aChangeLog, nDecCount, nullptr)))
			{
				SendNoticeMessage("Invalid Attack.");
			}
			else
				QWUInventory::SendInventoryOperation(this, false, aChangeLog);
		}

		pInfo->m_nCsStar = iPacket->Decode2();
		pInfo->m_nShootRange = iPacket->Decode1();
	}
	TryParsingDamageData(pInfo, iPacket, pInfo->GetDamagedMobCount(), pInfo->GetDamageCountPerMob());
	return pInfo;
}

void User::CheckCashItemExpire(unsigned int tCur)
{
	if (tCur < m_tNextCheckCashItemExpire)
		return;
	m_tNextCheckCashItemExpire = tCur + 3 * 60 * 1000;

	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	const GW_ItemSlotEquip* pItem = nullptr;
	auto tDateTime = GameDateTime::GetCurrentDate();

	std::vector<std::pair<char, long long int>> aliExpiredSN;
	for (auto& prItem : m_pCharacterData->mItemSlot[GW_ItemSlotBase::EQUIP])
	{
		pItem = prItem.second;
		if (pItem->bIsCash && pItem->liExpireDate > 0 && tDateTime > pItem->liExpireDate) 
		{
			aliExpiredSN.push_back({ GW_ItemSlotBase::EQUIP, pItem->liCashItemSN });
			m_pCharacterData->mItemRemovedRecord[GW_ItemSlotBase::EQUIP].insert({ pItem->liCashItemSN, true });
		}
	}

	for (auto& prItem : m_pCharacterData->mItemSlot[GW_ItemSlotBase::CASH])
	{
		pItem = prItem.second;
		if (!pItem->bIsPet && pItem->liExpireDate > 0 && tDateTime > pItem->liExpireDate)
		{
			aliExpiredSN.push_back({ GW_ItemSlotBase::CASH, pItem->liCashItemSN });
			m_pCharacterData->mItemRemovedRecord[GW_ItemSlotBase::CASH].insert({ pItem->liCashItemSN, true });
		}
	}

	if (aliExpiredSN.size() == 0)
		return;

	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::CashItemRequest);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(GetUserID());
	oPacket.Encode2(CenterCashItemRequestType::eExpireCashItemRequest);
	oPacket.Encode1((int)aliExpiredSN.size());
	for (auto& prItem : aliExpiredSN) 
	{
		oPacket.Encode1(prItem.first);
		oPacket.Encode8(prItem.second);
	}

	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

/*AttackInfo * User::TryParsingMeleeAttack(AttackInfo* pInfo, int nType, InPacket * iPacket)
{
	pInfo->m_nType = nType;
	pInfo->m_bFieldKey = iPacket->Decode1();
	pInfo->m_bAttackInfoFlag = iPacket->Decode1();
	pInfo->m_nDamagePerMob = pInfo->m_bAttackInfoFlag & 0xF;
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

	pInfo->m_nOption = iPacket->Decode1();
	pInfo->m_nDisplay = iPacket->Decode1();
	pInfo->m_nAction = pInfo->m_nDisplay & 0x7F;
	pInfo->m_nAttackActionType = iPacket->Decode1();
	pInfo->m_nAttackSpeed = iPacket->Decode1();

	pInfo->m_tLastAttackTime = iPacket->Decode4();
	TryParsingDamageData(pInfo, iPacket, pInfo->GetDamagedMobCount(), pInfo->GetDamageCountPerMob());
	return pInfo;
}

AttackInfo * User::TryParsingMagicAttack(AttackInfo* pInfo, int nType, InPacket * iPacket)
{
	pInfo->m_nType = nType;
	pInfo->m_bFieldKey = iPacket->Decode1();
	pInfo->m_bAttackInfoFlag = iPacket->Decode1();
	pInfo->m_nDamagePerMob = pInfo->m_bAttackInfoFlag & 0xF;
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
	pInfo->m_nAction = pInfo->m_nDisplay & 0x7F;
	pInfo->m_nAttackActionType = iPacket->Decode1();
	pInfo->m_nAttackSpeed = iPacket->Decode1();
	pInfo->m_tLastAttackTime = iPacket->Decode4();

	TryParsingDamageData(pInfo, iPacket, pInfo->GetDamagedMobCount(), pInfo->GetDamageCountPerMob());
	return pInfo;
}

AttackInfo * User::TryParsingShootAttack(AttackInfo* pInfo, int nType, InPacket * iPacket)
{
	pInfo->m_nType = nType;
	pInfo->m_bFieldKey = iPacket->Decode1();
	pInfo->m_bAttackInfoFlag = iPacket->Decode1();
	pInfo->m_nDamagePerMob = pInfo->m_bAttackInfoFlag & 0xF;
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
	pInfo->m_nAction = pInfo->m_nDisplay & 0x7F;
	pInfo->m_nAttackActionType = iPacket->Decode1();
	pInfo->m_nAttackSpeed = iPacket->Decode1();
	pInfo->m_tLastAttackTime = iPacket->Decode4();


	TryParsingDamageData(pInfo, iPacket, pInfo->GetDamagedMobCount(), pInfo->GetDamageCountPerMob());
	return pInfo;
}


AttackInfo *User::TryParsingBodyAttack(AttackInfo* pInfo, int nType, InPacket * iPacket)
{
	return TryParsingMeleeAttack(pInfo, nType, iPacket);
}*/

void User::OnPacket(InPacket *iPacket)
{
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
		case UserRecvPacketType::User_OnAliveCheckAck:
			m_tLastAliveCheckRespondTime = GameDateTime::GetTime();
			break;
		case UserRecvPacketType::User_OnStatChangeItemUseRequest:
			OnStatChangeItemUseRequest(iPacket, false);
			break;
		case UserRecvPacketType::User_OnStatChangeItemCancelRequest:
			OnStatChangeItemCancelRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnUserChat:
			OnChat(iPacket);
			break;
		case UserRecvPacketType::User_OnUserTransferFieldRequest:
			OnTransferFieldRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnUserTransferChannelRequest:
			OnTransferChannelRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnUserMigrateToCashShopRequest:
			OnMigrateToCashShopRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnUserMoveRequest:
			m_pField->OnUserMove(this, iPacket);
			break;
		case UserRecvPacketType::User_OnSitRequest:
			OnSitRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnSitOnPortableChairRequest:
			OnPortableChairSitRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnSetActiveEffectItem:
			OnSetActiveEffectItem(iPacket);
			break;
		case UserRecvPacketType::User_OnChangeStatRequest:
			OnChangeStatRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnUserChangeSlotRequest:
			QWUInventory::OnChangeSlotPositionRequest(this, iPacket);
			break;
		case UserRecvPacketType::User_OnSkillLearnItemUseRequest:
			OnSkillLearnItemUseRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnPortalScrollUseRequest:
			OnPortalScrollUseRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnUserAbilityUpRequest:
			OnAbilityUpRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnUserSkillUpRequest:
			USkill::OnSkillUpRequest(this, iPacket);
			break;
		case UserRecvPacketType::User_OnUserSkillUseRequest:
			USkill::OnSkillUseRequest(this, iPacket);
			break;
		case UserRecvPacketType::User_OnUserSkillCancelRequest:
			USkill::OnSkillCancelRequest(this, iPacket);
			break;
		case UserRecvPacketType::User_OnDropMoneyRequest:
			OnDropMoneyRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnUserAttack_MeleeAttack:
		case UserRecvPacketType::User_OnUserAttack_ShootAttack:
		case UserRecvPacketType::User_OnUserAttack_MagicAttack:
		case UserRecvPacketType::User_OnUserAttack_BodyAttack:
			OnAttack(nType, iPacket);
			break;
		case UserRecvPacketType::User_OnCharacterInfoRequest:
			OnCharacterInfoRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnEmotion:
			OnEmotion(iPacket);
			break;
		case UserRecvPacketType::User_OnSelectNpc:
			OnSelectNpc(iPacket);
			break;
		case UserRecvPacketType::User_OnScriptMessageAnswer:
			OnScriptMessageAnswer(iPacket);
			break;
		case UserRecvPacketType::User_OnQuestRequest:
			OnQuestRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnFuncKeyMappedModified:
			OnFuncKeyMappedModified(iPacket);
			break;
		case UserRecvPacketType::User_OnShopRequest:
			if (m_pTradingNpc)
				m_pTradingNpc->OnShopRequest(this, iPacket);
			break;
		case UserRecvPacketType::User_OnItemUpgradeRequest:
			QWUInventory::OnUpgradeItemRequest(this, iPacket);
			break;
		case UserRecvPacketType::User_OnActivatePetRequest:
			OnActivatePetRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnPartyRequest:
			PartyMan::GetInstance()->OnPartyRequest(this, iPacket);
			break;
		case UserRecvPacketType::User_OnPartyRequestRejected:
			PartyMan::GetInstance()->OnPartyRequestRejected(this, iPacket);
			break;
		case UserRecvPacketType::User_OnGuildRequest:
			GuildMan::GetInstance()->OnGuildRequest(this, iPacket);
			break;
		case UserRecvPacketType::User_OnGuildBBSRequest:
			GuildMan::GetInstance()->OnGuildBBSRequest(this, iPacket);
			break;
		case UserRecvPacketType::User_OnFriendRequest:
			FriendMan::GetInstance()->OnFriendRequest(this, iPacket);
			break;
		case UserRecvPacketType::User_OnSendGroupMessage:
			OnSendGroupMessage(iPacket);
			break;
		case UserRecvPacketType::User_OnSendWhisperMessage:
			OnWhisper(iPacket);
			break;
		case UserRecvPacketType::User_OnMiniRoomRequest:
			UMiniRoom::OnMiniRoom(this, iPacket);
			break;
		case UserRecvPacketType::User_OnTrunkRequest:
			if (m_pTrunk)
				m_pTrunk->OnPacket(this, iPacket);
			break;
		case UserRecvPacketType::User_OnMobSummonItemUseRequest:
			OnMobSummonItemUseRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnHit:
			OnHit(iPacket);
			break;
		case UserRecvPacketType::User_OnStoreBankRequest:
			if (m_pStoreBank)
				m_pStoreBank->OnPacket(iPacket);
			break;
		case UserRecvPacketType::User_OnPortalScriptRequest:
			OnPortalScriptRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnEntrustedShopRequest:
			OnOpenEntrustedShop(iPacket);
			break;
		case UserRecvPacketType::User_OnConsumeCashItemUseRequest:
			OnConsumeCashItemUseRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnGivePopularityRequest:
			OnGivePopularityRequest(iPacket);
			break;
		case UserRecvPacketType::User_OnPetFoodItemUseRequest:
			OnPetFoodItemUseRequest(iPacket);
			break;
		default:
			iPacket->RestorePacket();
			//Pet Packet
			if (nType >= UserRecvPacketType::User_OnPetMove
				&& nType <= UserRecvPacketType::User_OnPetDropPickupRequest)
				OnPetPacket(iPacket);
			//Summoned Packet
			else if (nType >= FlagMin(SummonedRecvPacketType)
				&& nType <= FlagMax(SummonedRecvPacketType))
				OnSummonedPacket(iPacket);
			//Field Packet
			else if (m_pField)
				m_pField->OnPacket(this, iPacket);
	}
	//ValidateStat();
	//SendCharacterStat(false, 0);
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
	
	if (m_pCharacterData->mStat->nHP == 0)
	{
		auto pReturnField = FieldMan::GetInstance()->GetField(m_pField->GetReturnMap());
		if (pReturnField)
		{
			TryTransferField(
				m_pField->GetReturnMap(), 
				pReturnField->GetPortalMap()->GetRandStartPoint()->GetPortalName()
			);
		}
		m_pCharacterData->mStat->nHP = 50;
		SendCharacterStat(false, BasicStat::BS_HP);
		m_pSecondaryStat->ResetAll(this);
		return;
	}
	
	Portal* pPortal = m_pField->GetPortalMap()->FindPortal(sPortalName);
	if (dwFieldReturn == -1 && pPortal == nullptr)
		return;

	TryTransferField(
		dwFieldReturn == -1 ? pPortal->GetTargetMap() : dwFieldReturn, 
		pPortal->GetTargetPortalName()
	);
}

bool User::TryTransferField(int nFieldID, const std::string& sPortalName)
{
	std::lock_guard<std::recursive_mutex> user_lock(m_mtxUserLock);
	SetTransferStatus(TransferStatus::eOnTransferField);
	Field *pTargetField = nFieldID == -1 ? m_pField : FieldMan::GetInstance()->GetField(nFieldID);
	if (pTargetField != nullptr)
	{
		Portal* pTargetPortal = pTargetField->GetPortalMap()->FindPortal(sPortalName);
		if(!pTargetPortal)
			pTargetPortal = pTargetField->GetPortalMap()->GetRandStartPoint();

		if (pTargetPortal)
		{
			SetPosX(pTargetPortal->GetX());
			SetPosY(pTargetPortal->GetY());
		}

		if (nFieldID == -1 && pTargetPortal)
			PostPortalTeleport(pTargetPortal->GetID());
		else
		{
			//Process Leaving
			LeaveField();
			if (GetFieldSet() && GetFieldSet() != pTargetField->GetFieldSet())
				GetFieldSet()->OnLeaveFieldSet(GetUserID());

			//Process Entering
			m_pField = pTargetField;
			m_pCharacterData->mStat->nPosMap = m_pField->GetFieldID();
			m_pCharacterData->mStat->nPortal = (pTargetPortal == nullptr ? 0x80 : pTargetPortal->GetID());
			//PostTransferField(nFieldID, pTargetPortal, false);
			SendSetFieldPacket(false);
			m_pField->OnEnter(this);
			m_pCharacterData->nFieldID = m_pField->GetFieldID();
			SetTransferStatus(TransferStatus::eOnTransferNone);

			ReregisterPet();
			ReregisterSummoned();
		}
		return true;
	}
	SetTransferStatus(TransferStatus::eOnTransferNone);
	return false;
}

void User::OnPortalScriptRequest(InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	if (!m_pField)
		return;

	int nFieldKey = iPacket->Decode1();
	std::string sPortalName = iPacket->DecodeStr();
	auto pPortal = m_pField->GetPortalMap()->FindPortal(sPortalName);
	if (pPortal)
	{
		/*Doing some checks here, let's see*/
		auto pScript = ScriptMan::GetInstance()->CreateScript(
			"./DataSrv/Script/Portal/" + pPortal->GetPortalScriptName() + ".lua",
			{ 0, m_pField }
		);
		if (!pScript)
		{
			auto sScriptName = "s_" + pPortal->GetPortalScriptName();
			auto sScriptFile = ScriptMan::GetInstance()->SearchScriptNameByFunc(
				"Portal",
				sScriptName
			);

			pScript = ScriptMan::GetInstance()->CreateScript(
				sScriptFile,
				{ 0, m_pField }
			);

			if (pScript)
			{
				pScript->SetPortal(pPortal);
				SetScript(pScript);
				pScript->Run(sScriptName);
			}
			else 
				SendChatMessage(0, "Unable to execute portal script: [" + sScriptName + "].");
		}
		else if (pScript) 
		{
			pScript->SetPortal(pPortal);
			SetScript(pScript);
			pScript->Run();
		}
	}
	SendCharacterStat(true, 0);
}

void User::OnTransferChannelRequest(InPacket *iPacket)
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
	oPacket.Encode2(CenterRequestPacketType::RequestTransferChannel);
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
	oPacket.Encode2(CenterRequestPacketType::RequestTransferShop);
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
	oPacket->Encode2(UserSendPacketType::UserCommon_OnChat);
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
	oPacket.Encode2((short)UserSendPacketType::UserLocal_OnSetFieldStage); //Set Stage
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

void User::PostPortalTeleport(int nPortalID)
{
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketType::UserLocal_OnTeleport);
	oPacket.Encode1(0); // bLoopback
	oPacket.Encode1(nPortalID);
	SendPacket(&oPacket);
}

void User::SetMovePosition(int x, int y, char bMoveAction, short nFSN)
{
	SetPosX(x);
	SetPosY(y);
	SetMoveAction(bMoveAction);
	SetFh(nFSN);
}

bool User::InspectDamageMiss(int nDamage, void* pMobAttackInfo, void *pMob_, unsigned int *aDamageRandom)
{
	Mob *pMob = (Mob*)pMob_;
	MobTemplate::MobAttackInfo *pAttackInfo = (MobTemplate::MobAttackInfo*)pMobAttackInfo;
	bool bCheckMiss = false;

	if (nDamage > 0)
		return true;

	if (pAttackInfo && pAttackInfo->bMagicAttack)
		bCheckMiss = m_pCalcDamage->CheckMDamageMiss(
			pMob->GetMobStat(), aDamageRandom[0]
		);
	else
		bCheckMiss = m_pCalcDamage->CheckPDamageMiss(
			pMob->GetMobStat(), aDamageRandom[0]
		);

	SkillEntry *pEntry = nullptr;
	if (m_pCharacterData->mStat->nJob == 412 ||
		m_pCharacterData->mStat->nJob == 422)
	{
		int nSLV = SkillInfo::GetInstance()->GetSkillLevel(
			m_pCharacterData,
			m_pCharacterData->mStat->nJob == 412 ? 4120002 : 4220002,
			&pEntry
		);
		if (nSLV &&	pEntry->GetLevelData(nSLV)->m_nProp > (int)(aDamageRandom[1] % 100))
			bCheckMiss = true;
	}

	if ((int)(aDamageRandom[2] % 100) < pMob->GetMobStat()->nBlind_)
		bCheckMiss = true;

	if (m_pCharacterData->mStat->nJob == 112 ||
		m_pCharacterData->mStat->nJob == 112)
	{
		auto& pShield = m_pCharacterData->GetItem(GW_ItemSlotBase::EQUIP, -10);
		int nSLV = SkillInfo::GetInstance()->GetSkillLevel(
			m_pCharacterData,
			m_pCharacterData->mStat->nJob == 112 ? 1120005 : 1220006,
			&pEntry
		);
		if (nSLV &&
			pShield &&
			pEntry->GetLevelData(nSLV)->m_nProp > (int)(aDamageRandom[3] % 1000))
			bCheckMiss = true;
	}

	return bCheckMiss;
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
	oPacket.Encode2(UserSendPacketType::UserRemote_OnAvatarModified);
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

void User::DecodeInternal(InPacket * iPacket)
{
	bool bDecodeInternal = iPacket->Decode1() == 1;
	if (!bDecodeInternal)
		return;
	int nChannelID = iPacket->Decode4();
	int nCount = iPacket->Decode4(), nSkillID, nSLV, nSetCooltime = 0;
	unsigned int tDurationRemained = 0, tCooltime = 0;
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);

	//Decode Temporary Internal
	const void *pSkill = nullptr;
	for (int i = 0; i < nCount; ++i)
	{
		nSkillID = iPacket->Decode4();
		tDurationRemained = iPacket->Decode4();
		nSLV = iPacket->Decode4();

		//Reset State Change Item
		if (nSkillID < 0)
		{
			auto pItem = ItemInfo::GetInstance()->GetStateChangeItem(-nSkillID);
			if (pItem)
				pItem->Apply(this, GameDateTime::GetTime(), false, false, true, tDurationRemained);
		}
		//Reset Skill Stat
		else if (pSkill = SkillInfo::GetInstance()->GetSkillByID(nSkillID), pSkill)
			USkill::OnSkillUseRequest(
				this,
				nullptr,
				(SkillEntry*)pSkill,
				nSLV,
				false,
				true,
				tDurationRemained
			);
		//Reset Mob Skill Stat
		else if (pSkill = SkillInfo::GetInstance()->GetMobSkill(nSkillID & 0xFF), pSkill)
			OnStatChangeByMobSkill(
				nSkillID & 0xFF,
				(nSkillID & 0xFF0000) >> 16,
				((MobSkillEntry*)pSkill)->GetLevelData((nSkillID & 0xFF0000) >> 16),
				0,
				0,
				false,
				true,
				tDurationRemained
			);
	}

	//Decode Cooltime Records
	nCount = iPacket->Decode4();
	for (int i = 0; i < nCount; ++i)
	{
		nSetCooltime = iPacket->Decode4();
		tCooltime = iPacket->Decode4();
		m_pSecondaryStat->m_mCooltimeOver[nSetCooltime] = tCooltime;
		SendSkillCooltimeSet(nSetCooltime, tCooltime);
	}

	//Decode Summoneds
	nCount = iPacket->Decode4();
	for (int i = 0; i < nCount; ++i)
	{
		nSkillID = iPacket->Decode4();
		nSLV = iPacket->Decode2();
		auto pSummoned = GetField()->GetSummonedPool()->CreateSummoned(
			this,
			nSkillID,
			nSLV,
			FieldPoint({ GetPosX(), GetPosY() }),
			iPacket->Decode4(),
			true
		);
		if (pSummoned)
			m_lSummoned.push_back(pSummoned);
	}
}

void User::EncodeInternal(OutPacket * oPacket)
{
	std::lock_guard<std::recursive_mutex> userGuard(m_mtxUserLock);
	oPacket->Encode4(GetChannelID());
	int nSkillID, nSLV;
	unsigned int tDurationRemained;

	//Encode Temporary Internal
	auto &pSS = m_pSecondaryStat;
	oPacket->Encode4((int)pSS->m_mSetByTS.size());
	for (auto& setFlag : pSS->m_mSetByTS)
	{
		nSkillID = *(setFlag.second.second[1]);
		tDurationRemained = (unsigned int)setFlag.second.first;
		nSLV = *(setFlag.second.second[3]);
		oPacket->Encode4(nSkillID);
		oPacket->Encode4(tDurationRemained);
		oPacket->Encode4(nSLV);
	}

	//Encode Cooltime Records
	oPacket->Encode4((int)pSS->m_mCooltimeOver.size());
	for (auto& prCooltime : pSS->m_mCooltimeOver)
	{
		oPacket->Encode4(prCooltime.first);
		oPacket->Encode4(prCooltime.second);
	}

	//Encode Summoned.
	oPacket->Encode4((int)m_lSummoned.size());
	for (auto& pSummoned : m_lSummoned)
	{
		oPacket->Encode4(pSummoned->GetSkillID());
		oPacket->Encode2(pSummoned->GetSkillLevel());
		oPacket->Encode4(pSummoned->GetTimeEnd());
	}
}

void User::ValidateStat(bool bCalledByConstructor)
{
	m_pBasicStat->SetFrom(m_pCharacterData, m_pSecondaryStat->nMaxHP_, m_pSecondaryStat->nMaxMP_, m_pSecondaryStat->nBasicStatUp_);
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
	ValidateEffectItem();
}

void User::ValidateEffectItem()
{
	if (m_pCharacterData->nActiveEffectItemID)
	{
		if (!m_pCharacterData->GetItemByID(m_pCharacterData->nActiveEffectItemID))
			m_pCharacterData->nActiveEffectItemID = 0;
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
		SendChangeJobEffect();
	}

	if (liFlag & BasicStat::BasicStatFlag::BS_HP)
		PostHPToPartyMembers();

	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnStatChanged);
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
		for (int i = 0; i < GetMaxPetIndex(); ++i)
			if (m_apPet[i])
				oPacket.Encode8(m_apPet[i]->m_pPetSlot->liCashItemSN);
			else
				oPacket.Encode8(0);
	}
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);

	SendPacket(&oPacket);
}

void User::SendPortableChairEffect(int nSeatID)
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserRemote_OnSetActivePortableChair);
	oPacket.Encode4(GetUserID());
	oPacket.Encode4(m_nActivePortableChairID);
	m_pField->SplitSendPacket(&oPacket, this);
}

void User::SendTemporaryStatReset(TemporaryStat::TS_Flag& flag)
{
	if (flag.IsEmpty())
		return;
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnTemporaryStatReset);
	flag.Encode(&oPacket);
	oPacket.Encode2(2000);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	SendPacket(&oPacket);

	OutPacket oRemote;
	oRemote.Encode2(UserSendPacketType::UserRemote_OnResetTemporaryStat);
	oRemote.Encode4(GetUserID());
	flag.Encode(&oRemote);
	oRemote.Encode2(0);
	GetField()->SplitSendPacket(&oRemote, nullptr);
}

void User::SendTemporaryStatSet(TemporaryStat::TS_Flag& flag, int tDelay)
{
	if (flag.IsEmpty())
		return;
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnTemporaryStatSet);
	m_pSecondaryStat->EncodeForLocal(&oPacket, flag);
	oPacket.Encode2(tDelay);
	oPacket.Encode1(0);
	SendPacket(&oPacket);

	OutPacket oForRemote;
	oForRemote.Encode2(UserSendPacketType::UserRemote_OnSetTemporaryStat);
	oForRemote.Encode4(GetUserID());
	m_pSecondaryStat->EncodeForRemote(&oForRemote, flag);
	oForRemote.Encode2(tDelay);
	GetField()->SplitSendPacket(&oForRemote, this);
}

void User::OnAttack(int nType, InPacket * iPacket)
{
	AttackInfo attackInfo, *pResult = nullptr;

	switch (nType)
	{
		case UserRecvPacketType::User_OnUserAttack_MeleeAttack:
			attackInfo.m_nAttackType = 0;
			pResult = (TryParsingAttackInfo(&attackInfo, nType, iPacket));
			break;
		case UserRecvPacketType::User_OnUserAttack_ShootAttack:
			attackInfo.m_nAttackType = 1;
			pResult = (TryParsingAttackInfo(&attackInfo, nType, iPacket));
			break;
		case UserRecvPacketType::User_OnUserAttack_MagicAttack:
			attackInfo.m_nAttackType = 2;
			pResult = (TryParsingAttackInfo(&attackInfo, nType, iPacket));
			break;
		case UserRecvPacketType::User_OnUserAttack_BodyAttack:
			pResult = (TryParsingAttackInfo(&attackInfo, nType, iPacket));
			break;
	}
	if (pResult)
	{
		auto pSkillEntry = SkillInfo::GetInstance()->GetSkillByID(pResult->m_nSkillID);
		auto pLevel = !pSkillEntry ? nullptr : pSkillEntry->GetLevelData(pResult->m_nSLV);
		unsigned int tCur = GameDateTime::GetTime();
		auto& pWeapon = m_pCharacterData->GetItem(GW_ItemSlotBase::EQUIP, -11);
		pResult->m_nWeaponItemID = pWeapon ? pWeapon->nItemID : 0;

		//Check Cooltime and HP/MP Consumption
		if (pLevel && 
			(tCur < GetSkillCooltime(pResult->m_nSkillID) || !USkill::ConsumeHPAndMPBySkill(this, pLevel)))
		{
			SendCharacterStat(true, 0);
			return;
		}
		SetSkillCooltime(pResult->m_nSkillID, pLevel ? pLevel->m_nCooltime : 0);

		//Apply to LifePool
		m_pField->GetLifePool()->OnUserAttack(
			this,
			pSkillEntry,
			pResult
		);

		if (pLevel && pResult->m_nSkillID == 2111003)
		{
			FieldRect rect = pLevel->m_rc;
			rect.OffsetRect(GetPosX(), GetPosY());
			unsigned int tStart = tCur + 700;
			//if (pResult->m_bAttackInfoFlag >> 4)
			//	tStart = pResult->tDelay;
			unsigned int tEnd = tCur + 700 + pLevel->m_nTime;
			m_pField->GetAffectedAreaPool()->InsertAffectedArea(
				false,
				GetUserID(),
				pResult->m_nSkillID,
				pResult->m_nSLV,
				tStart,
				tEnd,
				{ GetPosX(), GetPosY() },
				rect,
				false
			);
		}
	}
}

void User::OnHit(InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);

	int tDamagedTime = iPacket->Decode4();
	int nMobAttackIdx = iPacket->Decode1();
	int nElemAttr = iPacket->Decode1();
	int nDamage = iPacket->Decode4();
	int nMobTemplateID = 0,
		nMobID = 0,
		nCalcDamageMobStatIndex = 0,
		nReflect = 0,
		nMPDec = 0,
		nFlag = 0,
		nMobSkill = 0,
		nHPDec = 0,
		nAttackType = 0,
		nDamageClient = nDamage,
		nDamageSend = nDamage,
		nReflectType = 0;
	bool bReflect = false,
		bResetRnd = false,
		bShield = false;

	unsigned int aDamageRandom[4];
	for (int i = 0; i < sizeof(aDamageRandom) / sizeof(int); ++i)
		aDamageRandom[i] = GetCalcDamage()->GetRndGenForForCheckDamageMiss().Random();

	if (nDamage < -1)
		return;
	FieldPoint ptHit, ptUserPos;
	SkillEntry *pEntry = nullptr;
	MobTemplate::MobAttackInfo *pAttackInfo = nullptr;
	MobTemplate *pTemplate = nullptr;

	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserRemote_OnHit);
	oPacket.Encode4(GetUserID());
	oPacket.Encode1(nMobAttackIdx);
	oPacket.Encode4(nDamage);

	if (nMobAttackIdx <= -2)
		nMobSkill = iPacket->Decode2();
	else
	{
		nMobTemplateID = iPacket->Decode4();
		nMobID = iPacket->Decode4();
		nCalcDamageMobStatIndex = iPacket->Decode4();
		bool bDirection = iPacket->Decode1() != 0;
		nReflect = iPacket->Decode1();
		nReflectType = iPacket->Decode1();
		bool bStance = iPacket->Decode1() != 0;

		oPacket.Encode4(nMobTemplateID);
		oPacket.Encode1(bDirection ? 1 : 0);
		oPacket.Encode1(nReflect);
		bShield = nReflectType != 0;
		if (nReflect || nReflectType == 2)
		{
			bReflect = nReflect && iPacket->Decode1();
			int nMobID = iPacket->Decode4();
			bool bSkill = iPacket->Decode1() != 0;
			ptHit.x = iPacket->Decode2();
			ptHit.y = iPacket->Decode2();
			ptUserPos.x = iPacket->Decode2();
			ptUserPos.y = iPacket->Decode2();

			if (nReflect)
			{
				oPacket.Encode1(bReflect ? 1 : 0);
				oPacket.Encode4(nMobID);
				oPacket.Encode1(bSkill);
				oPacket.Encode2(ptHit.x);
				oPacket.Encode2(ptHit.y);
				/*oPacket.Encode2(ptUserPos.x);
				oPacket.Encode2(ptUserPos.y);*/
			}
		}
		oPacket.Encode1(bStance ? 1 : 0);
		bResetRnd = iPacket->Decode1() == 1;

		//Damage Check
		iPacket->Decode4();
		for (int i = 0; i < 14; ++i)
			iPacket->Decode4();
		//
	}

	if (!m_pField)
	{
		if (nMobAttackIdx > -2 && (nDamage > 0 || bResetRnd))
			GetCalcDamage()->GetRndGenForMob().Random();
		return;
	}

	//IsCounterAttackPossible(nMobTemplateID, nMobID, nMobAttackIdx, nReflect, nPowerGuard, nManaRelfect != 0, ptHit, ptUserPos);
	if (m_pCharacterData->mStat->nHP != 0)
	{
		//CheckHitSpeed(tDamagedTime, nDamage, nMobAttackIdx);
		m_bDeadlyAttack = false;
		if (nMobAttackIdx > -2)
			m_nInvalidDamageMissCount = 0;

		int nMagicGuard = m_pSecondaryStat->nMagicGuard_, nGuarded = 0;
		if (nMagicGuard)
		{
			nGuarded = nDamage * nMagicGuard / 100;
			if (!m_pSecondaryStat->nInfinity_ && m_pCharacterData->mStat->nMP < nGuarded)
				nGuarded = m_pCharacterData->mStat->nMP;
		}
		nDamage -= nGuarded;
		nMPDec = nGuarded;

		pTemplate = MobTemplate::GetMobTemplate(nMobTemplateID);
		if (pTemplate)
		{
			if (nMobAttackIdx >= 0 &&
				pTemplate->m_aAttackInfo.size() &&
				nMobAttackIdx < (int)pTemplate->m_aAttackInfo.size()) 
			{
				pAttackInfo = &(pTemplate->m_aAttackInfo[nMobAttackIdx]);
				nAttackType = pAttackInfo->nType;
			}
			
			if (nAttackType)
			{
				if (pAttackInfo->bDeadlyAttack)
				{
					nHPDec = m_pCharacterData->mStat->nHP - 1;
					nMPDec = m_pCharacterData->mStat->nMP - 1;
				}
				else if(pAttackInfo->nMPBurn)
				{
					nHPDec = 1;
					nMPDec = std::min(m_pCharacterData->mStat->nMP, pAttackInfo->nMPBurn);
				}
			}
		}
		if (nReflect && pTemplate)
		{
			if (bReflect && m_pSecondaryStat->nPowerGuard_)
			{
				nReflect = std::min(nReflect, m_pSecondaryStat->nPowerGuard_);
				nReflect = std::min(nReflect, m_pCharacterData->mStat->nHP);
				nHPDec = std::min(nHPDec, m_pCharacterData->mStat->nHP);
				nGuarded = std::min(nReflect * nHPDec / 100, m_pCharacterData->mStat->nMaxHP / 2);
				if (pTemplate->m_bIsBoss)
					nGuarded /= 2;
				if (nGuarded)
				{
					if (pTemplate->m_nFixedDamage)
						nGuarded = pTemplate->m_nFixedDamage;
					if (pTemplate->m_bInvincible)
						nGuarded = 0;
				}
				nDamage -= nGuarded;
				nDamageSend = nDamage;
			}
			else if (nAttackType && pAttackInfo->bMagicAttack && m_pSecondaryStat->nManaReflection_)
			{
				auto pEntry = SkillInfo::GetInstance()->GetSkillByID(2121002);
				auto pLevel = pEntry->GetLevelData(m_pSecondaryStat->nLvManaReflection_);
				nReflect = std::min(nReflect, pLevel->m_nX);
				nHPDec = std::min(nDamage, m_pCharacterData->mStat->nHP);
				nGuarded = std::min(nReflect * nHPDec / 100, m_pCharacterData->mStat->nHP / 20);
				switch ((m_pCharacterData->mStat->nJob / 10) % 10)
				{
				case 1:
					nFlag = 2121002;
					break;
				case 2:
					nFlag = 2221002;
					break;
				case 3:
					nFlag = 2321002;
					break;
				}
			}
		}
		if (m_pSecondaryStat->nMesoGuard_ && pTemplate)
		{
			int nSLV = SkillInfo::GetInstance()->GetSkillLevel(
				m_pCharacterData,
				4211005,
				&pEntry
			);
			if (pEntry && nSLV)
			{
				int nMoneyCon = pEntry->GetLevelData(nSLV)->m_nMoneyCon;
				int nGuarded = nDamage / 2;
				nMoneyCon = nGuarded * nMoneyCon / 100;
				if (nGuarded)
				{
					if (QWUser::GetMoney(this) < nMoneyCon)
					{
						nMoneyCon = (int)QWUser::GetMoney(this);
						nGuarded = 100 * nMoneyCon / (pEntry->GetLevelData(nSLV)->m_nMoneyCon);
					}
					QWUser::IncMoney(this, nMoneyCon, true);
					SendCharacterStat(false, BasicStat::BS_Meso);
					nDamage -= nGuarded;
					nDamageSend = nDamage;
					nFlag = 4211005;
				}
			}
		}
		if (m_pCharacterData->mStat->nJob / 100 == 1 &&
			m_pCharacterData->mStat->nJob % 10 == 2)
		{
			int nSkillID =
				m_pCharacterData->mStat->nJob == 112 ? 1120004 :
				m_pCharacterData->mStat->nJob == 122 ? 1220005 :
				1320005;
			int nSLV = SkillInfo::GetInstance()->GetSkillLevel(
				m_pCharacterData,
				nSkillID,
				&pEntry
			);
			if (nSLV) 
			{
				nDamage = nDamage + (nDamage * (1000 - (pEntry->GetLevelData(nSLV)->m_nX))) / -1000;
				nDamageSend = nDamage;
			}
		}
		if (bShield && m_pCharacterData->GetItem(GW_ItemSlotBase::EQUIP, -10)) 
		{
			SendChatMessage(0, "Blocking skill activated without shield.");
			nDamageSend = -1;
		}

		if (nFlag)
		{
		}

		nFlag = 0;
		nFlag |= QWUser::IncHP(this, -nDamage, false);
		if (nMPDec)
			nFlag |= QWUser::IncMP(this, -nMPDec, false);

		if (m_pCharacterData->mStat->nHP == 0)
			OnUserDead(m_pField->IsTown());
		else if (nDamage)
		{
			if (nMobSkill)
			{
				auto pMobSkill = SkillInfo::GetInstance()->GetMobSkill(nMobSkill >> 8);
				if (pMobSkill)
				{
					int nSLV = nMobSkill & 0xFF;
					if (nSLV <= pMobSkill->GetMaxLevel())
						OnStatChangeByMobSkill(
							nMobSkill >> 8,
							nSLV,
							pMobSkill->GetLevelData(nSLV),
							0,
							0
						);
				}
			}
			else if(nMobTemplateID)
				OnStatChangeByMobAttack(nMobTemplateID, nMobAttackIdx);
		}
		SendCharacterStat(false, nFlag);
	}
	oPacket.Encode4(nDamageSend);
	if (nDamageSend == -1)
	{
		if (m_pCharacterData->mStat->nJob == 412)
			oPacket.Encode4(4120002);
		if (m_pCharacterData->mStat->nJob == 422)
			oPacket.Encode4(4220002);
		if (m_pCharacterData->mStat->nJob == 122)
			oPacket.Encode4(1220006);
		if (m_pCharacterData->mStat->nJob == 112)
			oPacket.Encode4(1120005);
	}
	m_pField->SplitSendPacket(&oPacket, this);
	auto pMob = m_pField->GetLifePool()->GetMob(nMobID);

	//Inspect Damage
	if (pMob && pTemplate)
	{
		int nDamageSrv = 0, nRnd = 0;
		bool bCheckMiss = InspectDamageMiss(nDamageClient, pAttackInfo, pMob, aDamageRandom);

		if (nDamageClient > 0 || (Rand32::GetInstance()->Random() % 100) > 30 || bCheckMiss)
			m_bDeadlyAttack = false;
		else if(++m_nInvalidDamageMissCount > 10)
		{
			m_nInvalidDamageMissCount = 0;
			SendChatMessage(0, "Suspicious Hacking for Damage Miss Check.");
		}

		//InspectDamage --- In official impl, if m_nGradeCode > 0, it skips damage checking
		if (nDamageClient > 0 && pAttackInfo && pAttackInfo->bMagicAttack)
		{
			nDamageSrv = m_pCalcDamage->MDamage(
				pMob->GetMobStat(),
				pAttackInfo,
				0,
				nullptr,
				&nRnd
			);

			if (pAttackInfo->nMagicElemAttr)
			{
				int nResistanceSkillID = 0, nJob = m_pBasicStat->nJob;
				if (nJob / 10 == 13 && nJob % 10 >= 1)
					nResistanceSkillID = 1310000;
				else if (nJob / 10 == 21 && nJob % 10 >= 1)
					nResistanceSkillID = 2110000;
				else if (nJob / 10 == 22 && nJob % 10 >= 1)
					nResistanceSkillID = 2210000;
				else if (nJob / 10 == 23 && nJob % 10 >= 1)
					nResistanceSkillID = 2310000;

				/*
				This resistance skill level, in official impl., comes from CalcDamageStat
				*/
				int nResistance = SkillInfo::GetInstance()->GetResistance(
					!nResistanceSkillID ? 0 : SkillInfo::GetInstance()->GetSkillLevel(m_pCharacterData, nResistanceSkillID, nullptr),
					QWUser::GetJob(this), 
					pAttackInfo->nMagicElemAttr
				);
				nDamageSrv = std::max(1, (nDamageSrv * (100 - nResistance) / 100));
			}
		}
		else if(nDamageClient > 0)
		{
			nDamageSrv = m_pCalcDamage->PDamage(
				pMob->GetMobStat(),
				pAttackInfo,
				0,
				nullptr,
				&nRnd
			);
		}

		if (nMobAttackIdx > -2 && nDamage <= 0 && bResetRnd)
			GetCalcDamage()->GetRndGenForMob().Random();

		//WvsLogger::LogFormat("Client Damage : %d, Server Damage : %d Miss Check = %d\n", nDamageClient, nDamageSrv, (int)bCheckMiss);
		if (nDamageClient != nDamageSrv)
			SendChatMessage(0, StringUtility::Format(GET_STRING(GameSrv_User_Inconsistent_HitDamage), nDamageClient, nDamageSrv, (int)bCheckMiss));
		if ((pAttackInfo && (pAttackInfo->nMPBurn || pAttackInfo->bDeadlyAttack))
			|| nDamageClient <= nDamageSrv || bCheckMiss)
		{
			m_nInvalidDamageCount = std::max(0, m_nInvalidDamageCount - 1);
			return;
		}
		else if(nDamageClient >= nDamageSrv + 2)
			++m_nInvalidDamageCount;

		if (m_nInvalidDamageCount > 10)
			SendChatMessage(0, "Suspicious Hacking for Damage Value Check.");
	}
}

void User::OnLevelUp()
{
	auto liFlag = IncMaxHPAndMP(0x2000 | 0x8000, true);
	ValidateStat();

	QWUser::IncHP(this, (int)QWUser::GetMaxHPVal(this), true);
	QWUser::IncMP(this, (int)QWUser::GetMaxMPVal(this), true);
	SendCharacterStat(false, BasicStat::BS_HP | BasicStat::BS_MP | liFlag);

	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserRemote_OnEffect);
	oPacket.Encode4(GetUserID());
	oPacket.Encode1(Effect::eEffect_LevelUp);
	m_pField->SplitSendPacket(&oPacket, this);
}

void User::OnEmotion(InPacket *iPacket)
{
	int nItemID = iPacket->Decode4();
	if (nItemID < 8 || m_pCharacterData->GetItemByID(nItemID))
	{
		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketType::UserRemote_OnEmotion);
		oPacket.Encode4(GetUserID());
		oPacket.Encode4(nItemID);

		GetField()->SplitSendPacket(&oPacket, this);
	}
}

ZUniquePtr<CalcDamage>& User::GetCalcDamage()
{
	return m_pCalcDamage;
}

ZUniquePtr<SecondaryStat>& User::GetSecondaryStat()
{
	return this->m_pSecondaryStat;
}

ZUniquePtr<BasicStat>& User::GetBasicStat()
{
	return this->m_pBasicStat;
}

void User::OnUserDead(bool bTown)
{
	DecreaseEXP(bTown);
}

void User::DecreaseEXP(bool bTown)
{
	if (m_pBasicStat->nJob)
	{
		int nLevel = QWUser::GetLevel(this);
		long long int liEXP = 0, liSet = 0;
		double dDecRate = 0;
		if (nLevel > 200)
			liEXP = 0x7FFFFFFF;
		else
			liEXP = UtilUser::m_nEXP[nLevel];
		
		if (bTown)
			dDecRate = 0.01;
		else
		{
			if (m_pBasicStat->nJob / 100 == 3)
				dDecRate = (0.08);
			else
				dDecRate = (0.2);
			dDecRate = dDecRate / (double)QWUser::GetLUK(this) + 0.05;
		}
		if (nLevel < 200)
		{
			liSet = QWUser::GetEXP(this) - (int)((double)liEXP * dDecRate);
			liSet = std::max((long long int)0, std::min(liSet, liEXP - 1));
		}
		else
			liSet = 0;
		m_pCharacterData->mStat->nExp = liSet;
		SendCharacterStat(false, BasicStat::BS_EXP);
	}
}

std::recursive_mutex & User::GetLock()
{
	return m_mtxUserLock;
}

void User::Update()
{
	unsigned int tCur = GameDateTime::GetTime();
	m_pSecondaryStat->ResetByTime(this, GameDateTime::GetTime());

	CheckCashItemExpire(tCur);
	UpdateActivePet(tCur);

	if (tCur - m_tLastBackupTime >= 2 * 60 * 1000)
	{
		FlushCharacterData();
		m_tLastBackupTime = tCur;
	}

	//Send alive check 30s.
	if (tCur - m_tLastAliveCheckRequestTime > 30 * 1000)
	{
		if (m_tLastAliveCheckRespondTime == -1)
		{
			WvsLogger::LogFormat(WvsLogger::LEVEL_WARNING, "Alive check timed out, User: [%d][%s]\n", GetUserID(), GetName().c_str());
			m_pSocket->GetSocket().close();
			return;
		}
		m_tLastAliveCheckRespondTime = -1;
		m_tLastAliveCheckRequestTime = tCur;

		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketType::UserLocal_OnAliveCheckRequest);
		SendPacket(&oPacket);
	}
}

bool User::IsShowTo(FieldObj *pUser)
{
	if(!pUser || pUser == this)
		return false;

	return !m_pSecondaryStat->nDarkSight_ || 
		(m_pSecondaryStat->rDarkSight_ == AdminSkills::Admin_HyperHidden && 
			((User*)pUser)->m_nGradeCode && 
			m_nGradeCode <= ((User*)pUser)->m_nGradeCode
		);
}

void User::ResetTemporaryStat(int tCur, int nReasonID)
{
	if (nReasonID == 0)
	{
		
	}
}

void User::OnAbilityUpRequest(InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	if (QWUser::GetAP(this) <= 0) 
	{
		this->m_pSocket->GetSocket().close();
		return;
	}
	int tRequestTime = iPacket->Decode4();
	int nOption = iPacket->Decode4();
	long long int liFlag = 0;
	if (nOption == 0x100)
		liFlag |= QWUser::IncSTR(this, 1, true);
	else if (nOption == 0x200)
		liFlag |= QWUser::IncDEX(this, 1, true);
	else if (nOption == 0x400)
		liFlag |= QWUser::IncINT(this, 1, true);
	else if (nOption == 0x800)
		liFlag |= QWUser::IncLUK(this, 1, true);
	else
		liFlag |= IncMaxHPAndMP(nOption, false);
	
	if (liFlag)
		liFlag |= QWUser::IncAP(this, -1, true);

	SendCharacterStat(true, liFlag);
}

long long int User::IncMaxHPAndMP(int nFlag, bool bLevelUp)
{
	static auto pSkillHPBoost = SkillInfo::GetInstance()->GetSkillByID(1000001);
	static auto pSkillMPBoost = SkillInfo::GetInstance()->GetSkillByID(2000001);
	bool bIncHP = (nFlag & 0x2000) != 0;
	bool bIncMP = (nFlag & 0x8000) != 0;
	int nJobType = QWUser::GetJob(this) / 100;
	int nHPInc = 0, nMPInc = 0, nBaseInc = 0, nSkillLevel = 0;
	long long int liRet = 0;

	static int anIncTable[] = {
		0x0C, 0x10, 0x00, 0x0A, 0x0C, 0x14, 0x08, 0x0C, 0x00, 0x06, 0x08, 0x0F, 
		0x18, 0x1C, 0x00, 0x04, 0x06, 0x02, 0x14, 0x18, 0x00, 0x02, 0x04, 0x0F, 
		0x0A, 0x0E, 0x00, 0x16, 0x18, 0x14, 0x06, 0x0A, 0x00, 0x12, 0x14, 0x0F, 
		0x14, 0x18, 0x00, 0x0E, 0x10, 0x14, 0x10, 0x14, 0x00, 0x0A, 0x0C, 0x0F, 
		0x14, 0x18, 0x00, 0x0E, 0x10, 0x14, 0x10, 0x14, 0x00, 0x0A, 0x0C, 0x0F, 
		0x14, 0x18, 0x00, 0x0E, 0x10, 0x14, 0x10, 0x14, 0x00, 0x0A, 0x0C, 0x0F
	};

	if (nJobType < 0 || nJobType > 5)
		return 0;

	auto nIdx = 3 * ((bLevelUp == 0) + 2 * nJobType);
	if (bIncHP)
	{
		nBaseInc = anIncTable[2 * nIdx];
		nHPInc = nBaseInc + (rand() % (std::max(1, anIncTable[2 * nIdx + 1] - nBaseInc + 1)));
	}

	if (bIncMP)
	{
		nBaseInc = anIncTable[2 * nIdx + 3];
		nMPInc = nBaseInc + (rand() % (std::max(1, anIncTable[2 * nIdx + 4] - nBaseInc + 1)));
	}

	if (bIncHP)
	{
		auto pRecord = m_pCharacterData->GetSkill(1000001);
		if (pRecord && pRecord->nSLV > 0 && pSkillHPBoost)
		{
			nSkillLevel = std::min(pRecord->nSLV, pSkillHPBoost->GetMaxLevel());
			if (bLevelUp)
				nHPInc += pSkillHPBoost->GetLevelData(nSkillLevel)->m_nY;
			else
				nHPInc += pSkillHPBoost->GetLevelData(nSkillLevel)->m_nX;
		}
		liRet |= QWUser::IncMaxHPVal(this, nHPInc, true);
	}

	if (bIncMP)
	{
		auto pRecord = m_pCharacterData->GetSkill(2000001);
		if (pRecord && pRecord->nSLV > 0 && pSkillMPBoost)
		{
			nSkillLevel = std::min(pRecord->nSLV, pSkillMPBoost->GetMaxLevel());
			if (bLevelUp)
				nHPInc += pSkillMPBoost->GetLevelData(nSkillLevel)->m_nY;
			else
				nHPInc += pSkillMPBoost->GetLevelData(nSkillLevel)->m_nX;
		}
		liRet |= QWUser::IncMaxMPVal(this, nMPInc, true);
	}

	return liRet;
}

void User::OnStatChangeByMobSkill(int nSkillID, int nSLV, const MobSkillLevelData *pLevel, int tDelay, int nTemplateID, bool bResetBySkill, bool bForcedSetTime, unsigned int nForcedSetTime)
{
	auto tCur = GameDateTime::GetTime();
	if ((int)(Rand32::GetInstance()->Random() % 100) < (!pLevel->nProp ? 100 : pLevel->nProp))
	{
		if (pLevel->tTime <= 0)
		{

		}
		else
		{
			TemporaryStat::TS_Flag tsFlag = TemporaryStat::TS_Flag::GetDefault();
			int nDuration = tDelay + pLevel->tTime;
			//WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Mob Skill ID %d, Duration : %d\n", nSkillID, nDuration);
			switch (nSkillID)
			{
				case 120:
					if (!m_pSecondaryStat->nHolyShield_)
					{
						REGISTER_TS_BY_MOB(Seal, 1);
					}
					break;
				case 121:
					if (!m_pSecondaryStat->nHolyShield_)
					{
						REGISTER_TS_BY_MOB(Darkness, 1);
					}
					break;
				case 122:
					if (!m_pSecondaryStat->nHolyShield_)
					{
						REGISTER_TS_BY_MOB(Weakness, 1);
					}
					break;
				case 123: 
				{
					REGISTER_TS_BY_MOB(Stun, 1);
					break;
				}
				case 124: 
				{
					if (m_pSecondaryStat->nHolyShield_)
						break;
					REGISTER_TS_BY_MOB(Curse, 1);
					break;
				}
				case 125:
					if (!m_pSecondaryStat->nHolyShield_)
					{
						REGISTER_TS_BY_MOB(Poison, pLevel->nX);
					}
					break;
				case 126:
					if (!m_pSecondaryStat->nHolyShield_)
					{
						REGISTER_TS_BY_MOB(Slow, pLevel->nX);
					}
					break;
				case 128: 
				{
					REGISTER_TS_BY_MOB(Attract, pLevel->nX);
					break;
				}
				case 129: 
				{
					REGISTER_TS_BY_MOB(BanMap, pLevel->nX);
					m_pSecondaryStat->mBanMap = nTemplateID;
				}
					break;
				default:
					break;
			}
			SendTemporaryStatReset(tsFlag);
			if(!bResetBySkill)
				SendTemporaryStatSet(tsFlag, tDelay);
		}
	}
}

void User::OnStatChangeByMobAttack(int nMobTemplateID, int nMobAttackIdx)
{
	auto pTemplate = MobTemplate::GetMobTemplate(nMobTemplateID);
	if (pTemplate)
	{
		if (nMobAttackIdx >= 0 &&
			pTemplate->m_aAttackInfo.size() &&
			nMobAttackIdx < (int)pTemplate->m_aAttackInfo.size())
		{
			auto pSkill = SkillInfo::GetInstance()->GetMobSkill(pTemplate->m_aAttackInfo[nMobAttackIdx].nDisease);
			if (!pSkill)
				return;

			int nSLV = pTemplate->m_aAttackInfo[nMobAttackIdx].nSkillLevel;
			OnStatChangeByMobSkill(
				pSkill->GetSkillID(),
				nSLV,
				pSkill->GetLevelData(nSLV),
				0,
				0
			);
		}
	}
}

void User::OnSitRequest(InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	if (!m_pField)
		return;
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnSitResult);
	int nSeatID = iPacket->Decode2();
	if (nSeatID >= 0)
	{
		if (m_pField->OnSitRequest(this, nSeatID))
		{
			oPacket.Encode1(1);
			oPacket.Encode2(nSeatID);
		}
	}
	else if (m_nActivePortableChairID)
	{
		oPacket.Encode1(0);
		m_nActivePortableChairID = 0;
		SendPortableChairEffect(0);
	}
	else
	{
		oPacket.Encode1(0);
		m_pField->OnSitRequest(this, nSeatID);
	}
	SendPacket(&oPacket);
}

void User::OnPortableChairSitRequest(InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	if (m_pField)
	{
		int nItemID = iPacket->Decode4();
		auto pChair = ItemInfo::GetInstance()->GetPortableChairItem(nItemID);
		if (nItemID / 10000 == 301 &&
			pChair &&
			m_pCharacterData->GetItemByID(nItemID) &&
			pChair->nReqLevel <= QWUser::GetLevel(this))
		{
			m_nActivePortableChairID = nItemID;
			m_tPortableChairSittingTime = GameDateTime::GetTime();

			SendPortableChairEffect(0);
			SendCharacterStat(true, 0);
		}
		else
			m_pSocket->GetSocket().close();
	}
}

void User::OnSetActiveEffectItem(InPacket * iPacket)
{
	if (m_pField)
	{
		int nItemID = iPacket->Decode4();
		if (!nItemID || (nItemID / 10000 == 501 && m_pCharacterData->GetItemByID(nItemID)))
		{
			if (nItemID / 1000 != 5011)
				m_pCharacterData->nActiveEffectItemID = nItemID;
			
			OutPacket oPacket;
			oPacket.Encode2(UserSendPacketType::UserRemote_OnSetActiveEffectItem);
			oPacket.Encode4(GetUserID());
			oPacket.Encode4(nItemID);
			m_pField->SplitSendPacket(&oPacket, nullptr);
		}
	}
}

void User::OnChangeStatRequest(InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	unsigned int tCur = GameDateTime::GetTime();

	if (m_pField && tCur - m_tLastRecoveryTime >= 8000)
	{
		int nFlag = iPacket->Decode4(); //Always 20480 ??
		int nHPInc = iPacket->Decode2(), nHPIncSrv = 0;
		int nMPInc = iPacket->Decode2(), nMPIncSrv = 0;
		int nOption = iPacket->Decode1();
		double dRate = m_pField->GetRecoveryRate();

		PortableChairItem* pChair = nullptr;
		if (nOption & 2)
		{
			dRate *= 1.5;
			pChair = ItemInfo::GetInstance()->GetPortableChairItem(m_nActivePortableChairID);
		}

		auto& pEquip = m_pCharacterData->GetItem(GW_ItemSlotBase::EQUIP, -5);
		if (pEquip)
			dRate *= ItemInfo::GetInstance()->GetEquipItem(pEquip->nItemID)->dRecovery;

		long long int liFlag = 0;
		if (nHPInc > 0)
		{
			nHPIncSrv = (int)(((double)SkillInfo::GetInstance()->GetHPRecoveryUpgrade(m_pCharacterData) + 10.0) * dRate);
			if (pChair && pChair->nPortableChairRecoveryRateHP)
				nHPIncSrv = pChair->nPortableChairRecoveryRateHP;
			if (nHPInc > nHPIncSrv) //Might be suspicious hacking.
			{
				SendChatMessage(0, "[Warning]Suspicious hacking for \"HP\" recovery.");
				return;
			} 
			liFlag |= QWUser::IncHP(this, nHPIncSrv, false);
		}
		if (nMPInc > 0)
		{
			nMPIncSrv = (int)(((double)SkillInfo::GetInstance()->GetMPRecoveryUpgrade(m_pCharacterData) + 10.0) * dRate);
			if (pChair && pChair->nPortableChairRecoveryRateMP)
				nMPIncSrv = pChair->nPortableChairRecoveryRateMP;
			if (nMPInc > nMPIncSrv) //Might be suspicious hacking.
			{
				SendChatMessage(0, "[Warning]Suspicious hacking for \"MP\" recovery.");
				return;
			} 
			liFlag |= QWUser::IncMP(this, nMPIncSrv, false);
		}
		SendCharacterStat(false, liFlag);
		m_tLastRecoveryTime = tCur;
	}
}

bool User::IsAbleToLearnSkillByItem(void *pItem, bool &bSucceed, int &nTargetSkill)
{
	auto pSLearn = (SkillLearnItem*)pItem;
	int nItemID = pSLearn->nItemID / 10000;
	if (nItemID != 228 && nItemID != 229)
		return false;

	int nUserJob = m_pCharacterData->mStat->nJob;
	nTargetSkill = 0;
	bSucceed = false;

	for (auto nSkillID : pSLearn->aSkill)
	{
		if (nSkillID / 10000 == nUserJob)
		{
			nTargetSkill = nSkillID;
			break;
		}
	}
	if (!nTargetSkill)
		return false;

	auto pRecord = m_pCharacterData->GetSkill(nTargetSkill);
	if (nItemID == 228)
	{
		if (pRecord)
			return false;
		if (QWUSkillRecord::MakeSkillVisible(this, nTargetSkill, 0, pSLearn->nMasterLevel))
			bSucceed = true;
	}
	else if (nItemID == 229) 
	{
		if (!pRecord ||
			pRecord->nSLV < pSLearn->nReqLevel ||
			pRecord->nMasterLevel >= pSLearn->nMasterLevel)
			return false;
		if ((int)(Rand32::GetInstance()->Random() % 101) <= pSLearn->nSuccessRate &&
			QWUSkillRecord::MasterLevelChange(this, nTargetSkill, pSLearn->nMasterLevel))
			bSucceed = true;
	}
	return true;
}

void User::OnDropMoneyRequest(InPacket * iPacket)
{
	int tRequestTime = iPacket->Decode4();
	int nMoney = iPacket->Decode4();
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	if (m_pField &&
		nMoney >= 10 && nMoney <= 50000 && 
		QWUser::GetMoney(this) >= nMoney)
	{
		SendCharacterStat(true, QWUser::IncMoney(this, -nMoney, true));
		int pcy = GetPosY();
		m_pField->GetSpace2D()->GetFootholdUnderneath(GetPosX(), GetPosY() - 10, &pcy);
		ZUniquePtr<Reward> pReward = AllocObj(Reward);
		pReward->SetItem(nullptr);
		pReward->SetMoney(nMoney);
		m_pField->GetDropPool()->Create(
			pReward,
			GetUserID(),
			0,
			0,
			0,
			GetPosX(),
			GetPosY(),
			GetPosX(),
			pcy,
			0,
			0,
			0,
			false
		);
	}
}

void User::OnCharacterInfoRequest(InPacket *iPacket)
{
	int tRequestTime = iPacket->Decode4();
	int nUserID = iPacket->Decode4();
	auto pUser = User::FindUser(nUserID);
	if (!pUser)
	{
		SendCharacterStat(true, 0);
		return;
	}

	std::lock_guard<std::recursive_mutex> lock(pUser->m_mtxUserLock);
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnCharacterInfo);
	oPacket.Encode4(pUser->GetUserID());
	oPacket.Encode1((char)QWUser::GetLevel(pUser));
	oPacket.Encode2(QWUser::GetJob(pUser));
	oPacket.Encode2((short)QWUser::GetPOP(pUser));
	oPacket.Encode1(0); //Marriage
	oPacket.EncodeStr(pUser->m_sGuildName);

	//PersonalInfo
	oPacket.EncodeStr("MNMS 077");
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);
	oPacket.Encode1(0);

	GW_ItemSlotBase *pPetEquip = nullptr;
	//Encode Pet Info
	for (int i = 0; i < pUser->GetMaxPetIndex(); ++i)
		if (pUser->m_apPet[i])
		{
			oPacket.Encode1(1);
			oPacket.Encode4(pUser->m_apPet[i]->GetItemSlot()->nItemID);
			oPacket.EncodeStr(pUser->m_apPet[i]->GetItemSlot()->strPetName);
			oPacket.Encode1(pUser->m_apPet[i]->GetItemSlot()->nLevel);
			oPacket.Encode2(pUser->m_apPet[i]->GetItemSlot()->nTameness);
			oPacket.Encode1(pUser->m_apPet[i]->GetItemSlot()->nRepleteness);
			oPacket.Encode2(pUser->m_apPet[i]->GetItemSlot()->usPetSkill);

			pPetEquip = (GW_ItemSlotBase*)pUser->m_pCharacterData->GetItem(GW_ItemSlotBase::EQUIP, (-114 - (i * 8)));
			oPacket.Encode4(pPetEquip ? pPetEquip->nItemID : 0);
		}
	oPacket.Encode1(0);

	//Mount Info
	oPacket.Encode1(0);

	//Wishlist Info
	oPacket.Encode1(0);

	SendPacket(&oPacket);
}

unsigned char User::GetGradeCode() const
{
	return m_nGradeCode;
}

void User::SetSkillCooltime(int nReason, int tDuration)
{
	if (!tDuration || m_nGradeCode != eGrade_None)
		return;
	std::lock_guard<std::recursive_mutex> lock(m_pSecondaryStat->m_mtxLock);
	unsigned int tSet = GameDateTime::GetTime() + tDuration;
	m_pSecondaryStat->m_mCooltimeOver[nReason] = tSet;
	SendSkillCooltimeSet(nReason, tSet);
}

unsigned int User::GetSkillCooltime(int nReason)
{
	std::lock_guard<std::recursive_mutex> lock(m_pSecondaryStat->m_mtxLock);
	return m_pSecondaryStat->m_mCooltimeOver[nReason];
}

void User::SendSkillCooltimeSet(int nReason, unsigned int tTime)
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnSkillCooltimeSet);
	oPacket.Encode4(nReason);
	oPacket.Encode2(tTime ? ((tTime - GameDateTime::GetTime()) / 1000) : 0);
	SendPacket(&oPacket);
}

void User::OnGivePopularityRequest(InPacket * iPacket)
{
	int nTargetID = iPacket->Decode4(), nFailedReason = 0;
	auto pTarget = User::FindUser(nTargetID);
	if (!pTarget)
		nFailedReason = GivePopluarityMessage::eUserDoesNotExist;
	else if (QWUser::GetLevel(this) < 15)
		nFailedReason = GivePopluarityMessage::eLevelIsNotSatisfied;

	OutPacket oPacket;
	if (nFailedReason)
	{
		oPacket.Encode2(UserSendPacketType::UserLocal_OnGivePopularityResult);
		oPacket.Encode1(nFailedReason);
		SendPacket(&oPacket);
	}
	else
	{
		oPacket.Encode2(CenterRequestPacketType::CheckGivePopularityRequest);
		oPacket.Encode4((int)m_pSocket->GetSocketID());
		oPacket.Encode4(GetUserID());
		oPacket.Encode4(nTargetID);
		oPacket.Encode1((char)(iPacket->Decode1() != 0));

		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	}
}

void User::SendUseSkillEffect(int nSkillID, int nSLV)
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserRemote_OnEffect);
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

void User::SendUseSkillEffectByParty(int nSkillID, int nSLV)
{
	auto funcEncode = [](OutPacket& oPacket, int nSkillID, int nSLV) {
		oPacket.Encode4(nSkillID);
		oPacket.Encode1((char)nSLV);
		if (nSkillID == 1320006)
			oPacket.Encode1(1);
		else if (nSkillID == 1121001 || nSkillID == 1221001 || nSkillID == 1321001)
			oPacket.Encode1(1);
		oPacket.Encode1(0);
	};
	OutPacket oPacketSelf;
	oPacketSelf.Encode2(UserSendPacketType::UserLocal_OnEffect);
	oPacketSelf.Encode1(Effect::eEffect_OnSkillAppliedByParty);
	funcEncode(oPacketSelf, nSkillID, nSLV);
	SendPacket(&oPacketSelf);

	//For Remote
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserRemote_OnEffect);
	oPacket.Encode4(GetUserID());
	oPacket.Encode1(Effect::eEffect_OnSkillAppliedByParty);
	funcEncode(oPacket, nSkillID, nSLV);
	GetField()->SplitSendPacket(&oPacket, this);
}

void User::SendLevelUpEffect()
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnEffect);
	oPacket.Encode4(GetUserID());
	oPacket.Encode1(Effect::eEffect_LevelUp);
	GetField()->SplitSendPacket(&oPacket, this);
}

void User::SendChangeJobEffect()
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserRemote_OnEffect);
	oPacket.Encode4(GetUserID());
	oPacket.Encode1(Effect::eEffect_ChangeJobEffect);
	GetField()->SplitSendPacket(&oPacket, this);
}

void User::SendPlayPortalSE()
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnEffect);
	oPacket.Encode1(Effect::eEffect_PlayPortalSE);
	SendPacket(&oPacket);
}

void User::OnStatChangeItemUseRequest(InPacket * iPacket, bool bByPet)
{
	int tTick = iPacket->Decode4();
	short nTI = iPacket->Decode2();
	int nItemID = iPacket->Decode4();

	auto &pItem = m_pCharacterData->GetItem(2, nTI);
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

void User::OnMobSummonItemUseRequest(InPacket * iPacket)
{
	if (!GetField() || GetField()->GetFieldLimit() & 0x04)
	{
	FAILED:
		SendCharacterStat(true, 0);
		return;
	}
	std::lock_guard<std::recursive_mutex> lock(GetField()->GetFieldLock());
	int tRequestTime = iPacket->Decode4();
	int nTI = GW_ItemSlotBase::CONSUME;
	int nPOS = iPacket->Decode2();
	int nItemID = iPacket->Decode4();
	auto &pItem = m_pCharacterData->GetItem(nTI, nPOS);
	auto pMobSummonItem = ItemInfo::GetInstance()->GetMobSummonItem(nItemID);

	if (!pItem || !pMobSummonItem || pItem->nItemID != nItemID)
		goto FAILED;

	if (GetField()->GetLifePool()->OnMobSummonItemUseRequest(GetPosX(), GetPosY(), pMobSummonItem, false))
	{
		std::vector<InventoryManipulator::ChangeLog> aLog;
		int nDecRet = 0;
		if (!QWUInventory::RawRemoveItem(this, nTI, nPOS, 1, &aLog, nDecRet, nullptr) || nDecRet != 1)
			goto FAILED;
		QWUInventory::SendInventoryOperation(this, true, aLog);
	}
	else
		goto FAILED;
}

void User::OnSkillLearnItemUseRequest(InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(GetLock());
	int tRequestTime = iPacket->Decode4();
	int nPOS = iPacket->Decode2();
	int nItemID = iPacket->Decode4();
	auto pItem = (GW_ItemSlotBundle*)m_pCharacterData->GetItem(GW_ItemSlotBase::CONSUME, nPOS);
	auto pSkillLearnItem = ItemInfo::GetInstance()->GetSkillLearnItem(nItemID);
	if (pItem->nItemID != nItemID ||
		pItem->nNumber <= 0 ||
		!pSkillLearnItem)
	{
		SendCharacterStat(true, 0);
		return;
	}

	bool bSucceed = false;
	int nTargetSkill = 0;
	if (IsAbleToLearnSkillByItem(pSkillLearnItem, bSucceed, nTargetSkill))
	{
		std::vector<InventoryManipulator::ChangeLog> aChangeLog;
		int nDecRet = 0;
		if (!QWUInventory::RawRemoveItem(
			this,
			GW_ItemSlotBase::CONSUME,
			nPOS,
			1,
			&aChangeLog,
			nDecRet,
			nullptr
		))
		{
			SendNoticeMessage("Incorrect SkillLearn-item use request.");
			WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "Incorrect SkillLearn-item use request nPOS(%d), nItemID(%d)\n", nPOS, nItemID);
			return;
		}
		QWUInventory::SendInventoryOperation(this, false, aChangeLog);
		SendCharacterStat(true, 0);
		SendSkillLearnItemResult(nItemID, nTargetSkill, pSkillLearnItem->nMasterLevel, true, bSucceed);
	}
	else
		SendSkillLearnItemResult(nItemID, nTargetSkill, pSkillLearnItem->nMasterLevel, false, false);
}

void User::SendSkillLearnItemResult(int nItemID, int nTargetSkill, int nMasterLevel, bool bItemUsed, bool bSucceed)
{
	if (!nMasterLevel)
		return;

	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnSkillLearnItemResult);
	oPacket.Encode4(GetUserID());
	oPacket.Encode1(nItemID / 10000 == 229);
	oPacket.Encode4(nTargetSkill);
	oPacket.Encode4(nMasterLevel);
	oPacket.Encode1(bItemUsed ? 1 : 0);
	oPacket.Encode1(bSucceed ? 1 : 0);
	if (bItemUsed)
		m_pField->BroadcastPacket(&oPacket);
	else
		SendPacket(&oPacket);
}

void User::OnPortalScrollUseRequest(InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(GetField()->GetFieldLock());
	int tRequestTime = iPacket->Decode4();
	int nPOS = iPacket->Decode2();
	int nItemID = iPacket->Decode4();
	auto pItem = (GW_ItemSlotBundle*)m_pCharacterData->GetItem(GW_ItemSlotBase::CONSUME, nPOS);
	auto pScroll = ItemInfo::GetInstance()->GetPortalScrollItem(nItemID);
	if (!pItem ||
		!pScroll ||
		!CanAttachAdditionalProcess() ||
		nItemID != pItem->nItemID ||
		!pItem->nNumber)
	{
	PORTAL_SCROLL_USE_FAILED:
		SendCharacterStat(true, 0);
		return;
	}
	int nTo = pScroll->spec["moveTo"];
	if (nTo <= 0 || 
		nTo == 999999999 ||
		!FieldMan::GetInstance()->GetField(nTo))
		nTo = m_pField->GetReturnMap();

	if (!FieldMan::GetInstance()->GetField(nTo) ||
		nTo == m_pField->GetFieldID())
		goto PORTAL_SCROLL_USE_FAILED;

	if (FieldMan::GetInstance()->IsConnected(m_pField->GetFieldID(), nTo))
	{
		int nDecCount = 0;
		std::vector<InventoryManipulator::ChangeLog> aChangeLog;
		InventoryManipulator::RawRemoveItem(
			m_pCharacterData,
			GW_ItemSlotBase::CONSUME,
			nPOS,
			1,
			&aChangeLog,
			&nDecCount,
			nullptr
		);

		/*if (nDecCount != 1)
			throw "Invalid portal-scroll-item use request.";*/
		
		QWUInventory::SendInventoryOperation(this, true, aChangeLog);
		SendCharacterStat(true, 0);
		TryTransferField(nTo, "");
	}
}

void User::OnConsumeCashItemUseRequest(InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);

	int nPOS = iPacket->Decode2();
	int nItemID = iPacket->Decode4();
	auto pItem = m_pCharacterData->GetItem(GW_ItemSlotBase::CASH, nPOS);
	if (!pItem || pItem->nItemID != nItemID)
	{
	INVALID_CONSUME_CASHITEM:
		SendCharacterStat(true, 0);
		return;
	}

	int nType = ItemInfo::GetConsumeCashItemType(nItemID), nResult = 0;
	switch (nType)
	{
		case ItemInfo::CashItemType::CashItemType_SetPetName:
			break;
		case ItemInfo::CashItemType::CashItemType_SendMemo:
			break;
		case ItemInfo::CashItemType::CashItemType_StatChange:
			break;
		case ItemInfo::CashItemType::CashItemType_SkillChange:
			break;
		case ItemInfo::CashItemType::CashItemType_SetItemName:
			break;
		case ItemInfo::CashItemType::CashItemType_ItemProtector:
			break;
		case ItemInfo::CashItemType::CashItemType_Incubator:
			break;
		case ItemInfo::CashItemType::CashItemType_PetSkill:
			nResult = UserCashItemImpl::ConsumePetSkill(this, nItemID, iPacket);
			break;
		case ItemInfo::CashItemType::CashItemType_ShopScanner:
			break;
		case ItemInfo::CashItemType::CashItemType_MapTransfer:
			break;
		case ItemInfo::CashItemType::CashItemType_ADBoard:
			break;
		
		//Skip MapleTVs (which do not exist in TW ver.) impl.

		case ItemInfo::CashItemType::CashItemType_Morph:
			break;
		case ItemInfo::CashItemType::CashItemType_AvatarMegaphone:
			nResult = UserCashItemImpl::ConsumeAvatarMegaphone(this, nItemID, iPacket);
			break;
		case ItemInfo::CashItemType::CashItemType_NameChange:
			break;
		case ItemInfo::CashItemType::CashItemType_TransferWorld:
			break;
		case ItemInfo::CashItemType::CashItemType_PetFood:
			break;
		case ItemInfo::CashItemType::CashItemType_SpeakerChannel:
			nResult = UserCashItemImpl::ConsumeSpeakerChannel(this, iPacket);
			break;
		case ItemInfo::CashItemType::CashItemType_SpeakerWorld:
		case ItemInfo::CashItemType::CashItemType_SpeakerHeart:
		case ItemInfo::CashItemType::CashItemType_SpeakerSkull:
			nResult = UserCashItemImpl::ConsumeSpeakerWorld(this, nType, iPacket);
			break;
		case ItemInfo::CashItemType::CashItemType_Weather:
			break;
		case ItemInfo::CashItemType::CashItemType_JukeBox:
			break;
	}
	if (nResult)
	{
		int nDecCount = 0;
		std::vector<InventoryManipulator::ChangeLog> aLog;
		bool bRemove = QWUInventory::RawRemoveItem(
			this, GW_ItemSlotBase::CASH, nPOS, 1, &aLog, nDecCount, nullptr
		);
		if (!bRemove)
		{
			SendNoticeMessage(GET_STRING(GameSrv_User_Invalid_ConsumeItem_Usage));
			//Log to files or send some packets to the Center.
		}
		QWUInventory::SendInventoryOperation(
			this, true, aLog
		);
	}
	else
		goto INVALID_CONSUME_CASHITEM;
}

void User::OnCenterCashItemResult(InPacket* iPacket)
{
	int nResultType = iPacket->Decode2();
	switch (nResultType)
	{
		case CenterCashItemRequestType::eExpireCashItemRequest:
		{
			std::vector<InventoryManipulator::ChangeLog> aChangeLog;
			int nSize = iPacket->Decode1(), nTI = 0, nPOS = 0, nDecCount = 0;
			for (int i = 0; i < nSize; ++i)
			{
				nTI = iPacket->Decode1();
				nPOS = m_pCharacterData->FindCashItemSlotPosition(nTI, iPacket->Decode8());
				if (nPOS) 
				{
					auto pItem = m_pCharacterData->mItemSlot[nTI][nPOS];
					if (!pItem)
						continue;

					OutPacket oPacket;
					oPacket.Encode2(UserSendPacketType::UserLocal_OnMessage);
					oPacket.Encode1((char)Message::eCashItemExpireMessage);
					oPacket.Encode4(pItem->nItemID);
					SendPacket(&oPacket);

					QWUInventory::RawRemoveItem(
						this, nTI, nPOS, 1, &aChangeLog, nDecCount, nullptr
					);
				}
			}
			QWUInventory::SendInventoryOperation(
				this, true, aChangeLog
			);
			break;
		}
	}
}

void User::OnMigrateOut()
{
	//LeaveField();
	m_pSocket->GetSocket().close();
}

void User::SendSetFieldPacket(bool bCharacterData)
{
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnSetFieldStage);
	oPacket.Encode4(WvsBase::GetInstance<WvsGame>()->GetChannelID()); //Channel ID
	oPacket.Encode1(1); //bCurFieldKey
	oPacket.Encode1(bCharacterData ? 1 : 0); //bCharacterData
	oPacket.Encode2(0);

	if (bCharacterData)
	{
		auto uSeed1 = (unsigned int)Rand32::GetInstance()->Random();
		auto uSeed2 = (unsigned int)Rand32::GetInstance()->Random();
		auto uSeed3 = (unsigned int)Rand32::GetInstance()->Random();

		m_pCalcDamage->SetSeed(uSeed1, uSeed2, uSeed3);
		//m_RndActionMan->Seed(uSeed1, uSeed2, uSeed3);

		oPacket.Encode4(uSeed1);
		oPacket.Encode4(uSeed2);
		oPacket.Encode4(uSeed3);
		EncodeCharacterData(&oPacket);
		//CheckBerserk();
		//InsertCalcDamageStat();
	}
	else
	{
		oPacket.Encode4(m_pCharacterData->mStat->nPosMap);
		oPacket.Encode1(m_pCharacterData->mStat->nPortal);
		oPacket.Encode2(m_pCharacterData->mStat->nHP); //HP
		oPacket.Encode1(m_bChase);
		if (m_bChase)
		{
			oPacket.Encode4(m_nTargetPosition_X);
			oPacket.Encode4(m_nTargetPosition_Y);
		}
	}
	oPacket.Encode8(GameDateTime::GetCurrentDate()); //SystemTimeToFileTime
	SendPacket(&oPacket);
}

bool User::CanAttachAdditionalProcess()
{
	return m_pSocket &&
		m_nTransferStatus == TransferStatus::eOnTransferNone &&
		m_pCharacterData->mStat->nHP &&
		!m_pScript &&
		!m_pMiniRoom &&
		!m_pStoreBank &&
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

ZSharedPtr<User> User::FindUser(int nUserID)
{
	return WvsGame::GetInstance<WvsGame>()->FindUser(nUserID);
}

ZSharedPtr<User> User::FindUserByName(const std::string & strName)
{
	return WvsGame::GetInstance<WvsGame>()->FindUserByName(strName);
}

void User::Broadcast(OutPacket *oPacket)
{
	std::lock_guard<std::recursive_mutex>(WvsBase::GetInstance<WvsGame>()->GetUserLock());
	auto& mUser = WvsBase::GetInstance<WvsGame>()->GetConnectedUser();
	for (auto& prUser : mUser)
		prUser.second->SendPacket(oPacket);
}

void User::SendDropPickUpResultPacket(bool bPickedUp, bool bIsMoney, int nItemID, int nCount, bool bOnExcelRequest)
{
	SendCharacterStat(bOnExcelRequest, 0);
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnMessage);
	oPacket.Encode1((char)Message::eDropPickUpMessage);
	if (bPickedUp)
	{
		oPacket.Encode1(bIsMoney == true ? 1 : 0);
		if (bIsMoney)
		{
			//oPacket.Encode1(0);
			oPacket.Encode4(nCount);
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
	oPacket.Encode2(UserSendPacketType::UserLocal_OnMessage);
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
	if (m_pScript)
		FreeObj(m_pScript); //Revamped MemoryPool seems work well.

	m_pScript = (pScript);
	if (m_pScript)
		m_pScript->SetUser(this);
}

void User::OnSelectNpc(InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> lcok(m_mtxUserLock);
	int nLifeNpcID = iPacket->Decode4();
	auto &pNpc = m_pField->GetLifePool()->GetNpc(nLifeNpcID);
	if (!CanAttachAdditionalProcess() || !pNpc)
	{
		SendCharacterStat(false, 0);
		return;
	}

	auto pTemplate = NpcTemplate::GetInstance()->GetNpcTemplate(pNpc->GetTemplateID());
	if (pTemplate && pTemplate->GetTrunkCost())
	{
		m_nTrunkTemplateID = pNpc->GetTemplateID();
		OutPacket oPacket;
		oPacket.Encode2(CenterRequestPacketType::TrunkRequest);
		oPacket.Encode1(Trunk::TrunkRequest::rq_Trunk_Load);
		oPacket.Encode4(GetAccountID());
		oPacket.Encode4(GetUserID());
		WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
		return;
	}
	if (pTemplate && pNpc->GetTemplateID() == 9030000)
	{
		m_nTrunkTemplateID = pNpc->GetTemplateID();
		SetStoreBank(AllocObjCtor(StoreBank)(this));
		m_pStoreBank->SetStoreBankTemplateID(m_nTrunkTemplateID);
		m_pStoreBank->OnSelectStoreBankNPC();
		return;
	}
	if (pTemplate && pTemplate->HasShop())
	{
		OutPacket oPacket;
		oPacket.Encode2((int)NPCSendPacketTypes::NPC_OnNpcShopItemList);
		pTemplate->EncodeShop(this, &oPacket);
		m_pTradingNpc = pNpc;
		SendPacket(&oPacket);
		return;
	}

	if (pNpc != nullptr && GetScript() == nullptr)
	{
		auto pScript = ScriptMan::GetInstance()->CreateScript(
			"./DataSrv/Script/Npc/" + std::to_string(pNpc->GetTemplateID()) + ".lua",
			{ pNpc->GetTemplateID(), pNpc->GetField() }
		);
		if (pScript == nullptr) 
		{
			auto sScriptName = "s_" + pTemplate->GetScriptName();
			auto sScriptFile = ScriptMan::GetInstance()->SearchScriptNameByFunc(
				"Npc",
				sScriptName
			);

			pScript = ScriptMan::GetInstance()->CreateScript(
				sScriptFile,
				{ pNpc->GetTemplateID(), pNpc->GetField() }
			);
			if (pScript)
			{
				SetScript(pScript);
				pScript->Run(sScriptName);
			}
			else
				SendChatMessage(0, 
					"Failed to invoke built-in script (template id = : ["  + std::to_string(pNpc->GetTemplateID()) + "] by calling : [" + sScriptName + "]).");
			return;
		}
		else
		{
			SetScript(pScript);
			pScript->Run();
		}
	}
}

void User::OnScriptMessageAnswer(InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lcok(m_mtxUserLock);
	if (!GetScript())
		return;
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
	std::lock_guard<std::recursive_mutex> lcok(m_mtxUserLock);

	char nAction = iPacket->Decode1();
	int nQuestID = iPacket->Decode2(), nNpcID; 
	NpcTemplate* pNpcTemplate = nullptr;
	Npc* pNpc;
	if (nAction != 0 && nAction != 3)
	{
		nNpcID = iPacket->Decode4();
		auto& pUniqueNpc = m_pField->GetLifePool()->GetNpc(nNpcID);
		pNpcTemplate = NpcTemplate::GetInstance()->GetNpcTemplate(nNpcID);
		if (pUniqueNpc)
			pNpc = (Npc*)pUniqueNpc;
		
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

	enum QuestActionType
	{
		Action_LostQuestItem = 0x00,
		Action_AcceptQuest,
		Action_CompleteQuest,
		Action_ResignQuest,
		Action_ScriptStart,
		Action_ScriptEnd,
	};

	WvsLogger::LogFormat("OnQuestRequest npc id = %d, quest action = %d, pNpc == nullptr ? %d\n", 
		nNpcID, 
		(int)nAction,
		(int)(pNpc == nullptr));
	switch (nAction)
	{
		case QuestActionType::Action_LostQuestItem:
			OnLostQuestItem(iPacket, nQuestID);
			break;
		case QuestActionType::Action_AcceptQuest:
			OnAcceptQuest(iPacket, nQuestID, nNpcID, pNpc);
			break;
		case QuestActionType::Action_CompleteQuest:
			OnCompleteQuest(iPacket, nQuestID, nNpcID, pNpc, QuestMan::GetInstance()->IsAutoCompleteQuest(nQuestID));
			break;
		case QuestActionType::Action_ResignQuest:
			OnResignQuest(iPacket, nQuestID);
			break;
		case QuestActionType::Action_ScriptStart:
		case QuestActionType::Action_ScriptEnd:
			OnScriptLinkedQuest(iPacket, nQuestID, nNpcID, pNpc, nAction == QuestActionType::Action_ScriptEnd);
			break;
	}
}

void User::OnAcceptQuest(InPacket * iPacket, int nQuestID, int dwTemplateID, Npc * pNpc)
{
	if (!QuestMan::GetInstance()->CheckStartDemand(nQuestID, this))
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "[User][OnAcceptQuest]Unable to pass start-demand checking, user name: %s, quest id: %d\n",
			m_pCharacterData->strName.c_str(),
			nQuestID);
		return;
	}
	//WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "任務檢測成功。\n");
	TryQuestStartAct(nQuestID, dwTemplateID, pNpc);
}

void User::OnCompleteQuest(InPacket * iPacket, int nQuestID, int dwTemplateID, Npc * pNpc, bool bIsAutoComplete)
{
	if (!QuestMan::GetInstance()->CheckCompleteDemand(nQuestID, this))
	{
		WvsLogger::LogFormat(WvsLogger::LEVEL_ERROR, "[User][OnCompleteQuest]Unable to pass complete-demand checking, user name: %s, quest id: %d\n",
			m_pCharacterData->strName.c_str(),
			nQuestID);
		return;
	}
	//WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "任務檢測成功。\n");
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
	std::vector<ExchangeElement> aExchange;
	for (auto& actItem : aActItem)
	{
		int nCount = actItem->nCount - GetCharacterData()->GetItemCount(
			actItem->nItemID / 1000000, actItem->nItemID);
		if (nCount > 0)
		{
			aExchange.push_back({});
			aExchange.back().m_nItemID = actItem->nItemID;
			aExchange.back().m_nCount = nCount;
		}
	}
	if (QWUInventory::Exchange(this, 0, aExchange))
	{
		SendNoticeMessage(GET_STRING(GameSrv_User_Insufficient_SlotCount));
		SendCharacterStat(false, 0);
	}
}

void User::OnScriptLinkedQuest(InPacket * iPacket, int nQuestID, int dwTemplateID, Npc * pNpc, int nScriptActCategory)
{
	if ((!nScriptActCategory && !QuestMan::GetInstance()->CheckStartDemand(nQuestID, this))
		|| !QuestMan::GetInstance()->CheckCompleteDemand(nQuestID, this))
	{
		SendNoticeMessage(GET_STRING(GameSrv_User_QuestDemand_Check_Failed));
		SendCharacterStat(false, 0);
		return;
	}
	auto pScript = ScriptMan::GetInstance()->CreateScript(
		"./DataSrv/Script/Quest/" + std::to_string(nQuestID) + ".lua",
		{ dwTemplateID, m_pField }
	);
	if (pScript == nullptr)
	{
		auto pAct = !nScriptActCategory ?
			QuestMan::GetInstance()->GetStartAct(nQuestID) :
			QuestMan::GetInstance()->GetCompleteAct(nQuestID);

		auto sScriptName = pAct ? "s_" + pAct->sScriptName : "";
		auto sScriptFile = ScriptMan::GetInstance()->SearchScriptNameByFunc(
			"Portal",
			sScriptName
		);

		pScript = ScriptMan::GetInstance()->CreateScript(
			sScriptFile,
			{ 0, m_pField }
		);

		if (pScript)
		{
			SetScript(pScript);
			pScript->Run(sScriptName);
		}
		else
			SendChatMessage(0, "Unable to execute quest script: quest id = [" + std::to_string(nQuestID) + "], default script name = [" + sScriptName + "].");
		return;
	}
	SetScript(pScript);
	pScript->PushInteger("questID", nQuestID);

	if (!nScriptActCategory)
		pScript->Run("start");
	else
		pScript->Run("complete");
}

void User::TryQuestStartAct(int nQuestID, int nNpcID, Npc * pNpc)
{
	auto pStartAct = QuestMan::GetInstance()->GetStartAct(nQuestID);
	if (!pStartAct)
		return;
	QWUQuestRecord::Set(this, nQuestID, pStartAct->sInfo);
	if (!TryExchange(pStartAct->aActItem))
	{
		QWUQuestRecord::Remove(this, nQuestID, false);
		return;
	}

	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnQuestResult);
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
	if (!TryExchange(pCompleteAct->aActItem))
		return;

	long long int liFlag = 0;
	if (pCompleteAct->nEXP >= 0)
		liFlag |= QWUser::IncEXP(this, pCompleteAct->nEXP, false);
	liFlag |= QWUser::IncMoney(this, pCompleteAct->nMoney, false);
	SendCharacterStat(false, liFlag);
	QWUQuestRecord::SetComplete(this, nQuestID);
	SendQuestEndEffect();
}

bool User::TryExchange(const std::vector<ActItem*>& aActItem)
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
	std::vector<ExchangeElement> aExchange;
	for (auto& pItem : aActItem)
	{
		if (!AllowToGetQuestItem(pItem))
			continue;
		if (pItem->nProp != 0 && pItem->nItemID != nSelectedItemID)
			continue;

		aExchange.push_back({});
		aExchange.back().m_nItemID = pItem->nItemID;
		aExchange.back().m_nCount = pItem->nCount;
	}
	int nRet = QWUInventory::Exchange(this, 0, aExchange);
	switch (nRet)
	{
		case InventoryManipulator::ExchangeResult::Exchange_InsufficientSlotCount:
			SendNoticeMessage(GET_STRING(GameSrv_User_Insufficient_SlotCount));
			break;
		case InventoryManipulator::ExchangeResult::Exchange_InsufficientItemCount:
			SendNoticeMessage(GET_STRING(GameSrv_User_Insufficient_ItemCount));
			break;
		case InventoryManipulator::ExchangeResult::Exchange_InsufficientMeso:
			SendNoticeMessage(GET_STRING(GameSrv_User_Insufficient_Money));
			break;
		default:
			return true;
	}
	return false;
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
	oPacket.Encode2((short)UserSendPacketType::UserLocal_OnMessage);
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
	oPacketLocal.Encode2(UserSendPacketType::UserLocal_OnEffect);
	oPacketLocal.Encode1(Effect::eEffect_QuestCompleteEffect);
	SendPacket(&oPacketLocal);

	OutPacket oPacketRemote;
	oPacketRemote.Encode2(UserSendPacketType::UserLocal_OnEffect);
	oPacketRemote.Encode4(GetUserID());
	oPacketRemote.Encode1(Effect::eEffect_QuestCompleteEffect);
	GetField()->SplitSendPacket(&oPacketRemote, this);
}

void User::SendChatMessage(int nType, const std::string & sMsg)
{
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketType::UserLocal_OnBroadcastMsg);
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
	oPacket.Encode2((short)UserSendPacketType::User_OnFuncKeyMapped);
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

		oPacket.Encode2((short)UserSendPacketType::User_OnFuncKeyMapped + std::min(nType, 2));
		oPacket.Encode4(nItemID);
		SendPacket(&oPacket);
	}
	//SendFuncKeyMapped();
}

void User::OnPetPacket(InPacket * iPacket)
{
	//int nIndex = iPacket->Decode4();
	int nType = iPacket->Decode2();
	long long int liCashItemSN = iPacket->Decode8();
	int nIndex = -1;
	for(int i = 0; i < GetMaxPetIndex(); ++i)
		if (m_apPet[i] && m_apPet[i]->m_pPetSlot->liCashItemSN == liCashItemSN)
		{
			nIndex = i;
			break;
		}
	if (nIndex < 0)
		return;

	if (m_apPet[nIndex] != nullptr)
		m_apPet[nIndex]->OnPacket(iPacket, nType);
}

void User::ActivatePet(int nPos, int nRemoveReaseon, bool bOnInitialize)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	int nAvailableIdx = -1;
	int nMaxIndex = GetMaxPetIndex();
	std::vector<InventoryManipulator::ChangeLog> aChangeLog;
	ZSharedPtr<GW_ItemSlotBase> pItem = (GetCharacterData()->GetItem(GW_ItemSlotBase::CASH, nPos));

	if (!pItem || pItem->bIsPet == false)
		return;

	for (int i = 0; i < nMaxIndex; ++i)
		if (pItem && m_apPet[i] && m_apPet[i]->m_pPetSlot->nPOS == nPos)
		{
			((GW_ItemSlotPet*)pItem)->nActiveState = 0;
			m_apPet[i]->OnLeaveField(nRemoveReaseon);
			m_apPet[i].reset(nullptr);
			nAvailableIdx = -1;
			break;
		}
		else if (nAvailableIdx == -1 && m_apPet[i] == nullptr)
			nAvailableIdx = i;
	if (nAvailableIdx >= 0 && nPos > 0)
	{
		m_apPet[nAvailableIdx].reset(MakeUnique<Pet>(pItem));
		((GW_ItemSlotPet*)pItem)->nActiveState = 1;
		m_apPet[nAvailableIdx]->SetIndex(nAvailableIdx);
		m_apPet[nAvailableIdx]->Init(this);
		m_apPet[nAvailableIdx]->OnEnterField(m_pField);

		SendCharacterStat(false, BasicStat::BS_Pet);
	}
	else
		((GW_ItemSlotPet*)pItem)->nActiveState = 0;
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

void User::ReregisterPet()
{
	int nMaxPetIndex = GetMaxPetIndex();
	for (int i = 0; i < nMaxPetIndex; ++i)
		if (m_apPet[i] != nullptr)
			m_apPet[i]->OnEnterField(m_pField);
}

void User::UpdateActivePet(unsigned int tCur)
{
	if (!m_pField)
		return;

	std::lock_guard<std::recursive_mutex> fieldLock(m_pField->GetFieldLock());
	std::lock_guard<std::recursive_mutex> userLock(m_mtxUserLock);

	bool bRemove = false;
	for(int i = 0; i < MAX_PET_INDEX; ++i)
		if (m_apPet[i])
		{
			bRemove = false;
			m_apPet[i]->Update(tCur, bRemove);
			if (bRemove)
				ActivatePet(m_apPet[i]->GetItemSlot()->nPOS, 1, 0);
		}
}

void User::OnPetFoodItemUseRequest(InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	
	Pet *pPet = nullptr;
	int nLowestRepleteness = 100;
	for(int i = 0; i < MAX_PET_INDEX; ++i)
		if (m_apPet[i] && nLowestRepleteness > m_apPet[i]->GetItemSlot()->nRepleteness)
		{
			pPet = m_apPet[i];
			nLowestRepleteness = m_apPet[i]->GetItemSlot()->nRepleteness;
		}

	if (!pPet)
		SendCharacterStat(true, 0);
	else
	{
		iPacket->Decode4(); //tRequestTime
		int nPOS = iPacket->Decode2();
		int nItemID = iPacket->Decode4();

		auto pItem = GetCharacterData()->GetItem(GW_ItemSlotBase::CONSUME, nPOS);
		auto pFoodItem = ItemInfo::GetInstance()->GetPetFoodItem(nItemID);

		if (!pItem || pItem->nItemID != nItemID || !pFoodItem || QWUInventory::RemoveItem(this, pItem) != 1)
		{
			return;
			SendCharacterStat(true, 0);
		}

		pPet->OnEatFood(pFoodItem->niRepleteness);
		SendCharacterStat(true, 0);
	}
}

void User::OnSummonedPacket(InPacket * iPacket)
{
	int nType = iPacket->Decode2();
	int nFieldObjID = iPacket->Decode4();
	auto pSummoned = m_pField->GetSummonedPool()->GetSummoned(nFieldObjID);
	if (pSummoned)
		pSummoned->OnPacket(iPacket, nType);
}

void User::ReregisterSummoned()
{
	for (auto& pSummoned : m_lSummoned)
		m_pField->GetSummonedPool()->CreateSummoned(this, pSummoned, m_ptPos);
}

void User::CreateSummoned(const SkillEntry * pSkill, int nSLV, const FieldPoint & pt, bool bMigrate)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	if (!m_pField)
	{
		m_aMigrateSummoned.push_back(pSkill->GetSkillID());
		return;
	}
	std::vector<Summoned*> aSummonedToRemove;
	for (auto& pSummoned : m_lSummoned)
	{
		if (!(SkillInfo::IsPuppetSkill(pSkill->GetSkillID()) ^ SkillInfo::IsPuppetSkill(pSummoned->GetSkillID())))
			aSummonedToRemove.push_back(pSummoned);
	}
	for (auto& pSummoned : aSummonedToRemove)
		m_pField->GetSummonedPool()->RemoveSummoned(GetUserID(), pSummoned->GetSkillID(), 0);

	auto pSummoned = m_pField->GetSummonedPool()->CreateSummoned(
		this, 
		pSkill->GetSkillID(), 
		nSLV, m_ptPos, 
		GameDateTime::GetTime() + pSkill->GetLevelData(nSLV)->m_nTime
	);
	if (pSummoned)
		m_lSummoned.push_back(pSummoned);
}

//nForceRemoveSkillID = -1 means that remove all summoneds.
void User::RemoveSummoned(int nSkillID, int nLeaveType, int nForceRemoveSkillID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	if (nForceRemoveSkillID != 0)
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

void User::RemoveSummoned(Summoned *pSummoned)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	m_lSummoned.erase(
		std::find(m_lSummoned.begin(), m_lSummoned.end(), pSummoned)
	);
}

void User::AddPartyInvitedCharacterID(int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	m_snPartyInvitedCharacterID.insert(nCharacterID);
}

bool User::IsPartyInvitedCharacterID(int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);

	return m_snPartyInvitedCharacterID.find(nCharacterID) 
		!= m_snPartyInvitedCharacterID.end();
}

void User::RemovePartyInvitedCharacterID(int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	m_snPartyInvitedCharacterID.erase(nCharacterID);
}

void User::ClearPartyInvitedCharacterID()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
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
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);

	if (GetPartyID() == -1)
		return;
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserRemote_OnReceiveHP);
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

int User::GetMCarnivalTeam() const
{
	return m_nTeamForMCarnival;
}

void User::SetMCarnivalTeam(int nTeam)
{
	m_nTeamForMCarnival = nTeam;
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
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	m_snGuildInvitedCharacterID.erase(nCharacterID);
}

void User::ClearGuildInvitedCharacterID()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
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
	oPacket.Encode2(UserSendPacketType::UserRemote_OnGuildNameChanged);
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
	oPacket.Encode2(UserSendPacketType::UserRemote_OnGuildMarkChanged);
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
	oPacket.Encode2(CenterRequestPacketType::GroupMessage);
	oPacket.Encode4(GetUserID());
	oPacket.Encode1((char)nType);
	oPacket.Encode1(nReceiverNum);
	for (int i = 0; i < nReceiverNum; ++i)
		oPacket.Encode4(iPacket->Decode4());

	oPacket.Encode2(FieldSendPacketType::Field_OnGroupMessage);
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
			oPacketReply.Encode2(FieldSendPacketType::Field_OnWhisper);
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
				oPacketSend.Encode2(FieldSendPacketType::Field_OnWhisper);
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
			oPacket.Encode2(CenterRequestPacketType::WhisperMessage);
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
		std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
		OutPacket oPacket;
		oPacket.Encode2(UserSendPacketType::UserCommon_OnMiniRoomBalloon);
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

void User::SetEntrustedShopOpened(bool bOpened)
{
	m_bHasOpenedEntrustedShop = bOpened;
}

void User::CreateEmployee(bool bOpen)
{
	if (GetMiniRoom())
		if (m_pField->GetLifePool()->CreateEmployee(
			GetPosX(),
			GetPosY(),
			GetUserID(),
			GetName(),
			GetMiniRoom(),
			GetFh()
		))
		{
			OutPacket oPacket;
			oPacket.Encode2(CenterRequestPacketType::EntrustedShopRequest);
			oPacket.Encode1(EntrustedShopMan::EntrustedShopRequest::req_EShop_RegisterShop);
			oPacket.Encode4(GetUserID());
			WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
		}
}

void User::OnOpenEntrustedShop(InPacket * iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::EntrustedShopRequest);
	oPacket.Encode1(EntrustedShopMan::EntrustedShopRequest::req_EShop_OpenCheck);
	oPacket.Encode4(GetUserID());
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
}

void User::OnTrunkResult(InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxUserLock);
	int nResult = iPacket->Decode1();
	if (nResult != Trunk::TrunkRequest::rq_Trunk_Load && !m_pTrunk)
		return;

	switch (nResult)
	{
		case Trunk::TrunkResult::res_Trunk_Load:
		{
			m_pTrunk.reset(MakeUnique<Trunk>());
			m_pTrunk->Decode(iPacket);
			m_pTrunk->m_nTrunkTemplateID = m_nTrunkTemplateID;
			m_pTrunk->m_nTrunkCost = NpcTemplate::GetInstance()->GetNpcTemplate(m_nTrunkTemplateID)->GetTrunkCost();
			m_nTrunkTemplateID = 0;
			m_pTrunk->OnLoadDone(this, iPacket);
			break;
		}
		case Trunk::TrunkResult::res_Trunk_MoveSlotToTrunk:
			m_pTrunk->OnMoveSlotToTrunkDone(this, iPacket);
			break;
		case Trunk::TrunkResult::res_Trunk_MoveTrunkToSlot:
			m_pTrunk->OnMoveTrunkToSlotDone(this, iPacket);
			break;
		case Trunk::TrunkResult::res_Trunk_WithdrawMoney:
			m_pTrunk->OnWithdrawMoneyDone(this, iPacket);
			break;
	}
}

void User::SetTrunk(Trunk *pTrunk)
{
	m_pTrunk.reset(pTrunk);
}

ZUniquePtr<StoreBank>& User::GetStoreBank()
{
	return m_pStoreBank;
}

void User::SetStoreBank(StoreBank *pStoreBank)
{
	m_pStoreBank.reset(pStoreBank);
}

void User::SendQuestRecordMessage(int nKey, int nState, const std::string & sStringRecord)
{
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketType::UserLocal_OnMessage);
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
			oPacket.Encode8(GameDateTime::GetCurrentDate());
			break;
	}
	SendPacket(&oPacket);
}

void User::SendIncEXPMessage(bool bIsLastHit, int nIncEXP, bool bOnQuest, int nIncEXPBySMQ, int nEventPercentage, int nPartyBonusPercentage, int nPlayTimeHour, int nQuestBonusRate, int nQuestBonusRemainCount, int nPartyBonusEventRate, int nWeddingBonusEXP)
{
	OutPacket oPacket;
	oPacket.Encode2((short)UserSendPacketType::UserLocal_OnMessage);
	oPacket.Encode1((char)Message::eIncEXPMessage);
	oPacket.Encode1(bIsLastHit ? 1 : 0);
	oPacket.Encode4(nIncEXP);
	oPacket.Encode1(bOnQuest ? 1 : 0);
	oPacket.Encode4(nIncEXPBySMQ);
	oPacket.Encode1(nEventPercentage);
	oPacket.Encode1(nPartyBonusPercentage);
	oPacket.Encode4(nWeddingBonusEXP);
	if (nEventPercentage > 0)
		oPacket.Encode1(nPlayTimeHour);
	if (bOnQuest)
	{
		oPacket.Encode1(nQuestBonusRate);
		if (nQuestBonusRate > 0)
			oPacket.Encode1(nQuestBonusRemainCount);
	}
	oPacket.Encode1(nPartyBonusEventRate);
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
	ValidateStat();
	SendCharacterStat(false, 0);
	SendFuncKeyMapped();
	m_pField->OnEnter(this);
	auto bindT = std::bind(&User::Update, this);
	m_pUpdateTimer.reset(AsyncScheduler::CreateTask(bindT, 2000, true));
	m_pUpdateTimer->Start();
	SetTransferStatus(TransferStatus::eOnTransferNone);
	QWUQuestRecord::ValidateMobCountRecord(this);

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
	oPacket.Encode2(CenterRequestPacketType::PartyRequest);
	oPacket.Encode1(PartyMan::PartyRequest::rq_Party_Load);
	oPacket.Encode4(GetUserID());
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);

	OutPacket oPacketForGuildLoading;
	oPacketForGuildLoading.Encode2(CenterRequestPacketType::GuildRequest);
	oPacketForGuildLoading.Encode1(GuildMan::GuildRequest::rq_Guild_Load);
	oPacketForGuildLoading.Encode4(GetUserID());
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacketForGuildLoading);

	OutPacket oPacketForFriendLoading;
	oPacketForFriendLoading.Encode2(CenterRequestPacketType::FriendRequest);
	oPacketForFriendLoading.Encode1(FriendMan::FriendRequest::rq_Friend_Load);
	oPacketForFriendLoading.Encode4(GetUserID());
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacketForFriendLoading);

	m_nGradeCode = m_pCharacterData->nGradeCode;
	m_tMigrateTime = GameDateTime::GetTime();
}
