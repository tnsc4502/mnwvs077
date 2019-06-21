#include "Field_MonsterCarnival.h"
#include "FieldSet.h"
#include "LifePool.h"
#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\ReactorPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\MobPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\NpcPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\FieldPacketFlags.hpp"
#include "..\WvsLib\Net\InPacket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Random\Rand32.h"
#include "PartyMan.h"
#include "User.h"
#include "Mob.h"
#include "MobTemplate.h"
#include "SkillInfo.h"
#include "MobSkillEntry.h"
#include "MobSkillLevelData.h"
#include "MCSkillEntry.h"
#include "MCGuardianEntry.h"
#include "LifePool.h"
#include "ReactorPool.h"
#include "Reactor.h"
#include "DropPool.h"

#undef max

Field_MonsterCarnival::Field_MonsterCarnival(void *pData, int nFieldID)
	: Field(pData, nFieldID)
{
	auto empty = WZ::Node();
	auto& mc = (*((WZ::Node*)pData))["monsterCarnival"];
	m_nTimeDefault = mc["timeDefault"];
	m_nTimeExpand = mc["timeExpand"];
	m_nTimeMessage = mc["timeMessage"];
	m_nTimeFinish = mc["timeFinish"];
	m_sEffectWin = (std::string)mc["effectWin"];
	m_sEffectLose = (std::string)mc["effectLose"];
	m_sSoundWin = (std::string)mc["soundWin"];
	m_sSoundLose = (std::string)mc["soundLose"];
	m_nRewardMapWin = mc["rewardMapWin"];
	m_nRewardMapLose = mc["rewardMapLose"];
	m_bIsMapDivided = (((int)mc["mapDivided"]) != 0);
	m_nDeathCP = mc["deathCP"];

	auto& mobGenPos = mc["mobGenPos"];
	for (int i = 0; ; ++i)
	{
		auto& node = mobGenPos[std::to_string(i)];
		if (node == empty || node.Name() == "")
			break;
		MobGenPos pos;
		pos.nX = node["x"];
		pos.nY = node["y"];
		pos.nCy = node["cy"];
		pos.nFh = node["fh"];
		if (m_bIsMapDivided)
		{
			pos.nTeam = node["team"];
			m_aMobGenPosDivided[pos.nTeam].push_back(pos);
			m_aMobGenMax[0] = std::max(m_aMobGenMax[0], (int)m_aMobGenPosDivided[0].size());
			m_aMobGenMax[1] = std::max(m_aMobGenMax[1], (int)m_aMobGenPosDivided[1].size());
		}
		else 
		{
			m_aMobGenPosUnited.push_back(pos);
			m_aMobGenMax[0] = (int)m_aMobGenPosUnited.size();
			m_aMobGenMax[1] = m_aMobGenMax[0];
		}
		++m_nMobGenPosCount;
	}

	auto& mob = mc["mob"];
	for (int i = 0; ; ++i)
	{
		auto& node = mob[std::to_string(i)];
		if (node == empty || node.Name() == "")
			break;
		MCMob mob;
		mob.nID = node["id"];
		mob.nSpendCP = node["spendCP"];
		mob.nMobTime = node["mobTime"];
		m_aMob.push_back(mob);
	}
	m_aSummonedMobCount[0].resize(m_aMob.size(), 0);
	m_aSummonedMobCount[1].resize(m_aMob.size(), 0);

	auto& skill = mc["skill"];
	for (int i = 0; ; ++i)
	{
		auto& node = skill[std::to_string(i)];
		if (node == empty || node.Name() == "")
			break;
		m_anSkillID.push_back(node);
	}

	auto& guardianGenPos = mc["guardianGenPos"];
	for (int i = 0; ; ++i)
	{
		auto& node = guardianGenPos[std::to_string(i)];
		if (node == empty || node.Name() == "")
			break;
		GuardianGenPos pos;
		pos.nX = node["x"];
		pos.nY = node["y"];
		pos.nF = node["f"];
		if (m_bIsMapDivided)
		{
			pos.nTeam = node["team"];
			m_aGuardianGenPos.push_back(pos);
			++m_aGuardianGenMax[pos.nTeam];
		}
		else
		{
			m_aGuardianGenPos.push_back(pos);
			m_aGuardianGenMax[0] = (int)m_aGuardianGenPos.size();
			m_aGuardianGenMax[1] = m_aGuardianGenMax[0];
		}
	}

	m_apReactorGen.resize(m_aGuardianGenPos.size());
	m_abGuardianSummoned[0].resize(m_aGuardianGenPos.size(), false);
	m_abGuardianSummoned[1].resize(m_aGuardianGenPos.size(), false);

	auto& guardian = mc["guardian"];
	for (int i = 0; ; ++i)
	{
		auto& node = guardian[std::to_string(i)];
		if (node == empty || node.Name() == "")
			break;
		m_anGuardianID.push_back(node);
	}
	m_nGuardianID[0] = mc["reactorRed"];
	m_nGuardianID[1] = mc["reactorBlue"];

	auto& diff = mc["reward"]["cpDiff"];
	for (int i = 0; ; ++i)
	{
		auto& node = diff[std::to_string(i)];
		if (node == empty || node.Name() == "")
			break;
		m_anRewardRange.push_back(node);
	}

	auto& range = mc["probChange"]["cpDiff"];
	for (int i = 0; ; ++i)
	{
		auto& node = range[std::to_string(i)];
		if (node == empty || node.Name() == "")
			break;
		RewardRate rate;
		rate.dWinCoin = node["wInCoin"]; //Is that typo?
		rate.dWinRecovery = node["winRecovery"];
		rate.dWinNuff = node["winNuff"];
		rate.dWinCP = node["winCP"];

		rate.dLoseCoin = node["loseCoin"];
		rate.dLoseRecovery = node["loseRecovery"];
		rate.dLoseNuff = node["loseNuff"];
		rate.dLoseCP = node["loseCP"];
		
		m_aRewardRate.push_back(rate);
	}
	m_dClimaxRate = mc["climax"];
}

Field_MonsterCarnival::~Field_MonsterCarnival()
{
}

int Field_MonsterCarnival::OnUseSkill(MemberData &rmd, int nIndex)
{
	MCSkillEntry *pSkill = nullptr;

	if (nIndex < 0 || 
		nIndex >= (int)m_anSkillID.size() ||
		!(pSkill = SkillInfo::GetInstance()->GetMCSkill(m_anSkillID[nIndex])))
		return 5;

	if (rmd.nRestCP < pSkill->nSpendCP)
		return 1;
	
	rmd.nRestCP -= pSkill->nSpendCP;
	m_anRestCP[rmd.nTeam] -= pSkill->nSpendCP;
	return DoNuffToOpponent(rmd.nTeam, pSkill);
}

int Field_MonsterCarnival::DoNuffToOpponent(int nTeam, MCSkillEntry *pEntry)
{
	if (nTeam < 0 || nTeam >= 2)
		return 5;

	std::vector<User*> aUser;
	for (auto& prUser : m_mUser)
		if (prUser.second->GetMCarnivalTeam() != nTeam)
			aUser.push_back(prUser.second);

	int nApplyCount = (int)aUser.size(),
		nApplyIndex = 0;

	if (nApplyCount > 0)
	{
		MobSkillEntry *pMobSkill = nullptr;
		const MobSkillLevelData *pLevel = nullptr;
		if (pEntry->nTarget == 1)
		{
			nApplyIndex = (int)(Rand32::GetInstance()->Random() % nApplyCount);
			nApplyCount = nApplyIndex;
		}
		else if (pEntry->nTarget == 2)
		{
			--nApplyCount;
			nApplyIndex = 0;
		}
		else
			nApplyCount = -1;
		if (!pEntry->nMobSkillID)
			pEntry = SkillInfo::GetInstance()->GetMCRandomSkill();

		if (!pEntry ||
			!(pMobSkill = SkillInfo::GetInstance()->GetMobSkill(pEntry->nMobSkillID)) ||
			pEntry->nLevel > pMobSkill->GetMaxLevel() ||
			!(pLevel = pMobSkill->GetLevelData(pEntry->nLevel)))
			return 5;

		for (; nApplyIndex <= nApplyCount; ++nApplyIndex)
			aUser[nApplyIndex]->OnStatChangeByMobSkill(
				pEntry->nMobSkillID,
				pEntry->nLevel,
				pLevel,
				0,
				0
			);
	}
	return 0;
}

int Field_MonsterCarnival::OnSummonMob(MemberData &rmd, int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_aMob.size())
		return 5;
	
	if (m_nMobGenAvailable[rmd.nTeam] <= 0 || m_nMobGenAvailableUnited <= 0)
		return 2;

	auto& mob = m_aMob[nIndex];
	if (mob.nSpendCP > rmd.nRestCP)
		return 1;

	auto *pGen = AllocObj(LifePool::MobGen);
	auto &mobToGen = pGen->mob;
	auto pTemplate = MobTemplate::GetMobTemplate(mob.nID);
	pGen->nTeamForMCarnival = rmd.nTeam;
	pGen->nRegenInterval = mob.nMobTime;
	pGen->nRegenAfter = 1;
	pGen->mob.SetTemplateID(mob.nID);
	pGen->mob.SetMobTemplate(pTemplate);
	pGen->mob.SetMobGen(pGen);
	if (m_bIsMapDivided)
	{
		int nGenPos = (int)(Rand32::GetInstance()->Random() % m_aMobGenPosDivided[rmd.nTeam].size());
		auto& gen = m_aMobGenPosDivided[rmd.nTeam][nGenPos];
		mobToGen.SetPosX(gen.nX);
		mobToGen.SetPosX(gen.nY);
		mobToGen.SetFh(gen.nFh);
		mobToGen.SetCy(gen.nCy);
	}
	else
	{
		int nGenPos = (int)(Rand32::GetInstance()->Random() % m_aMobGenPosUnited.size());		
		auto& gen = m_aMobGenPosUnited[nGenPos];
		mobToGen.SetPosX(gen.nX);
		mobToGen.SetPosX(gen.nY);
		mobToGen.SetFh(gen.nFh);
		mobToGen.SetCy(gen.nCy);
	}
	--m_nMobGenAvailable[rmd.nTeam];
	--m_nMobGenAvailableUnited;
	m_pLifePool->InsertMCMobGen(pGen);
	rmd.nRestCP -= mob.nSpendCP;
	m_anRestCP[rmd.nTeam] -= mob.nSpendCP;
	return 0;
}

int Field_MonsterCarnival::OnSummonGuardian(MemberData & rmd, int nIndex)
{
	if (nIndex < 0 || 
		nIndex >= (int)m_aGuardianGenPos.size() || 
		(rmd.nTeam < 0 || rmd.nTeam >= 2))
		return 5;

	if (m_abGuardianSummoned[rmd.nTeam][nIndex])
		return 4;

	int nRnd = (int)(Rand32::GetInstance()->Random() % m_nGuardianGenAvailable[rmd.nTeam]);
	int nGenPos = 0;
	while (nGenPos < (int)m_aGuardianGenPos.size())
	{
		auto& gen = m_aGuardianGenPos[nGenPos];
		if (gen.nTeam == -1 ||
			(gen.nTeam == rmd.nTeam && gen.nOccupier == -1 && --nRnd >= 0))
			break;
		++nGenPos;
	}
	if (nGenPos >= (int)m_aGuardianGenPos.size())
		return 5;

	auto pGuardian = SkillInfo::GetInstance()->GetMCGuardian(m_anGuardianID[nIndex]);
	auto pRGen = AllocObj(ReactorPool::ReactorGen);
	pRGen->nTemplateID = m_nGuardianID[rmd.nTeam];
	pRGen->sName = std::to_string(nIndex + 100 * rmd.nTeam);
	pRGen->nX = m_aGuardianGenPos[nGenPos].nX;
	pRGen->nY = m_aGuardianGenPos[nGenPos].nY;
	pRGen->tRegenAfter = 0;
	m_aGuardianGenPos[nGenPos].nOccupier = rmd.nTeam;
	m_apReactorGen[nGenPos] = pRGen;
	--m_nGuardianGenAvailable[rmd.nTeam];
	--m_nGuardianGenAvailableUnited;

	m_pReactorPool->CreateReactor(pRGen);
	m_pReactorPool->SetState(pRGen->sName, 1);

	rmd.nRestCP -= pGuardian->nSpendCP;
	m_anRestCP[rmd.nTeam] -= pGuardian->nSpendCP;
	m_abGuardianSummoned[rmd.nTeam][nIndex] = true;
	return 0;
}

void Field_MonsterCarnival::AddCP(int nLastDamageCharacterID, int nAddCP)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldLock);
	auto findIter = m_mMember.find(nLastDamageCharacterID);
	if (findIter == m_mMember.end())
		return;
	
	if (nAddCP <= 0)
	{
		if (nAddCP + findIter->second.nRestCP <= 0)
			nAddCP *= -1;
		findIter->second.nRestCP += nAddCP;
		m_anRestCP[findIter->second.nTeam] += nAddCP;
	}
	else
	{
		findIter->second.nRestCP += nAddCP;
		findIter->second.nTotCP += nAddCP;
		m_anRestCP[findIter->second.nTeam] += nAddCP;
		m_anTotCP[findIter->second.nTeam] += nAddCP;
	}
	SendCP(findIter->second, User::FindUser(nLastDamageCharacterID));
}

void Field_MonsterCarnival::SendCP(MemberData &rmd, User *pUser)
{
	if (pUser)
	{
		OutPacket oPacket;
		oPacket.Encode2(FieldSendPacketFlag::Field_MCarnival_SendPersonalCP);
		oPacket.Encode2(rmd.nRestCP);
		oPacket.Encode2(rmd.nTotCP);
		pUser->SendPacket(&oPacket);
	}
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketFlag::Field_MCarnival_SendTeamCP);
	oPacket.Encode1(rmd.nTeam);
	oPacket.Encode2(m_anRestCP[rmd.nTeam]);
	oPacket.Encode2(m_anTotCP[rmd.nTeam]);
	BroadcastPacket(&oPacket);
}

void Field_MonsterCarnival::OnEnter(User *pUser)
{
	int nPartyID = pUser->GetPartyID();
	int nMCTeam = -1;

	if (nPartyID == atoi(m_pParentFieldSet->GetVar("MCParty1").c_str()))
		nMCTeam = 0;
	else if (nPartyID == atoi(m_pParentFieldSet->GetVar("MCParty2").c_str()))
		nMCTeam = 1;

	if (m_tEndTime == -1)
		m_tEndTime = GameDateTime::GetTime() + atoi(m_pParentFieldSet->GetVar("timeLimit").c_str()) * 1000;

	pUser->SetMCarnivalTeam(nMCTeam);
	Field::OnEnter(pUser);
	auto findIter = m_mMember.find(pUser->GetUserID());
	if (findIter == m_mMember.end())
	{
		findIter = m_mMember.insert({ pUser->GetUserID(), {} }).first;
		findIter->second.sName = pUser->GetName();
		findIter->second.nTeam = nMCTeam;
	}
	else
		findIter->second.bIsDead = false;

	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketFlag::Field_MCarnival_OnEnter);
	oPacket.Encode1(pUser->GetMCarnivalTeam());
	oPacket.Encode2(findIter->second.nRestCP); 
	oPacket.Encode2(findIter->second.nTotCP);
	oPacket.Encode2(m_anRestCP[nMCTeam]);
	oPacket.Encode2(m_anTotCP[nMCTeam]);
	oPacket.Encode2(m_anRestCP[1 - nMCTeam]);
	oPacket.Encode2(m_anTotCP[1 - nMCTeam]);
	for (auto& nCount : m_aSummonedMobCount[nMCTeam])
		oPacket.Encode1(nCount);
	pUser->SendPacket(&oPacket);
}

void Field_MonsterCarnival::OnLeave(User *pUser)
{
	Field::OnLeave(pUser);
	pUser->SetMCarnivalTeam(-1);
}

void Field_MonsterCarnival::OnPacket(User* pUser, InPacket* iPacket)
{
	int nType = iPacket->Decode2();
	switch (nType)
	{
		case FieldRecvPacketFlag::Field_MCarnival_OnRequest:
			OnRequest(pUser, iPacket);
			break;
		default:
			iPacket->RestorePacket();
			Field::OnPacket(pUser, iPacket);
			break;
	}
}

void Field_MonsterCarnival::OnRequest(User *pUser, InPacket *iPacket)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldLock);
	auto findIter = m_mMember.find(pUser->GetUserID());
	if (findIter == m_mMember.end())
		return;

	int nAction = iPacket->Decode1();
	int nArg = iPacket->Decode1();
	int nResult = 0;
	switch (nAction)
	{
		case 0:
			nResult = OnSummonMob(findIter->second, nArg);
			break;
		case 1:
			nResult = OnUseSkill(findIter->second, nArg);
			break;
		case 2:
			nResult = OnSummonGuardian(findIter->second, nArg);
			break;
		case 3:
			break;		
	}
	OutPacket oPacket;
	if (nResult)
	{
		oPacket.Encode2(FieldSendPacketFlag::Field_MCarnival_OnRequestResult_Failed);
		oPacket.Encode1(nResult);
		pUser->SendPacket(&oPacket);
	}
	else
	{
		oPacket.Encode2(FieldSendPacketFlag::Field_MCarnival_OnRequestResult_Success);
		oPacket.Encode1(nAction);
		oPacket.Encode1(nArg);
		oPacket.EncodeStr(pUser->GetName());

		for (auto& prUser : m_mUser)
			if (prUser.second->GetMCarnivalTeam() == findIter->second.nTeam)
				prUser.second->SendPacket(&oPacket);

		SendCP(findIter->second, pUser);
	}
}

void Field_MonsterCarnival::Reset(bool bShuffleReactor)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldLock);
	Field::Reset(bShuffleReactor);

	m_pLifePool->SetMobGenEnable(true);
	m_nState = 0;
	m_tEndTime = 0;
	m_tFinishMessage = 0;
	m_mMember.clear();
	m_mCharacterOut.clear();
	m_nMobGenAvailableUnited = m_nMobGenPosCount;
	m_nGuardianGenAvailableUnited = (int)m_aMobGenPosUnited.size();
	for (int i = 0; i < 2; ++i)
	{
		m_anRestCP[i] = 0;
		m_anTotCP[i] = 0;
		m_nMobGenAvailable[i] = m_aMobGenMax[i];
		m_nGuardianGenAvailable[i] = m_aGuardianGenMax[i];
		m_aSummonedMobCount[i].resize(m_aSummonedMobCount[i].size(), 0);
		m_abGuardianSummoned[i].resize(m_abGuardianSummoned[i].size(), false);
		for (auto& pos : m_aGuardianGenPos)
			pos.nOccupier = -1;
	}
	m_bCanceled = false;
}

void Field_MonsterCarnival::OnReactorDestroyed(Reactor* pReactor)
{
	int nSN = atoi(pReactor->GetReactorName().c_str());
	int nIndex = nSN % 100;
	int nTeam = nSN / 100;

	if (nIndex < 0 || 
		nIndex >= (int)m_aGuardianGenPos.size() ||
		nTeam < 0 ||
		nTeam >= 2 ||
		!m_abGuardianSummoned[nTeam][nIndex])
		return;

	int nGenPos = -1;
	for (int i = 0; i < (int)m_apReactorGen.size(); ++i)
	{
		if (m_apReactorGen[i] &&
			((ReactorPool::ReactorGen*)m_apReactorGen[i])->sName == pReactor->GetReactorName())
		{
			nGenPos = i;
			break;
		}
	}
	if (nGenPos == -1)
		return;
		
	FreeObj((ReactorPool::ReactorGen*)m_apReactorGen[nGenPos]);
	m_apReactorGen[nGenPos] = nullptr;
	int nGuardian = m_anGuardianID[nIndex];
	auto pGuardian = SkillInfo::GetInstance()->GetMCGuardian(nGuardian);
	if (m_aGuardianGenPos[nGenPos].nOccupier >= 0 && m_aGuardianGenPos[nGenPos].nOccupier < 2)
	{
		m_aGuardianGenPos[nGenPos].nOccupier = -1;
		++m_nGuardianGenAvailable[nTeam];
		++m_nGuardianGenAvailableUnited;
		m_abGuardianSummoned[nTeam][nIndex] = false;
		m_pLifePool->MobStatResetByGuardian(nTeam, pGuardian->nMobSkillID, pGuardian->nLevel);
	}
}

void Field_MonsterCarnival::ProcessTimeFinish()
{
	std::vector<int> anCharacterID[2];
	int nWinTeam = (m_anTotCP[0] > m_anTotCP[1] ? 0 : m_anTotCP[0] == m_anTotCP[1] ? 2 : 1);

	for (auto& prUser : m_mUser)
	{
		int nTeam = prUser.second->GetMCarnivalTeam();
		if (nTeam < 0 || nTeam >= 2)
			continue;
		anCharacterID[nTeam].push_back(prUser.second->GetUserID());

		OutPacket oPacket;
		oPacket.Encode2(FieldSendPacketFlag::Field_MCarnival_ShowGameResult);
		if (m_bCanceled)
			oPacket.Encode1(11);
		else
			oPacket.Encode1(8 + (nTeam == nWinTeam ? (nWinTeam == 2 ? 2 : 0) : 1));

		prUser.second->SendPacket(&oPacket);
	}

	for (int i = 0; i < 2; ++i)
	{
		EffectScreen(i == nWinTeam ? m_sEffectWin : m_sEffectLose, anCharacterID[i]);
		EffectSound(i == nWinTeam ? m_sSoundWin : m_sSoundLose, anCharacterID[i]);
	}
}

void Field_MonsterCarnival::Update()
{
	Field::Update();
	unsigned int tCur = GameDateTime::GetTime();
	if (m_tEndTime > 0 && tCur >= m_tEndTime)
	{
		m_pParentFieldSet->DestroyClock();
		m_pLifePool->RemoveAllMob(false);
		m_pDropPool->TryExpire(true); 
		m_pLifePool->SetMobGenEnable(false);
		ProcessTimeFinish();
		m_tEndTime = 0;
	}
	else
	{
		MCGuardianEntry *pGuardian = nullptr;
		for (int n = 0; n < 2; ++n)
		{
			for (int i = 0; i < (int)m_abGuardianSummoned[n].size(); ++i)
			{
				if (m_abGuardianSummoned[n][i])
				{
					pGuardian = SkillInfo::GetInstance()->GetMCGuardian(m_anGuardianID[i]);
					m_pLifePool->MobStatChangeByGuardian(n, pGuardian->nMobSkillID, pGuardian->nLevel);
				}
			}
		}
	}
}
