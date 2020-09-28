#pragma once
#ifndef TOWN_PORTAL_FLAG
#define TOWN_PORTAL_FLAG

#include "..\WvsLib\Net\PacketTypes.hpp"

DEFINE_PACKET_TYPE(TownPortalSendPacketType)
	REGISTER_TYPE(TownPortal_OnMakeEnterFieldPacket, 194);
	REGISTER_TYPE(TownPortal_OnMakeLeaveFieldPacket, 195);
END_PACKET_TYPE(TownPortalSendPacketType)


DEFINE_PACKET_TYPE(TownPortalRecvPacketType)
END_PACKET_TYPE(TownPortalRecvPacketType)
#endif