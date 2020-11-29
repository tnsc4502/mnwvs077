#include "TownPortal.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "TownPortalPacketTypes.hpp"


TownPortal::TownPortal()
{
}


TownPortal::~TownPortal()
{
}

void TownPortal::Init(int nCharacterID, int nX, int nY, unsigned tEnd)
{
	m_nCharacterID = nCharacterID;
	m_ptPos.x = nX;
	m_ptPos.y = nY;
	m_tEnd = tEnd;
}

void TownPortal::MakeEnterFieldPacket(OutPacket * oPacket)
{
	MakeEnterFieldPacket(oPacket, 1);
}

void TownPortal::MakeEnterFieldPacket(OutPacket * oPacket, int nEnterType)
{
	oPacket->Encode2(TownPortalSendPacketType::TownPortal_OnMakeEnterFieldPacket);
	oPacket->Encode1(nEnterType);
	oPacket->Encode4(m_nCharacterID);
	oPacket->Encode2(m_ptPos.x);
	oPacket->Encode2(m_ptPos.y);
}

void TownPortal::MakeLeaveFieldPacket(OutPacket * oPacket)
{
	MakeLeaveFieldPacket(oPacket, 1);
}

void TownPortal::MakeLeaveFieldPacket(OutPacket * oPacket, int nLeaveType)
{
	oPacket->Encode2(TownPortalSendPacketType::TownPortal_OnMakeLeaveFieldPacket);
	oPacket->Encode1(nLeaveType);
	oPacket->Encode4(m_nCharacterID);
}
