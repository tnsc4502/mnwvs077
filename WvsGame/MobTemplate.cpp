#include "MobTemplate.h"
#include "Reward.h"
#include "User.h"
#include "QuestMan.h"
#include "QWUQuestRecord.h"
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

	bool bFly = (info["fly"] == empty);
	bool bMove = (info["move"] == empty);
	bool bJump = (info["jump"] == empty);
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

int MobTemplate::GetElementAttribute(const char *s, int *aElemAttr)
{
	const char *v2; 
	unsigned __int16 v3;
	unsigned __int16 v4;
	int v5; 
	bool v6;
	int v7; 
	int v8; 

	if (!s)
		return 1;
	v2 = s;
	if (!*s)
		return 1;
	while (1)
	{
		v3 = *v2;
		v4 = v2[1];
		++v2;
		v5 = 0;
		if (!v4)
			goto LABEL_40;
		do
		{
			if (v4 < 0x30u)
				break;
			if (v4 > 0x39u)
				break;
			v5 = v4 + 10 * v5 - 48;
			++v2;
			v4 = *v2;
		} while (*v2);
		if (!v5)
			LABEL_40:
		v5 = 0;
		if ((signed int)v3 > 100)
			break;
		if (v3 == 100)
			goto LABEL_41;
		if ((signed int)v3 > 76)
		{
			v7 = v3 - 80;
			v6 = v3 == 80;
			goto LABEL_25;
		}
		if (v3 == 76)
			goto LABEL_31;
		if (v3 == 68)
		{
		LABEL_41:
			aElemAttr[6] = v5;
			goto LABEL_34;
		}
		if (v3 == 70)
			goto LABEL_33;
		if (v3 == 72)
			goto LABEL_32;
		if (v3 != 73)
			return 0;
	LABEL_17:
		aElemAttr[1] = v5;
	LABEL_34:
		if (!*v2)
			return 1;
	}
	if (v3 == 102)
	{
	LABEL_33:
		aElemAttr[2] = v5;
		goto LABEL_34;
	}
	if (v3 == 104)
	{
	LABEL_32:
		aElemAttr[5] = v5;
		goto LABEL_34;
	}
	if (v3 == 105)
		goto LABEL_17;
	if (v3 == 108)
	{
	LABEL_31:
		aElemAttr[3] = v5;
		goto LABEL_34;
	}
	v7 = v3 - 112;
	v6 = v3 == 112;
LABEL_25:
	if (v6)
	{
		*aElemAttr = v5;
		goto LABEL_34;
	}
	v8 = v7 - 3;
	if (!v8)
	{
		aElemAttr[4] = v5;
		goto LABEL_34;
	}
	if (v8 == 2)
	{
		aElemAttr[7] = v5;
		goto LABEL_34;
	}
	return 0;
}

int MobTemplate::GetMagicAttackElementAttribute(const char * s, int * nElemAttr)
{
	signed int v2; // ecx@1
	int result; // eax@1

	v2 = *s;
	result = 1;
	if (v2 > 83)
	{
		if (v2 == 102)
			goto LABEL_17;
		if (v2 == 105)
		{
		LABEL_16:
			*nElemAttr = 1;
			return result;
		}
		if (v2 == 108)
		{
		LABEL_15:
			*nElemAttr = 3;
			return result;
		}
		if (v2 != 115)
			return 0;
	LABEL_14:
		*nElemAttr = 4;
		return result;
	}
	if (v2 == 83)
		goto LABEL_14;
	if (!*s)
	{
		*nElemAttr = 0;
		return result;
	}
	if (v2 != 70)
	{
		if (v2 != 73)
		{
			if (v2 != 76)
				return 0;
			goto LABEL_15;
		}
		goto LABEL_16;
	}
LABEL_17:
	*nElemAttr = 2;
	return result;
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
