#include "MobTemplate.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "Reward.h"
#include "User.h"
#include "QuestMan.h"
#include "QWUQuestRecord.h"

std::map<int, MobTemplate*>* MobTemplate::m_MobTemplates = new std::map<int, MobTemplate*>();

WZ::Node* MobTemplate::m_MobWzProperty = &(stWzResMan->GetWz(Wz::Mob));

MobTemplate::MobTemplate()
{
}

MobTemplate::~MobTemplate()
{
}

const std::vector<RewardInfo*>& MobTemplate::GetMobReward()
{
	return *m_paMobReward;
}

//cloneNewOne為true，代表不使用共用屬性 (用於複寫特定怪物屬性)
MobTemplate* MobTemplate::GetMobTemplate(int dwTemplateID, bool bCloneNewOne)
{
	auto iter = m_MobTemplates->find(dwTemplateID);
	if (!bCloneNewOne &&  iter != m_MobTemplates->end())
		return (*m_MobTemplates)[dwTemplateID];

	if (iter == m_MobTemplates->end()) {
		RegisterMob(dwTemplateID); 
		iter = m_MobTemplates->find(dwTemplateID);
	}
	if (!bCloneNewOne) 
	{
		MobTemplate* clone = AllocObj(MobTemplate);
		*clone = *(iter->second);
		return clone;
	}
	return iter->second;
}

void MobTemplate::RegisterMob(int dwTemplateID)
{
	m_MobWzProperty = &(stWzResMan->GetWz(Wz::Mob));
	auto pTemplate = AllocObj(MobTemplate);
	std::string templateID = std::to_string(dwTemplateID);
	while (templateID.length() < 7)
		templateID = "0" + templateID;
	auto& info = (*m_MobWzProperty)[templateID]["info"];
	pTemplate->m_nLevel = info["level"];
	pTemplate->m_bIsBodyAttack = ((int)info["bodyAttack"] == 1);
	pTemplate->m_bIsChangeableMob = ((int)info["changeableMob"] == 1);
	pTemplate->m_bIsDamagedByMob = ((int)info["damagedByMob"] == 1);
	pTemplate->m_bIsSmartPhase = ((int)info["smartPhase"] == 1);
	pTemplate->m_lnMaxHP = (int)info["maxHP"];
	pTemplate->m_lnMaxMP = (int)info["maxMP"];
	pTemplate->m_nSpeed = info["speed"];
	pTemplate->m_nPAD = info["PADamage"];
	pTemplate->m_nPDD = info["PDDamage"];
	pTemplate->m_nPDRate = info["PDRate"];
	pTemplate->m_nMAD = info["MADamage"];
	pTemplate->m_nMDD = info["MDDamage"];
	pTemplate->m_nMDRate = info["MDRate"];
	pTemplate->m_nACC = info["acc"];
	pTemplate->m_nEVA = info["eva"];
	pTemplate->m_nPushed = info["pushed"];
	pTemplate->m_dFs = info["fs"];
	pTemplate->m_nSummonType = info["summonType"];
	pTemplate->m_nEXP = info["exp"];
	pTemplate->m_nCategory = info["category"];
	pTemplate->m_strElemAttr = info["elemAttr"];
	pTemplate->m_strMobType = info["mobType"];
	pTemplate->m_bIsExplosiveDrop = ((int)info["explosiveReward"] == 1);
	pTemplate->m_nTemplateID = dwTemplateID;

	auto& skillNode = info["skill"];
	for (auto& skill : skillNode)
		pTemplate->m_aMobSkill.push_back({ skill["skill"], skill["level"] });

	bool bFly = (info["fly"] == info.end());
	bool bMove = (info["move"] == info.end());
	bool bJump = (info["jump"] == info.end());
	if (bFly)
		pTemplate->m_nMoveAbility = 3;
	else if (bJump)
		pTemplate->m_nMoveAbility = 2;
	else
		pTemplate->m_nMoveAbility = (bMove != false) ? 1 : 0;

	pTemplate->m_paMobReward = Reward::GetMobReward(dwTemplateID);
	for (auto& pInfo : (*pTemplate->m_paMobReward))
		pTemplate->m_unTotalRewardProb += pInfo->m_unWeight;

	(*m_MobTemplates)[dwTemplateID] = pTemplate;
}

void MobTemplate::SetMobCountQuestInfo(User * pUser) const
{
	auto& aQuestKey = QuestMan::GetInstance()->GetQuestByMob(m_nTemplateID);
	for (auto& nKey : aQuestKey)
		QWUQuestRecord::SetMobRecord(pUser, nKey, m_nTemplateID);
}
