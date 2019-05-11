#include "Reward.h"
#include "..\WvsLib\Memory\MemoryPoolMan.hpp"
#include "..\WvsLib\Wz\ImgAccessor.h"
#include "..\Database\GW_ItemSlotBase.h"
#include "QuestMan.h"
#include "QuestDemand.h"

std::map<int, std::vector<RewardInfo*>> Reward::stMobRewardInfo;

Reward::Reward()
{
}

Reward::Reward(Reward * pOther)
{
	m_pItem = pOther->m_pItem;
	m_nMoney = pOther->m_nMoney;
	m_nType = pOther->m_nType;
	m_pInfo = pOther->m_pInfo;
}

Reward::~Reward()
{
}

void Reward::LoadReward()
{
	WZ::ImgAccessor img("./DataSrv/Reward");
	stMobRewardInfo.clear();
	for (auto& mobNode : img)
	{
		int nTemplateID = atoi(mobNode.Name().substr(1, mobNode.Name().size() - 1).c_str());
		unsigned int unTotalWeight = 0;
		for (auto& rewardNode : mobNode)
		{
			RewardInfo* pInfo = AllocObj(RewardInfo);
			for (auto& node : rewardNode)
			{
				if (node.Name() == "prob") {
					std::string strProb = node;
					auto dProb = atof(strProb.substr(4, strProb.size() - 4).c_str()) * 1000000000.0;
					pInfo->m_unWeight = (unsigned int)dProb;
				}
				else if (node.Name() == "money")
					pInfo->m_nMoney = (int)node;
				else if (node.Name() == "item")
				{
					pInfo->m_nItemID = (int)node;
					int nQuestID = QuestMan::GetInstance()->GetQuestByItem(pInfo->m_nItemID);
					if (nQuestID != 0)
					{
						auto pDemand = QuestMan::GetInstance()->GetCompleteDemand(nQuestID);
						if (pDemand)
						{
							pInfo->m_usQRKey = nQuestID;
							auto& aDemandItem = pDemand->m_mDemandItem;
							for (auto& nItem : aDemandItem)
								if (nItem.first == pInfo->m_nItemID)
									pInfo->m_nMaxCount = nItem.second;
						}
					}
				}
				else if (node.Name() == "min")
					pInfo->m_nMin = (int)node;
				else if (node.Name() == "max")
					pInfo->m_nMax = (int)node;
				else if (node.Name() == "period")
					pInfo->m_nPeriod = (int)node;
				else if (node.Name() == "premium")
					pInfo->m_bPremiumMap = ((int)node == 1);
			}

			stMobRewardInfo[nTemplateID].push_back(pInfo);
		}
	}
}

const std::vector<RewardInfo*>* Reward::GetMobReward(int nTemplateID)
{
	static std::vector<RewardInfo*> aEmpty;
	auto findIter = stMobRewardInfo.find(nTemplateID);
	if (findIter == stMobRewardInfo.end())
		return &aEmpty;
	return &(findIter->second);
}
