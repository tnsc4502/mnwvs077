#pragma once
#include <vector>
#include <map>
#include "FieldPoint.h"
#include "..\WvsLib\Common\CommonDef.h"

struct RewardInfo;
class ReactorTemplate
{
	ALLOW_PRIVATE_ALLOC

	friend class Reactor;
	struct EventInfo
	{
		FieldPoint m_aSpaceVertex[2];
		bool m_bCheckTargetRange = false;
		char m_nTouchable = 0;

		int m_nType = 0,
			m_nStateToBe = 0,
			m_nItemID = 0,
			m_nCount = 0,
			m_tHitDelay = 0;
	};

	struct StateInfo
	{
		std::vector<EventInfo> m_aEventInfo;
		int m_tTimeout = 0,
			m_tHitDelay = 0;
	};

	struct ActionInfo
	{
		std::string sAction, sArgs, sMessage;
		int nState = 0, nType = 0;
		std::vector<int> anArgs;
		std::vector<std::string> asArgs;
	};

	const std::vector<RewardInfo*>* m_aRewardInfo;
	std::vector<StateInfo> m_aStateInfo;
	std::vector<ActionInfo> m_aActionInfo;

	int m_nTemplateID, m_nReqHitCount = 0, m_tHitDelay = 0;
	bool m_bRemoveInFieldSet = false;

	ReactorTemplate();
	static std::map<int, ReactorTemplate*> m_mReactorTemplate;
public:

	static void RegisterReactor(int nTemplateID, void *pImg, void *pRoot);
	static void LoadEvent(StateInfo* pInfo, void *pImg);
	static void LoadAction(ReactorTemplate* pTemplate, const std::string& sAction);
	static void Load();
	static ReactorTemplate* GetReactorTemplate(int nTemplateID);

	StateInfo* GetStateInfo(int nState);
	EventInfo* GetEventInfo(int nState, int nEventIdx);
};

