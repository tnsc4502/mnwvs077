#pragma once
#ifndef EMPLOYEE_FLAG
#define EMPLOYEE_FLAG

#include "PacketFlags.hpp"

#define MAKE_MESSAGEBOX_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static MessageBoxSendPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_MESSAGEBOX_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static MessageBoxRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(MessageBoxSendPacketFlag)
MAKE_MESSAGEBOX_SEND_PACKET_FLAG(MessageBox_OnCreateFailed, 189);
MAKE_MESSAGEBOX_SEND_PACKET_FLAG(MessageBox_OnMakeEnterFieldPacket, 190);
MAKE_MESSAGEBOX_SEND_PACKET_FLAG(MessageBox_OnMakeLeaveFieldPacket, 191);

//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END


MAKE_FLAG_COLLECTION_BODY(MessageBoxRecvPacketFlag)


//µ²§ôOpcode Flagµù¥U
FLAG_COLLECTION_BODY_END
#endif