#include "StringUtility.h"
#include "..\Memory\ZMemory.h"

#include <algorithm>
#include <Windows.h>

void StringUtility::Split(const std::string & str, std::vector<std::string>& result, const std::string & delimeter)
{
	int nPrevPos = 0, nNextPos = 0, nSize = (int)str.size();
	while ((nNextPos = (int)str.find(delimeter, nPrevPos)) >= 0 && nNextPos < nSize)
	{
		result.push_back(std::move(str.substr(nPrevPos, nNextPos - nPrevPos)));
		nPrevPos = nNextPos + 1;
	}
	result.push_back(std::move(str.substr(nPrevPos, nSize - nPrevPos)));
}

void StringUtility::Split(const std::string & str, std::vector<int>& result, const std::string & delimeter)
{
	int nPrevPos = 0, nNextPos = 0, nSize = (int)str.size();
	while ((nNextPos = (int)str.find(delimeter, nPrevPos)) >= 0 && nNextPos < nSize)
	{
		result.push_back(atoi(str.substr(nPrevPos, nNextPos - nPrevPos).c_str()));
		nPrevPos = nNextPos + 1;
	}
	result.push_back(atoi(str.substr(nPrevPos, nSize - nPrevPos).c_str()));
}

void StringUtility::Split(const std::string & str, std::vector<double>& result, const std::string & delimeter)
{
	int nPrevPos = 0, nNextPos = 0, nSize = (int)str.size();
	while ((nNextPos = (int)str.find(delimeter, nPrevPos)) >= 0 && nNextPos < nSize)
	{
		result.push_back(atof(str.substr(nPrevPos, nNextPos - nPrevPos).c_str()));
		nPrevPos = nNextPos + 1;
	}
	result.push_back(atof(str.substr(nPrevPos, nSize - nPrevPos).c_str()));
}

std::string StringUtility::LeftPadding(std::string result, int totalSize, char paddingChar)
{
	while ((int)result.size() < totalSize)
		result = paddingChar + result;
	return result;
}

std::string StringUtility::RightPadding(std::string result, int totalSize, char paddingChar)
{
	while ((int)result.size() < totalSize)
		result += paddingChar;
	return result;
}

std::string StringUtility::ConvertUTF8ToSystemEncoding(const char * pStrUTF8)
{
#undef min
	const int MAX_CONVERT_SIZE = 1024;

	wchar_t wBuffer[MAX_CONVERT_SIZE] = { 0 };
	int nStrLen = std::min(MultiByteToWideChar(CP_UTF8, 0, pStrUTF8, -1, NULL, 0), 1023);
	memset(wBuffer, 0, nStrLen + 1);
	MultiByteToWideChar(CP_UTF8, 0, pStrUTF8, -1, wBuffer, nStrLen);

	int nStrLen_ = std::min(WideCharToMultiByte(CP_ACP, 0, wBuffer, -1, NULL, 0, NULL, NULL), 1023);
	char cBuffer[MAX_CONVERT_SIZE] = { 0 };
	memset(cBuffer, 0, nStrLen_ + 1);
	WideCharToMultiByte(CP_ACP, 0, wBuffer, -1, cBuffer, nStrLen_, NULL, NULL);
	return cBuffer;
}

std::string & StringUtility::Replace(std::string &str, const std::string & from, const std::string & to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos), start_pos) != std::string::npos)
		str.replace(start_pos, from.length(), to);
	return str;
}

std::string StringUtility::Format(const char * sFormat, ...)
{
	ZUniquePtr<char[]> sFormatted;
	STRUTILITY_PARSE_VAR_LIST(sFormat, sFormatted);
	return (char*)sFormatted;
	/*
	//Safer version but slower
	std::string sRet = "";
	auto nSize = snprintf(nullptr, 0, strSrc.data(), args...) + 1;
	ZUniquePtr<char[]> pBuffer = MakeUniqueArray<char>(nSize);
	snprintf((char*)pBuffer, nSize, strSrc.data(), args...);
	return std::string((char*)pBuffer, (char*)pBuffer + (int)nSize - 1);
	*/
}
