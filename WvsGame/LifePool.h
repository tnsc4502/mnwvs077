#pragma once
#include <map>
#include "Npc.h"
#include "Mob.h"
#include <atomic>
#include <mutex>
#include "..\WvsLib\Memory\ZMemory.h"

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
class Summoned;
class WzIterator;

class LifePool
{
public:
	const static int 
		LIFE_MAX_MOB_GEN = 40,
		LIFE_REGEN_PERIOD = 3200; //If the monster is killed, how long should it be re-generated

	struct MobGen
	{
		Mob mob;
		int nRegenInterval = 0, 
			nTeamForMCarnival = -1,
			nMobCount = 0;
		unsigned int nRegenAfter = 0;
	};

private:
	std::atomic<int> atomicObjectCounter = 0x1000;
	std::recursive_mutex m_lifePoolMutex;

	std::vector<Npc> m_lNpc;
	std::vector<ZUniquePtr<MobGen>> m_aMobGen, m_aMCMobGen;
	std::map<int, ZSharedPtr<Mob>> m_mMob;
	std::map<int, ZUniquePtr<Npc>> m_mNpc;
	std::map<int, ZUniquePtr<Employee>> m_mEmployee;
	std::multimap<int, Controller*> m_hCtrl;
	Controller* m_pCtrlNull;
	std::map<int, decltype(m_hCtrl)::iterator> m_mController;

	int m_nMobCapacityMin, 
		m_nMobCapacityMax, 
		m_aInitMobGenCount = 0, 
		m_nSubMobCount = 0, 
		m_nMobDamagedByMobState = 0;

	bool m_bMobGenEnable = true;
	Field* m_pField;

	void SetFieldObjAttribute(FieldObj* pFieldObj, WzIterator& dataNode);
	void OnMobPacket(User* pUser, int nType, InPacket* iPacket);
	void OnNpcPacket(User* pUser, int nType, InPacket* iPacket);
public:
	LifePool();
	~LifePool();

	void SetMaxMobCapacity(int max);
	int GetMaxMobCapacity() const;

	//Load
	void LoadNpcData(WzIterator& dataNode);
	void LoadMobData(WzIterator& dataNode);

	//Create & Remove
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

	//Employee
	Employee* CreateEmployee(int x, int y, int nEmployerID, const std::string& sEmployerName, MiniRoomBase* pMiniRoom, int nFh);
	void RemoveEmployee(int nEmployerID);

	//Init&Enter
	void Init(Field* pField, int nFieldID);
	void OnEnter(User* pUser);

	//Controller
	void InsertController(User* pUser);
	void RemoveController(User* pUser);
	void UpdateCtrlHeap(Controller* pController);
	bool ChangeMobController(int nCharacterID, Mob* pMobWanted, bool bChase);
	bool GiveUpMobController(Controller* pController);

	//Commands
	void TryKillingAllMobs(User* pUser);
	bool OnMobSummonItemUseRequest(int nX, int nY, MobSummonItem* pInfo, bool bNoDropPriority);

	//MC
	void MobStatChangeByGuardian(int nTeam, int nSkillID, int nSLV);
	void MobStatResetByGuardian(int nTeam, int nSkillID, int nSLV);

	//Update
	std::vector<ZSharedPtr<Mob>> FindAffectedMobInRect(FieldRect& rc, const ZSharedPtr<Mob>& pExcept);
	void RedistributeLife();
	void Update();
	void UpdateMobSplit(User* pUser);
	void OnPacket(User* pUser, int nType, InPacket* iPacket);

	//User Attack
	void OnUserAttack(User *pUser, const SkillEntry *pSkill, AttackInfo *pInfo);
	void OnUserAttack(User *pUser, int nMobID, int nDamage, const FieldPoint& ptHit, int tDelay, int nBlockingSkillID);
	void OnSummonedAttack(User *pUser, Summoned *pSummoned, const SkillEntry *pSkill, AttackInfo *pInfo);
	void ApplyUserAttack(User *pUser, const SkillEntry *pSkill, AttackInfo *pInfo);
	void OnMobStatChangeSkill(User *pUser, int nMobID, const SkillEntry *pSkill, int nSLV, int tDelay);
	void EncodeAttackInfo(User * pUser, AttackInfo *pInfo, OutPacket *oPacket);

	//Get
	std::recursive_mutex& GetLock();
	const ZUniquePtr<Npc>& GetNpc(int nFieldObjID);
	ZSharedPtr<Mob> GetMob(int nFieldObjID);
	std::vector<ZSharedPtr<Mob>> GetMobByTemplateID(int nTemplateID);
};

