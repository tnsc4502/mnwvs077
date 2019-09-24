#pragma once
#include <string>

//Use MappingFile instead of std::fstream
class WzMappedFileStream
{
	void* m_hFile;
	char* m_pFileBase, *m_pStream;

	unsigned long long int m_ulLength = 0;
	unsigned int m_uStreamPos = 0;

public:
	WzMappedFileStream(const std::wstring& sArchivePath);
	~WzMappedFileStream();

	char* GetStreamPtr();
	void SetPosition(unsigned int uPos);
	unsigned int GetPosition() const;
	unsigned long long int GetLength() const;
	void SetLength(unsigned long long int ulLength);
	void Read(char *pBuffer, unsigned int uSize);

	//ZDataFilter::_Read, T can only be int16, int32, int64
	template<typename T>
	T Read()
	{
		char b;
		Read(&b, 1);
		if (b == -128)
		{
			T ret;
			Read((char*)&ret, sizeof(T));
			return ret;
		}
		return b;
	}
};

