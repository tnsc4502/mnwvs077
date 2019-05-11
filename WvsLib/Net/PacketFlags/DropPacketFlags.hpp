#pragma once
#ifndef DROP_FLAG
#define DROP_FLAG

#include "PacketFlags.hpp"

#define MAKE_DROP_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static DropSendPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_DROP_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static DropRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(DropSendPacketFlag)
MAKE_DROP_SEND_PACKET_FLAG(Drop_OnMakeEnterFieldPacket, 187);
MAKE_DROP_SEND_PACKET_FLAG(Drop_OnMakeLeaveFieldPacket, 188);

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END


MAKE_FLAG_COLLECTION_BODY(DropRecvPacketFlag)

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END
#endif