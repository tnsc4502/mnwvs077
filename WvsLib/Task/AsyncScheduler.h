#pragma once
#include <agents.h>
#include <ppltasks.h>
#include <functional>
#include "..\Memory\MemoryPoolMan.hpp"

class AsyncScheduler
{
public:
	unsigned int m_nTimePeriod; //How frequent the timer "tick" (ms unit).
	bool m_bRepeat, m_bStarted, m_bTaskDone = false;

	Concurrency::timer<int> *m_TimerInstance = nullptr;
	Concurrency::call<int> *m_TaskCall = nullptr;

public:
	AsyncScheduler(unsigned int nTimeInMs, bool bRepeat) :
		m_nTimePeriod(nTimeInMs),
		m_bRepeat(bRepeat),
		m_bStarted(false)
	{}

	~AsyncScheduler()
	{
		FreeObj( m_TimerInstance );
		FreeObj( m_TaskCall );
	}

	void Start()
	{
		if (!m_bRepeat && m_bTaskDone)
		{
			m_TimerInstance = AllocObjCtor(concurrency::timer<int>)(
				m_nTimePeriod,
				0,
				m_TaskCall,
				m_bRepeat);
		}
		m_bTaskDone = false;
		m_bStarted = true;
		m_TimerInstance->start();
	}

	void Pause()
	{
		m_bTaskDone = true;
		m_bStarted = false;
		m_TimerInstance->pause();
	}

	void Abort()
	{
		if (m_TimerInstance)
		{
			m_TimerInstance->stop();
			m_bStarted = false;
		}
	}

	bool IsStarted() const
	{
		return m_bStarted;
	}

	bool IsTaskDone() const
	{
		return m_bTaskDone;
	}

	void OnTick()
	{
		m_bTaskDone = true;
		if (!m_bRepeat)
		{
			m_TimerInstance->stop();
			FreeObj( m_TimerInstance );
		}
	}

	template<typename FUNC_TYPE>
	static AsyncScheduler* CreateTask(FUNC_TYPE fTask, unsigned int timeInMs, bool repeat)
	{
		auto pInstance = AllocObjCtor(AsyncScheduler)(timeInMs, repeat);

		auto pTaskCall = AllocObjCtor(concurrency::call<int>)(
			[&, pInstance, fTask](int)
		{
			fTask();
			pInstance->OnTick();
		});
		pInstance->m_TaskCall = pTaskCall;

		pInstance->m_TimerInstance = AllocObjCtor(concurrency::timer<int>)(
			pInstance->m_nTimePeriod,
			0,
			pTaskCall,
			pInstance->m_bRepeat);
		return pInstance;
	}
};
