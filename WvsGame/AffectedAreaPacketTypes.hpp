#pragma once
#ifndef AFFECTED_AREA_FLAG
#define AFFECTED_AREA_FLAG

#include "..\WvsLib\Net\PacketTypes.hpp"

DEFINE_PACKET_TYPE(AffectedAreaSendPacketType)
		REGISTER_TYPE(AffectedArea_OnMakeEnterFieldPacket, 192);
		REGISTER_TYPE(AffectedArea_OnMakeLeaveFieldPacket, 193);
END_PACKET_TYPE(AffectedAreaSendPacketType)

DEFINE_PACKET_TYPE(AffectedAreaRecvPacketType)
END_PACKET_TYPE(AffectedAreaRecvPacketType)


#endif