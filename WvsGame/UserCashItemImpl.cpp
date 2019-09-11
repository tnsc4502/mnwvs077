/*
09/12/2019 added.
Implementations of cash item usages.
*/

#include "UserCashItemImpl.h"
#include "User.h"
#include "..\WvsLib\String\StringPool.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"

int UserCashItemImpl::ConsumeSpeakerChannel(User *pUser, InPacket *iPacket)
{
	std::string sMsg = pUser->GetName();
	sMsg += " : " + iPacket->DecodeStr();

	if (sMsg.length() >= 80)
	{
		pUser->SendNoticeMessage(GET_STRING(GameSrv_User_Invalid_Chat_Msg));
		return 0;
	}

	OutPacket oPacket;
	oPacket.Encode2(UserSendPacketFlag::UserLocal_OnBroadcastMsg);
	oPacket.Encode1(BroadcastMsgType::e_BroadcastMsg_Speaker);
	oPacket.EncodeStr(sMsg);
	User::Broadcast(&oPacket);

	return 1;
}
