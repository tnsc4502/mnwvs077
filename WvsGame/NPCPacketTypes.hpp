#pragma once
#ifndef NPC_FLAG
#define NPC_FLAG

#include "..\WvsLib\Net\PacketTypes.hpp"

DEFINE_PACKET_TYPE(NPCSendPacketTypes)
//Flags to Game
	REGISTER_TYPE(NPC_OnNpcShopItemList, 220);
	REGISTER_TYPE(NPC_OnNpcShopRequest, 221);

	REGISTER_TYPE(NPC_OnMakeEnterFieldPacket, 175);
	REGISTER_TYPE(NPC_OnMakeLeaveFieldPacket, 176);
	REGISTER_TYPE(NPC_OnNpcChangeController, 177);
	REGISTER_TYPE(NPC_OnMove, 178);
	REGISTER_TYPE(NPC_ScriptMessage, 219);  //ScriptMan::OnScriptMessage		 							
END_PACKET_TYPE(NPCSendPacketTypes)


DEFINE_PACKET_TYPE(NPCRecvPacketTypes)

//Flags to Game
	REGISTER_TYPE(NPC_OnMoveRequest, 149);

END_PACKET_TYPE(NPCRecvPacketTypes)

#endif