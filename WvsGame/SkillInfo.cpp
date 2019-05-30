
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Random\Rand32.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_SkillRecord.h"
#include "ItemInfo.h"
#include "SkillEntry.h"
#include "SkillLevelData.h"
#include "MobSkillEntry.h"
#include "MobSkillLevelData.h"
#include "SkillInfo.h"
#include "AdminSkills.h"
#include "BeginnersSkills.h"
#include "BowmanSkills.h"
#include "MagicSkills.h"
#include "ManagerSkills.h"
#include "PirateSkills.h"
#include "RogueSkills.h"
#include "WarriorSkills.h"
#include "MCSkillEntry.h"
#include "MCGuardianEntry.h"

#include <thread>
#include <unordered_map>


#define CHECK_SKILL_ATTRIBUTE(var, attribute) if(attributeSet.find(#attribute) != attributeSetEnd) (mappingTable[(&(var)) - pAttributeBase]=(std::string)skillCommonImg[#attribute]);
#define PARSE_SKILLDATA(attribute) ((std::string)skillCommonImg[#attribute]) == "" ? 0 : ((int)skillCommonImg[#attribute]);

SkillInfo::SkillInfo()
{
	m_nOnLoadingSkills = 0;
}


SkillInfo::~SkillInfo()
{
}

/*int SkillInfo::GetLoadingSkillCount() const
{
	return m_nOnLoadingSkills;
}*/

const std::map<int, std::map<int, SkillEntry*>*>& SkillInfo::GetSkills() const
{
	return m_mSkillByRootID;
}

const std::map<int, SkillEntry*>* SkillInfo::GetSkillsByRootID(int nRootID) const
{
	auto findIter = m_mSkillByRootID.find(nRootID);
	if (findIter == m_mSkillByRootID.end())
		return nullptr;

	return findIter->second;
}

const SkillEntry * SkillInfo::GetSkillByID(int nSkillID) const
{
	int nJobID = nSkillID / 10000;
	auto rootData = GetSkillsByRootID(nJobID);
	if (!rootData)
		return nullptr;

	auto findResult = rootData->find(nSkillID);
	return (findResult != rootData->end() ? findResult->second : nullptr);
}

MobSkillEntry* SkillInfo::GetMobSkill(int nMobSkillID) const
{
	auto findResult = m_mMobSKill.find(nMobSkillID);
	return (findResult != m_mMobSKill.end() ? findResult->second : nullptr);
}

MCSkillEntry* SkillInfo::GetMCRandomSkill() const
{
	int nRnd = (int)(Rand32::GetInstance()->Random() % m_mMCSkill.size());
	auto iter = m_mMCSkill.begin();
	for (int i = 0; i < (int)m_mMCSkill.size(); ++i)
		if (nRnd == i)
			return iter->second;
		else
			++iter;
	return nullptr;
}

MCSkillEntry* SkillInfo::GetMCSkill(int nIndex) const
{
	auto findResult = m_mMCSkill.find(nIndex);
	return (findResult != m_mMCSkill.end() ? findResult->second : nullptr);
}

MCGuardianEntry* SkillInfo::GetMCGuardian(int nIndex) const
{
	auto findResult = m_mMCGuardian.find(nIndex);
	return (findResult != m_mMCGuardian.end() ? findResult->second : nullptr);
}

SkillInfo * SkillInfo::GetInstance()
{
	static SkillInfo* pInstance = new SkillInfo;
	return pInstance;
}

bool SkillInfo::IsValidRootName(const std::string & sName)
{
	for (char c : sName)
		if (!isdigit(c))
			return false;
	return true;
}

int SkillInfo::GetBundleItemMaxPerSlot(int nItemID, GA_Character * pCharacterData)
{
	auto pItem = ItemInfo::GetInstance()->GetBundleItem(nItemID);
	if (pItem != nullptr)
	{
		int result = pItem->nMaxPerSlot;
		if (pCharacterData != nullptr &&  nItemID / 10000 == 207)
		{
			//精準暗器
		}
		return result == 0 ? 100 : result;
	}
	return 0;
}

void SkillInfo::LoadMobSkill()
{
	std::set<std::string> sNode;
	auto& mobSkillNode = stWzResMan->GetWz(Wz::Skill)["MobSkill"];
	MobSkillEntry* pEntry = nullptr;
	for (auto& mobSkill : mobSkillNode)
	{
		pEntry = AllocObj(MobSkillEntry);
		LoadMobSkillLeveData(pEntry, &(mobSkill["level"]));
		m_mMobSKill[atoi(mobSkill.Name().c_str())] = pEntry;
	}
}

void SkillInfo::LoadMobSkillLeveData(MobSkillEntry *pEntry, void * pData)
{
#undef max

	auto& skillLevelImg = *((WZ::Node*)pData);
	pEntry->m_apLevelData.push_back(nullptr); //for level 0
	MobSkillLevelData *pLevel = nullptr;
	auto empty = WZ::Node();

	for (auto& level : skillLevelImg)
	{
		pLevel = AllocObj(MobSkillLevelData);
		for (int i = 0; ; ++i)
		{
			auto& mobIDNode = level[std::to_string(i)];
			if (mobIDNode == empty || mobIDNode.Name() == "")
				break;
			pLevel->anTemplateID.push_back((int)mobIDNode);
		}
		pLevel->nX = level["x"];
		pLevel->nY = level["y"];
		pLevel->nLimit = level["limit"];
		pLevel->nSummonEffect = level["summonEffect"];
		pLevel->nHPBelow = level["hp"];
		pLevel->nMPCon = level["mpCon"];
		pLevel->tInterval = ((int)level["interval"]) * 1000;
		pLevel->tTime = ((int)level["time"]) * 1000;
		pLevel->nProp = level["prop"];
		pLevel->nCount = std::max(1, (int)level["count"]);
		
		auto&lt = level["lt"];
		if (lt != empty && lt.Name() != "")
		{
			pLevel->rcAffectedArea.left = lt["x"];
			pLevel->rcAffectedArea.top = lt["y"];
		}
		auto&rb = level["rb"];
		if (rb != empty && rb.Name() != "")
		{
			pLevel->rcAffectedArea.right = rb["x"];
			pLevel->rcAffectedArea.bottom = rb["y"];
		}

		pEntry->m_apLevelData.push_back(pLevel);
	}
}

void SkillInfo::LoadMCSkill()
{
	auto& mcSkillNode = stWzResMan->GetWz(Wz::Skill)["MCSkill"];
	for (auto& node : mcSkillNode)
	{
		auto pEntry = AllocObj(MCSkillEntry);
		pEntry->nLevel = node["level"];
		pEntry->nMobSkillID = node["mobSkillID"];
		pEntry->nSpendCP = node["spendCP"];
		pEntry->nTarget = node["target"];
		pEntry->sDesc = (std::string)node["desc"];
		pEntry->sName = (std::string)node["name"];

		m_mMCSkill.insert({ atoi(node.Name().c_str()), pEntry });
	}
}

void SkillInfo::LoadMCGuardian()
{
	auto& mcGNode = stWzResMan->GetWz(Wz::Skill)["MCGuardian"];
	for (auto& node : mcGNode)
	{
		auto pEntry = AllocObj(MCGuardianEntry);
		pEntry->nLevel = node["level"];
		pEntry->nMobSkillID = node["mobSkillID"];
		pEntry->nSpendCP = node["spendCP"];
		pEntry->nType = node["type"];
		pEntry->sDesc = (std::string)node["desc"];
		pEntry->sName = (std::string)node["name"];

		m_mMCGuardian.insert({ atoi(node.Name().c_str()), pEntry });
	}
}

void SkillInfo::IterateSkillInfo()
{
	WvsLogger::LogRaw("[SkillInfo::IterateSkillInfo]開始載入所有技能資訊 IterateSkillInfo Start.\n");
	static auto& skillWz = stWzResMan->GetWz(Wz::Skill);
	bool continued = false;
	int nRootID;
	static std::vector<std::pair<int, WZ::Node>> aRoot;
	for (auto& node : skillWz)
	{
		if (!IsValidRootName(node.Name()))
			continue;
		aRoot.push_back({ atoi(node.Name().c_str()), node["skill"] });
	}
	m_nRootCount = (int)aRoot.size();
	for (auto& node : aRoot)
	{
		nRootID = node.first;
		std::thread t(&SkillInfo::LoadSkillRoot, this, nRootID, (void*)(&node.second));
		t.detach();
	}
}

void SkillInfo::LoadSkillRoot(int nSkillRootID, void * pData)
{
	auto skillRootIter = m_mSkillByRootID.find(nSkillRootID);
	if (skillRootIter == m_mSkillByRootID.end()) 
		m_mSkillByRootID.insert({nSkillRootID, new std::map<int, SkillEntry*>() });
	auto& skillRootImg = *((WZ::Node*)pData);
	int nSkillID = 0;
	for (auto& skillImg : skillRootImg)
	{
		++m_nOnLoadingSkills;
		nSkillID = atoi(skillImg.Name().c_str());
		LoadSkill(nSkillRootID, nSkillID, (void*)&skillImg);
		//std::thread t(&SkillInfo::LoadSkill, this, nSkillRootID, nSkillID, (void*)&skillImg);
		//t.detach();
		--m_nOnLoadingSkills;
	}
	if (m_nOnLoadingSkills == 0 && m_mSkillByRootID.size() >= m_nRootCount)
	{
		WvsLogger::LogRaw("[SkillInfo::IterateSkillInfo]技能資訊載入完畢 IterateSkillInfo End.\n");
		//stWzResMan->ReleaseMemory();
		//system("pause");
		//auto pSkill = GetSkillByID(2211010)->GetLevelData(5);
		//printf("[SkillInfo::IterateSkillInfo]技能資訊載入完畢 IterateSkillInfo End 2 %d.\n", pSkill->m_nMpCon);
		/*for (auto& p : m_mSkillByRootID)
		{
			for (auto& pp : *(p.second))
			{
				//if(!pp.second->GetLevelData(1))
					printf("ID: %d Total : %d\n", pp.second->GetSkillID(), (int)pp.second->GetAllLevelData().size());
			}
		}*/
		/*int nTest = GetSkillByID(1000)->GetMaxLevel();
		printf("Test 2211010 : %d %d %d %d %d %d\n", 
			nTest,
			GetSkillByID(1000)->GetLevelData(3)->m_nMpCon,
			GetSkillByID(1000)->GetLevelData(3)->m_nDamage,
			GetSkillByID(1000)->GetLevelData(3)->m_nMobCount,
			GetSkillByID(1000)->GetLevelData(3)->m_nAttackCount,
			GetSkillByID(1000)->GetLevelData(3)->m_nTime);*/
	}
}

SkillEntry * SkillInfo::LoadSkill(int nSkillRootID, int nSkillID, void * pData)
{
	bool bLevelStructure = false;
	auto& skillDataImg = *((WZ::Node*)pData);
	auto& skillCommonImg = skillDataImg["level"];
	bLevelStructure = true;
	/*if (skillCommonImg == skillDataImg.end()) //部分初心者技能
	{
		bLevelStructure = true;
		skillCommonImg = skillDataImg["level"];
	}
	else
	{
		WvsLogger::LogFormat("Not bLevelStructure Found : %d\n", nSkillID);
	}*/
	auto& skillInfoImg = skillDataImg["info"];
	auto& skillListImg = skillDataImg["skillList"];
	auto& skillReqImg = skillDataImg["req"];
	SkillEntry* pResult = new SkillEntry;
	pResult->SetSkillID(nSkillID);
	pResult->SetMasterLevel(atoi(((std::string)skillDataImg["masterLevel"]).c_str()));
	pResult->SetMaxLevel(atoi(((std::string)skillCommonImg["maxLevel"]).c_str()));
	if(bLevelStructure)
		LoadLevelDataByLevelNode(nSkillID, pResult, (void*)&skillCommonImg);
	
	std::lock_guard<std::mutex> lock(m_mtxSkillResLock);
	m_mSkillByRootID[nSkillRootID]->insert({ nSkillID, pResult });
	return pResult;
}

void SkillInfo::LoadLevelDataByLevelNode(int nSkillID, SkillEntry * pEntry, void * pData)
{
	auto& skillLevelImg = *((WZ::Node*)pData);
	WZ::Node empty;
	int nMaxLevel = pEntry->GetMaxLevel();
	pEntry->AddLevelData(nullptr); //for lvl 0

	double d = 0;
	for (auto& skillCommonImg : skillLevelImg)
	{
		d = atof(skillCommonImg.Name().c_str());

		SkillLevelData* pLevelData = AllocObj(SkillLevelData);
		pLevelData->m_nAcc = PARSE_SKILLDATA(acc);
		pLevelData->m_nCraft = PARSE_SKILLDATA(craft);
		pLevelData->m_nAttackCount = PARSE_SKILLDATA(attackCount);
		pLevelData->m_nBulletConsume = PARSE_SKILLDATA(bulletConsume);
		pLevelData->m_nBulletCount = PARSE_SKILLDATA(bulletCount);
		pLevelData->m_nCooltime = PARSE_SKILLDATA(cooltime);
		pLevelData->m_nDamage = PARSE_SKILLDATA(damage);
		pLevelData->m_nEva = PARSE_SKILLDATA(eva);
		pLevelData->m_nHp = PARSE_SKILLDATA(hp);
		pLevelData->m_nHpCon = PARSE_SKILLDATA(hpCon);
		pLevelData->m_nItemCon = PARSE_SKILLDATA(itemCon);
		pLevelData->m_nItemConNo = PARSE_SKILLDATA(itemConNo);
		pLevelData->m_nJump = PARSE_SKILLDATA(jump);
		pLevelData->m_nMastery = PARSE_SKILLDATA(mastery);
		pLevelData->m_nMobCount = PARSE_SKILLDATA(mobCount);
		pLevelData->m_nMorph = PARSE_SKILLDATA(morph);
		pLevelData->m_nMp = PARSE_SKILLDATA(mp);
		pLevelData->m_nMpCon = PARSE_SKILLDATA(mpCon);
		pLevelData->m_nPad = PARSE_SKILLDATA(pad);
		pLevelData->m_nPdd = PARSE_SKILLDATA(pdd);
		pLevelData->m_nMad = PARSE_SKILLDATA(mad);
		pLevelData->m_nMdd = PARSE_SKILLDATA(mdd);
		pLevelData->m_nProp = PARSE_SKILLDATA(prop);
		pLevelData->m_nRange = PARSE_SKILLDATA(range);
		pLevelData->m_nSpeed = PARSE_SKILLDATA(speed);
		pLevelData->m_nTime = PARSE_SKILLDATA(time);
		pLevelData->m_nTime *= 1000;

		pLevelData->m_nX = PARSE_SKILLDATA(x);
		pLevelData->m_nY = PARSE_SKILLDATA(y);
		pLevelData->m_nZ = PARSE_SKILLDATA(z);
		pLevelData->m_nHpR = PARSE_SKILLDATA(hpR);
		pLevelData->m_nMpR = PARSE_SKILLDATA(mpR);
		pLevelData->m_nMoneyCon = PARSE_SKILLDATA(moneyCon);
		pLevelData->m_nLt = PARSE_SKILLDATA(lt);
		pLevelData->m_nRb = PARSE_SKILLDATA(rb);
		pLevelData->m_nFixDamage = PARSE_SKILLDATA(fixdamage);
		auto&lt = skillCommonImg["lt"];
		if (lt != empty && lt.Name() != "")
		{
			pLevelData->m_rc.left = lt["x"];
			pLevelData->m_rc.top = lt["y"];
		}
		auto&rb = skillCommonImg["rb"];
		if (rb != empty && rb.Name() != "")
		{
			pLevelData->m_rc.right = rb["x"];
			pLevelData->m_rc.bottom = rb["y"];
		}
		pEntry->AddLevelData(pLevelData);
	}
	pEntry->SetMaxLevel((int)pEntry->GetAllLevelData().size() - 1);
}

int SkillInfo::GetSkillLevel(GA_Character * pCharacter, int nSkillID, SkillEntry ** pEntry, int bNoPvPLevelCheck, int bPureStealSLV, int bNotApplySteal, int bCheckSkillRoot)
{
	auto pSkillTreeByJob = m_mSkillByRootID.find(nSkillID / 10000);
	if (pSkillTreeByJob == m_mSkillByRootID.end() || pSkillTreeByJob->second == nullptr)
		return 0;
	auto findIter = pSkillTreeByJob->second->find(nSkillID);
	if (findIter == pSkillTreeByJob->second->end() || findIter->second == nullptr)
		return 0;

	if(pEntry)
		*pEntry = findIter->second;
	auto characterSkillRecord = pCharacter->GetSkill(nSkillID);
	if (characterSkillRecord == nullptr)
		return 0;
	return characterSkillRecord->nSLV;
}

GW_SkillRecord * SkillInfo::GetSkillRecord(int nSkillID, int nSLV, long long int tExpired)
{
	auto pSkill = GetSkillByID(nSkillID);
	if (pSkill == nullptr)
		return nullptr;
	GW_SkillRecord* pRecord = AllocObj( GW_SkillRecord );

	pRecord->nSkillID = nSkillID;
	pRecord->nSLV = nSLV;
	pRecord->nMasterLevel = pSkill->GetMasterLevel();
	pRecord->tExpired = tExpired;
	return pRecord;
}

bool SkillInfo::IsSummonSkill(int nSkillID)
{
	switch (nSkillID)
	{
		case PirateSkills::AllAboard_521:
		case PirateSkills::AllAboard_521_2:
		case PirateSkills::AllAboard_521_3:
		case PirateSkills::AllAboard_521_4:
		case PirateSkills::AllAboard_521_5:
		case PirateSkills::AllAboard_521_6:
		case PirateSkills::AllAboard_521_7: //召喚船員
		case PirateSkills::Gaviota_521: //海鷗突擊隊
		case PirateSkills::TurretDeployment_571: //破城砲
		case PirateSkills::MonkeyMilitia_532_2: //雙胞胎猴子
		case MagicSkills::Ifrit_212: //召喚火魔
		case MagicSkills::Elquines_222: //召喚冰魔
		case MagicSkills::Bahamut_232: //聖龍精通
		case BowmanSkills::GoldenEagle_320: //金鷹召喚
		case BowmanSkills::Freezer_321: //召喚銀隼
		case BowmanSkills::SilverHawk_310: //銀鷹召喚
		case BowmanSkills::Phoenix_311: //召喚鳳凰
		case RogueSkills::DarkFlare_411: //絕殺領域
		case RogueSkills::DarkFlare_421_2: //絕殺領域
			return true;
	}
	return false;
}
