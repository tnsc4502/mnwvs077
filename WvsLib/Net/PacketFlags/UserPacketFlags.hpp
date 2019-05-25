#pragma once
#ifndef USER_FLAG
#define USER_FLAG

#include "PacketFlags.hpp"

#define MAKE_USER_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static UserSendPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_USER_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static UserRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(UserSendPacketFlag)
//以下開始註冊封包Opcode Flag

//對應Client的WvsContext::OnPacket

MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnTransferChannel, 8);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnInventoryOperation, 22);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnInventoryGrow, 23);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnStatChanged, 24);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnTemporaryStatSet, 25);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnTemporaryStatReset, 26);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnChangeSkillRecordResult, 27);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnSkillUseResult, 28);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnGivePopularityResult, 29);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnMessage, 30);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnMemoResult, 31);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnMapTransferResult, 32);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnClaimResult, 35);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnSetClaimSvrAvailableTime, 36);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnClaimSvrStatusChanged, 37);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnSetTamingMobInfo, 38);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnQuestClear, 39);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnEntrustedShopCheckResult, 40);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnSkillLearnItemResult, 41);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnSkillResetItemResult, 42);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnGatherItemResult, 43);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnCharacterInfo, 45);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnPartyResult, 46);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnFriendResult, 47);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnGuildResult, 49);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnTownPortal, 50);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnBroadcastMsg, 51);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnIncubatorResult, 52);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnShopScannerResult, 53);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnMarriageRequest, 55);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnMarriageResult, 56);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnWeddingGiftResult, 57);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnNotifyMarriedPartnerMapTrasnfer, 58);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnCashPetFoodResult, 59);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnSetWeekEventMessage, 60);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnSetPotionDiscountRate, 61);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnImitatedNPCResult, 63);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnImitatedNPCData, 64);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnAvatarMegaPacket, 73);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnMacroSysDataInit, 76);
//WvsContext::OnPacket 已知部分結束

//80~100 => Field::OnPacket

MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnMakeEnterFieldPacket, 101);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnMakeLeaveFieldPacket, 102);

//對應Client的UserCommon::OnPacket
MAKE_USER_SEND_PACKET_FLAG(UserCommon_OnChat, 103);
MAKE_USER_SEND_PACKET_FLAG(UserCommon_OnADBoard, 104);
MAKE_USER_SEND_PACKET_FLAG(UserCommon_OnMiniRoomBalloon, 105);
MAKE_USER_SEND_PACKET_FLAG(UserCommon_SetConsumeItemEffect, 106);
MAKE_USER_SEND_PACKET_FLAG(UserCommon_ShowItemUpgradeEffect, 107);
//108 Unknown

//UserCommon::OnPetPacket
MAKE_USER_SEND_PACKET_FLAG(UserCommon_Pet_OnMakeEnterFieldPacket, 109);
MAKE_USER_SEND_PACKET_FLAG(UserCommon_Pet_OnMakeEnterFieldPacket2, 110);
MAKE_USER_SEND_PACKET_FLAG(UserCommon_Pet_OnMove, 111);
MAKE_USER_SEND_PACKET_FLAG(UserCommon_Pet_OnAction, 112);
MAKE_USER_SEND_PACKET_FLAG(UserCommon_Pet_OnNameChanged, 113);
MAKE_USER_SEND_PACKET_FLAG(UserCommon_Pet_OnLoadExceptionList, 114);
MAKE_USER_SEND_PACKET_FLAG(UserCommon_Pet_OnActionCommand, 115);

//116~121 Summoned::OnPacket


//對應Client的UserRemote::OnPacket
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnMove, 123);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnMeleeAttack, 124);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnShootAttack, 125);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnMagicAttack, 126);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnBodyAttack, 127);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnSkillPrepare, 128);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnSkillCancel, 129);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnHit, 130);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnEmotion, 131);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnSetActiveEffectItem, 132);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnSetActivePortableChair, 133);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnAvatarModified, 134);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnEffect, 135);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnSetTemporaryStat, 136);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnResetTemporaryStat, 137);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnReceiveHP, 138);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnGuildNameChanged, 139);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnGuildMarkChanged, 140);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnThrowGrenade, 141);
MAKE_USER_SEND_PACKET_FLAG(UserRemote_OnSitResult, 141);

//對應Client的UserLocal::OnPacket
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnSitResult, 142); 
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnEffect, 143);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnTeleport, 144);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnMesoGive_Succeeded, 146);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnMesoGive_Failed, 147);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnQuestResult, 148);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnNotifyHPDecByField, 149);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnUserPetSkillChanged, 150);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnBalloonMsg, 151);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnPlayEventSound, 154);
MAKE_USER_SEND_PACKET_FLAG(UserLocal_OnSkillCooltimeSet, 155);


MAKE_USER_SEND_PACKET_FLAG(User_OnFuncKeyMapped, 249);
//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END

//Start Recv
MAKE_FLAG_COLLECTION_BODY(UserRecvPacketFlag)
MAKE_USER_RECV_PACKET_FLAG(User_OnUserTransferFieldRequest, 0x19);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserTransferChannelRequest, 0x1A);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserMigrateToCashShopRequest, 0x1B);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserMoveRequest, 0x1C);
MAKE_USER_RECV_PACKET_FLAG(User_OnGetUpFromChairRequest, 0x1D);
MAKE_USER_RECV_PACKET_FLAG(User_OnSitOnPortableChairRequest, 0x1E);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserAttack_MeleeAttack, 0x1F);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserAttack_ShootAttack, 0x20);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserAttack_MagicAttack, 0x21);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserAttack_BodyAttack, 0x22);
MAKE_USER_RECV_PACKET_FLAG(User_OnSetDamaged, 0x23);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserChat, 0x24);
MAKE_USER_RECV_PACKET_FLAG(User_OnCloseChatBallon, 0x25);
MAKE_USER_RECV_PACKET_FLAG(User_OnEmotion, 0x26);
MAKE_USER_RECV_PACKET_FLAG(User_OnItemEffect, 0x27);
MAKE_USER_RECV_PACKET_FLAG(User_OnSelectNpc, 0x2C);
MAKE_USER_RECV_PACKET_FLAG(User_OnScriptMessageAnswer, 0x2D);
MAKE_USER_RECV_PACKET_FLAG(User_OnShopRequest, 0x2E);
MAKE_USER_RECV_PACKET_FLAG(User_OnTrunkRequest, 0x2F);
MAKE_USER_RECV_PACKET_FLAG(User_OnEntrustedShopRequest, 0x30);
MAKE_USER_RECV_PACKET_FLAG(User_OnStoreBankRequest, 0x31);
MAKE_USER_RECV_PACKET_FLAG(User_OnSendCloseMessage, 0x32);
MAKE_USER_RECV_PACKET_FLAG(User_OnCreateUIShopScanner, 0x33);
MAKE_USER_RECV_PACKET_FLAG(User_OnClickUIShopScanResult, 0x34);
MAKE_USER_RECV_PACKET_FLAG(User_OnAdminShopAction, 0x35);
MAKE_USER_RECV_PACKET_FLAG(User_OnGatherItemRequest, 0x36);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserChangeSlotRequest, 0x37);
MAKE_USER_RECV_PACKET_FLAG(User_OnStatChangeItemUseRequest, 0x38);
MAKE_USER_RECV_PACKET_FLAG(User_OnStatChangeItemCancelRequest, 0x39);
MAKE_USER_RECV_PACKET_FLAG(User_OnTryRecovery, 0x3A);
MAKE_USER_RECV_PACKET_FLAG(User_OnMobSummonItemUseRequest, 0x3B);
MAKE_USER_RECV_PACKET_FLAG(User_OnPetFoodItemUseRequest, 0x3C);
MAKE_USER_RECV_PACKET_FLAG(User_OnTamingMobFoodItemUseRequest, 0x3D);
MAKE_USER_RECV_PACKET_FLAG(User_OnConsumeCashItemUseRequest, 0x3E);
MAKE_USER_RECV_PACKET_FLAG(User_OnBridleItemUseRequest, 0x41);
MAKE_USER_RECV_PACKET_FLAG(User_OnSkillLearnItemUseRequest, 0x42);
MAKE_USER_RECV_PACKET_FLAG(User_OnShopScannerItemUseRequest, 0x43);
MAKE_USER_RECV_PACKET_FLAG(User_OnItemUpgradeRequest, 0x44);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserAbilityUpRequest, 0x45);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserSkillUpRequest, 0x47);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserSkillUseRequest, 0x48);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserSkillCancelRequest, 0x49);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserSkillPrepareRequest, 0x4A);
MAKE_USER_RECV_PACKET_FLAG(User_OnDropMoneyRequest, 0x4B);
MAKE_USER_RECV_PACKET_FLAG(User_OnGivePopularityRequest, 0x4C);
MAKE_USER_RECV_PACKET_FLAG(User_OnCharacterInfoRequest, 0x4E);
MAKE_USER_RECV_PACKET_FLAG(User_OnActivatePetRequest, 0x4F);
MAKE_USER_RECV_PACKET_FLAG(User_OnMapTransferItemUseRequest, 0x50);
MAKE_USER_RECV_PACKET_FLAG(User_OnPortalScrollUseRequest, 0x51); 

MAKE_USER_RECV_PACKET_FLAG(User_OnQuestRequest, 0x57);
MAKE_USER_RECV_PACKET_FLAG(User_OnSendGroupMessage, 0x5C);
MAKE_USER_RECV_PACKET_FLAG(User_OnSendWhisperMessage, 0x5D);
MAKE_USER_RECV_PACKET_FLAG(User_OnMiniRoomRequest, 0x5F);
MAKE_USER_RECV_PACKET_FLAG(User_OnPartyRequest, 0x60);
MAKE_USER_RECV_PACKET_FLAG(User_OnPartyRequestRejected, 0x61);
MAKE_USER_RECV_PACKET_FLAG(User_OnGuildRequest, 0x62);
MAKE_USER_RECV_PACKET_FLAG(User_OnFriendRequest, 0x66);


MAKE_USER_RECV_PACKET_FLAG(User_OnFuncKeyMappedModified, 0x6B);
MAKE_USER_RECV_PACKET_FLAG(User_OnPetMove, 0x7B);
MAKE_USER_RECV_PACKET_FLAG(User_OnPetAction, 0x7C);
MAKE_USER_RECV_PACKET_FLAG(User_OnPetActionSpeak, 0x7D);
MAKE_USER_RECV_PACKET_FLAG(User_OnPetDropPickupRequest, 0x7E);
MAKE_USER_RECV_PACKET_FLAG(User_OnStatChangeItemUseRequestByPet, 0x7D);
MAKE_USER_RECV_PACKET_FLAG(User_OnUserPickupRequest, 0x9B);

/*MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
MAKE_USER_RECV_PACKET_FLAG(User_, ); //UserLocal::
*/
FLAG_COLLECTION_BODY_END		
#endif