#pragma once
#ifndef FIELD_FLAG
#define FIELD_FLAG

#include "PacketFlags.hpp"

namespace WhisperResult
{
	enum WhisperType
	{
		e_Whisper_Type_QueryLocation = 0x05,
		e_Whisper_Type_SendMessage = 0x06,
	};

	enum WhisperMessageResult
	{
		e_Whisper_Res_QuerySuccess = 0x09,
		e_Whisper_Res_Message_Ack = 0x0A,
		e_Whisper_Res_Message_Send = 0x12,
	};

	enum WhisperQuery
	{
		e_Whisper_QR_NotMigratedIn = 0x00,
		e_Whisper_QR_FieldID = 0x01,
		e_Whisper_QR_InShop = 0x02,
		e_Whisper_QR_ChannelID = 0x03,
	};
}

#define MAKE_FIELD_SEND_PACKET_FLAG(flagName, value) namespace FlagInstances{ const static FieldSendPacketFlag flagName {value, #flagName}; } const static int flagName = value

MAKE_FLAG_COLLECTION_BODY(FieldSendPacketFlag)
//以下開始註冊封包Opcode Flag
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnTransferFieldReqIgnored, 80);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnTransferChannelReqIgnored, 81);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnFieldSpecificData, 82);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnGroupMessage, 83);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnWhisper, 84);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSummonItemInavailable, 85);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnFieldEffect, 86);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnBlowWeather, 87);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnPlayJukeBox, 88);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnAdminResult, 89);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnQuiz, 90);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnDesc, 91);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnClock, 92);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnContiMove, 93);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnContiState, 94);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSetQuestClear, 95);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSetQuestTime, 96);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnSetObjectState, 97);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnDestroyClock, 98);
MAKE_FIELD_SEND_PACKET_FLAG(Field_OnFootHoldInfo, 100);

MAKE_FIELD_SEND_PACKET_FLAG(Field_MiniRoomRequest, 227);
//結束Opcode Flag註冊
FLAG_COLLECTION_BODY_END

#endif