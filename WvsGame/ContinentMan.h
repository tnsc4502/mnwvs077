#pragma once
#include <vector>
#include <mutex>
#include <string>
class AsyncScheduler;

class ContinentMan
{
public:
	struct ContiMov
	{
		enum ContiState
		{
			e_Conti_OnWaitingClock = 0,
			e_Conti_OnBoarding = 1,
			e_Conti_OnTakingOff = 2,
			e_Conti_OnMoving = 3,
			e_Conti_OnMobSummoned = 4,
			e_Conti_OnMobDestroyed = 5,
			e_Conti_OnArriving = 6,
		};

		//Conti State
		int m_nState = 0;
		bool m_bEvent = false, m_bEventDoing = false;

		//Conti Settings
		int m_nFieldIDStartShipMove = 0,
			m_nFieldIDEndShipMove = 0,
			m_nFieldIDMove = 0,
			m_nFieldIDWait = 0,
			m_nFieldIDCabin = 0,
			m_nFieldIDEnd = 0;

		long long int m_tTerm = 0,
			m_tMobGen = 0,
			m_tBoarding = 0,
			m_tWait = 0,
			m_tEvent = 0,
			m_tRequired = 0,
			m_tEventEnd = 0;

		//Mob Gen
		int m_nMobItemID = 0,
			m_nMobPosition_x = 0,
			m_nMobPosition_y = 0;

		//Reactor
		std::string m_sReactorName;
		int m_nStateOnStart = 0,
			m_nStateOnEnd = 0;

		void ResetEvent();
		void Load(void *pImg);
		int GetState();
		void SummonMob();
		void DestroyMob();
	};

private:
	ContinentMan();
	~ContinentMan();

	AsyncScheduler* m_pTimer;
	std::vector<ContiMov> m_aContiMove;
	std::recursive_mutex m_mtxLock;
public:

	static ContinentMan* GetInstance();
	void Init();
	void Update();
	void SendContiPacket(int nFieldID, int nTarget, int nFlag);
	void MoveField(int nFieldFrom, int nFieldTo);
	void SetReactorState(int nFieldID, const std::string& sName, int nState);
	int FindContiMove(int nFieldID);
	int GetInfo(int nFieldID, int nFlag);
};

