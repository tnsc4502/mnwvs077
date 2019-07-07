#pragma once

#include <cstdarg>
#include <vector>
#include <string>

class StringUtility
{
#define STRUTILITY_PARSE_VAR_LIST(sFormat, sFormatted) \
	int nPrinted, nAlloc = (int)strlen(sFormat) * 2;\
	va_list ap;\
	while (1) { \
		sFormatted.reset(AllocArray(char, nAlloc));\
		va_start(ap, sFormat);\
		nPrinted = vsnprintf(&sFormatted[0], nAlloc, sFormat, ap);\
		va_end(ap);\
		if (nPrinted < 0 || nPrinted >= nAlloc)\
			nAlloc += abs(nPrinted - nAlloc + 1);\
		else\
			break;\
	}

public:
	static void Split(const std::string & str, std::vector<std::string>& result, const std::string & delimeter);
	static void Split(const std::string & str, std::vector<int>& result, const std::string & delimeter);
	static void Split(const std::string & str, std::vector<double>& result, const std::string & delimeter);

	static std::string LeftPadding(std::string result, int totalSize, char paddingChar);
	static std::string RightPadding(std::string result, int totalSize, char paddingChar);
	static std::string ConvertUTF8ToSystemEncoding(const char* pStrUTF8);

	template<typename T>
	static std::string VectorToString(const std::vector<T>& container, const std::string& delimeter)
	{
		std::string ret = "";
		int nSize = (int)container.size(), i = 0;
		for(auto iter = container.begin(); iter != container.end(); ++iter)
			ret += (std::to_string(*iter) + (i++ != nSize - 1 ? delimeter : ""));
		return ret;
	}

	static std::string& Replace(std::string& str, const std::string& from, const std::string& to);
	static std::string Format(const char *sFormat, ...);
};