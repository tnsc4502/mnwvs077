#pragma once
#include <vector>
#include <map>
#include <list>
#include <string>
#include <atomic>
#include <mutex>
#include "..\WvsLib\Memory\ZMemory.h"

class Field;
class Reactor;
class User;
class InPacket;
class Npc;

class ReactorPool
{
public:
	struct ReactorGen
	{
		static std::atomic<int> stGenID;
		int nX = 0,
			nY = 0,
			tRegenInterval = 0,
			nTemplateID = 0,
			nReactorCount = 0;

		unsigned int tRegenAfter = 0;
		bool bFlip = false, bForceGen = false;
		std::string sName;
	};

private:
	const static int UPDATE_INTERVAL = 7000;

	Field* m_pField;
	std::vector<ReactorGen> m_aReactorGen;
	std::map<int, ZUniquePtr<Reactor>> m_mReactor;
	std::map<std::string, int> m_mReactorName;
	std::list<Npc*> m_lNpc;

	bool m_bReactorHitEnable = false;
	int m_nReactorTotalHit = 0;
	unsigned int m_tLastCreateReactorTime;

	std::recursive_mutex m_mtxReactorPoolMutex;
public:
	ReactorPool();
	~ReactorPool();

	void Init(Field* pField, void* pImg);
	void TryCreateReactor(bool bReset);
	void CreateReactor(ReactorGen *pPrg);
	void OnEnter(User *pUser);
	void OnPacket(User *pUser, int nType, InPacket *iPacket);
	void OnHit(User *pUser, InPacket *iPacket);
	int GetState(const std::string& sName);
	void SetState(const std::string& sName, int nState);
	void RemoveAllReactor();
	void RemoveReactor(Reactor *pReactor, bool bForce = false);
	void RegisterNpc(Npc* pNpc);
	void RemoveNpc();
	std::recursive_mutex& GetLock();
	void Update(unsigned int tCur);
	void Reset(bool bShuffle);

	//Guild FieldSet
	int GetReactorTotalHit() const;
	void SetReactorTotalHit(int nHit);
	bool IsReactorHitEnabled() const;
	void SetReactorHitEnable(bool bEnable);
};

