#pragma once
#ifndef TOWN_PORTAL_FLAG
#define TOWN_PORTAL_FLAG

#include "PacketFlags.hpp"

#define MAKE_TOWN_PORTAL_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static TownPortalSendPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_TOWN_PORTAL_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static TownPortalRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(TownPortalSendPacketFlag)
MAKE_TOWN_PORTAL_SEND_PACKET_FLAG(TownPortal_OnMakeEnterFieldPacket, 194);
MAKE_TOWN_PORTAL_SEND_PACKET_FLAG(TownPortal_OnMakeLeaveFieldPacket, 195);

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END


MAKE_FLAG_COLLECTION_BODY(TownPortalRecvPacketFlag)

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END
#endif