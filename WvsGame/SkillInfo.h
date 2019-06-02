#pragma once
#include <map>
#include <mutex>
#include <atomic>

struct GA_Character;
struct GW_SkillRecord;
class SkillEntry;
class MobSkillEntry;
struct MCSkillEntry;
struct MCGuardianEntry;

class SkillInfo
{
	//How many root img should be processed parallelly
	int m_nRootCount = 0;
	std::atomic<int> m_nOnLoadingSkills;
	std::mutex m_mtxSkillResLock;
	std::map<int, std::map<int, SkillEntry*> *> m_mSkillByRootID;
	std::map<int, MobSkillEntry*> m_mMobSKill;
	std::map<int, MCSkillEntry*> m_mMCSkill;
	std::map<int, MCGuardianEntry*> m_mMCGuardian;

public:
	SkillInfo();
	~SkillInfo();

	const std::map<int, std::map<int, SkillEntry*> *>& GetSkills() const;
	const std::map<int, SkillEntry*> * GetSkillsByRootID(int nRootID) const;
	const SkillEntry* GetSkillByID(int nSkillID) const;
	MobSkillEntry* GetMobSkill(int nMobSkillID) const;
	MCSkillEntry* GetMCRandomSkill() const;
	MCSkillEntry* GetMCSkill(int nIndex) const;
	MCGuardianEntry* GetMCGuardian(int nIndex) const;

	static SkillInfo* GetInstance();
	static bool IsValidRootName(const std::string& sName);
	int GetBundleItemMaxPerSlot(int nItemID, GA_Character* pCharacterData);
	int GetElementAttribute(const char *s, int *nElemAttr);
	int GetAmplification(GA_Character* pCharacter, int nJob, int nSkillID, int *pnIncMPCon);
	void LoadMobSkill();
	void LoadMobSkillLeveData(MobSkillEntry* pEntry, void *pData);
	void LoadMCSkill();
	void LoadMCGuardian();
	void IterateSkillInfo();
	void LoadSkillRoot(int nSkillRootID, void* pData);
	SkillEntry* LoadSkill(int nSkillRootID, int nSkillID, void* pData);
	void LoadLevelDataByLevelNode(int nSkillID, SkillEntry* pEntry, void* pData);
	int GetSkillLevel(GA_Character* pCharacter, int nSkillID, SkillEntry** pEntry, int bNoPvPLevelCheck, int bPureStealSLV, int bNotApplySteal, int bCheckSkillRoot);
	GW_SkillRecord* GetSkillRecord(int nSkillID, int nSLV, long long int tExpired);
	static bool IsSummonSkill(int nSkillID);
	int GetMasteryFromSkill(GA_Character* pCharacter, int nSkillID, SkillEntry *pEntry, int *pnInc);
};

