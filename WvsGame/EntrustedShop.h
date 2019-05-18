#pragma once
#include "PersonalShop.h"

class EntrustedShop : public PersonalShop
{
public:
	EntrustedShop();
	~EntrustedShop();

	void OnPacket(User *pUser, int nType, InPacket *iPacket);
};

