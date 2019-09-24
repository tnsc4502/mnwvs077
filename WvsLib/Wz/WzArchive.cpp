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
	m_pStream = AllocObjCtor(WzStreamType)(sArchivePath);
	m_pCipher = pChipher;


	//WzPackage::Init
	char aHeader[5] = { 0 };
	Read(aHeader, 4);

	if (strcmp(aHeader, "PKG1"))
	{
		//Raw .Img file
		m_bEncrypted = false;
		m_pTopNameSpace = AllocObjCtor(WzNameSpaceProperty)(this, WzNameSpace::WzNameSpaceType::Type_Property, sArchiveName, 0);
		m_pTopNameSpace->OnGetItem();
		return;
	}

	unsigned long long ulLength = 0;
	Read((char*)&ulLength, sizeof(ulLength));
	SetLength(ulLength);

	unsigned int uBeginPos = 0;
	Read((char*)&uBeginPos, sizeof(uBeginPos));
	m_uBeginPos = uBeginPos;
	SetPosition(m_uBeginPos);

	unsigned int uEncVersion = 0;
	Read((char*)&uEncVersion, sizeof(short));
	std::string sVersion;

	m_pTopNameSpace = AllocObjCtor(WzPackage)(this, WzNameSpace::WzNameSpaceType::Type_Directory, sArchiveName, GetPosition());
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
		FreeObj(m_pStream);
	if (m_pTopNameSpace)
		FreeObj(m_pTopNameSpace);
}

WzArchive *WzArchive::Mount(const std::wstring & sArchivePath, const std::string& sArchiveName, CipherType* pChipher)
{
	if (!filesystem::exists(sArchivePath))
		return nullptr;

	auto pArchive = AllocObjCtor(WzArchive)(sArchivePath, sArchiveName, pChipher);
	return pArchive;
}

void WzArchive::SetPosition(unsigned int uPos)
{
	m_pStream->SetPosition(uPos);
}

WzStreamType* WzArchive::GetStream()
{
	return m_pStream;
}

CipherType* WzArchive::GetCipher()
{
	return m_pCipher;
}

unsigned int WzArchive::GetPosition() const
{
	return m_pStream->GetPosition();
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

void WzArchive::Read(char *pBuffer, unsigned int uSize)
{
	m_pStream->Read(pBuffer, uSize);
}

WzNameSpace* WzArchive::GetRoot()
{
	return m_pTopNameSpace;
}

std::string WzArchive::DecodeString()
{
	return m_pCipher->DecodeString(this);
}

std::string WzArchive::DecodePropString(unsigned int uRootPropPos)
{
	unsigned int nType = 0;
	Read((char*)&nType, 1);
	switch (nType)
	{
		case 0x00:
		case 0x73:
			return DecodeString();
		case 0x01:
		case 0x1B:
		{
			//Read offset of that foreign resource.
			Read((char*)&nType, 4);
			unsigned int uCurrentPos = GetPosition();
			SetPosition(uRootPropPos + nType);
			std::string ret = DecodeString();
			SetPosition(uCurrentPos);
			return ret;
		}
		default:
			return "";
			//WvsException::FatalError("Unknown type of prop string <%d>.", nType);
	}
}

bool WzArchive::Encrypted() const
{
	return m_bEncrypted;
}

