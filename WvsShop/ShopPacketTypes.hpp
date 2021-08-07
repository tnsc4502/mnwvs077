#pragma once
#ifndef SHOP_FLAG
#define SHOP_FLAG

#include "..\WvsLib\Net\PacketTypes.hpp"
#include <typeinfo>

//==================================SHOP SEND===============================
DEFINE_PACKET_TYPE(ShopSendPacketType)
	REGISTER_TYPE(Client_ValidateState, 10); //Stage::OnSetCashShop
	REGISTER_TYPE(Client_SetCashShop, 79); //Stage::OnSetCashShop
	REGISTER_TYPE(User_QueryCashResult, 244);
	REGISTER_TYPE(User_CashItemResult, 245);

END_PACKET_TYPE(ShopSendPacketType)

//==================================SHOP RECV===============================
DEFINE_PACKET_TYPE(ShopRecvPacketType)
	REGISTER_TYPE(User_OnQueryCashReques, 0xB5); 
	REGISTER_TYPE(User_OnCashItemRequest, 0xB6); 
	REGISTER_TYPE(User_OnCouponRequest, 0x460);
END_PACKET_TYPE(ShopRecvPacketType)


#endif