#pragma once
#include <string>

class WvsException
{
	typedef void(*CALLBACK_FUNC)();

	WvsException();
	~WvsException();

public:
	static CALLBACK_FUNC ms_pfCallBack;
	static std::string  ms_sAppName;
	static void RegisterUnhandledExceptionFilter(const std::string& sAppName, CALLBACK_FUNC pfCallBack);
	static void FatalError(const char *sFormat, ...);
};

