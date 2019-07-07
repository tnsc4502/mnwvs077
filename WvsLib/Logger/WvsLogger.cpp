#include <cstdarg>
#include <thread>
#include "WvsLogger.h"
#include "boost\lockfree\queue.hpp"
#include "..\String\StringUtility.h"
#include "..\Memory\ZMemory.h"

#ifdef _WIN32
#include <Windows.h>
#endif

std::mutex WvsLogger::m_mtxConsoleGuard;
std::condition_variable WvsLogger::m_cv;

boost::lockfree::queue<WvsLogger::WvsLogData*> g_qMsgQueue(WvsLogger::MAX_MSG_QUEUE_CAPACITY);
WvsLogger* WvsLogger::pInstnace = AllocObj( WvsLogger );

void WvsLogger::StartMonitoring()
{
#ifdef _WIN32
	auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	WvsLogData *pData;
	while (1)
	{
		std::unique_lock<std::mutex> lk(m_mtxConsoleGuard);
		if (g_qMsgQueue.empty())
			m_cv.wait(lk);
		if (g_qMsgQueue.pop(pData))
		{
#ifdef _WIN32
			SetConsoleTextAttribute(hConsole, pData->m_nConsoleColor);
#endif
			fputs(pData->m_strData.c_str(), stdout);
			FreeObj(pData);
		}
	}
}

WvsLogger::WvsLogger()
{
	setbuf(stdout, NULL);
	g_qMsgQueue.reserve(MAX_MSG_QUEUE_CAPACITY);
	new std::thread(&WvsLogger::StartMonitoring);
}

WvsLogger::~WvsLogger()
{
}

void WvsLogger::PushLogImpl(int nConsoleColor, const std::string & strLog)
{
	WvsLogData* pLogData = AllocObj(WvsLogData);
	pLogData->m_nConsoleColor = nConsoleColor;
	pLogData->m_strData.assign(strLog);
	g_qMsgQueue.push(pLogData);
	m_cv.notify_all();
}

void WvsLogger::LogRaw(int nConsoleColor, const char *sFormat)
{
	pInstnace->PushLogImpl(nConsoleColor, sFormat);
}

void WvsLogger::LogRaw(const char *sFormat)
{
	pInstnace->PushLogImpl(LEVEL_NORMAL, sFormat);
}

void WvsLogger::LogFormat(const char *sFormat, ...)
{
	ZUniquePtr<char[]> sFormatted;
	STRUTILITY_PARSE_VAR_LIST(sFormat, sFormatted);
	pInstnace->PushLogImpl(LEVEL_NORMAL, (char*)sFormatted);
}

void WvsLogger::LogFormat(int nConsoleColor, const char *sFormat, ...)
{
	ZUniquePtr<char[]> sFormatted;
	STRUTILITY_PARSE_VAR_LIST(sFormat, sFormatted);
	pInstnace->PushLogImpl(nConsoleColor, (char*)sFormatted);
}

WvsLogger::WvsLogData::WvsLogData()
{
	time_t rawtime;
	struct tm * timeinfo;
	char aBuffer[64];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(aBuffer, sizeof(aBuffer), "%d-%m-%Y %I:%M:%S", timeinfo);
	m_strDateTime.assign(aBuffer);
}
