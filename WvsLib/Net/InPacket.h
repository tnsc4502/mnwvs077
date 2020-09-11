#pragma once
#include <string>

class InPacket
{
private:
	unsigned char* m_aBuff;
	unsigned short m_nPacketSize, m_nReadPos;

public:
	InPacket(unsigned char* buff, unsigned short size);
	~InPacket();

	char Decode1();
	short Decode2();
	int Decode4();
	long long int Decode8();
	std::string DecodeStr();
	void DecodeBuffer(unsigned char* dst, int size);

	/*Return the pointer to raw buffer.*/
	unsigned char* GetPacket() const;

	/*Return the number of bytes received.*/
	unsigned short GetPacketSize() const;

	/*Return the number of bytes remained.*/
	unsigned short RemainedCount() const;

	/*Return the number of bytes currently read = the position of packet reader.s*/
	unsigned short GetReadCount() const;

	/*Reset reader pos = Seek(0).*/
	void RestorePacket();

	/*Set reader pos*/
	void Seek(int nPos);

	/*Increase or decrease reader pos by nOffset.*/
	void Offset(int nOffset);

	//Just for debugging purposes.
	void Print();
};