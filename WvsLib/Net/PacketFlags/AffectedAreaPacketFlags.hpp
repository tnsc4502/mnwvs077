#pragma once
#ifndef AFFECTED_AREA_FLAG
#define AFFECTED_AREA_FLAG

#include "PacketFlags.hpp"

#define MAKE_AFFECTED_AREA_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static AffectedAreaSendPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_AFFECTED_AREA_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static AffectedAreaRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(AffectedAreaSendPacketFlag)
MAKE_AFFECTED_AREA_SEND_PACKET_FLAG(AffectedArea_OnMakeEnterFieldPacket, 192);
MAKE_AFFECTED_AREA_SEND_PACKET_FLAG(AffectedArea_OnMakeLeaveFieldPacket, 193);

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END


MAKE_FLAG_COLLECTION_BODY(AffectedAreaRecvPacketFlag)

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END
#endif