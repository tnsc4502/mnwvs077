#include "WvsException.h"
#include <ctime>
#include <chrono>
#include "..\String\StringUtility.h"
#include "..\Memory\ZMemory.h"

#ifdef _WIN32
#include <Windows.h>

#pragma warning(disable:4091)
#include <DbgHelp.h>
#pragma warning(default:4091)

#include <stdlib.h>

void ExceptionCallBack_Default()
{

}

WvsException::CALLBACK_FUNC WvsException::ms_pfCallBack = ExceptionCallBack_Default;
std::string WvsException::ms_sAppName;

struct StackTrace
{
	std::string FunctionName, FileName;
	int LineNumber;
};

void DumpStackTrace(PEXCEPTION_POINTERS pExcp, FILE *pFile)
{
	fprintf(pFile, "[StackInfo Dump]\n");
	HANDLE hProcess = GetCurrentProcess();
	SymInitialize(hProcess, NULL, TRUE);

	CONTEXT CR = *pExcp->ContextRecord;
	STACKFRAME64 StackFrame;
	memset(&StackFrame, 0, sizeof(StackFrame));
#if defined(_WIN64)
	int nMachineType = IMAGE_FILE_MACHINE_AMD64;
	StackFrame.AddrPC.Offset = CR.Rip;
	StackFrame.AddrFrame.Offset = CR.Rbp;
	StackFrame.AddrStack.Offset = CR.Rsp;
#else
	int nMachineType = IMAGE_FILE_MACHINE_I386;
	StackFrame.AddrPC.Offset = CR.Eip;
	StackFrame.AddrFrame.Offset = CR.Ebp;
	StackFrame.AddrStack.Offset = CR.Esp;
#endif
	StackFrame.AddrPC.Mode = AddrModeFlat;
	StackFrame.AddrFrame.Mode = AddrModeFlat;
	StackFrame.AddrStack.Mode = AddrModeFlat;

	SYMBOL_INFO Symbol;
	Symbol.MaxNameLen = 255;
	Symbol.SizeOfStruct = sizeof(SYMBOL_INFO);

	DWORD64 liAddr = 0;
	while (StackWalk64(
			nMachineType,
			GetCurrentProcess(),
			GetCurrentThread(),
			&StackFrame,
			&CR,
			NULL, &
			SymFunctionTableAccess64,
			&SymGetModuleBase64,
			NULL
		))
	{
		liAddr = 0;
		if (SymFromAddr(hProcess, (DWORD64)StackFrame.AddrPC.Offset, &liAddr, &Symbol))
		{
			IMAGEHLP_MODULE64 ModuleInfo;
			memset(&ModuleInfo, 0, sizeof(IMAGEHLP_MODULE64));
			ModuleInfo.SizeOfStruct = sizeof(ModuleInfo);
			if (::SymGetModuleInfo64(hProcess, Symbol.ModBase, &ModuleInfo))
				fprintf(pFile, "%s: ", ModuleInfo.ModuleName);

			fprintf(pFile, "%s + 0x%llX\n", Symbol.Name, liAddr);
		}
	}
}

long __stdcall WvsUnhandledExceptionFilter(PEXCEPTION_POINTERS pExcp)
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
	DumpStackTrace(pExcp, fp);
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