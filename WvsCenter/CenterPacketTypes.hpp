#pragma once
#include "..\WvsLib\Net\PacketTypes.hpp"

enum CenterCashItemRequestType
{
	eLoadCashItemLockerRequest,
	eLoadWishItemRequest,
	eBuyCashItemRequest,
	eGiftCashItemRequest,
	eGiftPackageCashItemRequest,
	eBuyCoupleCashItemRequest,
	eBuyFriendshipCashItemRequest,
	eSetWishItemRequest,
	eIncSlotCountRequest,
	eIncTrunkCountRequest,
	eMoveCashItemLtoSRequest,
	eMoveCashItemStoLRequest,
	eAvatarMegaphoneRequest,
	eCheckMembershopCouponRequest,
	eStatChangeRequest,
	eSkillChangeRequest,
	eSetPetNameRequest,
	eSetPetLifeRequest,
	eMovePetStatRequest,
	eSetPetSkillRequest,
	eSendMemoRequest,
	eExpireCashItemRequest,
	eGetMaplePointRequest,
	eLoadPetExceptionList,
	eUpdatePetExceptionList,
	eExchangePetIDRequest,
};

DEFINE_PACKET_TYPE(CenterRequestPacketType)

	//Request from WvsLogin
	REGISTER_TYPE(RegisterCenterRequest, 0x4000);
	REGISTER_TYPE(RequestCharacterList, 0x4001);
	REGISTER_TYPE(RequestCreateNewCharacter, 0x4002);
	REGISTER_TYPE(RequestCheckDuplicatedID, 0x4003);
	REGISTER_TYPE(RequestGameServerInfo, 0x4004);
	REGISTER_TYPE(RequestLoginAuth, 0x4005);

	//Request from WvsGame/WvsShop
	REGISTER_TYPE(RequestMigrateIn, 0x5000);
	REGISTER_TYPE(RequestMigrateOut, 0x5001);
	REGISTER_TYPE(RequestTransferChannel, 0x5002);
	REGISTER_TYPE(RequestTransferShop, 0x5003);
	REGISTER_TYPE(PartyRequest, 0x5004);
	REGISTER_TYPE(GuildRequest, 0x5005);
	REGISTER_TYPE(FriendRequest, 0x5006);
	REGISTER_TYPE(GroupMessage, 0x5007);
	REGISTER_TYPE(WhisperMessage, 0x5008);
	REGISTER_TYPE(TrunkRequest, 0x5009);
	REGISTER_TYPE(EntrustedShopRequest, 0x500A);
	REGISTER_TYPE(GameClientDisconnected, 0x500B);
	//REGISTER_TYPE(CheckMigrationState, 0x500C);
	REGISTER_TYPE(GuildBBSRequest, 0x500D);
	REGISTER_TYPE(BroadcastPacket, 0x500E);
	REGISTER_TYPE(CheckGivePopularityRequest, 0x500F);
	REGISTER_TYPE(CashItemRequest, 0x5010);
	REGISTER_TYPE(FlushCharacterData, 0x5011);

	//Center specific
	REGISTER_TYPE(CheckMigrationState, 0x6000);
END_PACKET_TYPE(CenterRequestPacketType)

DEFINE_PACKET_TYPE(CenterResultPacketType)

	//Result types for WvsLogin
	REGISTER_TYPE(RegisterCenterAck, 0x4A00);
	REGISTER_TYPE(CenterStatChanged, 0x4A01);
	REGISTER_TYPE(CharacterListResponse, 0x4A02);
	REGISTER_TYPE(GameServerInfoResponse, 0x4A03);
	REGISTER_TYPE(CenterMigrateInResult, 0x4A04);
	REGISTER_TYPE(TransferChannelResult, 0x4A05);
	REGISTER_TYPE(MigrateCashShopResult, 0x4A06);
	REGISTER_TYPE(CreateCharacterResult, 0x4A07);
	REGISTER_TYPE(LoginAuthResult, 0x4A08);
	REGISTER_TYPE(CheckDuplicatedIDResult, 0x4A09);

	//Result types for WvsGame/WvsShop
	REGISTER_TYPE(PartyResult, 0x5A00);
	REGISTER_TYPE(GuildResult, 0x5A01);
	REGISTER_TYPE(FriendResult, 0x5A02);
	REGISTER_TYPE(RemoteBroadcasting, 0x5A03);
	REGISTER_TYPE(TrunkResult, 0x5A04);
	REGISTER_TYPE(EntrustedShopResult, 0x5A05);
	REGISTER_TYPE(GuildBBSResult, 0x5A06);
	REGISTER_TYPE(CheckGivePopularityResult, 0x5A07);
	REGISTER_TYPE(CashItemResult, 0x5A08);

	//From WvsCenter
	REGISTER_TYPE(CheckMigrationStateResult, 0x6A00);
END_PACKET_TYPE(CenterResultPacketType)
