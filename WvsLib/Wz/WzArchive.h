#pragma once
#include "..\Common\CommonDef.h"
#include "WzMappedFileStream.h"

#include <string>

typedef WzMappedFileStream WzStreamType;

class WzStreamCodec;
typedef WzStreamCodec CipherType;

class WzPackage;
class WzStreamCodec;
class WzNameSpace;

class WzArchive
{
	ALLOW_PRIVATE_ALLOC

	unsigned long long int m_ulLength = 0;
	unsigned int m_uBeginPos = 0, m_uStreamPos = 0, m_uKey = 0;

	WzStreamType* m_pStream = nullptr;
	WzNameSpace* m_pTopNameSpace = nullptr;
	CipherType* m_pCipher = nullptr;

	WzArchive(const std::wstring& sArchivePath, const std::string& sArchiveName, CipherType* pChipher);

public:
	~WzArchive();
	static WzArchive* Mount(const std::wstring& sArchivePath, const std::string& sArchiveName, CipherType* pChipher);
	WzStreamType* GetStream();
	CipherType* GetCipher();
	unsigned int GetBeginPos() const;
	unsigned int GetArchiveKey() const;
	unsigned long long int GetLength() const;
	void SetLength(unsigned long long int ulLength);
	WzNameSpace* GetRoot();
	std::string DecodeString(WzStreamType *pStream);
	std::string DecodePropString(WzStreamType *pStream, unsigned int uRootPropPos);
};

