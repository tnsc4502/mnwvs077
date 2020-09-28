#pragma once
#ifndef EMPLOYEE_FLAG
#define EMPLOYEE_FLAG

#include "..\WvsLib\Net\PacketTypes.hpp"

DEFINE_PACKET_TYPE(EmployeeSendPacketType)
	REGISTER_TYPE(Employee_OnMakeEnterFieldPacket, 184);
	REGISTER_TYPE(Employee_OnMakeLeaveFieldPacket, 185);
	REGISTER_TYPE(Employee_OnMiniRoomBalloon, 186);
END_PACKET_TYPE(EmployeeSendPacketType)


DEFINE_PACKET_TYPE(EmployeeRecvPacketType)

	REGISTER_TYPE(Employee__, 0x38E);
END_PACKET_TYPE(EmployeeRecvPacketType)
#endif