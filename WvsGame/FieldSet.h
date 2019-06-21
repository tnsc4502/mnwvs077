#pragma once
#include <mutex>
#include <unordered_map>
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
	struct ActionInfo
	{
		struct ReactorInfo
		{
			int nEventState = 0;
			std::string sName;
		};

		enum EventType
		{
			e_Event_ReactorAction,
			e_Event_EventAction,
		};

		enum ActionType
		{
			e_Action_Reactor_SetReactorState = 3,
		};

		union VAL_TYPE
		{
			int nVal;
			double dVal;
			char sVal[32];

			void FromStr(const std::string& s);
		};

		int nActionType = 0, 
			nEventType = 0, 
			nFieldIdx = 0;

		std::vector<ReactorInfo> ari;
		std::vector<VAL_TYPE> avArgs;
	};

	std::recursive_mutex m_mtxFieldSetLock;
	std::map<std::string, std::string> m_mVariable;
	std::map<int, User*> m_mUser;
	std::unordered_map<unsigned int, ActionInfo> m_mEventAction;
	std::vector<ActionInfo> m_aReactorActionInfo;
	std::vector<int> m_aFieldID, m_aJobType, m_aFieldUnAffected;
	std::vector<Field*> m_aField, m_aField_Affected;
	std::string m_sFieldSetName, m_sScriptName;
	void *m_pCfg;

	int m_nTimeLimit, 
		m_nPartyMemberMin, 
		m_nPartyMemberMax,
		m_nLevelMin,
		m_nLevelMax;

	long long int m_tStartTime;
	bool m_bParty, m_bEnd, m_bInvokeUpdateFunc;
	Script* m_pScript;
	AsyncScheduler *m_pFieldSetTimer;
	ScriptFieldSet* m_pScriptFieldSet;

	void LoadReactorAction(void *pAction);
	void LoadEachAction(void *pSub, int nFieldIdx);

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
	void InitConfig();
	const std::string& GetFieldSetName() const;
	ScriptFieldSet* GetScriptFieldSet() const;

	int Enter(int nCharacterID, int nFieldInfo, bool bJoin = false);
	bool CheckEnterRequirement(User *pUser);
	bool IsPartyFieldSet() const;
	bool IsGuildFieldSet() const;
	int GetFieldIndex(Field *pField) const;
	void SetVar(const std::string& sVarName, const std::string& sVal);
	const std::string& GetVar(const std::string& sVarName);
	int GetUserCount() const;

	void OnUserEnterField(User* pUser, Field *pField);
	void MakeClockPacket(OutPacket& oPacket);
	void OnLeaveFieldSet(int nCharacterID);
	void TransferAll(int nFieldID, const std::string& sPortal);
	void TransferParty(int nPartyID, int nFieldID, const std::string& sPortal);
	int TryEnter(const std::vector<User*>& lpUser, int nFieldIdx, int nEnterChar);
	void Update();
	void Reset();
	void ForceClose();
	void DestroyClock();
	void ResetTimeLimit(int nTimeLimit, bool bResetTimeTick);
	void IncExpAll(int nCount);
	void BroadcastMsg(int nType, const std::string& sMsg, int nNPCTemplateID);

	void OnTime(unsigned int uEventSN);
	int GetReactorState(int nFieldIdx, const std::string& sReactorName);
	void SetReactorState(int nFieldIdx, const std::string& sReactorName, int nState, int nOrder);
	void DoReactorAction(const ActionInfo& prai);
	void DoEventAction(const ActionInfo& prai);
	void CheckReactorAction(Field *pField, const std::string sReactorName, unsigned int tEventTime);
};

