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
#include "Field.h"
#include "Portal.h"
#include "PortalMap.h"
#include "FieldMan.h"

#include "..\Database\GW_ItemSlotPet.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_Memo.h"

#include "..\WvsLib\String\StringPool.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsCenter\ShopScannerMan.h"
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

int UserCashItemImpl::ConsumeSetItemName(User * pUser, InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());

	short nPOS = iPacket->Decode1();
	auto pItem = pUser->GetCharacterData()->GetItem(GW_ItemSlotBase::EQUIP, nPOS);
	if (pItem && !pItem->bIsCash && ((GW_ItemSlotEquip*)pItem)->sTitle == "")
	{
		((GW_ItemSlotEquip*)pItem)->sTitle = pUser->GetName();
		std::vector<InventoryManipulator::ChangeLog> aChange;
		InventoryManipulator::InsertChangeLog(aChange, InventoryManipulator::ChangeType::Change_RemoveFromSlot, GW_ItemSlotBase::EQUIP, nPOS, pItem, 0, 0);
		InventoryManipulator::InsertChangeLog(aChange, InventoryManipulator::ChangeType::Change_AddToSlot, GW_ItemSlotBase::EQUIP, nPOS, pItem, 0, 0);
		QWUInventory::SendInventoryOperation(pUser, true, aChange);

		return 1;
	}
	return 0;
}

int UserCashItemImpl::ConsumeADBoard(User * pUser, InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	if (!pUser->GetMiniRoom())
	{
		std::string sADBoard = iPacket->DecodeStr();
		if (sADBoard.size() <= 128)
			pUser->SetADBoard(sADBoard);
	}
	//Always return 0 (the item won't get removed until it expires).
	return 0;
}

int UserCashItemImpl::ConsumeItemProtector(User * pUser, InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	int nTI = iPacket->Decode4();
	int nPOS = iPacket->Decode4();
	
	auto pItem = pUser->GetCharacterData()->GetItem(nTI, nPOS);
	if (pItem && !pItem->IsProtectedItem())
	{
		pItem->nAttribute |= GW_ItemSlotBase::ItemAttribute::eItemAttr_Protected;
		std::vector<InventoryManipulator::ChangeLog> aChange;
		InventoryManipulator::InsertChangeLog(aChange, InventoryManipulator::ChangeType::Change_RemoveFromSlot, GW_ItemSlotBase::EQUIP, nPOS, pItem, 0, 0);
		InventoryManipulator::InsertChangeLog(aChange, InventoryManipulator::ChangeType::Change_AddToSlot, GW_ItemSlotBase::EQUIP, nPOS, pItem, 0, 0);
		QWUInventory::SendInventoryOperation(pUser, true, aChange);

		return 1;
	}
	return 0;
}

int UserCashItemImpl::ConsumeWeatherItem(User * pUser, int nItemID, InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetField()->OnWeather(nItemID, pUser->GetName(), iPacket->DecodeStr());
	return 1;
}

int UserCashItemImpl::ConsumeJukeBox(User * pUser, int nItemID, InPacket * iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(pUser->GetLock());
	pUser->GetField()->OnJukeBox(nItemID, iPacket->Decode4(), pUser);
	return 1;
}

int UserCashItemImpl::ConsumeSendMemo(User* pUser, int nPOS, InPacket* iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2(CenterRequestPacketType::MemoRequest);
	oPacket.Encode4(pUser->GetSocketID());
	oPacket.Encode4(pUser->GetUserID());
	oPacket.Encode1(GW_Memo::MemoRequestType::eMemoReq_Send);
	oPacket.EncodeStr(iPacket->DecodeStr());
	oPacket.EncodeStr(pUser->GetName());
	oPacket.EncodeStr(iPacket->DecodeStr());
	oPacket.Encode8(GameDateTime::GetCurrentDate());
	oPacket.Encode1(0);
	oPacket.Encode2(nPOS);

	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
	return 0;
}

int UserCashItemImpl::ConsumeShopScanner(User * pUser, int nPOS, InPacket * iPacket)
{
	int nItemID = iPacket->Decode4();
	OutPacket oCenterPacket;
	oCenterPacket.Encode2(CenterRequestPacketType::ShopScannerRequest);
	oCenterPacket.Encode4(pUser->GetSocketID());
	oCenterPacket.Encode4(pUser->GetUserID());
	oCenterPacket.Encode1(ShopScannerMan::ShopScannerRequestType::eScanner_OnSearch);
	oCenterPacket.Encode1(WvsBase::GetInstance<WvsGame>()->GetCenter()->GetWorldID());
	oCenterPacket.Encode4(nItemID);
	oCenterPacket.Encode2(nPOS);
	WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oCenterPacket);
	return 0;
}

int UserCashItemImpl::ConsumeMapTransfer(User * pUser, int nItemID, InPacket * iPacket)
{
	bool bCanTransferContinent = (nItemID % 10000 / 1000) ? true : false;
	bool bTransferToUser = iPacket->Decode1() ? true : false;

	int nListSize = bCanTransferContinent ? GA_Character::MaxMapTransferExCount : GA_Character::MaxMapTransferCount,
		*paList = bCanTransferContinent ? pUser->GetCharacterData()->anMapTransferEx : pUser->GetCharacterData()->anMapTransfer;

	if (bTransferToUser)
	{
		std::string sUserName = iPacket->DecodeStr();
		auto pTarget = User::FindUserByName(sUserName);
		if (pTarget && pTarget->GetField() && IsMapTransferAvailable(pUser, 999999999, pTarget, bCanTransferContinent))
		{
			auto pPortal = pTarget->GetField()->GetPortalMap()->FindCloseStartPoint(pTarget->GetPosX(), pTarget->GetPosY());
			pUser->TryTransferField(pTarget->GetField()->GetFieldID(), pPortal ? pPortal->GetPortalName() : "");
		}
		else
			return false;
	}
	else
	{
		int nFieldID = iPacket->Decode4();
		for (int i = 0; i < nListSize; ++i)
			if (paList[i] == nFieldID)
				break;
			else if (i == nListSize - 1)
			{
				return false;
			}

		if (IsMapTransferAvailable(pUser, nFieldID, nullptr, bCanTransferContinent))
		{
			pUser->TryTransferField(nFieldID, "");
			return true;
		}
	}
	return false;
}

bool UserCashItemImpl::IsMapTransferAvailable(User *pUser, int nFieldID, User * pTarget, bool bCanTransferContinent)
{
	auto pField = pUser->GetField(), pTargetField = pTarget ? pTarget->GetField() : FieldMan::GetInstance()->GetField(nFieldID);

	if (!pField || !pTargetField || pField->GetFieldLimit() & 0x40 || pField->GetFieldID() / 1000000 % 100 == 9)
	{
		pUser->SendMapTransferItemResult(MapTransferResult::eMapTransfer_InValidFieldID, bCanTransferContinent);
		return false;
	}

	if (pTarget && QWUser::GetHP(pTarget) == 0)
	{
		pUser->SendMapTransferItemResult(MapTransferResult::eMapTransfer_InValidUserStat, bCanTransferContinent);
		return false;
	}

	if (QWUser::GetLevel(pUser) < 7 && pTargetField->GetFieldID() > 9999999)
	{
		pUser->SendMapTransferItemResult(MapTransferResult::eMapTransfer_LevelNotSatisfied, bCanTransferContinent);
		return false;
	}

	if (pField->GetFieldID() == pTargetField->GetFieldID())
	{
		pUser->SendMapTransferItemResult(MapTransferResult::eMapTransfer_TransferOnIdenticalField, bCanTransferContinent);
		return false;
	}

	if (pField->GetFieldID() / 10000000 == 19 ||
		pTargetField->GetFieldLimit() & 0x40 ||
		pTargetField->GetFieldID() == 180000000 ||
		!(pTargetField->GetFieldID() / 100000000) && pField->GetFieldID() / 100000000 ||
		(pField->GetFieldID() / 1000000) % 100 == 9 ||
		(pTargetField->GetFieldID() / 1000000) % 100 == 9 ||
		(pField->GetFieldID() / 10000 == 20009) ||
		(pTargetField->GetFieldID() / 10000 == 20009))
	{
		pUser->SendMapTransferItemResult(MapTransferResult::eMapTransfer_InValidFieldID, bCanTransferContinent);
		return false;
	}

	if (!bCanTransferContinent && !FieldMan::GetInstance()->IsConnected(pField->GetFieldID(), pTargetField->GetFieldID()))
	{
		pUser->SendMapTransferItemResult(MapTransferResult::eMapTransfer_InValidFieldStat, bCanTransferContinent);
		return false;
	}

	return true;
}
