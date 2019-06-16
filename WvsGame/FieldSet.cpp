#include "FieldSet.h"
#include "Field.h"
#include "FieldMan.h"
#include "User.h"
#include "ScriptMan.h"
#include "PartyMan.h"
#include "QWUser.h"
#include "ScriptFieldSet.h"
#include <functional>
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\DateTime\GameDateTime.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\PacketFlags\FieldPacketFlags.hpp"

FieldSet::FieldSet()
{
}

FieldSet::~FieldSet()
{
	m_pFieldSetTimer->Abort();
	delete m_pFieldSetTimer;
	if (m_pScript) 
	{
		m_pScript->Abort();
		//FreeObj(m_pScript);
	}
}

void FieldSet::Init(const std::string & sCfgFilePath)
{
	m_pCfg = ConfigLoader::Get(sCfgFilePath);
	InitConfig();
	m_pScript = ScriptMan::GetInstance()->CreateScript(m_sScriptName, { 0, nullptr });
	if (!m_pScript)
		return;

	m_pScript->SetFieldSet(this);
	m_pScriptFieldSet = AllocObj(ScriptFieldSet);
	m_pScriptFieldSet->SetFieldSet(this);
	m_pScript->PushInteger("userID", -1);

	lua_pcall(m_pScript->GetLuaState(), 0, 0, 0); //Initialize all functions
	auto bindT = std::bind(&FieldSet::Update, this);
	m_pFieldSetTimer = AsyncScheduler::CreateTask(bindT, 500, true);
}

void FieldSet::InitConfig()
{
	auto pCfg = (ConfigLoader*)m_pCfg;
	m_aFieldID = pCfg->GetArray<int>("FieldList");
	m_aField.clear();
	Field *pField;
	for (auto& nFieldID : m_aFieldID)
	{
		pField = FieldMan::GetInstance()->GetField(nFieldID);
		pField->SetFieldSet(this);
		m_aField.push_back(pField);
	}
	m_sFieldSetName = pCfg->StrValue("FieldSetName");
	m_sScriptName = pCfg->StrValue("ScriptName");
	m_nTimeLimit = pCfg->IntValue("TimeLimit");
	m_bParty = pCfg->IntValue("Party") == 1;
	m_bInvokeUpdateFunc = pCfg->IntValue("InvokeUpdate") == 1;
	m_nPartyMemberMin = pCfg->IntValue("PartyMember_Min");
	m_nPartyMemberMax = pCfg->IntValue("PartyMember_Max");
	m_nLevelMin = pCfg->IntValue("Level_Min");
	m_nLevelMax = pCfg->IntValue("Level_Max");
	m_aJobType = pCfg->GetArray<int>("JobType");

	auto aVariable = pCfg->GetArray<std::string>("FieldSetVar");
	std::vector<std::string> aKeyValue;
	for (auto& sPair : aVariable)
	{
		aKeyValue.clear();
		StringUtility::Split(sPair, aKeyValue, ":");
		if (aKeyValue.size() != 2)
			continue;
		m_mVariable[aKeyValue[0]] = aKeyValue[1];
	}
}

const std::string & FieldSet::GetFieldSetName() const
{
	return m_sFieldSetName;
}

//bJoin : For monster carnival 2nd team.
int FieldSet::Enter(int nCharacterID, int nFieldInfo, bool bJoin)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldSetLock);

	if (!bJoin && m_pFieldSetTimer->IsStarted())
		return FieldSetEnterResult::res_Invalid_AlreadyRegistered;

	auto pUser = User::FindUser(nCharacterID);
	std::vector<int> anUser;
	if (pUser && m_bParty)
	{
		auto pParty = PartyMan::GetInstance()->GetPartyByCharID(pUser->GetUserID());
		if (!pParty)
			return FieldSetEnterResult::res_Invalid_InvalidParty;
		if (pParty->party.nPartyBossCharacterID != nCharacterID)
			return FieldSetEnterResult::res_Invalid_NotPartyBoss;
		for (int i = 0; i < PartyMan::MAX_PARTY_MEMBER_COUNT; ++i)
			if (pParty->party.anChannelID[i] == pUser->GetChannelID() &&
				pParty->party.anFieldID[i] == pUser->GetField()->GetFieldID())
				anUser.push_back(pParty->party.anCharacterID[i]);
		if (anUser.size() < m_nPartyMemberMin || anUser.size() > m_nPartyMemberMax)
			return FieldSetEnterResult::res_Invalid_PartyMemberCount;
	}
	else if (pUser)
		anUser.push_back(nCharacterID);

	if (pUser)
	{
		std::vector<User*> apUser;
		User *pMember = nullptr;
		for (auto& nID : anUser)
		{
			pMember = User::FindUser(nID);
			if (!pMember)
				return FieldSetEnterResult::res_Invalid_PartyMemberCount;
			if (!CheckEnterRequirement(pMember))
				return FieldSetEnterResult::res_Invalid_PartyMemberStat;
			apUser.push_back(pMember);
		}
		if(!bJoin)
			Reset();
		TryEnter(apUser, nFieldInfo, nCharacterID);
		if (bJoin)
			m_pScript->SafeInvocation("onJoin", { nCharacterID });
	}

	return FieldSetEnterResult::res_Success;
}

bool FieldSet::CheckEnterRequirement(User * pUser)
{
	int nLevel = QWUser::GetLevel(pUser);
	int nJobType = QWUser::GetJob(pUser) / 100;
	if (nLevel < m_nLevelMin || (m_nLevelMax && nLevel > m_nLevelMax))
		return false;
	for (auto& nType : m_aJobType)
		if (nType == nJobType)
			return true;
	return false;
}

bool FieldSet::IsPartyFieldSet() const
{
	return m_bParty;
}

bool FieldSet::IsGuildFieldSet() const
{
	return false;
}

void FieldSet::OnLeaveFieldSet(int nCharacterID)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldSetLock);
	m_mUser.erase(nCharacterID);
	if (m_mUser.size() == 0) 
	{
		ForceClose();
		m_pFieldSetTimer->Pause();
	}
}

void FieldSet::TransferAll(int nFieldID, const std::string& sPortal)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldSetLock);
	auto iter = m_mUser.begin();
	while(iter != m_mUser.end())
		if (iter->second)
			(iter++)->second->TryTransferField(nFieldID, sPortal);
}

void FieldSet::TransferParty(int nPartyID, int nFieldID, const std::string &sPortal)
{
	int anCharacterID[PartyMan::MAX_PARTY_MEMBER_COUNT];
	PartyMan::GetInstance()->GetSnapshot(nPartyID, anCharacterID);
	for (auto& nID : anCharacterID)
	{
		auto findIter = m_mUser.find(nID);
		if (findIter == m_mUser.end())
			continue;
		findIter->second->TryTransferField(nFieldID, sPortal);
	}
}

int FieldSet::TryEnter(const std::vector<User*>& lpUser, int nFieldIdx, int nEnterChar)
{
	m_pFieldSetTimer->Start();
	for (auto& pUser : lpUser)
	{
		m_mUser[pUser->GetUserID()] = pUser;
		pUser->TryTransferField(
			m_aFieldID[nFieldIdx],
			""
		);
	}
	return 0;
}

void FieldSet::Update()
{
	auto tCur = GameDateTime::GetTime();

	if (m_bInvokeUpdateFunc)
		m_pScript->SafeInvocation("onUpdate", { tCur });

	if (m_mUser.size() > 0 &&
		m_bParty &&
		m_mUser.begin()->second->GetPartyID() == -1)
		ForceClose();

	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldSetLock);
	if (m_tStartTime >= 0 && tCur - m_tStartTime > (unsigned int)m_nTimeLimit * 1000) 
	{
		m_tStartTime = -1;
		m_pScript->SafeInvocation("onTimeout", { m_tStartTime == 0 });
	}
}

void FieldSet::Reset()
{
	m_mVariable.clear();
	InitConfig();
	m_pScript->SafeInvocation("onInit");
	for (auto pField : m_aField)
		pField->Reset(true);
	m_tStartTime = GameDateTime::GetTime();
}

void FieldSet::ForceClose()
{
	m_tStartTime = 0;
	DestroyClock();
}

void FieldSet::DestroyClock()
{
	OutPacket oPacket;
	oPacket.Encode2(FieldSendPacketFlag::Field_OnDestroyClock);
	oPacket.GetSharedPacket()->ToggleBroadcasting();

	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldSetLock);
	for (auto& prUser : m_mUser)
		if (prUser.second)
			prUser.second->SendPacket(&oPacket);
}

void FieldSet::ResetTimeLimit(int nTimeLimit, bool bResetTimeTick)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldSetLock);
	if (bResetTimeTick)
		m_tStartTime = GameDateTime::GetTime();
	m_nTimeLimit = nTimeLimit;
}

void FieldSet::SetVar(const std::string& sVarName, const std::string& sVal)
{
	m_mVariable[sVarName] = sVal;
}

const std::string & FieldSet::GetVar(const std::string& sVarName)
{
	return m_mVariable[sVarName];
}

void FieldSet::OnUserEnterField(User* pUser, Field *pField)
{
	m_pScript->SafeInvocation("onUserEnter", { pField->GetFieldID(), pUser->GetUserID() });
	if (m_pFieldSetTimer->IsStarted()) 
	{
		OutPacket oPacket;
		MakeClockPacket(oPacket);
		pUser->SendPacket(&oPacket);
	}
}

void FieldSet::MakeClockPacket(OutPacket & oPacket)
{
	auto tCur = GameDateTime::GetTime();
	oPacket.Encode2((short)FieldSendPacketFlag::Field_OnClock);
	oPacket.Encode1(2);
	oPacket.Encode4((int)((m_tStartTime + m_nTimeLimit * 1000) - GameDateTime::GetTime()) / 1000 + 1);
}

int FieldSet::GetUserCount() const
{
	return (int)m_mUser.size();
}

void FieldSet::IncExpAll(int nCount)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldSetLock);
	for (auto& prUser : m_mUser)
		if (prUser.second)
			QWUser::IncEXP(prUser.second, nCount, false);
}

ScriptFieldSet* FieldSet::GetScriptFieldSet() const
{
	return m_pScriptFieldSet;
}
