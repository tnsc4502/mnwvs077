#pragma once
#include <fstream>
#include <string>

class WzStream
{
	std::ifstream m_iStream;
	unsigned long long int m_ulLength = 0;
	unsigned int m_uStreamPos = 0;

public:
	WzStream(const std::wstring& sArchivePath);
	~WzStream();

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

