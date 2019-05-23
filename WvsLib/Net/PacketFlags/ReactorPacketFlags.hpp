#pragma once
#ifndef REACTOR_FLAG
#define REACTOR_FLAG

#include "PacketFlags.hpp"

#define MAKE_REACTOR_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static ReactorSendPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_REACTOR_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static ReactorRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(ReactorSendPacketFlag)

MAKE_REACTOR_SEND_PACKET_FLAG(Reactor_OnReactorChangeState, 196);
MAKE_REACTOR_SEND_PACKET_FLAG(Reactor_OnReactorEnterField, 198);
MAKE_REACTOR_SEND_PACKET_FLAG(Reactor_OnReactorLeaveField, 199);

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END


MAKE_FLAG_COLLECTION_BODY(ReactorRecvPacketFlag)

MAKE_REACTOR_RECV_PACKET_FLAG(Reactor_OnHitReactor, 0x9E);
MAKE_REACTOR_RECV_PACKET_FLAG(Reactor_OnClickReactor, 0x9D);

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END
#endif