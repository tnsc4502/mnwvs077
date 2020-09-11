#include "OutPacket.h"
#include "InPacket.h"
#include "..\Logger\WvsLogger.h"
#include "..\Memory\MemoryPoolMan.hpp"

void OutPacket::ExtendSize(int nExtendRate = 2)
{
	decltype(m_pSharedPacket->m_aBuff) newBuff = AllocArray(unsigned char, (m_pSharedPacket->m_nBuffSize * nExtendRate));
	memcpy(newBuff, m_pSharedPacket->m_aBuff, m_pSharedPacket->m_nBuffSize);
	FreeArray(m_pSharedPacket->m_aBuff);
	m_pSharedPacket->m_nBuffSize *= nExtendRate;
	m_pSharedPacket->m_aBuff = newBuff;
}

OutPacket::OutPacket()
{
	m_pSharedPacket = AllocObj(SharedPacket);
	(*((long long int*)m_pSharedPacket->m_aBuff)) = (long long int)(m_pSharedPacket);
}

OutPacket::~OutPacket()
{
	DecRefCount();
}

void OutPacket::Encode1(char value)
{
	if (m_pSharedPacket->m_nPacketSize + sizeof(value) >= m_pSharedPacket->m_nBuffSize)
		ExtendSize();
	*(decltype(value)*)(m_pSharedPacket->m_aBuff + m_pSharedPacket->m_nPacketSize) = value;
	m_pSharedPacket->m_nPacketSize += sizeof(value);
}

void OutPacket::Encode2(short value)
{
	if (m_pSharedPacket->m_nPacketSize + sizeof(value) >= m_pSharedPacket->m_nBuffSize)
		ExtendSize();
	*(decltype(value)*)(m_pSharedPacket->m_aBuff + m_pSharedPacket->m_nPacketSize) = value;
	m_pSharedPacket->m_nPacketSize += sizeof(value);
}

void OutPacket::Encode4(int value)
{
	if (m_pSharedPacket->m_nPacketSize + sizeof(value) >= m_pSharedPacket->m_nBuffSize)
		ExtendSize();
	*(decltype(value)*)(m_pSharedPacket->m_aBuff + m_pSharedPacket->m_nPacketSize) = value;
	m_pSharedPacket->m_nPacketSize += sizeof(value);
}

void OutPacket::Encode8(long long int value)
{
	if (m_pSharedPacket->m_nPacketSize + sizeof(value) >= m_pSharedPacket->m_nBuffSize)
		ExtendSize();
	*(decltype(value)*)(m_pSharedPacket->m_aBuff + m_pSharedPacket->m_nPacketSize) = value;
	m_pSharedPacket->m_nPacketSize += sizeof(value);
}

void OutPacket::EncodeBuffer(unsigned char *buff, int nSize, int nZero)
{
	if (m_pSharedPacket->m_nPacketSize + nSize + nZero >= m_pSharedPacket->m_nBuffSize)
		ExtendSize((int)std::ceil((m_pSharedPacket->m_nPacketSize + nSize + nZero) / m_pSharedPacket->m_nBuffSize) + 1);
	if (buff == nullptr) 
	{
		int nEncode4Count = nSize / 4;
		int remain = nSize % 4;
		for (int i = 0; i < nEncode4Count; ++i)
			Encode4(0);
		for (int i = 0; i < remain; ++i)
			Encode1(0);
	}
	else 
	{
		memcpy(m_pSharedPacket->m_aBuff + m_pSharedPacket->m_nPacketSize, buff, nSize);
		m_pSharedPacket->m_nPacketSize += nSize;
		if (nZero > 0)
			for (int i = 0; i < nZero; ++i)
				Encode1(0);
	}
}

void OutPacket::EncodeStr(const std::string &str)
{
	Encode2((short)str.size());
	if (m_pSharedPacket->m_nPacketSize + str.size() >= m_pSharedPacket->m_nBuffSize)
		ExtendSize();
	memcpy(m_pSharedPacket->m_aBuff + m_pSharedPacket->m_nPacketSize, str.c_str(), str.size());
	m_pSharedPacket->m_nPacketSize += (unsigned int)str.size();
}

void OutPacket::Release()
{
	FreeArray(m_pSharedPacket->m_aBuff);
	//delete[] m_pSharedPacket->aBuff;
}

void OutPacket::Reset()
{
	m_pSharedPacket->m_nPacketSize = INITIAL_WRITE_INDEX;
}

void OutPacket::IncRefCount()
{
	m_pSharedPacket->IncRefCount();
}

void OutPacket::DecRefCount()
{
	m_pSharedPacket->DecRefCount();
}

void OutPacket::Print()
{
	WvsLogger::LogRaw("OutPacket to bytes (HEX):");
	for (unsigned int i = 0; i < m_pSharedPacket->m_nPacketSize; ++i)
		WvsLogger::LogFormat("0x%02X ", (int)m_pSharedPacket->m_aBuff[i]);
	WvsLogger::LogRaw("\n");
}

OutPacket::SharedPacket * OutPacket::GetSharedPacket()
{
	return m_pSharedPacket;
}

void OutPacket::CopyFromTransferedPacket(InPacket *oPacket)
{
}

void OutPacket::EncodeHexString(const std::string& str)
{
	auto cStr = str.c_str();
	int d, n;
	while (sscanf_s(cStr, "%X%n", &d, &n) == 1)
	{
		cStr += n;
		Encode1((char)d);
	}
}

OutPacket::SharedPacket::SharedPacket()
	: m_aBuff(AllocArray(unsigned char, DEFAULT_BUFF_SIZE)),
	m_nBuffSize(DEFAULT_BUFF_SIZE),
	m_nPacketSize(INITIAL_WRITE_INDEX),
	m_nRefCount(1)
{
}

OutPacket::SharedPacket::~SharedPacket()
{
}

void OutPacket::SharedPacket::ToggleBroadcasting()
{
	m_bBroadcasting = true;
}

bool OutPacket::SharedPacket::IsBroadcasting() const
{
	return m_bBroadcasting;
}

void OutPacket::SharedPacket::IncRefCount()
{
	++m_nRefCount;
}

void OutPacket::SharedPacket::DecRefCount()
{
	if (--m_nRefCount <= 0)
	{
		FreeArray(m_aBuff);
		for (auto p : m_aBroadcasted) 
			FreeArray((unsigned char*)p);
		//delete[] aBuff;
		//MSMemoryPoolMan::GetInstance()->DestructArray(aBuff);
		//delete this;
		FreeObj(this);
	}
}

void OutPacket::SharedPacket::AttachBroadcastingPacket(void * p)
{
	m_aBroadcasted.push_back(p);
}
