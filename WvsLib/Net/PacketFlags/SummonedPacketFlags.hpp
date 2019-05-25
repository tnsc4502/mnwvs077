#pragma once
#ifndef SUMMONED_FLAG
#define SUMMONED_FLAG

#include "PacketFlags.hpp"

#define MAKE_SUMMONED_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static SummonedSendPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_SUMMONED_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static SummonedRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(SummonedSendPacketFlag)
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnCreated, 116);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnRemoved, 117);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnMove, 118);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnAttack, 119);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnSkill, 120);
MAKE_SUMMONED_SEND_PACKET_FLAG(Summoned_OnHit, 121);

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END

MAKE_FLAG_COLLECTION_BODY(SummonedRecvPacketFlag)
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnMoveRequest, 131);
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnAttackRequest, 132);
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnHitRequest, 133);
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnDoingHealRequest, 134);
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnRemoveRequest, 135);
/*MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnAttackForPvPRequest, 0x268);
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnActionRequest, 0x269);
MAKE_SUMMONED_RECV_PACKET_FLAG(Summoned_OnAssistAttackRequest, 0x26A);*/

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END
#endif