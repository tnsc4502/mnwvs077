#pragma once
#ifndef LOGIN_FLAG
#define LOGIN_FLAG

#include "..\WvsLib\Net\PacketTypes.hpp"

enum LoginAuthRequest
{
	rq_LoginAuth_RefreshLoginState = 0x00,
};

enum LoginAuthResult
{
	res_LoginAuth_RefreshLoginState = 0x00,
	res_LoginAuth_UnRegisterMigratinon = 0x01,
};

DEFINE_PACKET_TYPE(LoginSendPacketType) 
//以下開始註冊封包Opcode Flag

//Flags for Center

//Flags for Game Client
	REGISTER_TYPE(Client_CheckPasswordResponse, 0x00);
	REGISTER_TYPE(Client_WorldInformationResponse, 0x02);
	REGISTER_TYPE(Client_ClientSelectWorldResult, 0x03);
	REGISTER_TYPE(Client_WorldStatusResult, 0x13);
	REGISTER_TYPE(Client_ClientSelectCharacterResult, 0x04);
	REGISTER_TYPE(Client_ClientCheckDuplicatedIDResult, 0x05);
	REGISTER_TYPE(Client_ClientCreateCharacterResult, 0x06);
	REGISTER_TYPE(Client_RequestSelectGenderAnd2ndPassword, 0x11);
	REGISTER_TYPE(Client_SelectGenderAnd2ndPasswordResult, 0x12);
	REGISTER_TYPE(Client_ClientSecondPasswordResult, 0x1B);
	REGISTER_TYPE(Client_ClientChannelBackgroundResponse, 0x34);
	REGISTER_TYPE(Client_ClientLoginBackgroundResponse, 0x2A);
	REGISTER_TYPE(Client_ClientStartResponse, 0x2F);

//結束Opcode Flag註冊
END_PACKET_TYPE(LoginSendPacketType)


DEFINE_PACKET_TYPE(LoginRecvPacketType)
//以下開始註冊封包Opcode Flag
//Flags for Game Client
	REGISTER_TYPE(Client_ClientCheckPasswordRequest, 0x01);
	REGISTER_TYPE(Client_ClientSelectWorld, 0x04);
	REGISTER_TYPE(Client_ClientWorldStatusCheck, 0x13);
	REGISTER_TYPE(Client_ClientSelectGenderAnd2ndPassword, 0x14);
	REGISTER_TYPE(Client_ClientSecondPasswordCheck, 0x6D);
	REGISTER_TYPE(Client_ClientMigrateIn, 0x06);
	REGISTER_TYPE(Client_ClientSelectCharacter, 0x05);
	REGISTER_TYPE(Client_ClientRequestServerList, 0x03);
	REGISTER_TYPE(Client_ClientCheckDuplicatedID, 0x07);
	REGISTER_TYPE(Client_ClientCreateNewCharactar, 0x08);
	REGISTER_TYPE(Client_ClientDeleteCharactar, 0x09);
	REGISTER_TYPE(Client_ClientLoginBackgroundRequest, 0xA0);
	REGISTER_TYPE(Client_ClientRequestStart, 0xFFF7);
//結束Opcode Flag註冊
END_PACKET_TYPE(LoginRecvPacketType)
#endif