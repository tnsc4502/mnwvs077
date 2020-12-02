#pragma once
#ifndef FIELD_FLAG
#define FIELD_FLAG

#include "..\WvsLib\Net\PacketTypes.hpp"

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

DEFINE_PACKET_TYPE(FieldSendPacketType)
	REGISTER_TYPE(Field_OnTransferFieldReqIgnored, 80);
	REGISTER_TYPE(Field_OnTransferChannelReqIgnored, 81);
	REGISTER_TYPE(Field_OnFieldSpecificData, 82);
	REGISTER_TYPE(Field_OnGroupMessage, 83);
	REGISTER_TYPE(Field_OnWhisper, 84);
	REGISTER_TYPE(Field_OnSummonItemInavailable, 85);
	REGISTER_TYPE(Field_OnFieldEffect, 86);
	REGISTER_TYPE(Field_OnBlowWeather, 87);
	REGISTER_TYPE(Field_OnPlayJukeBox, 88);
	REGISTER_TYPE(Field_OnAdminResult, 89);
	REGISTER_TYPE(Field_OnQuiz, 90);
	REGISTER_TYPE(Field_OnDesc, 91);
	REGISTER_TYPE(Field_OnClock, 92);
	REGISTER_TYPE(Field_OnContiMove, 93);
	REGISTER_TYPE(Field_OnContiState, 94);
	REGISTER_TYPE(Field_OnSetQuestClear, 95);
	REGISTER_TYPE(Field_OnSetQuestTime, 96);
	REGISTER_TYPE(Field_OnSetObjectState, 97);
	REGISTER_TYPE(Field_OnDestroyClock, 98);
	REGISTER_TYPE(Field_OnFootHoldInfo, 100);
	REGISTER_TYPE(Field_TrunkRequest, 222);
	REGISTER_TYPE(Field_StoreBankMessage, 223);
	REGISTER_TYPE(Field_StoreBankRequest, 224);
	REGISTER_TYPE(Field_MiniRoomRequest, 227);

	REGISTER_TYPE(Field_GuildBoss_OnHealerMove, 206);
	REGISTER_TYPE(Field_GuildBoss_OnPullyStateChange, 207);

	REGISTER_TYPE(Field_MCarnival_OnEnter, 208);
	REGISTER_TYPE(Field_MCarnival_SendPersonalCP, 209);
	REGISTER_TYPE(Field_MCarnival_SendTeamCP, 210);
	REGISTER_TYPE(Field_MCarnival_OnRequestResult_Success, 211);
	REGISTER_TYPE(Field_MCarnival_OnRequestResult_Failed, 212);
	REGISTER_TYPE(Field_MCarnival_OnProcessForDeath, 213);
	REGISTER_TYPE(Field_MCarnival_ShowMemberOutMsg, 214);
	REGISTER_TYPE(Field_MCarnival_ShowGameResult, 215);
END_PACKET_TYPE(FieldSendPacketType)

DEFINE_PACKET_TYPE(FieldRecvPacketType)
	REGISTER_TYPE(Field_GuildBoss_OnPullyHit, 160);
	REGISTER_TYPE(Field_MCarnival_OnRequest, 169);
	REGISTER_TYPE(Field_OnContiMoveStateRequest, 171);
END_PACKET_TYPE(FieldRecvPacketType)

#endif