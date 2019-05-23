#pragma once
#ifndef GAME_FLAG
#define GAME_FLAG

#include "PacketFlags.hpp"

#define MAKE_GAME_SRV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static GameSrvSendPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(GameSrvSendPacketFlag)
//以下開始註冊封包Opcode Flag

//Flags to Center
MAKE_GAME_SRV_PACKET_FLAG(FlushCharacterData, 0x6000);
MAKE_GAME_SRV_PACKET_FLAG(RequestMigrateIn, 0x6001);
MAKE_GAME_SRV_PACKET_FLAG(RequestMigrateOut, 0x6002);
MAKE_GAME_SRV_PACKET_FLAG(RequestTransferChannel, 0x6003);
MAKE_GAME_SRV_PACKET_FLAG(RequestTransferShop, 0x6004);
MAKE_GAME_SRV_PACKET_FLAG(PartyRequest, 0x6005);
MAKE_GAME_SRV_PACKET_FLAG(GuildRequest, 0x6006);
MAKE_GAME_SRV_PACKET_FLAG(FriendRequest, 0x6007);
MAKE_GAME_SRV_PACKET_FLAG(GroupMessage, 0x6008);
MAKE_GAME_SRV_PACKET_FLAG(WhisperMessage, 0x6009);
MAKE_GAME_SRV_PACKET_FLAG(TrunkRequest, 0x600A);


//Flags to Game
MAKE_GAME_SRV_PACKET_FLAG(Client_SetFieldStage, 77); //Stage::OnSetField

//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END

#endif