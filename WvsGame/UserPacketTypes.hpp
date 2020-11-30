#pragma once
#ifndef USER_FLAG
#define USER_FLAG

namespace BroadcastMsgType 
{
	enum //BroadcastMsgType
	{
		e_BroadcastMsg_SpeakerChannel = 0x02,
		e_BroadcastMsg_SpeakerWorld = 0x03,
		e_BroadcastMsg_SpeakerHeart = 0x08,
		e_BroadcastMsg_SpeakerSkull = 0x09,
	};
}

#include "..\WvsLib\Net\PacketTypes.hpp"

DEFINE_PACKET_TYPE(UserSendPacketType)
//Start to register packet flags.

//Corresponding to WvsContext::OnPacket in the client.
	REGISTER_TYPE(UserLocal_OnTransferChannel, 8);
	REGISTER_TYPE(UserLocal_OnAliveCheckRequest, 9);
	REGISTER_TYPE(UserLocal_OnInventoryOperation, 22);
	REGISTER_TYPE(UserLocal_OnInventoryGrow, 23);
	REGISTER_TYPE(UserLocal_OnStatChanged, 24);
	REGISTER_TYPE(UserLocal_OnTemporaryStatSet, 25);
	REGISTER_TYPE(UserLocal_OnTemporaryStatReset, 26);
	REGISTER_TYPE(UserLocal_OnChangeSkillRecordResult, 27);
	REGISTER_TYPE(UserLocal_OnSkillUseResult, 28);
	REGISTER_TYPE(UserLocal_OnGivePopularityResult, 29);
	REGISTER_TYPE(UserLocal_OnMessage, 30);
	REGISTER_TYPE(UserLocal_OnMemoResult, 31);
	REGISTER_TYPE(UserLocal_OnMapTransferResult, 32);
	REGISTER_TYPE(UserLocal_OnClaimResult, 35);
	REGISTER_TYPE(UserLocal_OnSetClaimSvrAvailableTime, 36);
	REGISTER_TYPE(UserLocal_OnClaimSvrStatusChanged, 37);
	REGISTER_TYPE(UserLocal_OnSetTamingMobInfo, 38);
	REGISTER_TYPE(UserLocal_OnQuestClear, 39);
	REGISTER_TYPE(UserLocal_OnEntrustedShopCheckResult, 40);
	REGISTER_TYPE(UserLocal_OnSkillLearnItemResult, 41);
	REGISTER_TYPE(UserLocal_OnSkillResetItemResult, 42);
	REGISTER_TYPE(UserLocal_OnGatherItemResult, 43);
	REGISTER_TYPE(UserLocal_OnCharacterInfo, 45);
	REGISTER_TYPE(UserLocal_OnPartyResult, 46);
	REGISTER_TYPE(UserLocal_OnFriendResult, 47);
	REGISTER_TYPE(UserLocal_OnGuildResult, 49);
	REGISTER_TYPE(UserLocal_OnTownPortal, 50);
	REGISTER_TYPE(UserLocal_OnBroadcastMsg, 51);
	REGISTER_TYPE(UserLocal_OnIncubatorResult, 52);
	REGISTER_TYPE(UserLocal_OnShopScannerResult, 53);
	REGISTER_TYPE(UserLocal_OnMarriageRequest, 55);
	REGISTER_TYPE(UserLocal_OnMarriageResult, 56);
	REGISTER_TYPE(UserLocal_OnWeddingGiftResult, 57);
	REGISTER_TYPE(UserLocal_OnNotifyMarriedPartnerMapTrasnfer, 58);
	REGISTER_TYPE(UserLocal_OnCashPetFoodResult, 59);
	REGISTER_TYPE(UserLocal_OnSetWeekEventMessage, 60);
	REGISTER_TYPE(UserLocal_OnSetPotionDiscountRate, 61);
	REGISTER_TYPE(UserLocal_OnImitatedNPCResult, 63);
	REGISTER_TYPE(UserLocal_OnImitatedNPCData, 64);
	REGISTER_TYPE(UserLocal_OnGuildBBSResult, 68);
	REGISTER_TYPE(UserLocal_OnAvatarMegaPacket, 73);
	REGISTER_TYPE(UserLocal_OnMacroSysDataInit, 76);
	REGISTER_TYPE(UserLocal_OnSetFieldStage, 77);
//WvsContext::OnPacket 

//80~100 => Field::OnPacket

	REGISTER_TYPE(UserRemote_OnMakeEnterFieldPacket, 101);
	REGISTER_TYPE(UserRemote_OnMakeLeaveFieldPacket, 102);

//Corresponding to UserCommon::OnPacket
	REGISTER_TYPE(UserCommon_OnChat, 103);
	REGISTER_TYPE(UserCommon_OnADBoard, 104);
	REGISTER_TYPE(UserCommon_OnMiniRoomBalloon, 105);
	REGISTER_TYPE(UserCommon_SetConsumeItemEffect, 106);
	REGISTER_TYPE(UserCommon_ShowItemUpgradeEffect, 107);
//108 Unknown

//UserCommon::OnPetPacket
	REGISTER_TYPE(UserCommon_Pet_OnMakeEnterFieldPacket, 109);
	REGISTER_TYPE(UserCommon_Pet_OnMakeEnterFieldPacket2, 110);
	REGISTER_TYPE(UserCommon_Pet_OnMove, 111);
	REGISTER_TYPE(UserCommon_Pet_OnAction, 112);
	REGISTER_TYPE(UserCommon_Pet_OnNameChanged, 113);
	REGISTER_TYPE(UserCommon_Pet_OnLoadExceptionList, 114);
	REGISTER_TYPE(UserCommon_Pet_OnActionCommand, 115);

//116~121 Summoned::OnPacket


//Corresponding to UserRemote::OnPacket
	REGISTER_TYPE(UserRemote_OnMove, 123);
	REGISTER_TYPE(UserRemote_OnMeleeAttack, 124);
	REGISTER_TYPE(UserRemote_OnShootAttack, 125);
	REGISTER_TYPE(UserRemote_OnMagicAttack, 126);
	REGISTER_TYPE(UserRemote_OnBodyAttack, 127);
	REGISTER_TYPE(UserRemote_OnSkillPrepare, 128);
	REGISTER_TYPE(UserRemote_OnSkillCancel, 129);
	REGISTER_TYPE(UserRemote_OnHit, 130);
	REGISTER_TYPE(UserRemote_OnEmotion, 131);
	REGISTER_TYPE(UserRemote_OnSetActiveEffectItem, 132);
	REGISTER_TYPE(UserRemote_OnSetActivePortableChair, 133);
	REGISTER_TYPE(UserRemote_OnAvatarModified, 134);
	REGISTER_TYPE(UserRemote_OnEffect, 135);
	REGISTER_TYPE(UserRemote_OnSetTemporaryStat, 136);
	REGISTER_TYPE(UserRemote_OnResetTemporaryStat, 137);
	REGISTER_TYPE(UserRemote_OnReceiveHP, 138);
	REGISTER_TYPE(UserRemote_OnGuildNameChanged, 139);
	REGISTER_TYPE(UserRemote_OnGuildMarkChanged, 140);
	REGISTER_TYPE(UserRemote_OnThrowGrenade, 141);
	REGISTER_TYPE(UserRemote_OnSitResult, 141);

//Corresponding to UserLocal::OnPacket
	REGISTER_TYPE(UserLocal_OnSitResult, 142); 
	REGISTER_TYPE(UserLocal_OnEffect, 143);
	REGISTER_TYPE(UserLocal_OnTeleport, 144);
	REGISTER_TYPE(UserLocal_OnMesoGive_Succeeded, 146);
	REGISTER_TYPE(UserLocal_OnMesoGive_Failed, 147);
	REGISTER_TYPE(UserLocal_OnQuestResult, 148);
	REGISTER_TYPE(UserLocal_OnNotifyHPDecByField, 149);
	REGISTER_TYPE(UserLocal_OnUserPetSkillChanged, 150);
	REGISTER_TYPE(UserLocal_OnBalloonMsg, 151);
	REGISTER_TYPE(UserLocal_OnPlayEventSound, 154);
	REGISTER_TYPE(UserLocal_OnSkillCooltimeSet, 155);


	REGISTER_TYPE(User_OnFuncKeyMapped, 249);
//Registration ended.
END_PACKET_TYPE(UserSendPacketType)

//Start Recv
DEFINE_PACKET_TYPE(UserRecvPacketType)
	REGISTER_TYPE(User_OnAliveCheckAck, 0x0A);
	REGISTER_TYPE(User_OnUserTransferFieldRequest, 0x19);
	REGISTER_TYPE(User_OnUserTransferChannelRequest, 0x1A);
	REGISTER_TYPE(User_OnUserMigrateToCashShopRequest, 0x1B);
	REGISTER_TYPE(User_OnUserMoveRequest, 0x1C);
	REGISTER_TYPE(User_OnSitRequest, 0x1D);
	REGISTER_TYPE(User_OnSitOnPortableChairRequest, 0x1E);
	REGISTER_TYPE(User_OnUserAttack_MeleeAttack, 0x1F);
	REGISTER_TYPE(User_OnUserAttack_ShootAttack, 0x20);
	REGISTER_TYPE(User_OnUserAttack_MagicAttack, 0x21);
	REGISTER_TYPE(User_OnUserAttack_BodyAttack, 0x22);
	REGISTER_TYPE(User_OnHit, 0x23);
	REGISTER_TYPE(User_OnUserChat, 0x24);
	REGISTER_TYPE(User_OnCloseADBoard, 0x25);
	REGISTER_TYPE(User_OnEmotion, 0x26);
	REGISTER_TYPE(User_OnSetActiveEffectItem, 0x27);
	REGISTER_TYPE(User_OnSelectNpc, 0x2C);
	REGISTER_TYPE(User_OnScriptMessageAnswer, 0x2D);
	REGISTER_TYPE(User_OnShopRequest, 0x2E);
	REGISTER_TYPE(User_OnTrunkRequest, 0x2F);
	REGISTER_TYPE(User_OnEntrustedShopRequest, 0x30);
	REGISTER_TYPE(User_OnStoreBankRequest, 0x31);
	REGISTER_TYPE(User_OnSendCloseMessage, 0x32);
	REGISTER_TYPE(User_OnCreateUIShopScanner, 0x33);
	REGISTER_TYPE(User_OnClickUIShopScanResult, 0x34);
	REGISTER_TYPE(User_OnAdminShopAction, 0x35);
	REGISTER_TYPE(User_OnGatherItemRequest, 0x36);
	REGISTER_TYPE(User_OnUserChangeSlotRequest, 0x37);
	REGISTER_TYPE(User_OnStatChangeItemUseRequest, 0x38);
	REGISTER_TYPE(User_OnStatChangeItemCancelRequest, 0x39);
	REGISTER_TYPE(User_OnTryRecovery, 0x3A);
	REGISTER_TYPE(User_OnMobSummonItemUseRequest, 0x3B);
	REGISTER_TYPE(User_OnPetFoodItemUseRequest, 0x3C);
	REGISTER_TYPE(User_OnTamingMobFoodItemUseRequest, 0x3D);
	REGISTER_TYPE(User_OnConsumeCashItemUseRequest, 0x3E);
	REGISTER_TYPE(User_OnBridleItemUseRequest, 0x41);
	REGISTER_TYPE(User_OnSkillLearnItemUseRequest, 0x40);
	REGISTER_TYPE(User_OnShopScannerItemUseRequest, 0x41);
	REGISTER_TYPE(User_OnPortalScrollUseRequest, 0x43);
	REGISTER_TYPE(User_OnItemUpgradeRequest, 0x44);
	REGISTER_TYPE(User_OnUserAbilityUpRequest, 0x45);
	REGISTER_TYPE(User_OnChangeStatRequest, 0x46);
	REGISTER_TYPE(User_OnUserSkillUpRequest, 0x47);
	REGISTER_TYPE(User_OnUserSkillUseRequest, 0x48);
	REGISTER_TYPE(User_OnUserSkillCancelRequest, 0x49);
	REGISTER_TYPE(User_OnUserSkillPrepareRequest, 0x4A);
	REGISTER_TYPE(User_OnDropMoneyRequest, 0x4B);
	REGISTER_TYPE(User_OnGivePopularityRequest, 0x4C);
	REGISTER_TYPE(User_OnCharacterInfoRequest, 0x4E);
	REGISTER_TYPE(User_OnActivatePetRequest, 0x4F);
	REGISTER_TYPE(User_OnTemporaryStatUpdateRequest, 0x50);
	REGISTER_TYPE(User_OnPortalScriptRequest, 0x51);
	REGISTER_TYPE(User_OnMapTransferItemRequest, 0x53);
	REGISTER_TYPE(User_OnQuestRequest, 0x57);
	REGISTER_TYPE(User_OnSendGroupMessage, 0x5C);
	REGISTER_TYPE(User_OnSendWhisperMessage, 0x5D);
	REGISTER_TYPE(User_OnMiniRoomRequest, 0x5F);
	REGISTER_TYPE(User_OnPartyRequest, 0x60);
	REGISTER_TYPE(User_OnPartyRequestRejected, 0x61);
	REGISTER_TYPE(User_OnGuildRequest, 0x62);
	REGISTER_TYPE(User_OnFriendRequest, 0x66);
	REGISTER_TYPE(User_OnMemoRequest, 0x67);
	REGISTER_TYPE(User_OnEnterTownPortalRequest, 0x69);
	REGISTER_TYPE(User_OnFuncKeyMappedModified, 0x6B);
	REGISTER_TYPE(User_OnGuildBBSRequest, 0x71);
	REGISTER_TYPE(User_OnPetMove, 0x7B);
	REGISTER_TYPE(User_OnPetAction, 0x7C);
	REGISTER_TYPE(User_OnPetActionSpeak, 0x7D);
	REGISTER_TYPE(User_OnPetDropPickupRequest, 0x7E);
	REGISTER_TYPE(User_OnStatChangeItemUseRequestByPet, 0x7D);
	REGISTER_TYPE(User_OnUserPickupRequest, 0x9B);

/*	REGISTER_TYPE(User_, ); //UserLocal::
	REGISTER_TYPE(User_, ); //UserLocal::
	REGISTER_TYPE(User_, ); //UserLocal::
	REGISTER_TYPE(User_, ); //UserLocal::
	REGISTER_TYPE(User_, ); //UserLocal::
	REGISTER_TYPE(User_, ); //UserLocal::
	REGISTER_TYPE(User_, ); //UserLocal::
	REGISTER_TYPE(User_, ); //UserLocal::
	REGISTER_TYPE(User_, ); //UserLocal::
	REGISTER_TYPE(User_, ); //UserLocal::
	REGISTER_TYPE(User_, ); //UserLocal::
	REGISTER_TYPE(User_, ); //UserLocal::
	REGISTER_TYPE(User_, ); //UserLocal::
	REGISTER_TYPE(User_, ); //UserLocal::
*/
END_PACKET_TYPE(UserRecvPacketType)
#endif