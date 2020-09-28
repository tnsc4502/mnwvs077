#pragma once
#ifndef REACTOR_FLAG
#define REACTOR_FLAG

#include "..\WvsLib\Net\PacketTypes.hpp"

DEFINE_PACKET_TYPE(ReactorSendPacketType)
	REGISTER_TYPE(Reactor_OnReactorChangeState, 196);
	REGISTER_TYPE(Reactor_OnReactorEnterField, 198);
	REGISTER_TYPE(Reactor_OnReactorLeaveField, 199);
END_PACKET_TYPE(ReactorSendPacketType)


DEFINE_PACKET_TYPE(ReactorRecvPacketType)
	REGISTER_TYPE(Reactor_OnHitReactor, 0x9E);
	REGISTER_TYPE(Reactor_OnClickReactor, 0x9D);
END_PACKET_TYPE(ReactorRecvPacketType)
#endif