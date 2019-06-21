#include "FieldSetEventManager.h"
#include "FieldSet.h"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\DateTime\GameDateTime.h"

FieldSetEventManager::FieldSetEventManager()
{
	m_uEventCounter = 100;
	m_pTimer = AsyncScheduler::CreateTask(
		std::bind(&FieldSetEventManager::Update, this),
		500,
		true
	);
	m_pTimer->Start();
}

FieldSetEventManager::~FieldSetEventManager()
{
	FreeObj(m_pTimer);
}

void FieldSetEventManager::Update()
{
	auto tCur = GameDateTime::GetTime();
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	for (auto& prFieldSetInfo : m_maEvent)
		for (auto iter = prFieldSetInfo.second.begin(); iter != prFieldSetInfo.second.end();) 
		{
			if (iter->uTime < tCur)
			{
				prFieldSetInfo.first->OnTime(iter->uEventSN);
				iter = prFieldSetInfo.second.erase(iter);
			}
			else
				++iter;
		}
}

FieldSetEventManager *FieldSetEventManager::GetInstance()
{
	static FieldSetEventManager *pInstance = new FieldSetEventManager;
	return pInstance;
}

unsigned int FieldSetEventManager::RegisterEvent(FieldSet *pFieldSet, unsigned int tDelay)
{
	unsigned int uEventKey = ++m_uEventCounter;
	std::lock_guard<std::recursive_mutex> lock(m_mtxLock);
	EventInfo eventInfo;
	eventInfo.uTime = GameDateTime::GetTime() + tDelay;
	eventInfo.uEventSN = uEventKey;
	m_maEvent[pFieldSet].push_back(eventInfo);
	return uEventKey;
}
