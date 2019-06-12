#include "InPacket.h"
#include <iostream>
#include "..\Logger\WvsLogger.h"

InPacket::InPacket(unsigned char* buff, unsigned short size)
	: aBuff(buff),
	  nPacketSize(size), nReadPos(0)
{
}

InPacket::~InPacket()
{
}

char InPacket::Decode1()
{
	if(nReadPos >= nPacketSize || nReadPos + sizeof(char) > nPacketSize)
		throw std::runtime_error("Access violation at InPacket::Decode1 [Reached the end of resource].");

	char ret = *(char*)(aBuff + nReadPos);
	nReadPos += sizeof(char);
	return ret;
}

short InPacket::Decode2()
{
	if (nReadPos >= nPacketSize || nReadPos + sizeof(short) > nPacketSize)
		throw std::runtime_error("Access violation at InPacket::Decode2 [Reached the end of resource].");

	short ret = *(short*)(aBuff + nReadPos);
	nReadPos += sizeof(short);
	return ret;
}

int InPacket::Decode4()
{
	if (nReadPos >= nPacketSize || nReadPos + sizeof(int) > nPacketSize)
		throw std::runtime_error("Access violation at InPacket::Decode4 [Reached the end of resource].");

	int ret = *(int*)(aBuff + nReadPos);
	nReadPos += sizeof(int);
	return ret;
}

long long int InPacket::Decode8()
{
	if (nReadPos >= nPacketSize || nReadPos + sizeof(long long int) > nPacketSize)
		throw std::runtime_error("Access violation at InPacket::Decode8 [Reached the end of resource].");

	long long int ret = *(long long int*)(aBuff + nReadPos);
	nReadPos += sizeof(long long int);
	return ret;
}

std::string InPacket::DecodeStr()
{
	if (nReadPos >= nPacketSize || nReadPos + sizeof(short) > nPacketSize)
		throw std::runtime_error("Access violation at InPacket::DecodeStr (Size Header) [Reached the end of resource].");

	short size = Decode2();
	if (nReadPos >= nPacketSize || nReadPos + size > nPacketSize)
		throw std::runtime_error("Access violation at InPacket::DecodeStr (Str Data) [Reached the end of resource].");

	std::string ret((char*)aBuff + nReadPos, size);
	nReadPos += size;
	return ret;
}

void InPacket::DecodeBuffer(unsigned char* dst, int size)
{
	if (nReadPos >= nPacketSize || nReadPos + size > nPacketSize)
		throw std::runtime_error("Access violation at InPacket::DecodeBuffer [Reached the end of resource].");

	if(dst)
		memcpy(dst, aBuff + nReadPos, size);
	nReadPos += size;
}

void InPacket::Print()
{
	for (int i = 0; i < nPacketSize; ++i)
		WvsLogger::LogFormat("0x%02X ", (int)aBuff[i]);
	WvsLogger::LogRaw("\n");
}

unsigned char* InPacket::GetPacket() const
{
	return aBuff;
}

unsigned short InPacket::GetPacketSize() const
{
	return nPacketSize;
}

unsigned short InPacket::RemainedCount() const
{
	if (nPacketSize <= nReadPos)
		return 0;
	return nPacketSize - nReadPos;
}

unsigned short InPacket::GetReadCount() const
{
	return nReadPos;
}

void InPacket::RestorePacket()
{
	nReadPos = 0;
}

void InPacket::Seek(int nPos)
{
	if(nPos >= 0 && nPos < nPacketSize)
		nReadPos = nPos;
}

void InPacket::Offset(int nOffset)
{
	nReadPos += nOffset;
	if (nReadPos < 0 || nReadPos >= nPacketSize)
		nReadPos -= nOffset;
}
