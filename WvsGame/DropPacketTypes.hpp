#pragma once
#ifndef DROP_FLAG
#define DROP_FLAG

#include "..\WvsLib\Net\PacketTypes.hpp"

DEFINE_PACKET_TYPE(DropSendPacketType)
	REGISTER_TYPE(Drop_OnMakeEnterFieldPacket, 187);
	REGISTER_TYPE(Drop_OnMakeLeaveFieldPacket, 188);
END_PACKET_TYPE(DropSendPacketType)


DEFINE_PACKET_TYPE(DropRecvPacketType)
END_PACKET_TYPE(DropRecvPacketType)
#endif