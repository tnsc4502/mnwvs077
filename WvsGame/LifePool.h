#pragma once
#include <map>
#include "Npc.h"
#include "Mob.h"
#include <atomic>
#include <mutex>
#include "..\WvsLib\Wz\WzResMan.hpp"

struct AttackInfo;
struct MobSummonItem;
struct FieldRect;
class User;
class Field;
class Controller;
class InPacket;
class SkillEntry;
class Drop;
class Employee;
class MiniRoomBase;

class LifePool
{
public:
	const static int MAX_WINDOW_VIEW_X = 1280, 
		MAX_WINDOW_VIEW_Y = 1024, 
		MAX_MOB_GEN = 40,
		REGEN_PERIOD = 2500; //If the monster is killed, how long it should be re-generated

	struct MobGen
	{
		Mob mob;
		int nRegenAfter = 0, 
			nRegenInterval = 0, 
			nTeamForMCarnival = -1,
			nMobCount = 0;
	};

private:
	std::atomic<int> atomicObjectCounter = 0x1000;
	std::recursive_mutex m_lifePoolMutex;

	std::vector<Npc> m_lNpc;
	std::vector<MobGen*> m_aMobGen, m_aMCMobGen;
	std::map<int, Mob*> m_mMob;
	std::map<int, Npc*> m_mNpc;
	std::map<int, Employee*> m_mEmployee;
	std::multimap<int, Controller*> m_hCtrl;
	Controller* m_pCtrlNull;
	std::map<int, decltype(m_hCtrl)::iterator> m_mController;

	int m_nMobCapacityMin, 
		m_nMobCapacityMax, 
		m_aInitMobGenCount = 0, 
		m_nSubMobCount = -1, 
		m_nMobDamagedByMobState = 0;

	bool m_bMobGenEnable = true;
	Field* m_pField;

	void SetFieldObjAttribute(FieldObj* pFieldObj, WZ::Node& dataNode);
	void OnMobPacket(User* pUser, int nType, InPacket* iPacket);
	void OnNpcPacket(User* pUser, int nType, InPacket* iPacket);
public:
	LifePool();
	~LifePool();

	void SetMaxMobCapacity(int max);
	int GetMaxMobCapacity() const;
	void LoadNpcData(WZ::Node& dataNode);
	void LoadMobData(WZ::Node& dataNode);
	Npc* CreateNpc(int nTemplateID, int nX, int nY, int nFh);
	Npc* CreateNpc(const Npc&);
	void TryCreateMob(bool bReset);

	void SetMobGenEnable(bool bEnable);
	void InsertMCMobGen(MobGen* pGen);
	void CreateMob(int nTemplateID, int nX, int nY, int nFh, int bNoDropPriority, int nType, unsigned int dwOption, int bLeft, int nMobType, Controller* pOwner);
	void CreateMob(const Mob&, int nX, int nY, int nFh, int bNoDropPriority, int nType, unsigned int dwOption, int bLeft, int nMobType, Controller* pOwner);
	void RemoveNpc(Npc *pNpc);
	void RemoveMob(Mob* pMob);
	void RemoveAllMob(bool bExceptMobDamagedByMob);
	void Reset();
	int GetMobCount() const;
	Employee* CreateEmployee(int x, int y, int nEmployerID, const std::string& sEmployerName, MiniRoomBase* pMiniRoom, int nFh);
	void RemoveEmployee(int nEmployerID);
	void Init(Field* pField, int nFieldID);
	void OnEnter(User* pUser);
	void InsertController(User* pUser);
	void RemoveController(User* pUser);
	void UpdateCtrlHeap(Controller* pController);
	bool ChangeMobController(int nCharacterID, Mob *pMobWanted, bool bChase);
	bool GiveUpMobController(Controller* pController);
	bool OnMobSummonItemUseRequest(int nX, int nY, MobSummonItem* pInfo, bool bNoDropPriority);
	void MobStatChangeByGuardian(int nTeam, int nSkillID, int nSLV);
	void MobStatResetByGuardian(int nTeam, int nSkillID, int nSLV);
	std::vector<Mob*> FindAffectedMobInRect(FieldRect& rc, Mob* pExcept);

	void RedistributeLife();
	void Update();
	void OnPacket(User* pUser, int nType, InPacket* iPacket);
	void OnUserAttack(User *pUser, const SkillEntry *pSkill, AttackInfo *pInfo);
	void OnMobStatChangeSkill(User *pUser, int nMobID, const SkillEntry *pSkill, int nSLV, int tDelay);
	void EncodeAttackInfo(User * pUser, AttackInfo *pInfo, OutPacket *oPacket);
	std::recursive_mutex& GetLock();
	Npc* GetNpc(int nFieldObjID);
	Mob* GetMob(int nFieldObjID);
	void UpdateMobSplit(User* pUser);
};

