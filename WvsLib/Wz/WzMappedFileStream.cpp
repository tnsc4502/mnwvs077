#include "WzMappedFileStream.h"
#include <Windows.h>

WzMappedFileStream::WzMappedFileStream(const std::wstring& sArchivePath)
{
	m_hFile = CreateFileW(sArchivePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE) throw("Failed to open file");
	HANDLE hMap = CreateFileMappingA(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (!hMap)
	{
		MessageBoxA(nullptr, "Failed to create file mapping", "WzMappedFileStream.cpp", 0);  
		throw("Failed to create file mapping");
	};
	m_pFileBase = reinterpret_cast<char *>(MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0));
	if (!m_pFileBase) throw("Failed to map view of file");
	m_pStream = m_pFileBase;
}

WzMappedFileStream::WzMappedFileStream(const WzMappedFileStream & rhs)
{
	m_hFile = nullptr;
	m_pFileBase = rhs.m_pFileBase;
	m_pStream = rhs.m_pStream;
	m_bEncrypted = rhs.m_bEncrypted;
	m_ulLength = rhs.m_ulLength;
	m_uStreamPos = rhs.m_uStreamPos;
}

WzMappedFileStream::~WzMappedFileStream()
{
	CloseHandle(m_hFile);
}

char * WzMappedFileStream::GetStreamPtr()
{
	return m_pStream;
}

void WzMappedFileStream::SetPosition(unsigned int uPos)
{
	m_pStream = (m_pFileBase + uPos);
	m_uStreamPos = uPos;
}

unsigned int WzMappedFileStream::GetPosition() const
{
	return m_uStreamPos;
}

unsigned long long int WzMappedFileStream::GetLength() const
{
	return m_ulLength;
}

void WzMappedFileStream::SetLength(unsigned long long int ulLength)
{
	m_ulLength = ulLength;
}

void WzMappedFileStream::SetEncrypted(bool bEncrypted)
{
	m_bEncrypted = bEncrypted;
}

bool WzMappedFileStream::Encrypted() const
{
	return m_bEncrypted;
}

void WzMappedFileStream::Read(char* pBuffer, unsigned int uSize)
{
	memcpy(pBuffer, m_pStream, uSize);
	m_pStream += uSize;
	m_uStreamPos += uSize;
}
