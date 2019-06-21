#pragma once
#include <map>
#include <vector>
#include <mutex>
#include <atomic>

class FieldSet;
class AsyncScheduler;

class FieldSetEventManager
{
public:
	struct EventInfo
	{
		unsigned int uEventSN;
		unsigned int uTime;
	};

private:
	std::recursive_mutex m_mtxLock;
	std::atomic<unsigned int> m_uEventCounter;
	AsyncScheduler *m_pTimer;
	FieldSetEventManager();
	~FieldSetEventManager();
	std::map<FieldSet*, std::vector<EventInfo>> m_maEvent;
	void Update();

public:

	static FieldSetEventManager* GetInstance();
	unsigned int RegisterEvent(FieldSet* pFieldSet, unsigned int tDelay);
};

