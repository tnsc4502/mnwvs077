#include "MobTemplate.h"
#include "Reward.h"
#include "User.h"
#include "QuestMan.h"
#include "QWUQuestRecord.h"
#include "SkillInfo.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

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

MobTemplate* MobTemplate::GetMobTemplate(int dwTemplateID)
{
	auto iter = m_MobTemplates->find(dwTemplateID);
	if (iter != m_MobTemplates->end())
		return (*m_MobTemplates)[dwTemplateID];

	if (iter == m_MobTemplates->end()) 
	{
		RegisterMob(dwTemplateID); 
		iter = m_MobTemplates->find(dwTemplateID);
	}
	return iter == m_MobTemplates->end() ? nullptr : iter->second;
}

void MobTemplate::RegisterMob(int dwTemplateID)
{
#undef max
	m_MobWzProperty = &(stWzResMan->GetWz(Wz::Mob));
	std::string templateID = std::to_string(dwTemplateID);
	auto empty = WZ::Node();

	while (templateID.length() < 7)
		templateID = "0" + templateID;
	auto& mobNode = (*m_MobWzProperty)[templateID];
	if (mobNode == WZ::Node())
		return;

	auto& info = mobNode["info"];
	auto pTemplate = AllocObj(MobTemplate);
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
	pTemplate->m_nFlySpeed = info["flySpeed"];
	pTemplate->m_nSummonType = info["summonType"];
	pTemplate->m_nEXP = info["exp"];
	pTemplate->m_nGetCP = info["getCP"];
	pTemplate->m_nCategory = info["category"];
	pTemplate->m_strElemAttr = info["elemAttr"];
	GetElementAttribute(pTemplate->m_strElemAttr.c_str(), pTemplate->m_aDamagedElemAttr);
	pTemplate->m_strMobType = info["mobType"];
	pTemplate->m_nFixedDamage = info["fixedDamage"];
	pTemplate->m_bInvincible = ((int)info["invincible"] == 1);
	pTemplate->m_bIsExplosiveDrop = ((int)info["explosiveReward"] == 1);
	pTemplate->m_bOnlyNormalAttack = ((int)info["onlyNormalAttack"] == 1);
	pTemplate->m_bIsBoss = ((int)info["boss"] == 1);
	pTemplate->m_nTemplateID = dwTemplateID;
	auto& reviveNode = info["revive"];
	for (auto& revive : reviveNode)
		pTemplate->m_aReviveTemplateID.push_back((int)revive);

	auto& skillNode = info["skill"];
	for (auto& skill : skillNode)
		pTemplate->m_aMobSkill.push_back({ skill["skill"], skill["level"] });
	pTemplate->MakeSkillContext();
	for (int i = 1; ; ++i)
	{
		auto& atkNode = mobNode["attack" + std::to_string(i)];
		if (atkNode == empty || atkNode.Name() == "")
			break;
		pTemplate->m_aAttackInfo.push_back({});
		auto& attackInfo = pTemplate->m_aAttackInfo.back();
		attackInfo.nType = atkNode["type"];
		attackInfo.nConMP = atkNode["conMP"];
		attackInfo.nDisease = atkNode["disease"];
		attackInfo.nSkillLevel = atkNode["level"];
		attackInfo.nMPBurn = atkNode["mpBurn"];
		attackInfo.nPAD = std::max(pTemplate->m_nPAD, (int)atkNode["PADamage"]);
		attackInfo.bMagicAttack = ((int)atkNode["magic"] == 1);
		attackInfo.bKnockBack = ((int)atkNode["knockBack"] == 1);
		attackInfo.bDeadlyAttack = ((int)atkNode["deadlyAttack"] == 1);
		attackInfo.bDoFirst = ((int)atkNode["doFirst"] == 1);
		if (attackInfo.bMagicAttack)
			GetMagicAttackElementAttribute(
				((std::string)atkNode["elemAttr"]).c_str(),
				&(attackInfo.nMagicElemAttr)
			);
	}

	bool bFly = (info["fly"] != empty);
	bool bMove = (info["move"] != empty);
	bool bJump = (info["jump"] != empty);
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

int MobTemplate::GetElementAttribute(const std::string& s, int *aElemAttr)
{
	/*
	The return value seems dispensable.
	*/
	int nSize = (int)s.size(), nLevel = 0;
	char cAttr = 0, nAttr = 0;

	/*
	"s" should be consisted of pairs that each pair contains one alphabetic and at least one numeral digit.
	While at least one numberal digit is anticipated, giving no digit is legal, and nLevel will be ZERO.
	*/
	if (!nSize)
		return true;
	
	for (int i = 0; i < nSize; )
	{
		nLevel = 0;
		cAttr = s[i++];

		/* Parse numeral digit(s) into nLevel. */
		while (i < nSize && s[i] >= '0' && s[i] <= '9')
			(nLevel *= 10) += (s[i++] - '0');

		/* Parse element attribute character. Where P and U are unknown meaning. (P = Physical ? ( = 0))*/
		nAttr = SkillInfo::GetElementAttribute(cAttr);
		if (nAttr || (cAttr == 'p' || cAttr == 'P'))
			aElemAttr[nAttr] = nLevel;
	}
	return true;
}

int MobTemplate::GetMagicAttackElementAttribute(const char * s, int * nElemAttr)
{
	return SkillInfo::GetElementAttribute(s, nElemAttr);
}

void MobTemplate::MakeSkillContext()
{
	int nMobSkillCount = (int)m_aMobSkill.size();
	m_aSkillContext.resize(nMobSkillCount);
	for (int i = 0; i < nMobSkillCount; ++i)
	{
		m_aSkillContext[i].nSkillID = m_aMobSkill[i].first;
		m_aSkillContext[i].nSLV = m_aMobSkill[i].second;
		m_aSkillContext[i].tLastSkillUse = 1;
		m_aSkillContext[i].nSummoned = 0;
	}
}

int MobTemplate::GetSkillIndex(int nSkillID, int nSLV)
{
	int nCount = (int)m_aMobSkill.size();
	for (int i = 0; i < nCount; ++i)
		if (m_aMobSkill[i].first == nSkillID &&
			m_aMobSkill[i].second == nSLV)
			return i;
	return -1;
}

void MobTemplate::SetMobCountQuestInfo(User * pUser) const
{
	auto& aQuestKey = QuestMan::GetInstance()->GetQuestByMob(m_nTemplateID);
	for (auto& nKey : aQuestKey)
		QWUQuestRecord::SetMobRecord(pUser, nKey, m_nTemplateID);
}
