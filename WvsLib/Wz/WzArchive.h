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

	bool m_bEncrypted = true;
	unsigned long long int m_ulLength = 0;
	unsigned int m_uBeginPos = 0, m_uStreamPos = 0, m_uKey = 0;

	WzStreamType* m_pStream = nullptr;
	WzNameSpace* m_pTopNameSpace = nullptr;
	CipherType* m_pCipher = nullptr;

	WzArchive(const std::wstring& sArchivePath, const std::string& sArchiveName, CipherType* pChipher);

public:
	~WzArchive();
	static WzArchive* Mount(const std::wstring& sArchivePath, const std::string& sArchiveName, CipherType* pChipher);
	void SetPosition(unsigned int uPos);
	WzStreamType* GetStream();
	CipherType* GetCipher();
	unsigned int GetPosition() const;
	unsigned int GetBeginPos() const;
	unsigned int GetArchiveKey() const;
	unsigned long long int GetLength() const;
	void SetLength(unsigned long long int ulLength);
	void Read(char *pBuffer, unsigned int uSize);
	WzNameSpace* GetRoot();
	std::string DecodeString();
	std::string DecodePropString(unsigned int uRootPropPos);
	bool Encrypted() const;

	//ZDataFilter::_Read
	template<typename T>
	T ReadFilter()
	{
		return m_pStream->Read<T>();
	}
};

