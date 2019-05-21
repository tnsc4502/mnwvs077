#include "ReactorEventManager.h"
#include "ReactorTemplate.h"
#include "Reactor.h"
#include <functional>
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Task\AsyncScheduler.h"


ReactorEventManager::ReactorEventManager()
{
	m_mtxEventIDCounter = 1;
	m_pTimer = AsyncScheduler::CreateTask(std::bind((&ReactorEventManager::Update), this), 200, true);
	m_pTimer->Start();
}

ReactorEventManager::~ReactorEventManager()
{
}

void ReactorEventManager::Update()
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxTimerLock);
	if (m_mEvent.size() == 0)
		return;

	int tCur = GameDateTime::GetTime();
	auto iter = m_mEvent.begin();
	std::vector<int> aRemoveKey;
	for(auto iter : m_mEvent)
	{
		if (tCur - iter.second.tEventTime > 0)
		{
			iter.second.pReactor->OnTime(iter.second.nEventSN);
			aRemoveKey.push_back(iter.first);
		}
	}
	for (auto nKey : aRemoveKey)
		m_mEvent.erase(nKey);
}

ReactorEventManager* ReactorEventManager::GetInstance()
{
	static ReactorEventManager* pInstance = new ReactorEventManager;
	return pInstance;
}

int ReactorEventManager::RegisterEvent()
{
	return ++m_mtxEventIDCounter;
}

void ReactorEventManager::SetEvent(Reactor * pReactor, int tTime, int nEventSN)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxTimerLock);
	EventInfo info;
	info.pReactor = pReactor;
	info.tEventTime = tTime;
	info.nEventSN = nEventSN;
	m_mEvent.insert({ nEventSN, info });
}

void ReactorEventManager::Cancel(int nEventSN)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxTimerLock);
	m_mEvent.erase(nEventSN);
}
