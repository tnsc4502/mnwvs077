#pragma once

#include <map>
#include <mutex>
#include <atomic>

class AsyncScheduler;
class Reactor;

class ReactorEventManager
{
	struct EventInfo
	{
		Reactor *pReactor;
		int tEventTime, nEventSN;
	};
	std::atomic<int> m_mtxEventIDCounter;
	std::recursive_mutex m_mtxTimerLock;
	AsyncScheduler* m_pTimer;
	std::map<int, EventInfo> m_mEvent;

	ReactorEventManager();
	~ReactorEventManager();

public:
	static ReactorEventManager* GetInstance();

	int RegisterEvent();
	void Update();
	void SetEvent(Reactor *pReactor, int tTime, int nEventSN);
	void Cancel(int nEventSN);
};

