#include "ReactorTemplate.h"
#include "Reward.h"
#include "..\WvsLib\Wz\WzResMan.hpp"
#include "..\WvsLib\Wz\ImgAccessor.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"

std::map<int, ReactorTemplate*> ReactorTemplate::m_mReactorTemplate;

ReactorTemplate::ReactorTemplate()
{
}

void ReactorTemplate::RegisterReactor(int nTemplateID, void *pImg, void *pRoot)
{
	if (m_mReactorTemplate.find(nTemplateID) != m_mReactorTemplate.end())
		return;

	auto& ref = *((WZ::Node*)pImg);
	auto empty = WZ::Node();
	ReactorTemplate *pTemplate = AllocObj(ReactorTemplate);
	pTemplate->m_nTemplateID = nTemplateID;

	//Load event info.
	for (int i = 0; ; ++i)
	{
		pTemplate->m_aStateInfo.push_back(StateInfo{});
		auto* pStateInfo = &(pTemplate->m_aStateInfo[pTemplate->m_aStateInfo.size() - 1]);
		auto& node = ref[std::to_string(i)];
		if (node == empty)
			break;
		LoadEvent(pStateInfo, &(node));
	}

	//Load basic info.
	pTemplate->m_nReqHitCount = ref["info"]["hitCount"];
	pTemplate->m_tHitDelay = ref["info"]["delay"];
	pTemplate->m_bRemoveInFieldSet = ((int)ref["info"]["removeInFieldSet"] == 1 ? true : false);

	//Load link template
	auto& linkNode = ref["info"]["link"];
	if (linkNode != empty && linkNode.Name() != "")
	{
		RegisterReactor(linkNode, &((*((WZ::Node*)pRoot))[(std::string)linkNode]), pRoot);
		auto itLinkTemplate = m_mReactorTemplate.find(linkNode);
		if (itLinkTemplate != m_mReactorTemplate.end()) 
		{
			pTemplate->m_nTemplateID = nTemplateID;
			*pTemplate = *itLinkTemplate->second;
		}
	}

	//Load action info.
	auto& actionNode = ref["action"];
	if (actionNode != empty)
		LoadAction(pTemplate, (std::string)actionNode);

	//Load reward info.
	pTemplate->m_aRewardInfo = Reward::GetReactorReward(nTemplateID);
	m_mReactorTemplate[nTemplateID] = pTemplate;
}

void ReactorTemplate::LoadEvent(StateInfo * pInfo, void * pImg)
{
	auto& ref_ = (*((WZ::Node*)pImg));
	auto& ref = ref_["event"];
	auto empty = WZ::Node();
	if (ref == empty)
		return;
	EventInfo eventInfo;

	for (int i = 0; ; ++i)
	{
		auto& node = ref[std::to_string(i)];
		if (node == empty)
			break;

		eventInfo.m_nType = (int)node["type"];
		eventInfo.m_nStateToBe = (int)node["state"];
		eventInfo.m_tHitDelay = (int)node["delay"];
		
		auto& lt = node["lt"];
		if (lt != empty)
		{
			auto& rb = node["rb"];
			eventInfo.m_aSpaceVertex[0].x = (int)lt["x"];
			eventInfo.m_aSpaceVertex[0].y = (int)lt["y"];
			eventInfo.m_aSpaceVertex[1].x = (int)rb["x"];
			eventInfo.m_aSpaceVertex[1].y = (int)rb["y"];
			eventInfo.m_bCheckTargetRange = true;
		}
		if (ref_["info"]["activateByTouch"] != empty)
			eventInfo.m_nTouchable = 2;
		else
			eventInfo.m_nTouchable =
				(node["2"] != empty && (int)node["2"] > 0) ||
				(node["clickArea"] != empty) ||
				(eventInfo.m_nType == 9);

		eventInfo.m_nItemID = (int)node["0"];
		eventInfo.m_nCount = (int)node["1"];

		pInfo->m_aEventInfo.push_back(eventInfo);
		pInfo->m_tTimeout = (int)ref["timeOut"];
		pInfo->m_tHitDelay = (int)ref["delay"];
	}
}

void ReactorTemplate::LoadAction(ReactorTemplate* pTemplate, const std::string& sAction)
{
	static auto funcIsNumber = [](const std::string& strCheck) {
		for (auto c : strCheck) 
			if (c != '-' && (c < '0' || c > '9')) return false;
		return true;
	};
	static WZ::ImgAccessor img("./DataSrv/ReactorAction");
	auto& actionNode = img[sAction];
	auto empty = WZ::Node();
	pTemplate->m_aActionInfo.clear();
	if (actionNode != img.end())
		for (auto& action : actionNode)
		{
			ActionInfo info;
			info.nState = action["state"];
			info.nType = action["type"];
			info.sMessage = action["message"];
			info.sAction = sAction;

			for (int i = 0; ; ++i)
			{
				auto& argNode = action[std::to_string(i)];
				if (argNode == empty || argNode.Name() == "")
					break;
				if (funcIsNumber((std::string)argNode))
					info.anArgs.push_back((int)argNode);
				else
					info.asArgs.push_back((std::string)argNode);

				if (info.nType == 10)
					break;
			}
			pTemplate->m_aActionInfo.push_back(std::move(info));
		}
}

void ReactorTemplate::Load()
{
	auto& ref = stWzResMan->GetWz(Wz::Reactor);
	for (auto& reactor : ref)
		RegisterReactor(atoi(reactor.Name().c_str()), &reactor, &ref);
}

ReactorTemplate * ReactorTemplate::GetReactorTemplate(int nTemplateID)
{
	auto findIter = m_mReactorTemplate.find(nTemplateID);
	if (findIter == m_mReactorTemplate.end())
		return nullptr;
	return findIter->second;
}

ReactorTemplate::StateInfo * ReactorTemplate::GetStateInfo(int nState)
{
	if (nState < 0 || nState >= m_aStateInfo.size())
		return nullptr;
	return &(m_aStateInfo[nState]);
}

ReactorTemplate::EventInfo * ReactorTemplate::GetEventInfo(int nState, int nEventIdx)
{
	auto pInfo = GetStateInfo(nState);
	if (pInfo == nullptr || (nEventIdx < 0) || (nEventIdx >= pInfo->m_aEventInfo.size()))
		return nullptr;
	return &(pInfo->m_aEventInfo[nEventIdx]);
}
