#pragma once
#include <mutex>
#include <map>
#include <vector>
#include <string>

class User;
class Field;
class Script;
class AsyncScheduler;
class OutPacket;
class ScriptFieldSet;

class FieldSet
{
	std::recursive_mutex m_mtxFieldSetLock;
	std::map<std::string, std::string> m_mVariable;
	std::map<int, User*> m_mUser;
	std::vector<int> m_aFieldID, m_aJobType;
	std::vector<Field*> m_aField;
	std::string m_sFieldSetName, m_sScriptName;

	int m_nTimeLimit, 
		m_nStartTime, 
		m_nPartyMemberMin, 
		m_nPartyMemberMax,
		m_nLevelMin,
		m_nLevelMax;

	bool m_bParty, m_bEnd;
	Script* m_pScript;
	AsyncScheduler *m_pFieldSetTimer;
	ScriptFieldSet* m_pScriptFieldSet;

public:
	enum FieldSetEnterResult
	{
		res_Success = 0x00,
		res_Invalid_InvalidParty = 0x01,
		res_Invalid_NotPartyBoss = 0x02,
		res_Invalid_PartyMemberCount = 0x03,
		res_Invalid_PartyMemberStat = 0x04,
		res_Invalid_AlreadyRegistered = 0x05
	};

	FieldSet();
	~FieldSet();

	void Init(const std::string& sCfgFilePath);
	const std::string& GetFieldSetName() const;
	int Enter(int nCharacterID, int nFieldInfo);
	bool CheckEnterRequirement(User *pUser);
	bool IsPartyFieldSet() const;
	bool IsGuildFieldSet() const;
	void OnLeaveFieldSet(int nCharacterID);
	void TransferAll(int nFieldID, const std::string& sPortal);
	int TryEnter(const std::vector<User*>& lpUser, int nFieldIdx, int nEnterChar);
	void Update();
	void Reset();
	void EndFieldSet();
	void DestroyClock();
	void SetVar(const std::string& sVarName, const std::string& sVal);
	const std::string& GetVar(const std::string& sVarName);
	void OnUserEnterField(User* pUser, Field *pField);
	void MakeClockPacket(OutPacket& oPacket);
	int GetUserCount() const;
	void IncExpAll(int nCount);
	ScriptFieldSet* GetScriptFieldSet() const;
};

