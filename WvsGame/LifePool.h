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

class LifePool
{
	const static int MAX_WINDOW_VIEW_X = 1280, 
		MAX_WINDOW_VIEW_Y = 1024, 
		MAX_MOB_GEN = 40,
		REGEN_PERIOD = 2500; //If the monster is killed, how long it should be re-generated

	struct MobGen
	{
		Mob mob;
		int nRegenAfter = 0, nRegenInterval = 0, nMobCount = 0;
	};

	std::recursive_mutex m_lifePoolMutex;
	std::atomic<int> atomicObjectCounter = 0x1000;

	//在這個地圖中所有可能的NPC物件
	std::vector<Npc> m_lNpc;

	//在這個地圖中所有可能的怪物物件
	std::vector<MobGen*> m_aMobGen;

	//在這個地圖中真正產生出的怪物, key是Object ID
	std::map<int, Mob*> m_mMob;

	//在這個地圖中真正產生出的Npc, key是Object ID
	std::map<int, Npc*> m_mNpc;

	/* 地圖中有關玩家控制權的紀錄，使用std::map 不須像正服創建 Max Heap 與 Min Heap
	   Key是 控制數量 Value : Controller是代表控制者的實體指標
	   User必須保存map的iterator 以便進行快速刪除 [ std::map的iterator不會因為容器本身新增刪除而無效 ]
	*/
	std::multimap<int, Controller*> m_hCtrl;

	/*
	當地圖中無任何玩家可作為Controller，將所有怪物控制權轉交給Null Controller，方便管理
	*/
	Controller* m_pCtrlNull;

	//紀錄User對應到的m_hCtrl iterator
	std::map<int, decltype(m_hCtrl)::iterator> m_mController;

	//設定物件在地圖中的基本屬性
	void SetFieldObjAttribute(FieldObj* pFieldObj, WZ::Node& dataNode);

	//場域有關的屬性
	int m_nMobCapacityMin, m_nMobCapacityMax, m_aInitMobGenCount = 0, m_nSubMobCount = -1, m_nMobDamagedByMobState = 0;

	bool m_bMobGenEnable = true;

	Field* m_pField;

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

	void CreateMob(int nTemplateID, int nX, int nY, int nFh, int bNoDropPriority, int nType, unsigned int dwOption, int bLeft, int nMobType, Controller* pOwner);
	void CreateMob(const Mob&, int nX, int nY, int nFh, int bNoDropPriority, int nType, unsigned int dwOption, int bLeft, int nMobType, Controller* pOwner);
	void RemoveNpc(Npc *pNpc);
	void RemoveMob(Mob* pMob);
	void RemoveAllMob(bool bExceptMobDamagedByMob);
	void Reset();
	int GetMobCount() const;

	void Init(Field* pField, int nFieldID);
	void OnEnter(User* pUser);

	void InsertController(User* pUser);
	void RemoveController(User* pUser);
	void UpdateCtrlHeap(Controller* pController);
	bool GiveUpMobController(Controller* pController);
	bool OnMobSummonItemUseRequest(int nX, int nY, MobSummonItem* pInfo, bool bNoDropPriority);
	std::vector<Mob*> FindAffectedMobInRect(FieldRect& rc, Mob* pExcept);

	void RedistributeLife();
	void Update();
	void OnPacket(User* pUser, int nType, InPacket* iPacket);
	void OnUserAttack(User *pUser, const SkillEntry *pSkill, AttackInfo *pInfo);
	void EncodeAttackInfo(User * pUser, AttackInfo *pInfo, OutPacket *oPacket);
	std::recursive_mutex& GetLock();
	Npc* GetNpc(int nFieldObjID);
	Mob* GetMob(int nFieldObjID);
	void UpdateMobSplit(User* pUser);
};

