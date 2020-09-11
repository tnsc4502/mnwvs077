#include "InPacket.h"
#include <iostream>
#include "..\Logger\WvsLogger.h"

InPacket::InPacket(unsigned char* buff, unsigned short size)
	: m_aBuff(buff),
	  m_nPacketSize(size), m_nReadPos(0)
{
}

InPacket::~InPacket()
{
}

char InPacket::Decode1()
{
	if(m_nReadPos >= m_nPacketSize || m_nReadPos + sizeof(char) > m_nPacketSize)
		throw std::runtime_error("Access violation at InPacket::Decode1 [Reached the end of resource].");

	char ret = *(char*)(m_aBuff + m_nReadPos);
	m_nReadPos += sizeof(char);
	return ret;
}

short InPacket::Decode2()
{
	if (m_nReadPos >= m_nPacketSize || m_nReadPos + sizeof(short) > m_nPacketSize)
		throw std::runtime_error("Access violation at InPacket::Decode2 [Reached the end of resource].");

	short ret = *(short*)(m_aBuff + m_nReadPos);
	m_nReadPos += sizeof(short);
	return ret;
}

int InPacket::Decode4()
{
	if (m_nReadPos >= m_nPacketSize || m_nReadPos + sizeof(int) > m_nPacketSize)
		throw std::runtime_error("Access violation at InPacket::Decode4 [Reached the end of resource].");

	int ret = *(int*)(m_aBuff + m_nReadPos);
	m_nReadPos += sizeof(int);
	return ret;
}

long long int InPacket::Decode8()
{
	if (m_nReadPos >= m_nPacketSize || m_nReadPos + sizeof(long long int) > m_nPacketSize)
		throw std::runtime_error("Access violation at InPacket::Decode8 [Reached the end of resource].");

	long long int ret = *(long long int*)(m_aBuff + m_nReadPos);
	m_nReadPos += sizeof(long long int);
	return ret;
}

std::string InPacket::DecodeStr()
{
	if (m_nReadPos >= m_nPacketSize || m_nReadPos + sizeof(short) > m_nPacketSize)
		throw std::runtime_error("Access violation at InPacket::DecodeStr (Size Header) [Reached the end of resource].");

	short size = Decode2();
	if (m_nReadPos >= m_nPacketSize || m_nReadPos + size > m_nPacketSize)
		throw std::runtime_error("Access violation at InPacket::DecodeStr (Str Data) [Reached the end of resource].");

	std::string ret((char*)m_aBuff + m_nReadPos, size);
	m_nReadPos += size;
	return ret;
}

void InPacket::DecodeBuffer(unsigned char* dst, int size)
{
	if (m_nReadPos >= m_nPacketSize || m_nReadPos + size > m_nPacketSize)
		throw std::runtime_error("Access violation at InPacket::DecodeBuffer [Reached the end of resource].");

	if(dst)
		memcpy(dst, m_aBuff + m_nReadPos, size);
	m_nReadPos += size;
}

void InPacket::Print()
{
	std::string sOutput;
	char aBuffer[6] = { 0 };
	for (int i = 0; i < m_nPacketSize; ++i) 
	{
		sprintf_s(aBuffer, "0x%02X ", (int)m_aBuff[i]);
		sOutput += aBuffer;
	}
		//WvsLogger::LogFormat("0x%02X ", (int)aBuff[i]);
	sOutput += "\n";
	WvsLogger::LogRaw(sOutput.c_str());
	//WvsLogger::LogRaw("\n");
}

unsigned char* InPacket::GetPacket() const
{
	return m_aBuff;
}

unsigned short InPacket::GetPacketSize() const
{
	return m_nPacketSize;
}

unsigned short InPacket::RemainedCount() const
{
	if (m_nPacketSize <= m_nReadPos)
		return 0;
	return m_nPacketSize - m_nReadPos;
}

unsigned short InPacket::GetReadCount() const
{
	return m_nReadPos;
}

void InPacket::RestorePacket()
{
	m_nReadPos = 0;
}

void InPacket::Seek(int nPos)
{
	if(nPos >= 0 && nPos < m_nPacketSize)
		m_nReadPos = nPos;
}

void InPacket::Offset(int nOffset)
{
	m_nReadPos += nOffset;
	if (m_nReadPos < 0 || m_nReadPos >= m_nPacketSize)
		m_nReadPos -= nOffset;
}
