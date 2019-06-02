#pragma once
#ifndef LOGIN_FLAG
#define LOGIN_FLAG

#include "PacketFlags.hpp"

#define MAKE_LOGIN_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static LoginSendPacketFlag flagName {value, #flagName}; } const static int flagName = value
#define MAKE_LOGIN_RECV_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static LoginRecvPacketFlag flagName {value, #flagName}; } const static int flagName = value

enum LoginAuthRequest
{
	rq_LoginAuth_RefreshLoginState = 0x00,
};

enum LoginAuthResult
{
	res_LoginAuth_RefreshLoginState = 0x00,
	res_LoginAuth_UnRegisterMigratinon = 0x01,
};

MAKE_FLAG_COLLECTION_BODY(LoginSendPacketFlag) 
//以下開始註冊封包Opcode Flag

//Flags for Center
MAKE_LOGIN_SEND_PACKET_FLAG(Center_RegisterCenterRequest, 1);
MAKE_LOGIN_SEND_PACKET_FLAG(Center_RequestCharacterList, 2);
MAKE_LOGIN_SEND_PACKET_FLAG(Center_RequestCreateNewCharacter, 3);
MAKE_LOGIN_SEND_PACKET_FLAG(Center_RequestGameServerInfo, 4);
MAKE_LOGIN_SEND_PACKET_FLAG(Center_RequestLoginAuth, 5);

//Flags for Game Client
MAKE_LOGIN_SEND_PACKET_FLAG(Client_CheckPasswordResponse, 0x00);
MAKE_LOGIN_SEND_PACKET_FLAG(Client_WorldInformationResponse, 0x02);
MAKE_LOGIN_SEND_PACKET_FLAG(Client_ClientSelectWorldResult, 0x03);
MAKE_LOGIN_SEND_PACKET_FLAG(Client_WorldStatusResult, 0x13);
MAKE_LOGIN_SEND_PACKET_FLAG(Client_ClientSelectCharacterResult, 0x04);
MAKE_LOGIN_SEND_PACKET_FLAG(Client_ClientCheckDuplicatedIDResult, 0x05);
MAKE_LOGIN_SEND_PACKET_FLAG(Client_ClientCreateCharacterResult, 0x06);
MAKE_LOGIN_SEND_PACKET_FLAG(Client_ClientSecondPasswordResult, 0x1B);
MAKE_LOGIN_SEND_PACKET_FLAG(Client_ClientChannelBackgroundResponse, 0x34);
MAKE_LOGIN_SEND_PACKET_FLAG(Client_ClientLoginBackgroundResponse, 0x2A);
MAKE_LOGIN_SEND_PACKET_FLAG(Client_ClientStartResponse, 0x2F);

//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END


MAKE_FLAG_COLLECTION_BODY(LoginRecvPacketFlag)
//以下開始註冊封包Opcode Flag
//Flags for Game Client
MAKE_LOGIN_RECV_PACKET_FLAG(Client_ClientCheckPasswordRequest, 0x01);
MAKE_LOGIN_RECV_PACKET_FLAG(Client_ClientSelectWorld, 0x04);
MAKE_LOGIN_RECV_PACKET_FLAG(Client_ClientWorldStatusCheck, 0x13);
MAKE_LOGIN_RECV_PACKET_FLAG(Client_ClientSecondPasswordCheck, 0x6D);
MAKE_LOGIN_RECV_PACKET_FLAG(Client_ClientMigrateIn, 0x06);
MAKE_LOGIN_RECV_PACKET_FLAG(Client_ClientSelectCharacter, 0x05);
MAKE_LOGIN_RECV_PACKET_FLAG(Client_ClientRequestServerList, 0x03);
MAKE_LOGIN_RECV_PACKET_FLAG(Client_ClientCheckDuplicatedID, 0x07);
MAKE_LOGIN_RECV_PACKET_FLAG(Client_ClientCreateNewCharactar, 0x08);
MAKE_LOGIN_RECV_PACKET_FLAG(Client_ClientDeleteCharactar, 0x09);
MAKE_LOGIN_RECV_PACKET_FLAG(Client_ClientLoginBackgroundRequest, 0xA0);
MAKE_LOGIN_RECV_PACKET_FLAG(Client_ClientRequestStart, 0xFFF7);
//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END
#endif