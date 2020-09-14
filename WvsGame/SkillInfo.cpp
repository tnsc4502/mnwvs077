
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Random\Rand32.h"
#include "..\WvsLib\Common\WvsGameConstants.hpp"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterLevel.h"
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
#include "ThiefSkills.h"
#include "WarriorSkills.h"
#include "MCSkillEntry.h"
#include "MCGuardianEntry.h"
#include "..\WvsLib\String\StringPool.h"

#include <thread>
#include <unordered_map>

auto t1 = std::chrono::high_resolution_clock::now();

#define CHECK_SKILL_ATTRIBUTE(var, attribute) if(attributeSet.find(#attribute) != attributeSetEnd) (mappingTable[(&(var)) - pAttributeBase]=(std::string)skillCommonImg[#attribute]);
#define PARSE_SKILLDATA(attribute) ((int)skillCommonImg[#attribute]);

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
			//ºë·Ç·t¾¹
		}
		return result == 0 ? 100 : result;
	}
	return 0;
}

int SkillInfo::GetElementAttribute(const char cAttr)
{
	switch (cAttr)
	{
		case 'i':
		case 'I':
			return ElementAttribute::e_ElemAttr_Ice;
		case 'f':
		case 'F':
			return ElementAttribute::e_ElemAttr_Fire;
		case 'l':
		case 'L':
			return ElementAttribute::e_ElemAttr_Lightning;
		case 's':
		case 'S':
			return ElementAttribute::e_ElemAttr_Poison;
		case 'h':
		case 'H':
			return ElementAttribute::e_ElemAttr_Holy;
		case 'd':
		case 'D':
			return ElementAttribute::e_ElemAttr_Dark;
		case 'u':
		case 'U':
			return ElementAttribute::e_ElemAttr_U;
		default:
			return 0;
	}
}

int SkillInfo::GetElementAttribute(const char *s, int *nElemAttr)
{
	/*
	105 = i, 73 = I result = 1 (Ice)
	102 = f, 70 = F result = 2 (Fire)
	108 = l, 76 = L result = 3 (Lightning)
	115 = s, 83 = S result = 4 (Poison)
	104 = h, 72 = H result = 5 (Holy)
	100 = d, 68 = D result = 6 (Dark)
	117 = u, 85 = U result = 7
	*/

	*nElemAttr = GetElementAttribute(*s); 
	return *nElemAttr ? true : false;
}

int SkillInfo::GetAmplification(GA_Character *pCharacter, int nJob, int nSkillID, int *pnIncMPCon)
{
	SkillEntry *pEntry = nullptr;
	const SkillLevelData *pLevel = nullptr;
	int nSLV = 0;
	if (WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 211))
	{
		nSLV = GetSkillLevel(pCharacter, 2110001, &pEntry);
		if (nSLV)
			pLevel = pEntry->GetLevelData(nSLV);
	}
	if (WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 221))
	{
		nSLV = GetSkillLevel(pCharacter, 2210001, &pEntry);
		if (nSLV)
			pLevel = pEntry->GetLevelData(nSLV);
	}
	bool bCheck = false;
	int nRet = 100;
	if (pnIncMPCon)
		*pnIncMPCon = 0;
	if (pLevel)
	{
		if (pnIncMPCon)
		{
			if (nSkillID <= 2121007)
			{
				if (nSkillID < 2121006)
				{
					if (nSkillID <= 2111006)
					{
						nRet = pLevel->m_nY;
						if (
							!(nSkillID != 2111006
								&& (nSkillID < 2001004 || nSkillID > 2001005 && (nSkillID <= 2101003 || nSkillID > 2101005 && (nSkillID <= 2111001 || nSkillID > 2111003)))
								)
							)
							*pnIncMPCon = pLevel->m_nX;
					}
					if (nSkillID != 2121001)
					{
						nRet = pLevel->m_nY;
						if (nSkillID == 2121003)
							*pnIncMPCon = pLevel->m_nX;
					}
				}
				else
				{
					*pnIncMPCon = pLevel->m_nX;
					nRet = pLevel->m_nY;
				}
			}
			if (nSkillID > 2221001)
			{
				nRet = pLevel->m_nY;
				if (!(nSkillID != 2221003 && (nSkillID <= 2221005 || nSkillID > 2221007)))
					*pnIncMPCon = pLevel->m_nX;
			}
			if (nSkillID == 2221001)
			{
				*pnIncMPCon = pLevel->m_nX;
				nRet = pLevel->m_nY;
			}
			if (nSkillID >= 2201004)
			{
				if (nSkillID <= 2201005)
				{
					*pnIncMPCon = pLevel->m_nX;
					nRet = pLevel->m_nY;
				}
				if (nSkillID > 2211001)
				{
					if (nSkillID > 2211003)
					{
						nRet = pLevel->m_nY;
						if (nSkillID == 2211006)
							*pnIncMPCon = pLevel->m_nX;
					}
					else
					{
						*pnIncMPCon = pLevel->m_nX;
						nRet = pLevel->m_nY;
					}
				}
			}
		}
		else
			nRet = pLevel->m_nY;

		/*switch (nSkillID)
		{
			case 2001004:
			case 2001005:

			case 2101004:
			case 2101005:
			case 2111002:
			case 2111003:
			case 2111006:
			case 2121001:
			case 2121003:
			case 2121006:
			case 2121007:

			case 2201004:
			case 2201005:
			case 2211002:
			case 2211003:
			case 2211006:
			case 2221001:
			case 2221003:
			case 2221006:
			case 2221007:
				if(pnIncMPCon)
					*pnIncMPCon = pLevel->m_nX;
				break;
		}
		nRet = pLevel->m_nY;*/
	}
	return nRet;
}

int SkillInfo::GetResistance(int nSLV, int nJob, int nElement)
{
	if (nElement <= 0 || nElement >= 8 || !nSLV)
		return 0;

	if (WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 131))
		return GetSkillByID(1310000)->GetLevelData(nSLV)->m_nX;
	if (WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 211)) 
	{
		if(nElement == 2 || nElement == 4)
			return GetSkillByID(2110000)->GetLevelData(nSLV)->m_nX;
	}

	if (!WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 221)) 
	{
		if (WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 231))
			return GetSkillByID(2310000)->GetLevelData(nSLV)->m_nX;
		return 0;
	}

	if(nElement == 1 || nElement == 3)
		return GetSkillByID(2210000)->GetLevelData(nSLV)->m_nX;
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
		m_mMobSKill[atoi(mobSkill.GetName().c_str())] = pEntry;
	}
}

void SkillInfo::LoadMobSkillLeveData(MobSkillEntry *pEntry, void * pData)
{
#undef max

	auto& skillLevelImg = *((WzIterator*)pData);
	pEntry->m_apLevelData.push_back(nullptr); //for level 0
	MobSkillLevelData *pLevel = nullptr;
	auto empty = skillLevelImg.end();

	for (auto& level : skillLevelImg)
	{
		pLevel = AllocObj(MobSkillLevelData);
		for (int i = 0; ; ++i)
		{
			auto& mobIDNode = level[std::to_string(i)];
			if (mobIDNode == empty || mobIDNode.GetName() == "")
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
		if (lt != empty && lt.GetName() != "")
		{
			pLevel->rcAffectedArea.left = lt["x"];
			pLevel->rcAffectedArea.top = lt["y"];
		}
		auto&rb = level["rb"];
		if (rb != empty && rb.GetName() != "")
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

		m_mMCSkill.insert({ atoi(node.GetName().c_str()), pEntry });
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

		m_mMCGuardian.insert({ atoi(node.GetName().c_str()), pEntry });
	}
}

void SkillInfo::IterateSkillInfo()
{
	t1 = std::chrono::high_resolution_clock::now();
	WvsLogger::LogRaw("[SkillInfo::IterateSkillInfo<IterateSkillInfo>]On iterating all skills....\n");
	static auto& skillWz = stWzResMan->GetWz(Wz::Skill);
	bool continued = false;
	int nRootID;
	static std::vector<std::pair<int, std::string>> aRoot;
	auto& aChildName = skillWz.EnumerateChildName();
	for (auto& sName : aChildName)
	{
		if (!IsValidRootName(sName))
			continue;

		++m_nOnLoadingSkills;
		aRoot.push_back({ atoi(sName.c_str()), sName });
	}

	m_nRootCount = (int)aRoot.size();
	for (auto& node : aRoot)
	{
		nRootID = node.first;
		std::thread t(&SkillInfo::LoadSkillRoot, this, nRootID, (node.second));
		t.detach();
	}
}

void SkillInfo::LoadSkillRoot(int nSkillRootID, const std::string& sName)
{
	static auto& skillWz = stWzResMan->GetWz(Wz::Skill);
	auto skillRootIter = m_mSkillByRootID.find(nSkillRootID);
	if (skillRootIter == m_mSkillByRootID.end()) 
		m_mSkillByRootID.insert({nSkillRootID, new std::map<int, SkillEntry*>() });
	auto& skillRootImg = skillWz[sName]["skill"];
	int nSkillID = 0;
	for (auto& skillImg : skillRootImg)
	{
		nSkillID = atoi(skillImg.GetName().c_str());
		LoadSkill(nSkillRootID, nSkillID, (void*)&skillImg);
	}
	--m_nOnLoadingSkills;
	if (m_nOnLoadingSkills == 0 && m_mSkillByRootID.size() >= m_nRootCount) 
	{
		auto t2 = std::chrono::high_resolution_clock::now();
		stWzResMan->RemountAll();
		WvsLogger::LogFormat("[SkillInfo::IterateSkillInfo<IterateSkillInfo>]Skill information are completely loaded in %lld us.\n", std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());
	}
}

SkillEntry * SkillInfo::LoadSkill(int nSkillRootID, int nSkillID, void * pData)
{
	int nElemAttr = 0;
	bool bLevelStructure = false;
	auto& skillDataImg = *((WzIterator*)pData);
	std::string sElemAttr = skillDataImg["elemAttr"];
	if (sElemAttr != "")
		GetElementAttribute(sElemAttr.c_str(), &nElemAttr);

	auto& skillCommonImg = skillDataImg["level"];
	bLevelStructure = true;

	auto& skillInfoImg = skillDataImg["info"];
	auto& skillListImg = skillDataImg["skillList"];
	auto& skillReqImg = skillDataImg["req"];
	SkillEntry* pResult = new SkillEntry;
	pResult->SetAttackElemAttr(nElemAttr);
	pResult->SetSkillID(nSkillID);
	pResult->SetMasterLevel(atoi(((std::string)skillDataImg["masterLevel"]).c_str()));
	pResult->SetMaxLevel(atoi(((std::string)skillCommonImg["maxLevel"]).c_str()));
	pResult->SetInvisible((int)skillDataImg["invisible"] == 1 ? true : false);
	if(bLevelStructure)
		LoadLevelDataByLevelNode(nSkillID, pResult, (void*)&skillCommonImg, (void*)&(skillDataImg));
	
	std::lock_guard<std::mutex> lock(m_mtxSkillResLock);
	m_mSkillByRootID[nSkillRootID]->insert({ nSkillID, pResult });
	return pResult;
}

void SkillInfo::LoadLevelDataByLevelNode(int nSkillID, SkillEntry * pEntry, void * pData, void *pRoot)
{
	auto& skillLevelImg = *((WzIterator*)pData);
	auto& skillData = *((WzIterator*)pRoot);
	WzIterator empty = skillData.end();
	int nMaxLevel = pEntry->GetMaxLevel();
	pEntry->AddLevelData(nullptr); //for lvl 0

	double d = 0;
	int nLevel = 0;
	for (auto& skillCommonImg : skillLevelImg)
	{
		//if(nSkillID > 1000000)
		//	printf("IterateLevel[%s] : h%d, onLoading = %s\n", skillCommonImg.GetName().c_str(), ++nLevel, ((std::string)skillCommonImg["hs"]).c_str());
		//d = atof(skillCommonImg.GetName().c_str());

		SkillLevelData* pLevelData = AllocObj(SkillLevelData);
		pLevelData->m_nAcc = PARSE_SKILLDATA(acc);
		pLevelData->m_nCraft = PARSE_SKILLDATA(craft);
		pLevelData->m_nAttackCount = PARSE_SKILLDATA(attackCount);
		pLevelData->m_nBulletConsume = PARSE_SKILLDATA(bulletConsume);
		pLevelData->m_nBulletCount = PARSE_SKILLDATA(bulletCount);
		pLevelData->m_nCooltime = PARSE_SKILLDATA(cooltime);
		pLevelData->m_nCooltime *= 1000;

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
		pLevelData->m_nCriticalDamage = PARSE_SKILLDATA(criticalDamage);
		auto&lt = skillCommonImg["lt"];
		if (lt != empty && lt.GetName() != "")
		{
			pLevelData->m_rc.left = lt["x"];
			pLevelData->m_rc.top = lt["y"];
		}
		auto&rb = skillCommonImg["rb"];
		if (rb != empty && rb.GetName() != "")
		{
			pLevelData->m_rc.right = rb["x"];
			pLevelData->m_rc.bottom = rb["y"];
		}
		if (IsSummonSkill(nSkillID))
			pLevelData->m_nMobCount = skillData["summon"]["attack1"]["info"]["mobCount"];

		pEntry->SetLevelData(pLevelData, atoi(skillCommonImg.GetName().c_str()));
	}
	//pEntry->SetMaxLevel((int)pEntry->GetAllLevelData().size() - 1);
}

int SkillInfo::GetSkillLevel(GA_Character * pCharacter, int nSkillID, SkillEntry ** pEntry)
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
		case WarriorSkills::DarkKnight_Beholder:
		case MagicSkills::Highest_Magic_IL_Ifrit:
		case MagicSkills::Highest_Magic_FP_Elquines:
		case MagicSkills::Highest_Magic_Holy_Bahamut:
		case MagicSkills::Adv_Magic_Holy_SummonDragon:
		case BowmanSkills::Sniper_GoldenEagle:
		case BowmanSkills::Marksman_Freezer:
		case BowmanSkills::Ranger_SilverHawk:
		case BowmanSkills::Bow_Master_Phoenix:
		case BowmanSkills::Sniper_Puppet:
		case BowmanSkills::Ranger_Puppet:
		case PirateSkills::Outlaw_Octopus:
		case PirateSkills::Corsair_WrathOfTheOctopi:
		case PirateSkills::Outlaw_Gaviota:
			return true;
	}
	return false;
}

bool SkillInfo::IsPartyStatChangeSkill(int nSkillID)
{
	switch (nSkillID)
	{
		//BasicStatUp
		case WarriorSkills::Hero_MapleWarrior:
		case WarriorSkills::Paladin_MapleWarrior:
		case WarriorSkills::DarkKnight_MapleWarrior:
		case MagicSkills::Highest_Magic_FP_MapleWarrior:
		case MagicSkills::Highest_Magic_IL_MapleWarrior:
		case MagicSkills::Highest_Magic_Holy_MapleWarrior:
		case BowmanSkills::Bow_Master_MapleWarrior:
		case BowmanSkills::Marksman_MapleWarrior:
		case ThiefSkills::NightsLord_MapleWarrior:
		case ThiefSkills::Shadower_MapleWarrior:
		case PirateSkills::Buccaneer_MapleWarrior:
		case PirateSkills::Corsair_MapleWarrior:

		//SharpEyes
		case BowmanSkills::Bow_Master_SharpEyes:
		case BowmanSkills::Marksman_SharpEyes:

		case WarriorSkills::Fighter_Rage:
		case WarriorSkills::Spearman_IronWill:
		case WarriorSkills::Spearman_HyperBody:
		case MagicSkills::Magic_FP_Meditation:
		case MagicSkills::Magic_IL_Meditation:
		case MagicSkills::Magic_Holy_Heal:
		case MagicSkills::Adv_Magic_Holy_HolySymbol:
		case MagicSkills::Adv_Magic_Holy_Dispel:
		case MagicSkills::Highest_Magic_Holy_HolyShield:
		case MagicSkills::Highest_Magic_Holy_Resurrection:
		case ThiefSkills::Hermit_MesoUp:
		case ThiefSkills::Assassin_Haste:
		case ThiefSkills::Bandit_Haste:
		case PirateSkills::Buccaneer_SpeedInfusion:
		case AdminSkills::Admin_Super_HasteSuper:
		case AdminSkills::Admin_Super_HolySymbol:
			return true;
	}
	return false;
}

bool SkillInfo::IsAdminSkill(int nSkillID)
{
	int nJobRoot = nSkillID / 10000;
	return nJobRoot == 800 || nJobRoot == 900 || nJobRoot == 910;
}

int SkillInfo::GetMasteryFromSkill(GA_Character* pCharacter, int nSkillID, SkillEntry* pEntry, int* pnInc)
{
	int nSLV = GetSkillLevel(pCharacter, nSkillID, &pEntry);
	if (pnInc)
	{
		if (nSLV <= 0)
			*pnInc = 0;
		else
			*pnInc = pEntry->GetLevelData(nSLV)->m_nX;
	}
	if (nSLV)
		return pEntry->GetLevelData(nSLV)->m_nMastery;
	return 0;
}

int SkillInfo::GetEndureDuration(GA_Character *pCharacter)
{
	int nJob = pCharacter->mStat->nJob, nSLV = 0;
	SkillEntry *pEntry = nullptr;
	if (nJob / 100 == 1)
	{
		nSLV = GetSkillLevel(
			pCharacter,
			1000002,
			&pEntry
		);
		if (nSLV && pEntry)
			return pEntry->GetLevelData(nSLV)->m_nTime;
	}
	else if (nJob / 100 == 4)
	{
		if (WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 410))
		{
			nSLV = GetSkillLevel(
				pCharacter,
				4100002,
				&pEntry
			);
			if (nSLV && pEntry)
				return pEntry->GetLevelData(nSLV)->m_nTime;
		}
		else if (WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 420))
		{
			nSLV = GetSkillLevel(
				pCharacter,
				4200001,
				&pEntry
			);
			if (nSLV && pEntry)
				return pEntry->GetLevelData(nSLV)->m_nTime;
		}
	}
	return 0;
}

int SkillInfo::GetHPRecoveryUpgrade(GA_Character * pCharacter)
{
	int nJob = pCharacter->mStat->nJob, nSLV = 0;
	SkillEntry *pEntry = nullptr;
	if (nJob / 100 == 1)
	{
		nSLV = GetSkillLevel(
			pCharacter,
			1000000,
			&pEntry
		);
		if (nSLV && pEntry)
			return pEntry->GetLevelData(nSLV)->m_nHp;
	}
	else if (nJob / 100 == 4)
	{
		if (WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 410))
		{
			nSLV = GetSkillLevel(
				pCharacter,
				4100002,
				&pEntry
			);
			if (nSLV && pEntry)
				return pEntry->GetLevelData(nSLV)->m_nHp;
		}
		else if (WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 420))
		{
			nSLV = GetSkillLevel(
				pCharacter,
				4200001,
				&pEntry
			);
			if (nSLV && pEntry)
				return pEntry->GetLevelData(nSLV)->m_nHp;
		}
	}
	return 0;
}

int SkillInfo::GetMPRecoveryUpgrade(GA_Character * pCharacter)
{
	int nJob = pCharacter->mStat->nJob, nSLV = 0;
	SkillEntry *pEntry = nullptr;
	if(nJob / 100 != 2)
	{
		if (WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 410))
		{
			nSLV = GetSkillLevel(
				pCharacter,
				4100002,
				&pEntry
			);
			if (nSLV && pEntry)
				return pEntry->GetLevelData(nSLV)->m_nMp;
		}
		else if (WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 420))
		{
			nSLV = GetSkillLevel(
				pCharacter,
				4200001,
				&pEntry
			);
			if (nSLV && pEntry)
				return pEntry->GetLevelData(nSLV)->m_nMp;
		}
		else if (WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 111))
		{
			nSLV = GetSkillLevel(
				pCharacter,
				1110000,
				&pEntry
			);
			if (nSLV && pEntry)
				return pEntry->GetLevelData(nSLV)->m_nMp;
		}
		else if (WvsGameConstants::IsCorrectJobForSkillRoot(nJob, 121))
		{
			nSLV = GetSkillLevel(
				pCharacter,
				1210000,
				&pEntry
			);
			if (nSLV && pEntry)
				return pEntry->GetLevelData(nSLV)->m_nMp;
		}
	}
	return
		(int) ((double)GetSkillLevel(
			pCharacter,
			2000000,
			&pEntry
		) * (double)pCharacter->mLevel->nLevel * 0.1);
}
