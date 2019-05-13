#include "CommandManager.h"
#include "SkillInfo.h"
#include "SkillEntry.h"
#include "ItemInfo.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_CharacterLevel.h"
#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_CharacterStat.h"
#include "User.h"
#include "USkill.h"
#include "Field.h"
#include "Reward.h"
#include "Summoned.h"
#include "Mob.h"
#include "MobTemplate.h"
#include "LifePool.h"
#include "DropPool.h"
#include <vector>
#include "..\WvsLib\String\StringUtility.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "ScriptMan.h"
#include "GuildMan.h"

#undef GetJob
#undef SetJob
#include "QWUser.h"

typedef CommandManager::PARAM_TYPE PARAM_TYPE;

int GetInt(PARAM_TYPE aInput, int nIdx, int nDefault = -1)
{
	if (nIdx >= aInput.size())
		return nDefault;

	return atoi(aInput[nIdx].c_str());
}

const std::string& Get(PARAM_TYPE aInput, int nIdx, const std::string& sDefault = "")
{
	if (nIdx >= aInput.size())
		return sDefault;

	return (aInput[nIdx]);
}

int CmdFuncItem(User *pUser, PARAM_TYPE aInput)
{
	int nItemID = GetInt(aInput, 1);
	if (nItemID / 1000000 >= 1)
	{
		auto pNewItem = ItemInfo::GetInstance()->GetItemSlot(nItemID, ItemInfo::ItemVariationOption::ITEMVARIATION_NORMAL);
		if (pNewItem)
		{
			if (pNewItem->nType != GW_ItemSlotBase::GW_ItemSlotType::EQUIP
				&& pNewItem->nType != GW_ItemSlotBase::GW_ItemSlotType::CASH)
				((GW_ItemSlotBundle*)pNewItem)->nNumber = GetInt(aInput, 1, 1);

			//pNewItem->liExpireDate = GameDateTime::GetDateExpireFromPeriod(1);
			Reward reward;
			reward.SetType(1);
			reward.SetItem(pNewItem);
			pUser->GetField()->GetDropPool()->Create(
				&reward,
				0,
				0,
				0,
				0,
				pUser->GetPosX(),
				pUser->GetPosY(),
				pUser->GetPosX(),
				pUser->GetPosY(),
				0,
				1,
				0,
				0);
		}
	}
	return 1;
}

int CmdFuncMaxSkill(User *pUser, PARAM_TYPE aInput)
{
	int nJob = pUser->GetCharacterData()->mStat->nJob;
	int nLevel = nJob % 10;
	while (nLevel >= 0)
	{
		auto pSkills = SkillInfo::GetInstance()->GetSkillsByRootID(nJob - (2 - nLevel));
		for (const auto& pSkill : *pSkills)
		{
			auto pEntry = pSkill.second;
			auto pMaxLevelData = pEntry->GetLevelData(pEntry->GetMaxLevel());
			USkill::OnSkillUpRequest(
				pUser,
				pEntry->GetSkillID(),
				pEntry->GetMaxLevel(),
				false,
				false);
		}
		--nLevel;
	}

	//Beginner
	auto pSkills = SkillInfo::GetInstance()->GetSkillsByRootID((nJob / 100) * 100);
	for (const auto& pSkill : *pSkills)
	{
		auto pEntry = pSkill.second;
		auto pMaxLevelData = pEntry->GetLevelData(pEntry->GetMaxLevel());
		USkill::OnSkillUpRequest(
			pUser,
			pEntry->GetSkillID(),
			pEntry->GetMaxLevel(),
			false,
			false);
	}
	return 1;
}

int CmdFuncTransfer(User *pUser, PARAM_TYPE aInput)
{
	int nFieldID = GetInt(aInput, 1, 100000000);
	pUser->TryTransferField(
		nFieldID,
		""
	);
	return 1;
}

int CmdFuncMob(User *pUser, PARAM_TYPE aInput)
{
	int nTemplateID = GetInt(aInput, 1, 100100);

	Mob mob;
	mob.SetPosX(pUser->GetPosX());
	mob.SetPosY(pUser->GetPosY());
	mob.SetTemplateID(nTemplateID);
	mob.SetMobTemplate(MobTemplate::GetMobTemplate(mob.GetTemplateID()));
	pUser->GetField()->GetLifePool()->CreateMob(
		mob,
		pUser->GetPosX(),
		pUser->GetPosY(),
		pUser->GetFh(),
		0,
		-2,
		0,
		0,
		0,
		nullptr
	);

	return 1;
}


CommandManager::CommandManager()
{
	m_mCmdInvoke["#item"] = CmdFuncItem;
	m_mCmdInvoke["#maxskill"] = CmdFuncMaxSkill;
	m_mCmdInvoke["#transfer"] = CmdFuncTransfer;
	m_mCmdInvoke["#mob"] = CmdFuncMob;

	m_mCmdInvoke["#cg"] = (
		[](User*pUser, PARAM_TYPE aInput)->int
	{
		GuildMan::MemberData memberData;
		memberData.Set(
			pUser->GetName(),
			QWUser::GetLevel(pUser),
			QWUser::GetJob(pUser),
			true
		);
		GuildMan::GetInstance()->OnCreateNewGuildRequest(
			pUser,
			"Hello",
			{
				{ pUser->GetUserID(), memberData }
			}
		);

		return 1;
	});

	m_mCmdInvoke["#dg"] = (
		[](User*pUser, PARAM_TYPE aInput)->int
	{
		GuildMan::GetInstance()->OnRemoveGuildRequest(
			pUser
		);
		return 1;
	});

	m_mCmdInvoke["#pos"] = (
		[](User*pUser, PARAM_TYPE aInput)->int
	{
		pUser->SendChatMessage(0, "Position:(" + std::to_string(pUser->GetPosX()) + ", " + std::to_string(pUser->GetPosY()) + ")");
		return 1;
	});

	m_mCmdInvoke["#script"] = (
		[](User*pUser, PARAM_TYPE aInput)->int
	{			
			auto pScript = ScriptMan::GetInstance()->GetScript(
				"./DataSrv/Script/Npc/script_test.lua",
				2100,
				pUser->GetField()
			);
			if (pScript == nullptr)
			{
				pUser->SendChatMessage(0, "Tset Npc : script_test.lua has no script.");
				return 1;
			}
			pScript->SetUser(pUser);
			pUser->SetScript(pScript);

			pScript->Run(); 
			return 1;
	});
}

CommandManager::~CommandManager()
{
}

CommandManager * CommandManager::GetInstance()
{
	static CommandManager* pInstance = new CommandManager;
	return pInstance;
}

void CommandManager::Process(User * pUser, const std::string & input)
{
	std::vector<std::string> token;
	StringUtility::Split(input, token, " ");

	if (token.size() >= 1 && token[0][0] == '#')
	{
		auto pFunc = m_mCmdInvoke.find(token[0]);
		if (pFunc != m_mCmdInvoke.end())
			pFunc->second(pUser, token);
	}
}
