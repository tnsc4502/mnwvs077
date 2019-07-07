#include "WvsException.h"
#include <ctime>
#include <chrono>
#include "..\String\StringUtility.h"
#include "..\Memory\ZMemory.h"

#ifdef _WIN32
#include <Windows.h>

void ExceptionCallBack_Default()
{

}

WvsException::CALLBACK_FUNC WvsException::ms_pfCallBack = ExceptionCallBack_Default;
std::string WvsException::ms_sAppName;

long __stdcall WvsUnhandledExceptionFilter(_EXCEPTION_POINTERS *pExcp)
{
	char sFileName[128] = { 0 };
	std::time_t start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	tm tm;
	localtime_s(&tm, &start_time);
	std::strftime(sFileName, sizeof(sFileName), "%d_%m_%Y___%H_%M_%S", &tm);

	FILE * fp;
	std::string fileName = (std::string)"./SysLog/" + WvsException::ms_sAppName  + "/" + sFileName + ".log";
	fopen_s(&fp, fileName.c_str(), "w+");
	fprintf(fp, "Exception Address: %018llX\n", (unsigned long long int)pExcp->ExceptionRecord->ExceptionAddress);
	fprintf(fp, "Exception Code: %018llX\n", (unsigned long long int)pExcp->ExceptionRecord->ExceptionCode);
	fprintf(fp, "Exception Flag: %018llX\n", (unsigned long long int)pExcp->ExceptionRecord->ExceptionFlags);
	fprintf(fp, "(x64)CPU Registers:\n");
	fprintf(fp, "Rax: %018llX\nRbx: %018llX\nRcx: %018llX\nRdx: %018llX\nRsi: %018llX\nRsp: %018llX\n",
		pExcp->ContextRecord->Rax,
		pExcp->ContextRecord->Rbx,
		pExcp->ContextRecord->Rcx,
		pExcp->ContextRecord->Rdx,
		pExcp->ContextRecord->Rsi,
		pExcp->ContextRecord->Rsp);
	fclose(fp);
	WvsException::ms_pfCallBack();
	return EXCEPTION_EXECUTE_HANDLER;
}

WvsException::WvsException()
{
}

WvsException::~WvsException()
{
}

void WvsException::RegisterUnhandledExceptionFilter(const std::string &sAppName, WvsException::CALLBACK_FUNC pfCallBack)
{
	if(pfCallBack)
		ms_pfCallBack = pfCallBack;
	ms_sAppName = std::move(sAppName);
	SetUnhandledExceptionFilter(WvsUnhandledExceptionFilter);
}

void WvsException::FatalError(const char *sFormat, ...)
{
	ZUniquePtr<char[]> sFormatted;
	STRUTILITY_PARSE_VAR_LIST(sFormat, sFormatted);
	MessageBox(nullptr, (char*)sFormatted, "Fatal Error", 0);
	exit(-1);
}

#elif

void WvsException::RegisterUnhandledExceptionFilter(const std::string &sAppName, WvsException::CALLBACK_FUNC pfCallBack)
{
	static_assert(false, "WvsException::RegisterUnhandledExceptionFilter: Unsupported platform.");
}

#endif