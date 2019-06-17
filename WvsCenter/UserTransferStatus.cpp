#include "UserTransferStatus.h"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"

UserTransferStatus::UserTransferStatus()
{
}


UserTransferStatus::~UserTransferStatus()
{
}

void UserTransferStatus::Decode(InPacket * iPacket)
{
	m_nChannelID = iPacket->Decode4();
	//Decode Temporary Status
	int nCount = iPacket->Decode4();
	m_aTS.resize(nCount);
	for (int i = 0; i < nCount; ++i)
		m_aTS[i].Decode(iPacket);
	
	//Decode Cooltime Records.
	nCount = iPacket->Decode4();
	m_aCooltime.resize(nCount);
	for (int i = 0; i < nCount; ++i)
		m_aCooltime[i].Decode(iPacket);

	//Decode Summoned Records
	nCount = iPacket->Decode4();
	m_aSummoned.resize(nCount);
	for (int i = 0; i < nCount; ++i)
		m_aSummoned[i].Decode(iPacket);
}

void UserTransferStatus::Encode(OutPacket * oPacket) const
{
	oPacket->Encode4(m_nChannelID);
	//Encode Temporary Status
	oPacket->Encode4((int)m_aTS.size());
	for (auto& ts : m_aTS)
		ts.Encode(oPacket);

	//Encode Cooltime Records
	oPacket->Encode4((int)m_aCooltime.size());
	for (auto& cooltime : m_aCooltime)
		cooltime.Encode(oPacket);

	//Encode Summoned Records
	oPacket->Encode4((int)m_aSummoned.size());
	for (auto& summoned : m_aSummoned)
		summoned.Encode(oPacket);
}

void UserTransferStatus::TransferTemporaryStatus::Encode(OutPacket *oPacket) const
{
	oPacket->Encode4(nSkillID);
	oPacket->Encode4(tTime);
	oPacket->Encode4(nSLV);
}

void UserTransferStatus::TransferTemporaryStatus::Decode(InPacket *iPacket)
{
	nSkillID = iPacket->Decode4();
	tTime = iPacket->Decode4();
	nSLV = iPacket->Decode4();
}

void UserTransferStatus::TransferCooltime::Encode(OutPacket *oPacket) const
{
	oPacket->Encode4(nReason);
	oPacket->Encode4(tTime);
}

void UserTransferStatus::TransferCooltime::Decode(InPacket *iPacket)
{
	nReason = iPacket->Decode4();
	tTime = iPacket->Decode4();
}

void UserTransferStatus::TransferSummoned::Encode(OutPacket * oPacket) const
{
	oPacket->Encode4(nSkillID);
	oPacket->Encode2(nSLV);
	oPacket->Encode4(tTime);
}

void UserTransferStatus::TransferSummoned::Decode(InPacket * iPacket)
{
	nSkillID = iPacket->Decode4();
	nSLV = iPacket->Decode2();
	tTime = iPacket->Decode4();
}
