#include "WzArchive.h"
#include "WzStream.h"
#include "WzPackage.h"
#include "WzNameSpaceProperty.h"
#include "StandardFileSystem.h"
#include "..\Memory\MemoryPoolMan.hpp"
#include "..\Exception\WvsException.h"
#include "WzStreamCodec.h"

WzArchive::WzArchive(const std::wstring& sArchivePath, const std::string& sArchiveName, CipherType* pChipher)
{
	m_pStream = new (WzStreamType)(sArchivePath);
	m_pCipher = pChipher;


	//WzPackage::Init
	char aHeader[5] = { 0 };
	m_pStream->Read(aHeader, 4);

	if (strcmp(aHeader, "PKG1"))
	{
		//Raw .Img file
		m_pStream->SetEncrypted(false);
		m_pTopNameSpace = new (WzNameSpaceProperty)(this, WzNameSpace::WzNameSpaceType::Type_Property, sArchiveName, 0);
		m_pTopNameSpace->OnGetItem();
		return;
	}

	unsigned long long ulLength = 0;
	m_pStream->Read((char*)&ulLength, sizeof(ulLength));
	SetLength(ulLength);

	unsigned int uBeginPos = 0;
	m_pStream->Read((char*)&uBeginPos, sizeof(uBeginPos));
	m_uBeginPos = uBeginPos;
	m_pStream->SetPosition(m_uBeginPos);

	unsigned int uEncVersion = 0;
	m_pStream->Read((char*)&uEncVersion, sizeof(short));
	std::string sVersion;

	m_pTopNameSpace = new (WzPackage)(this, WzNameSpace::WzNameSpaceType::Type_Directory, sArchiveName, m_pStream->GetPosition());
	//Find matched version key.
	for (unsigned int uVersion = 512; uVersion > 0; --uVersion)
	{
		m_uKey = 0;
		sVersion = std::to_string(uVersion);
		for (auto c : sVersion)
			m_uKey = 32 * m_uKey + c + 1;
		if ((0xFF ^ (m_uKey >> 24) ^ (m_uKey << 8 >> 24) ^ (m_uKey << 16 >> 24) ^ (m_uKey << 24 >> 24)) == uEncVersion
			&& ((WzPackage*)m_pTopNameSpace)->LoadDirectory(true))
			break;
	}
	((WzPackage*)m_pTopNameSpace)->LoadDirectory();
}

WzArchive::~WzArchive()
{
	if (m_pStream)
		delete (m_pStream);
	if (m_pTopNameSpace)
		delete (m_pTopNameSpace);
}

WzArchive *WzArchive::Mount(const std::wstring & sArchivePath, const std::string& sArchiveName, CipherType* pChipher)
{
	if (!filesystem::exists(sArchivePath))
		return nullptr;

	auto pArchive = new (WzArchive)(sArchivePath, sArchiveName, pChipher);
	return pArchive;
}

WzStreamType* WzArchive::GetStream()
{
	return m_pStream;
}

CipherType* WzArchive::GetCipher()
{
	return m_pCipher;
}

unsigned int WzArchive::GetBeginPos() const
{
	return m_uBeginPos;
}

unsigned int WzArchive::GetArchiveKey() const
{
	return m_uKey;
}

unsigned long long int WzArchive::GetLength() const
{
	return m_pStream->GetLength();
}

void WzArchive::SetLength(unsigned long long int ulLength)
{
	m_pStream->SetLength(ulLength);
}

WzNameSpace* WzArchive::GetRoot()
{
	return m_pTopNameSpace;
}

std::string WzArchive::DecodeString(WzStreamType *pStream)
{
	return m_pCipher->DecodeString(pStream);
}

std::string WzArchive::DecodePropString(WzStreamType *pStream, unsigned int uRootPropPos)
{
	return m_pCipher->DecodePropString(pStream, uRootPropPos);
}