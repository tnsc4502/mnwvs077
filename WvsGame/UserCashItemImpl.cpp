/*
09/12/2019 added.
Implementations of cash item usages.
*/

#include "UserCashItemImpl.h"
#include "User.h"
#include "QWUser.h"
#include "QWUInventory.h"
#include "QWUSkillRecord.h"
#include "WvsGame.h"
#include "ItemInfo.h"

#include "..\Database\GW_ItemSlotPet.h"
#include "..\Database\GA_Character.hpp"
#include "..\WvsLib\String\StringPool.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsCenter\CenterPacketTypes.hpp"
#include "..\WvsGame\UserPacketTypes.hpp"

int UserCashItemImpl::ConsumeSpeakerChannel(User *pUser, InPacket *iPacket)
{
	std::string sMsg = pUser->GetName();
	sMsg += " : " + iPacket->DecodeStr();

	if (sMsg.length() >= 80)
	{
		pUser->SendNoticeMessage(GET_STRING(GameSrv_User_Invalid_Chat_Msg));
		return 0;
	}

	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnBroadcastMsg);
	oPacket.Encode1(BroadcastMsgType::e_BroadcastMsg_SpeakerChannel);
	oPacket.EncodeStr(sMsg);
	User::Broadcast(&oPacket);

	return 1;
}

int UserCashItemImpl::ConsumeSpeakerWorld(User *pUser, int nCashItemType, InPacket *iPacket)
{
	std::string sMsg = pUser->GetName();
	sMsg += " : " + iPacket->DecodeStr();

	if (sMsg.length() >= 60)
	{
		pUser->SendNoticeMessage(GET_STRING(GameSrv_User_Invalid_Chat_Msg));
		return 0;
	}

	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::BroadcastPacket);
	oPacket.Encode1(0); //nGameSrvCount = 0 --> broadcast to all game servers.
	oPacket.Encode2(UserSendPacketType::UserLocal_OnBroadcastMsg);

	if (nCashItemType == ItemInfo::CashItemType::CashItemType_SpeakerHeart)
		oPacket.Encode1(BroadcastMsgType::e_BroadcastMsg_SpeakerHeart);
	else if(nCashItemType == ItemInfo::CashItemType::CashItemType_SpeakerSkull)
		oPacket.Encode1(BroadcastMsgType::e_BroadcastMsg_SpeakerSkull);
	else
		oPacket.Encode1(BroadcastMsgType::e_BroadcastMsg_SpeakerWorld);

	oPacket.EncodeStr(sMsg);
	oPacket.Encode1(pUser->GetChannelID());
	oPacket.Encode1(iPacket->Decode1() != 0 ? 1 : 0);
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	return 1;
}

int UserCashItemImpl::ConsumeAvatarMegaphone(User *pUser, int nItemID, InPacket *iPacket)
{
	if (QWUser::GetLevel(pUser) <= 10)
	{
		pUser->SendNoticeMessage(GET_STRING(GameSrv_User_Chat_Level_Not_Reached));
		return 0;
	}
	std::string sMsg = iPacket->DecodeStr();
	if (sMsg.length() > 65)
	{
		pUser->SendNoticeMessage(GET_STRING(GameSrv_User_Invalid_Chat_Msg));
		return 0;
	}

	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::BroadcastPacket);
	oPacket.Encode1(0); //nGameSrvCount = 0 --> broadcast to all game servers.
	oPacket.Encode2(UserSendPacketType::UserLocal_OnAvatarMegaPacket);
	oPacket.Encode4(nItemID);
	oPacket.EncodeStr(pUser->GetName());
	oPacket.EncodeStr(sMsg);
	oPacket.Encode4(pUser->GetChannelID());
	oPacket.Encode1(iPacket->Decode1() != 0 ? 1 : 0);
	pUser->EncodeAvatar(&oPacket);
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	return 1;
}

int UserCashItemImpl::ConsumePetSkill(User * pUser, int nItemID, InPacket * iPacket)
{
	long long int liPetSN = iPacket->Decode8();
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());

	int nPOS = pUser->GetCharacterData()->FindCashItemSlotPosition(GW_ItemSlotBase::CASH, liPetSN);
	GW_ItemSlotPet* pPet = pUser->GetCharacterData()->GetItem(GW_ItemSlotBase::CASH, nPOS);
	if (!pPet)
		return 0;

	auto pPetSkillChangeItem = ItemInfo::GetInstance()->GetPetSkillChangeItem(nItemID);
	if (!pPetSkillChangeItem)
		return 0;

	if (pPet->usPetSkill & pPetSkillChangeItem->nFlag)
	{
		pUser->SendChatMessage(5, "您的寵物已經擁有此技能。");
		return 0;
	}

	pPet->usPetSkill |= pPetSkillChangeItem->nFlag;
	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketType::UserLocal_OnUserPetSkillChanged);
	oPacket.Encode8(liPetSN);
	oPacket.Encode1(pPetSkillChangeItem->bSet);
	oPacket.Encode2((short)pPetSkillChangeItem->nFlag);
	pUser->SendPacket(&oPacket);
	QWUInventory::UpdatePetItem(pUser, pPet->nPOS);
	return 1;
}

int UserCashItemImpl::ConsumeStatChange(User * pUser, InPacket * iPacket)
{
	int nInc = iPacket->Decode4();
	int nDec = iPacket->Decode4();

	auto liFlag = QWUser::CanStatChange(pUser, nInc, nDec);
	if (!liFlag)
		return 0;

	pUser->SendCharacterStat(false, liFlag);
	return 1;
}

int UserCashItemImpl::ConsumeSkillChange(User * pUser, int nItemID, InPacket *iPacket)
{
	int nIncSkillID = iPacket->Decode4();
	int nDecSkillID = iPacket->Decode4();

	if (!QWUSkillRecord::CanSkillChange(pUser, nIncSkillID, nDecSkillID, nItemID % 10))
	{
		std::lock_guard<std::recursive_mutex> userGuard(pUser->GetLock());
		std::vector<GW_SkillRecord*> aChange;

		QWUSkillRecord::SkillUp(pUser, nIncSkillID, 1, false, false, false, aChange);
		QWUSkillRecord::SkillUp(pUser, nDecSkillID, -1, false, false, false, aChange);
		QWUSkillRecord::SendCharacterSkillRecord(pUser, aChange);

		return 1;
	}
	return 0;
}
