#pragma once
#ifndef EMPLOYEE_FLAG
#define EMPLOYEE_FLAG

#include "PacketFlags.hpp"

#define MAKE_EMPLOYEE_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static EmployeeSendPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_EMPLOYEE_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static EmployeeRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(EmployeeSendPacketFlag)
MAKE_EMPLOYEE_SEND_PACKET_FLAG(Employee_OnMakeEnterFieldPacket, 184);
MAKE_EMPLOYEE_SEND_PACKET_FLAG(Employee_OnMakeLeaveFieldPacket, 185);
MAKE_EMPLOYEE_SEND_PACKET_FLAG(Employee_OnMiniRoomBalloon, 186);

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END


MAKE_FLAG_COLLECTION_BODY(EmployeeRecvPacketFlag)

MAKE_EMPLOYEE_RECV_PACKET_FLAG(Employee__, 0x38E);

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END
#endif