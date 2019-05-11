#pragma once
#ifndef NPC_FLAG
#define NPC_FLAG

#include "PacketFlags.hpp"

#define MAKE_NPC_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static NPCPacketFlags flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(NPCPacketFlags)
//以下開始註冊封包Opcode Flag

//Flags to Game
MAKE_NPC_PACKET_FLAG(NPC_OnNpcShopItemList, 220);
MAKE_NPC_PACKET_FLAG(NPC_OnNpcShopRequest, 221);

MAKE_NPC_PACKET_FLAG(NPC_OnMakeEnterFieldPacket, 175);
MAKE_NPC_PACKET_FLAG(NPC_OnMakeLeaveFieldPacket, 176);
MAKE_NPC_PACKET_FLAG(NPC_OnNpcChangeController, 177);
MAKE_NPC_PACKET_FLAG(NPC_OnMove, 178);
MAKE_NPC_PACKET_FLAG(NPC_ScriptMessage, 219);  //ScriptMan::OnScriptMessage

												 							
FLAG_COLLECTION_BODY_END
//結束Opcode Flag註冊

#endif