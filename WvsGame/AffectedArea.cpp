#include "AffectedArea.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\AffectedAreaPacketFlags.hpp"

AffectedArea::AffectedArea()
{
}


AffectedArea::~AffectedArea()
{
}

void AffectedArea::MakeEnterFieldPacket(OutPacket * oPacket)
{
	oPacket->Encode2(AffectedAreaSendPacketFlag::AffectedArea_OnMakeEnterFieldPacket);
	oPacket->Encode4(GetFieldObjectID());
	oPacket->Encode1(m_bMobSkill ? 1 : 0);
	oPacket->Encode4(m_nOwnerID);
	oPacket->Encode4(m_nSkillID);
	oPacket->Encode1((char)m_nSLV);
	int tTime = (int)(((long long int)m_tStart - (long long int)GameDateTime::GetTime()) / 100);
	if (tTime < 0)
		tTime = 0;
	oPacket->Encode2((short)tTime);
	oPacket->EncodeBuffer((unsigned char*)&m_rcAffectedArea, 16);
	oPacket->Encode1(m_bSmoke ? 1 : 0);
	oPacket->Encode4(0);
}

void AffectedArea::MakeLeaveFieldPacket(OutPacket * oPacket)
{
	oPacket->Encode2(AffectedAreaSendPacketFlag::AffectedArea_OnMakeLeaveFieldPacket);
	oPacket->Encode4(GetFieldObjectID());
}

int AffectedArea::GetSkillID() const
{
	return m_nSkillID;
}

int AffectedArea::GetSkillLevel() const
{
	return m_nSLV;
}

int AffectedArea::GetOwnerID() const
{
	return m_nOwnerID;
}
