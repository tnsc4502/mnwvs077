#include "FieldSet.h"
#include "Field.h"
#include "FieldMan.h"
#include "User.h"
#include "ScriptMan.h"
#include "PartyMan.h"
#include "QWUser.h"
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
	auto pCfg = ConfigLoader::Get(sCfgFilePath);
	m_aFieldID = pCfg->GetArray<int>("FieldList");
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
	m_nPartyMemberMin = pCfg->IntValue("PartyMember_Min");
	m_nPartyMemberMax = pCfg->IntValue("PartyMember_Max");
	m_nLevelMin = pCfg->IntValue("Level_Min");
	m_nLevelMax = pCfg->IntValue("Level_Max");
	m_aJobType = pCfg->GetArray<int>("JobType");

	/*//Load FieldSet Variables
	auto aVariable = pCfg->GetArray<std::string>("FieldSetVar");
	std::vector<std::string> aKeyValue;
	for (auto& sPair : aVariable)
	{
		aKeyValue.clear();
		StringUtility::Split(sPair, aKeyValue, ":");
		if (aKeyValue.size() != 2)
			continue;
		m_mVariable[aKeyValue[0]] = aKeyValue[1];
	}*/

	m_pScript = ScriptMan::GetInstance()->GetScript(m_sScriptName, 0, nullptr);
	lua_pushinteger(m_pScript->GetLuaState(), -1);
	lua_setglobal(m_pScript->GetLuaState(), "userID");
	lua_pcall(m_pScript->GetLuaState(), 0, 0, 0); //Initialize all functions
	auto bindT = std::bind(&FieldSet::Update, this);
	m_pFieldSetTimer = AsyncScheduler::CreateTask(bindT, 1000, true);
}

const std::string & FieldSet::GetFieldSetName() const
{
	return m_sFieldSetName;
}

int FieldSet::Enter(int nCharacterID, int nFieldInfo)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldSetLock);

	auto pUser = User::FindUser(nCharacterID);
	std::vector<int> anUser;
	if (pUser && m_bParty) 
	{
		auto pParty = PartyMan::GetInstance()->GetPartyByCharID(pUser->GetUserID());
		if(!pParty)
			return FieldSetEnterResult::res_Invalid_InvalidParty;
		if (pParty->party.nPartyBossCharacterID != nCharacterID)
			return FieldSetEnterResult::res_Invalid_NotPartyBoss;
		if (m_pFieldSetTimer->IsStarted())
			return FieldSetEnterResult::res_Invalid_AlreadyRegistered;
		for (int i = 0; i < PartyMan::MAX_PARTY_MEMBER_COUNT; ++i)
			if (pParty->party.anChannelID[i] == pUser->GetChannelID() &&
				pParty->party.anFieldID[i] == pUser->GetField()->GetFieldID())
				anUser.push_back(pParty->party.anCharacterID[i]);

		if (anUser.size() < m_nPartyMemberMin || anUser.size() > m_nPartyMemberMax)
			return FieldSetEnterResult::res_Invalid_PartyMemberCount;
	}
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
		Reset();
		TryEnter(apUser, nFieldInfo, nCharacterID);
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
		EndFieldSet();
}

void FieldSet::TransferAll(int nFieldID, const std::string& sPortal)
{
	std::lock_guard<std::recursive_mutex> lock(m_mtxFieldSetLock);
	auto iter = m_mUser.begin();
	while(iter != m_mUser.end())
		if (iter->second)
			(iter++)->second->TryTransferField(nFieldID, sPortal);
}

int FieldSet::TryEnter(const std::vector<User*>& lpUser, int nFieldIdx, int nEnterChar)
{
	m_pFieldSetTimer->Start();
	m_nStartTime = GameDateTime::GetTime();
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

	if (m_mUser.size() > 0 &&
		m_bParty &&
		m_mUser.begin()->second->GetPartyID() == -1)
		EndFieldSet();

	if (tCur - m_nStartTime > m_nTimeLimit * 1000)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mtxFieldSetLock);
		lua_getglobal(m_pScript->GetLuaState(), "onTimeout");
		auto nResult = lua_resume(m_pScript->GetLuaState(), m_pScript->GetLuaCoroutineState(), 0);
		m_pFieldSetTimer->Pause();
	}
}

void FieldSet::Reset()
{
	m_mVariable.clear();
	lua_pcall(m_pScript->GetLuaState(), 0, 0, 0); //Initialize all functions
	for (auto pField : m_aField)
		pField->Reset(true);
}

void FieldSet::EndFieldSet()
{
	m_nStartTime = 0;
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
	lua_getglobal(m_pScript->GetLuaState(), "onUserEnter");
	lua_pushinteger(m_pScript->GetLuaState(), pField->GetFieldID());
	lua_pushinteger(m_pScript->GetLuaState(), pUser->GetUserID());
	lua_resume(m_pScript->GetLuaState(), m_pScript->GetLuaCoroutineState(), 2);

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
	oPacket.Encode4(((m_nStartTime + m_nTimeLimit * 1000) - GameDateTime::GetTime()) / 1000 + 1);
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
