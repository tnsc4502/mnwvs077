#pragma once
#ifndef SUMMONED_FLAG
#define SUMMONED_FLAG

#include "..\WvsLib\Net\PacketTypes.hpp"

DEFINE_PACKET_TYPE(SummonedSendPacketType)
	REGISTER_TYPE(Summoned_OnCreated, 116);
	REGISTER_TYPE(Summoned_OnRemoved, 117);
	REGISTER_TYPE(Summoned_OnMove, 118);
	REGISTER_TYPE(Summoned_OnAttack, 119);
	REGISTER_TYPE(Summoned_OnSkill, 120);
	REGISTER_TYPE(Summoned_OnHit, 121);
END_PACKET_TYPE(SummonedSendPacketType)

DEFINE_PACKET_TYPE(SummonedRecvPacketType)
	REGISTER_TYPE(Summoned_OnMoveRequest, 131);
	REGISTER_TYPE(Summoned_OnAttackRequest, 132);
	REGISTER_TYPE(Summoned_OnHitRequest, 133);
	REGISTER_TYPE(Summoned_OnDoingHealRequest, 134);
	REGISTER_TYPE(Summoned_OnRemoveRequest, 135);
/*	REGISTER_TYPE(Summoned_OnAttackForPvPRequest, 0x268);
	REGISTER_TYPE(Summoned_OnActionRequest, 0x269);
	REGISTER_TYPE(Summoned_OnAssistAttackRequest, 0x26A);*/
END_PACKET_TYPE(SummonedRecvPacketType)
#endif