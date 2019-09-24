#include "WzStream.h"
#include <iostream>

WzStream::WzStream(const std::wstring& sArchivePath)
	: m_iStream(sArchivePath, std::ios::binary | std::ios::in)
{
	if (!m_iStream)
		std::wcout << "Invalid file: " << sArchivePath << std::endl;
}

WzStream::~WzStream()
{
}

void WzStream::SetPosition(unsigned int uPos)
{
	m_iStream.seekg(uPos, std::ios::beg);
	m_uStreamPos = uPos;
}

unsigned int WzStream::GetPosition() const
{
	return m_uStreamPos;
}

unsigned long long int WzStream::GetLength() const
{
	return m_ulLength;
}

void WzStream::SetLength(unsigned long long int ulLength)
{
	m_ulLength = ulLength;
}

void WzStream::Read(char *pBuffer, unsigned int uSize)
{
	m_uStreamPos += uSize;
	m_iStream.read(pBuffer, uSize);
}