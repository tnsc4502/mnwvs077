#pragma once
#include "PacketFlags.hpp"

#define MAKE_CENTER_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static CenterSendPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(CenterSendPacketFlag)
//以下開始註冊封包Opcode Flag

//Center To Login
MAKE_CENTER_SEND_PACKET_FLAG(RegisterCenterAck, 0x01);
MAKE_CENTER_SEND_PACKET_FLAG(CenterStatChanged, 0x02);
MAKE_CENTER_SEND_PACKET_FLAG(CharacterListResponse, 0x03);
MAKE_CENTER_SEND_PACKET_FLAG(GameServerInfoResponse, 0x04);
MAKE_CENTER_SEND_PACKET_FLAG(CenterMigrateInResult, 0x05);
MAKE_CENTER_SEND_PACKET_FLAG(TransferChannelResult, 0x06);
MAKE_CENTER_SEND_PACKET_FLAG(MigrateCashShopResult, 0x07);
MAKE_CENTER_SEND_PACKET_FLAG(CreateCharacterResult, 0x08);
MAKE_CENTER_SEND_PACKET_FLAG(LoginAuthResult, 0x09);
MAKE_CENTER_SEND_PACKET_FLAG(CheckDuplicatedIDResult, 0x0A);

//Center To Game
MAKE_CENTER_SEND_PACKET_FLAG(PartyResult, 0x100);
MAKE_CENTER_SEND_PACKET_FLAG(GuildResult, 0x101);
MAKE_CENTER_SEND_PACKET_FLAG(FriendResult, 0x102);
MAKE_CENTER_SEND_PACKET_FLAG(RemoteBroadcasting, 0x103);
MAKE_CENTER_SEND_PACKET_FLAG(TrunkResult, 0x104);
MAKE_CENTER_SEND_PACKET_FLAG(EntrustedShopResult, 0x105);
MAKE_CENTER_SEND_PACKET_FLAG(GuildBBSResult, 0x106);
MAKE_CENTER_SEND_PACKET_FLAG(CheckGivePopularityResult, 0x107);

//Center To Shop
MAKE_CENTER_SEND_PACKET_FLAG(CashItemResult, 0x200);


//Common
MAKE_CENTER_SEND_PACKET_FLAG(CheckMigrationState, 0x500);

//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END