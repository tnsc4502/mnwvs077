#include "PartyMan.h"
#include "User.h"
#include "Center.h"
#include "WvsGame.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\PacketFlags\GamePacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\CenterPacketFlags.hpp"

PartyMan::PartyMan()
{
}

PartyMan::~PartyMan()
{
}

#ifdef _WVSGAME
void PartyMan::OnPartyRequest(User *pUser, InPacket * iPacket)
{
	int nRequest = iPacket->Decode1();
	switch (nRequest)
	{
		case PartyRequest::rq_Party_Create:
		{
			OutPacket oPacket;
			oPacket.Encode2(GameSendPacketFlag::PartyRequest);
			oPacket.Encode1(PartyRequest::rq_Party_Create);
			oPacket.Encode4(pUser->GetUserID());
			WvsBase::GetInstance<WvsGame>()->GetCenter()->SendPacket(&oPacket);
		}
		break;
	}
}
#endif

PartyMan * PartyMan::GetInstance()
{
	static PartyMan *pInstance = new PartyMan();

	return pInstance;
}
